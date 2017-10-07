#include "../include/fpga2cpu_cmd.h"
#include "../driver/1-wire_driver/common/one_wire_hal.h"
#include "..\driver\1-wire_driver\common\one_wire_net.h"
#include "../include/bsp_internalFlash.h"
#include "../include/fsmc.h"
#include <string.h>
#define STATUS_ADDR 0x0100
#define verinfolen 24
char adconfig[]={0x00,0x96,0xD0,0xE8,0x00,
                 0x50,0x00,0x00,0x00,0x00,0x00,0x00};
char devicestatus[64]={0};
int statuslen=64;
int adconfig_len=sizeof(adconfig);
char verinfobuff[verinfolen]={0};
//0x0c sn210(3 bytes) 0x0d sn543(3 bytes)
int config_addr0=0x1f;
char cpu2fpgadata[4]={0};
char cdbuffer[4]={0};
//todo: update from flash on powerup
int pre_chn_cfg=0x26;
char probID[66]={0};
int SN210_OFFSET=SN210*4;
int SN543_OFFSET=SN543*4;
int pre_chn_cfg_OFFSET=PRE_CFG*4;
void cpu2fpga(char*data)
{
	//fpga_write(0x04,data[0]);
	fpga_write(0x04,cpu2fpgadata[0]);
	fpga_write(0x05,data[1]);
	fpga_write(0x06,data[2]);
	fpga_write(0x07,data[3]);
	fpga_write(0x08,1);
}
 void task_devicestatus()
 {
	 char dummy[4];
	 devicestatus[0]=0x55;
	 devicestatus[1]=0xaa;
	 devicestatus[2]=0x55;
	 devicestatus[3]=0xaa;
	 devicestatus[4]=0x10;//ver num
	 getverinfo(SN210);
	 getverinfo(SN543);
	 memcpy(devicestatus+5,verinfobuff+SN210_OFFSET+1,3);
	 memcpy(devicestatus+8,verinfobuff+SN543_OFFSET+1,3);
	 devicestatus[11]=0;//battery;
	 devicestatus[12]=0;
	 devicestatus[13]=0;//battery level
	 devicestatus[14]=0;//battery status
	 devicestatus[15]=get_sample_freq(dummy);
	 devicestatus[16]=get_max_f_num(dummy);
	 devicestatus[17]=get_f_num(dummy);
	 devicestatus[18]=get_front_prob_num();
	 for(int i=0;i<0xd;i++){
	 dummy[0]=SET_F_PROB_ID_TEST+i;
	 decoder(dummy);
 }
	 memcpy(devicestatus+32,probID,32);
	 
 }
 int get_front_prob_num()
 {
	 cpu2fpgadata[1]=0;
			for(int i=0;i<ONE_WIRE_PORT_NUM;i++)
		{
			cpu2fpgadata[1]+=one_wire_device_num[i];
		}
		cpu2fpga(cpu2fpgadata);
		return cpu2fpgadata[1];
 }
void write_status()
{
	int i=0;
	for(;i<statuslen;i++)
	{
		fpga_write(STATUS_ADDR+i,*(devicestatus+i));
	}
}
void set_sample_freq(int val)
{	

	switch(val)
	{
		case 1://256
			adconfig[1]=0x96;
			break;
		case 2://512
			adconfig[1]=0x95;
			break;
		default:
			adconfig[1]=0x96;
		break;
	}
	int i=0;
	for(i=0;i<adconfig_len;i++)
	{
		fpga_write(config_addr0+i,adconfig[i]);
	}
	fpga_write(config_addr0,1);

}
int get_sample_freq(char*cmd)
{
	cpu2fpgadata[0]=cmd[0];
	switch(adconfig[1])
	{
		case 0x95://512
			cpu2fpgadata[1]=2;
			break;
		case 0x96://256
			cpu2fpgadata[1]=1;
			break;
		default:
			cpu2fpgadata[1]=2;
			break;
	}
	cpu2fpga(cpu2fpgadata);
	return cpu2fpgadata[1];
}
void set_max_f_num(char*cmd,int burn)
{
	int max_f_num=cmd[1];
	readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,verinfolen,0);
	verinfobuff[pre_chn_cfg_OFFSET]=max_f_num;
	writeflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,verinfolen,0);
	
}

