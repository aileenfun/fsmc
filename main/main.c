/*
 * --------------------
 * Company							: LUOYANG GINGKO TECHNOLOGY CO.,LTD.
 : 洛阳银杏科技有限公司
 * --------------------
 * Project Name					: iCore3
 * Description					: iCore3_fsmc
 * --------------------
 * Tool Versions				: uVision V5.17.0.0
 * Target Device				: STM32F407IGT6
 * --------------------
 * Engineer							: ysloveivy.
 * Revision							: 1.0
 * Created Date					: 2016.01.03
 * --------------------
 * Engineer							:
 * Revision							:
 * Modified Date				:
 * --------------------
 * Additional Comments	:
 *
 * --------------------
 */
//-----------------------include--------------------------//
#include "..\fwlib\inc\stm32f4xx_gpio.h"

#include "..\include\led.h"
#include "..\include\main.h"
#include "..\include\fsmc.h"
#include "..\include\timer3.h"
#include "..\include\systick.h"
#include "..\include\exti.h"
#include "..\include\bsp_internalFlash.h"
#include "..\include\adc.h"
#include "..\driver\1-wire_driver\common\one_wire_hal.h"
#include "..\driver\1-wire_driver\common\one_wire_net.h"
#include "..\include\fpga2cpu_cmd.h"
//------------------------define--------------------------//
#define Thres_v3_3 2.8
#define Thres_vbat 10

//-----------------------variable-------------------------//

//------------------Function Prototype--------------------//

//-----------------------Function-------------------------//
/*
 * Name					: main
 * Description	: ---
 * Author				: ysloveivy.
 *
 * History
 * --------------------
 * Rev					: 0.00
 * Date					: 01/03/2016
 * 
 * create.
 * --------------------
 */
 #define AD_CMD_START 0x08
 #define AD_CMD_STOP 0x0a
 #define AD_HW_POWER 0x0C
 #define AD_HW_START 0x0a
 #define AD_HW_RESET 0x0b
 
 #define RDATAC 0x10
 #define SDATAC 0x11
 #define F_CS 0x0d
 #define F_DATA 0x12
 #define F_RDCMD 0x13
 #define F_RDDATA 0x14
 #define F_ARMCTRL 0x0f //0 arm ctrl, 1 fpga ctrl
 
 #define delaytime 99999
 void delay(uint32_t t)
 {
	 while(t--);
 }
 void delayms(uint32_t t)
 {
	 systick.ms_flag=0;
	 while(!systick.ms_flag);
 }
 void adstart()
 {
	 //fpga_write(0x0f,0);
	fpga_write(F_CS,1);//cs
	fpga_write(F_CS,0);//cs
	fpga_write(F_DATA,AD_CMD_START);
	delay(delaytime);
	fpga_write(F_CS,0);//cs
	fpga_write(0x0f,1);
 }
 void spiwrite(char addr, char*data,int len)//page 43
 {
	 char temp=0x40;
	 fpga_write(F_ARMCTRL,0);//take spi control from fpga
	 fpga_write(F_CS,1);
	 delay(delaytime);
	 fpga_write(F_CS,0);//cs
	 delay(delaytime);
	 fpga_write(F_DATA,SDATAC);//stop continous read
	 delay(delaytime);
	 fpga_write(F_CS,1);//cs
	 delay(delaytime);

	fpga_write(F_CS,0);
	 temp=temp|addr;
	fpga_write(F_DATA,temp);
	delay(delaytime);
	fpga_write(F_DATA,len-1);
	delay(delaytime);
	 for(int i=0;i<len;i++)
	 {
		fpga_write(F_DATA,data[i]);
		 delay(delaytime);
	 }
	
	fpga_write(F_CS,1);//cs
	 delay(delaytime);
	 fpga_write(F_CS,0);
	 fpga_write(F_DATA,RDATAC);//start continous read
	 delay(delaytime);
	 //fpga_write(F_DATA,AD_CMD_START);
	 fpga_write(F_CS,1);
 }
 char spiread(char addr)//page43
 {
	 char data;
	 char temp=0;
	 fpga_write(F_ARMCTRL,0);//take spi control from fpga
	 delay(delaytime);
	 fpga_write(F_CS,1);
	 delay(delaytime);
	 fpga_write(F_CS,0);//cs
	 delay(delaytime);
	 fpga_write(F_DATA,SDATAC);//stop continous read
	 delay(delaytime);
	 fpga_write(F_CS,1);//cs
	 delay(delaytime);
	 fpga_write(F_CS,0);//cs
	 temp=0x20|addr;
	 fpga_write(F_DATA,temp);
	 delay(delaytime);
	 fpga_write(F_DATA,0);//length
	 delay(delaytime);
	 fpga_write(F_RDCMD,1);//read command
	 delay(delaytime);
	 data=fpga_read(F_RDDATA);
	 delay(delaytime);
	 fpga_write(F_CS,1);
	 delay(delaytime);
	 fpga_write(F_CS,0);
	 //fpga_write(F_ARMCTRL,1);//release spi control
	 fpga_write(F_DATA,RDATAC);//start continous read
	 delay(delaytime);
	 //fpga_write(F_DATA,AD_CMD_START);
	 fpga_write(F_CS,1);
	 return data;
 }
 void task_pwtest()
 {
	 int i=0;
	 for(i = 0;i < 2;i++){
			adc.read(i);
		}
		float VCC3_3=adc.value[0]*paramv3_3;
		float VBAT=adc.value[1]*paramvbat;
		if(VCC3_3<Thres_v3_3)
		{
				
		}
		if(VBAT<Thres_vbat)
		{
			PW_LED1_OFF;
			PW_LED2_ON;
		}
		else
		{
			PW_LED1_ON;
			PW_LED2_OFF;
		}
		if(BATCHARGE_INPUT)
		{
			if(GET_PW_LED2_STATUS)
			{
				PW_LED2_OFF;
			}
			else
			{
				PW_LED2_ON;
			}
		}
 }
 void ad_powerup()
 {
	// fpga_write(AD_HW_START,0);
	 fpga_write(AD_HW_POWER,0);
	 delayms(200);
	 fpga_write(AD_HW_POWER,1);
	 fpga_write(F_CS,1);
	 fpga_write(AD_HW_START,1);
	 fpga_write(AD_HW_RESET,0);
	 delayms(20);
	 fpga_write(AD_HW_START,0);
	 delayms(10);
	 fpga_write(AD_HW_RESET,1);
	 delayms(5);
	 fpga_write(AD_HW_RESET,1);
	 delayms(1);
	 fpga_write(F_CS,0);
	 delayms(10);
	 //fpga_write(AD_HW_START,1);
 }
