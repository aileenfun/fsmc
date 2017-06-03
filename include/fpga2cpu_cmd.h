#ifndef _FPGA2CPU_CMD
#define _FPGA2CPU_CMD


typedef enum FPGA2CPUCMD
{	
	FPGAVERSION=0x07,
	CPUVERSION=0x08,
	PCBVERSION=0x09,
	GETF210SN=0x0a,
	GETF543SN=0x0b,
	SETF210SN=0x0c,
	SET543SN=0x0d,
	GETFBATVOLT=0x0e,
	GETFBATSTAT=0x0f,
	SETFSAMPLE=0x10,
	GETFSAMPLE=0x14,
	STARTFADC=0x18,
	STOPFADC=0x19,
	SET_PRE_CH_NUM=0x1a,
	GET_PRE_CH_NUM=0x1b,
	SET_RRE_RES_TEST=0x1c,
	SET_PRECFG=0x1d,
	GET_PRECFG=0x15,
	SETFDROPTHRES=0x30,
	GETFDEVICEID1=0x31,
	GETFDEVICEID2=0x32,
	GETFDEVICEID3=0x33,
	GETFDEVICEID4=0x34,
	GETFDEVICEID5=0x35,
	GETFDEVICEID6=0x36,
	SETFDEVICEID1=0x37,
	SETFDEVICEID2=0x38,
	SETFDEVICEID3=0x39,
	SETFDEVICEID4=0x3a,
	SETFDEVICEID5=0x3b,
	SETFPROBIDTEST=0x40,
	GETFPROBNUM=0x41,
	GETFPROBID=0x42
}FPGA2CPUCMD;
typedef enum {FpgaInfo,CpuInfo,PcbInfo,SN210,SN543}VERINFO;
void cpu2fpga(char*data);
void set_sample_freq(int val);
void get_sample_freq(char*cmd);
void set_f_num(char*cmd,int burn);
void get_f_num(char*cmd);
void getverinfo( VERINFO vi);
void getfpgaver(char *cmd);
void getcpuver(char *cmd);
void getpcbver(char *cmd);
void getf210sn(char *cmd);
void getf543sn(char *cmd);
void setverinfo(char *cmd,VERINFO vi,int burn);
void setf543sn(char*cmd);
void setf210sn(char*cmd);
void decoder(char *cmd);
#endif