int get_max_f_num(char* cmd)
{
	readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,verinfolen,0);
	cpu2fpgadata[1]=verinfobuff[pre_chn_cfg_OFFSET];
	cpu2fpga(cpu2fpgadata);
	return cpu2fpgadata[1];
}
void set_f_num(char*cmd)
{
	pre_chn_cfg=cmd[1];
	//readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,verinfolen,0);
	int pre_max_num=0x27;//verinfobuff[pre_chn_cfg_OFFSET];
	if(pre_chn_cfg>pre_max_num)
	{//0x27 max allowed
		pre_chn_cfg=pre_max_num;
	}
	switch(pre_chn_cfg)
	{
		case 0x26:
			fpga_write(0x15,1);
			break;
		case 0x27:
			fpga_write(0x15,0);
			break;
		default:
			fpga_write(0x15,0);
			break;
	
	}
	/*
	if(burn)
	{
		readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,verinfolen,0);
		verinfobuff[pre_chn_cfg_OFFSET]=cmd[1];
		writeflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,verinfolen,0);
		readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,verinfolen,0);
	}
	*/
}
int get_f_num(char*cmd)
{
	//readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,verinfolen,0);
	//pre_chn_cfg=verinfobuff[pre_chn_cfg_OFFSET];
	cpu2fpgadata[1]=pre_chn_cfg;
	cpu2fpga(cpu2fpgadata);
	return pre_chn_cfg;
}

void getverinfo( VERINFO vi)
{
	/* this code use verinfobuff[0-4] as temp buffer, all verinfo will put in there, then goto fpga
	if(vi<SN210)
		readflash(FPGA_VER_SECTOR,FPGA_VER_SECTOR_END,verinfobuff,4,vi*4);
	else
		readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,4,(vi-SN210)*4);
	
	cpu2fpgadata[1]=*(verinfobuff+(vi-SN210)*4+1);
	cpu2fpgadata[2]=*(verinfobuff+(vi-SN210)*4+2);
	cpu2fpgadata[3]=*(verinfobuff+(vi-SN210)*4+3);
	*/
	readflash(FPGA_VER_SECTOR,FPGA_VER_SECTOR_END,verinfobuff,4,vi*4);
	cpu2fpgadata[1]=*(verinfobuff+vi*4+1);
	cpu2fpgadata[2]=*(verinfobuff+vi*4+2);
	cpu2fpgadata[3]=*(verinfobuff+vi*4+3);
	cpu2fpga(cpu2fpgadata);
}
void setverinfo(char *cmd,VERINFO vi,int burn)
{
	//only sn210 and sn543 use this command, call cmd sn543 will burn into flash
	if(vi==SN210)
	{
		for(int i=0;i<4;i++)
		cdbuffer[i]=cmd[i];
		return;
	}
	
	readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,verinfolen,0);
	for(int i=0;i<4;i++)
	{
		*(verinfobuff+SN543_OFFSET+i)=*(cmd+i);//543 buffer into verinfobuff
		*(verinfobuff+SN210_OFFSET+i)=*(cdbuffer+i);//210buffer
	}
	
	if(burn)
		writeflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,verinfolen,0);
}
void getfpgaver(char *cmd)
{
	getverinfo(FpgaInfo);
}

void getcpuver(char *cmd)
{
	getverinfo(CpuInfo);
}
void getpcbver(char *cmd)
{
	getverinfo(PcbInfo);
}
void getf210sn(char *cmd)
{
	getverinfo(SN210);
}
void getf543sn(char *cmd)
{
	getverinfo(SN543);
}