int main(void)
{
	int i;
	char data=0;
	unsigned short int fsmc_read_data;
	/*初始化*/
	led.initialize();
	fsmc.initialize();
	timer3.initialize();
	exti.initialize();
	adc.initialize();
	systick.initialize();

		fpga_write(F_ARMCTRL,0);//get spi control

	//InternalFlash_Test();//good
	//char flashbuff[20]={0};
	
	//readflash(flashbuff,20);
	char adconfig[]={0x95,0xD0,0xE8,0x00,
		0x50,0x50,0x50,0x50,
		0x50,0x50,0x50,0x50,
		0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00};
	
	int len=sizeof(adconfig);

//	for(int i=0;i<8;i++)
//	{
//		spiwrite(0x01,adconfig,len);
//	}
	for(int i=0;i<len;i++)
	{
		spiwrite(0x01+i,adconfig+i,1);
	}
	ad_powerup();	
		for(int i=0;i<len;i++)
	{
		spiwrite(0x01+i,adconfig+i,1);
	}
	for(int i=0;i<10;i++)
	{
		char five=0x50;
		spiwrite(0x09,&five,1);
	}

	char temp1=spiread(0x00);

	fpga_write(AD_HW_START,1);
//	adstart();
	fpga_write(F_ARMCTRL,0);//release spi control
	
//$$$$$$$$serial flash test$$$$$$
	//int r=0;
//	r=write_FPGAPCB_VER();
//	getverinfo(FpgaInfo);
//	getverinfo(PcbInfo);
//	getverinfo(CpuInfo);
//	
//char cmd[4]={0x10,0xef,0xdf,0xcf};
//	setf543sn(cmd);
//	cmd[0]=0x00;
//	cmd[1]=0x10;
//	cmd[2]=0x20;
//	cmd[3]=0x30;
//	setf210sn(cmd);
//	getf543sn(cmd);
//	getf210sn(cmd);

////$$$$$$one wire driver test$$$$$$$
one_wire_device_num[0] = one_wire_family_search(0, &one_wire_data[0][0], DS2401_FAMILY_CODE, 0);

	while(1)
	{
		/*
		one_wire_device_num[0] = one_wire_family_search(0, &one_wire_data[0][0], DS2401_FAMILY_CODE, 0);
		one_wire_device_num[1] = one_wire_family_search(1, &one_wire_data[1][0], DS2401_FAMILY_CODE, 0);
		one_wire_device_num[2] = one_wire_family_search(2, &one_wire_data[2][0], DS2401_FAMILY_CODE, 0);
		one_wire_device_num[3] = one_wire_family_search(3, &one_wire_data[3][0], DS2401_FAMILY_CODE, 0);
		*/
		//readflash(verinfo,15,0);
		delay_ms(2000);
		task_pwtest();
		if(systick.second_flag)
		{
			systick.second_flag=0;
			
		}
		
	}

}
