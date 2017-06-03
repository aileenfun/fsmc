#include "../include/fpga2cpu_cmd.h"
#include "../driver/1-wire_driver/common/one_wire_hal.h"
#include "../include/bsp_internalFlash.h"
#include "../include/fsmc.h"
char adconfig[]={0x00,0x96,0xD0,0xE8,0x00,
                 0x50,0x00,0x00,0x00,0x00,0x00};
char verinfobuff[24]={0};
int config_addr0=0x1f;
char cpu2fpgadata[4]={0};
char cdbuffer[4]={0};
//todo: update from flash on powerup
int pre_chn_cfg=0x26;
void cpu2fpga(char*data)
{
	//fpga_write(0x04,data[0]);
	fpga_write(0x04,cpu2fpgadata[0]);
	fpga_write(0x05,data[1]);
	fpga_write(0x06,data[2]);
	fpga_write(0x07,data[3]);
	fpga_write(0x08,1);
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
	for(i=0;i<11;i++)
	{
		fpga_write(config_addr0+i,adconfig[i]);
	}
	fpga_write(config_addr0,1);

}
void get_sample_freq(char*cmd)
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
}
void set_f_num(char*cmd,int burn)
{
	pre_chn_cfg=cmd[1];
	readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,9,0);
	int pre_max_num=verinfobuff[8];
	if(pre_chn_cfg>pre_max_num)
	{//cmd==0x27>0x26 max allowed
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
	if(burn)
	{
		readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,9,0);
		verinfobuff[8]=cmd[1];
		writeflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,9,0);
		readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,9,0);
	}
	
}
void get_f_num(char*cmd)
{
	readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,9,0);
	pre_chn_cfg=verinfobuff[8];
	cpu2fpgadata[1]=pre_chn_cfg;
	cpu2fpga(cpu2fpgadata);
}

void getverinfo( VERINFO vi)
{
	if(vi<SN210)
	readflash(FPGA_VER_SECTOR,FPGA_VER_SECTOR_END,verinfobuff,4,vi*4);
	else
		readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,4,(vi-SN210)*4);
	
	cpu2fpgadata[1]=*(verinfobuff+(vi-SN210)*4+1);
	cpu2fpgadata[2]=*(verinfobuff+(vi-SN210)*4+2);
	cpu2fpgadata[3]=*(verinfobuff+(vi-SN210)*4+3);
	cpu2fpga(cpu2fpgadata);
}
void setverinfo(char *cmd,VERINFO vi,int burn)
{
	if(vi==SN210)
	{
		for(int i=0;i<4;i++)
		cdbuffer[i]=cmd[i];
		return;
	}
	
	readflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,9,0);
	for(int i=0;i<4;i++)
	{
		*(verinfobuff+4+i)=*(cmd+i);
		*(verinfobuff+i)=*(cdbuffer+i);
	}
	
	if(burn)
		writeflash(F_AMP_SN_SECTOR,F_AMP_SN_SECTOR_END,verinfobuff,9,0);
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

void decoder(char *cmd)
{
	char tempdata1=0;
	cpu2fpgadata[0]=cmd[0];
	switch (cmd[0])
	{
		case FPGAVERSION:
			getfpgaver(cmd);
		break;
		case CPUVERSION:
			getcpuver(cmd);
		break;
		case PCBVERSION:
			getpcbver(cmd);
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
			fpga_write(0x0f,1);
		break;
		case STOPFADC:
			fpga_write(0x0f,0);
		break;
		case SET_PRE_CH_NUM:
			set_f_num(cmd,1);
		break;
		case GET_PRE_CH_NUM:
				get_f_num(cmd);
			break;
		case SET_RRE_RES_TEST:
			break;
		case SET_PRECFG:
			set_f_num(cmd,0);
			break;
		case GET_PRECFG:
			get_f_num(cmd);
			break;
		case SETFDROPTHRES:
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
		case SETFPROBIDTEST:
		break;
		case GETFPROBNUM:
		//	one_wire_device_num[0] = one_wire_family_search(0, &one_wire_data[0][0], DS2401_FAMILY_CODE, 0);
		
		break;
		case GETFPROBID:
		//	one_wire_device_num[0] = one_wire_family_search(0, &one_wire_data[0][0], DS2401_FAMILY_CODE, 0);
		
		break;
		case GETFPROBID+1:
		//	one_wire_device_num[0] = one_wire_family_search(0, &one_wire_data[0][0], DS2401_FAMILY_CODE, 0);
			break;
		default:
		break;
	}
}