void setf543sn(char*cmd)
{
	setverinfo(cmd,SN543,1);
}
void setf210sn(char*cmd)
{
	setverinfo(cmd,SN210,0);
}
void get_f_id(char*cmd)
{
	char c_id[22*3];
	int prob_id_offset=cmd[0]-GET_F_PROB_ID;
	
}
void set_f_threshold(char*cmd)
{
//	if(adconfig[4]!=0)//0: prob drop thresh test, other: impendency test
//	{
//		adconfig[8]=0;
//		adconfig[9]=0;
//		return;
//	}
	adconfig[4]&=~ADS1299_COMP_TH_MASK;
	adconfig[4]+=cmd[1]<<ADS1299_COMP_TH_SHIFT;
	int i=0;
	adconfig[8]=0xFF;
	adconfig[9]=0xFF;
	adconfig[11]=0x02;
		for(i=0;i<adconfig_len;i++)
	{
		fpga_write(config_addr0+i,adconfig[i]);
	}
	fpga_write(config_addr0,1);
}
void set_impd_test(char *cmd)
{
	if(cmd[1]==0)
		return;
	adconfig[4]=0;//set back to default
	adconfig[4]&=~ADS1299_ILEAD_OFF_MASK;
	adconfig[4]&=~ADS1299_FLEAD_OFF_MASK;
	char temp1=cmd[2]&0xf0;//nA
	char temp2=cmd[2]&0x0f;//Hz
	temp1=(temp1>>4)<<2;
	temp1=temp1+temp2;

	adconfig[4]+=temp1;
	
	adconfig[8]=0xFF;
	adconfig[9]=0x00;//0xFF;
	for(int i=0;i<adconfig_len;i++)
	{
		fpga_write(config_addr0+i,adconfig[i]);
	}
	fpga_write(config_addr0,1);
	
	
}
void decoder(char *cmd)
{
	char tempdata1=0;
	cpu2fpgadata[0]=cmd[0];
	int prob_id_offset=0;
	if(cmd[0]>=GET_F_PROB_ID)
	{
		prob_id_offset=cmd[0]-GET_F_PROB_ID;
		cpu2fpgadata[1]=probID[prob_id_offset*3];
		cpu2fpgadata[2]=probID[prob_id_offset*3+1];
		cpu2fpgadata[3]=probID[prob_id_offset*3+2];
		cpu2fpga(cpu2fpgadata);
		return;
	}
	switch (cmd[0])
	{
		case FPGAVERSION:
			getfpgaver(cmd);
		break;
		case CPUVERSION:
			getcpuver(cmd);
		break;
		case PCBVERSION:
			cpu2fpgadata[1]=0x10;
		cpu2fpgadata[1]=0x10;
		cpu2fpgadata[2]=0x10;
		cpu2fpga(cpu2fpgadata);	
		//getpcbver(cmd);
		break;
		case GETF210SN:
			getf210sn(cmd);
		break;
		case GETF543SN:
			getf543sn(cmd);
		break;
		case SETF210SN:
			
			setf210sn(cmd);
		break;
		case SET543SN:
			
		setf543sn(cmd);
		break;
		case GETFBATVOLT:
		break;
		case GETFBATSTAT:
		break;
		case SETFSAMPLE:
			set_sample_freq(cmd[1]);
		break;
		case GETFSAMPLE:
			get_sample_freq(cmd);
		break;
		case STARTFADC:

			if(cmd[1]==0x03)
				set_impd_test(cmd);
			else if(cmd[1]==0xff)
			{
				write_status();
				fpga_write(0x0f,2);
			}
			else
				fpga_write(0x0f,1);
		break;
		case STOPFADC:
			//adconfig[8]=0xFF;
		//adconfig[9]=0xFF;
//		for(int i=0;i<11;i++)
//	{
//		fpga_write(config_addr0+i,adconfig[i]);
//	}
//	fpga_write(config_addr0,1);
			adconfig[8]=0;
			adconfig[9]=0;
			adconfig[11]=0;
			fpga_write(0x0f,0);
		break;
		case SET_MAX_PRE_CH_NUM://max prob channel num
			set_max_f_num(cmd,1);
		break;
		case GET_MAX_PRE_CH_NUM:
				get_max_f_num(cmd);
			break;
		case SET_RRE_RES_TEST:
			break;
		case SET_NOW_PRE_CH_NUM://current prob channel num
			set_f_num(cmd);
			break;
		case GET_NOW_PRE_CH_NUM:
			get_f_num(cmd);
			break;
		case SET_F_DROP_THRES:
			set_f_threshold(cmd);
		break;
		case GETFDEVICEID1:
			cpu2fpgadata[1]=0x90;
			cpu2fpgadata[2]=0x10;
			cpu2fpgadata[3]=0x11;
			cpu2fpga(cpu2fpgadata);
		break;
		
		case GETFDEVICEID2:
		break;
		case GETFDEVICEID3:
		break;
		case GETFDEVICEID4:
		break;
		case GETFDEVICEID5:
		break;
		case GETFDEVICEID6:
		break;
		case SETFDEVICEID1:
		break;
		case SETFDEVICEID2:
		break;
		case SETFDEVICEID3:
		break;
		case SETFDEVICEID4:
		break;
		case SETFDEVICEID5:
		break;
		case SET_F_PROB_ID_TEST:
			memset(probID,0,66);
			for(int i=0;i<ONE_WIRE_PORT_NUM;i++)
		{
			memset(&one_wire_data[i][0],0,sizeof(one_wire_data_type));
			one_wire_device_num[i] = one_wire_family_search(i, &one_wire_data[i][0], DS2401_FAMILY_CODE, 0);
			
			for(int j=0;j<8;j++)
			{
				probID[i*8+j]=*(&(one_wire_data[i][0].rom_id.family_code)+j);
			}
		}
		probID[65]=0;
			for(int i=0;i<ONE_WIRE_PORT_NUM;i++)
		{
			probID[65]+=one_wire_device_num[i];
		}
		
		break;
		case GET_F_PROB_NUM:
			 get_front_prob_num();
		break;

		default:
		break;
	}
}