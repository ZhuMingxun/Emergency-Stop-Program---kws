#include "ambe.h"

#define AMBE_RST_SET()  GPIO_SET(AMBE_RST_PORT,AMBE_RST_PINS)
#define AMBE_RST_CLR()  GPIO_CLR(AMBE_RST_PORT,AMBE_RST_PINS)
#define AMBE_RST_OUT()  GPIO_OUT(AMBE_RST_PORT,AMBE_RST_PINS)

#define AMBE_PWR_SET()  GPIO_SET(AMBE_PWR_PORT,AMBE_PWR_PINS)
#define AMBE_PWR_CLR()  GPIO_CLR(AMBE_PWR_PORT,AMBE_PWR_PINS)
#define AMBE_PWR_OUT()  GPIO_OUT(AMBE_PWR_PORT,AMBE_PWR_PINS)

#define AMBE_PRD_SET()  GPIO_SET(AMBE_PRD_PORT,AMBE_PRD_PINS)
#define AMBE_PRD_CLR()  GPIO_CLR(AMBE_PRD_PORT,AMBE_PRD_PINS)
#define AMBE_PRD_OUT()  GPIO_OUT(AMBE_PRD_PORT,AMBE_PRD_PINS)

#define AMBE_PD0_SET()  GPIO_SET(AMBE_PD0_PORT,AMBE_PD0_PINS)
#define AMBE_PD0_CLR()  GPIO_CLR(AMBE_PD0_PORT,AMBE_PD0_PINS)
#define AMBE_PD0_GET()  GPIO_GET(AMBE_PD0_PORT,AMBE_PD0_PINS)
#define AMBE_PD0_OUT()  GPIO_OUT(AMBE_PD0_PORT,AMBE_PD0_PINS)
#define AMBE_PD0_IN()   GPIO_IN(AMBE_PD0_PORT,AMBE_PD0_PINS)

#define AMBE_PD1_SET()  GPIO_SET(AMBE_PD1_PORT,AMBE_PD1_PINS)
#define AMBE_PD1_CLR()  GPIO_CLR(AMBE_PD1_PORT,AMBE_PD1_PINS)
#define AMBE_PD1_GET()  GPIO_GET(AMBE_PD1_PORT,AMBE_PD1_PINS)
#define AMBE_PD1_OUT()  GPIO_OUT(AMBE_PD1_PORT,AMBE_PD1_PINS)
#define AMBE_PD1_IN()   GPIO_IN(AMBE_PD1_PORT,AMBE_PD1_PINS)

#define AMBE_PD2_SET()  GPIO_SET(AMBE_PD2_PORT,AMBE_PD2_PINS)
#define AMBE_PD2_CLR()  GPIO_CLR(AMBE_PD2_PORT,AMBE_PD2_PINS)
#define AMBE_PD2_GET()  GPIO_GET(AMBE_PD2_PORT,AMBE_PD2_PINS)
#define AMBE_PD2_OUT()  GPIO_OUT(AMBE_PD2_PORT,AMBE_PD2_PINS)
#define AMBE_PD2_IN()   GPIO_IN(AMBE_PD2_PORT,AMBE_PD2_PINS)

#define AMBE_PD3_SET()  GPIO_SET(AMBE_PD3_PORT,AMBE_PD3_PINS)
#define AMBE_PD3_CLR()  GPIO_CLR(AMBE_PD3_PORT,AMBE_PD3_PINS)
#define AMBE_PD3_GET()  GPIO_GET(AMBE_PD3_PORT,AMBE_PD3_PINS)
#define AMBE_PD3_OUT()  GPIO_OUT(AMBE_PD3_PORT,AMBE_PD3_PINS)
#define AMBE_PD3_IN()   GPIO_IN(AMBE_PD3_PORT,AMBE_PD3_PINS)

#define AMBE_PD4_SET()  GPIO_SET(AMBE_PD4_PORT,AMBE_PD4_PINS)
#define AMBE_PD4_CLR()  GPIO_CLR(AMBE_PD4_PORT,AMBE_PD4_PINS)
#define AMBE_PD4_GET()  GPIO_GET(AMBE_PD4_PORT,AMBE_PD4_PINS)
#define AMBE_PD4_OUT()  GPIO_OUT(AMBE_PD4_PORT,AMBE_PD4_PINS)
#define AMBE_PD4_IN()   GPIO_IN(AMBE_PD4_PORT,AMBE_PD4_PINS)

#define AMBE_PD5_SET()  GPIO_SET(AMBE_PD5_PORT,AMBE_PD5_PINS)
#define AMBE_PD5_CLR()  GPIO_CLR(AMBE_PD5_PORT,AMBE_PD5_PINS)
#define AMBE_PD5_GET()  GPIO_GET(AMBE_PD5_PORT,AMBE_PD5_PINS)
#define AMBE_PD5_OUT()  GPIO_OUT(AMBE_PD5_PORT,AMBE_PD5_PINS)
#define AMBE_PD5_IN()   GPIO_IN(AMBE_PD5_PORT,AMBE_PD5_PINS)

#define AMBE_PD6_SET()  GPIO_SET(AMBE_PD6_PORT,AMBE_PD6_PINS)
#define AMBE_PD6_CLR()  GPIO_CLR(AMBE_PD6_PORT,AMBE_PD6_PINS)
#define AMBE_PD6_GET()  GPIO_GET(AMBE_PD6_PORT,AMBE_PD6_PINS)
#define AMBE_PD6_OUT()  GPIO_OUT(AMBE_PD6_PORT,AMBE_PD6_PINS)
#define AMBE_PD6_IN()   GPIO_IN(AMBE_PD6_PORT,AMBE_PD6_PINS)

#define AMBE_PD7_SET()  GPIO_SET(AMBE_PD7_PORT,AMBE_PD7_PINS)
#define AMBE_PD7_CLR()  GPIO_CLR(AMBE_PD7_PORT,AMBE_PD7_PINS)
#define AMBE_PD7_GET()  GPIO_GET(AMBE_PD7_PORT,AMBE_PD7_PINS)
#define AMBE_PD7_OUT()  GPIO_OUT(AMBE_PD7_PORT,AMBE_PD7_PINS)
#define AMBE_PD7_IN()   GPIO_IN(AMBE_PD7_PORT,AMBE_PD7_PINS)

#define AMBE_DPE_GET()  GPIO_GET(AMBE_DPE_PORT,AMBE_DPE_PINS)
#define AMBE_DPE_IN()   GPIO_IN(AMBE_DPE_PORT,AMBE_DPE_PINS)

#define AMBE_EPR_GET()  GPIO_GET(AMBE_EPR_PORT,AMBE_EPR_PINS)
#define AMBE_EPR_IN()   GPIO_IN(AMBE_EPR_PORT,AMBE_EPR_PINS)

char FrameRdData[34];
char FrameWrData[34];

uint16_t waitcnt=0;

const char SINE_DATA[34]={
0x13,0xec,0x06,0x00,0x64,0xB0,0xD8,
0x00,0x20,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};
const char SINE_DATA1[6]={
0x13,0xec,0x06,0x64,0xB0,0xD8};

const char MUTE_DATA[34]={
0x13,0xec,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xA0,0x44,0xD4,0x16,
0x88,0x4B,0xA0,0x22,0xEA,0x02,0x2A,
0x85,0x41,0x82,0x21,0x8A,0x67,0x87,
0x23,0x0E,0x63,0xA8,0x82,0x8A};

const char GAIN_DATA[34]={
0x13,0xec,0x02,0x00,0x08,0x80,0x01,
0xF4,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};

const char SLEEP_DATA[34]={
0x13,0xec,0x04,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};

const char WAKE_DATA[34]={
0x13,0xec,0xfe,0x20,0x00,0x00,0x00,
0x00,0x00,0x00,0xd0,0x00,0xd0,0x00,
0xd0,0x00,0xd0,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00};

void  AMBEReset(void);
void  AMBESetMute(void);
void  AMBESetGain(void);

void Delay(void)
{
	char i;
	for(i=0;i<20;i++)
	{
		__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();
		__NOP();__NOP();__NOP();__NOP();
	}
}

void PinInit( void )
{
 	//GPIO_CON();
  AMBE_RST_IOCON();
	AMBE_RST_OUT();
 	AMBE_PWR_IOCON();
	AMBE_PWR_OUT();
 	AMBE_PRD_IOCON();
	AMBE_PRD_OUT();
 	AMBE_DPE_IOCON();
	AMBE_DPE_IN();
 	AMBE_EPR_IOCON();
	AMBE_EPR_IN();
 	AMBE_PD0_IOCON();
	AMBE_PD0_OUT();
 	AMBE_PD1_IOCON();
	AMBE_PD1_OUT();
 	AMBE_PD2_IOCON();
	AMBE_PD2_OUT();
 	AMBE_PD3_IOCON();
	AMBE_PD3_OUT();
 	AMBE_PD4_IOCON();
	AMBE_PD4_OUT();
	AMBE_PD5_IOCON();
	AMBE_PD5_OUT();
	AMBE_PD6_IOCON();
	AMBE_PD6_OUT();
 	AMBE_PD7_IOCON();
	AMBE_PD7_OUT();
}

void ParallelSet(uint8_t val)
{
	AMBE_PD0_OUT();AMBE_PD1_OUT();
	AMBE_PD2_OUT();AMBE_PD3_OUT();
	AMBE_PD4_OUT();AMBE_PD5_OUT();
	AMBE_PD6_OUT();AMBE_PD7_OUT();
	if(val&0x01){AMBE_PD0_SET();}else{AMBE_PD0_CLR();}
	if(val&0x02){AMBE_PD1_SET();}else{AMBE_PD1_CLR();}
	if(val&0x04){AMBE_PD2_SET();}else{AMBE_PD2_CLR();}
	if(val&0x08){AMBE_PD3_SET();}else{AMBE_PD3_CLR();}
	if(val&0x10){AMBE_PD4_SET();}else{AMBE_PD4_CLR();}
	if(val&0x20){AMBE_PD5_SET();}else{AMBE_PD5_CLR();}
	if(val&0x40){AMBE_PD6_SET();}else{AMBE_PD6_CLR();}
	if(val&0x80){AMBE_PD7_SET();}else{AMBE_PD7_CLR();}
}

uint8_t ParalleGet(void)
{
	uint8_t val=0;
	AMBE_PD0_IN();AMBE_PD1_IN();
	AMBE_PD2_IN();AMBE_PD3_IN();
	AMBE_PD4_IN();AMBE_PD5_IN();
	AMBE_PD6_IN();AMBE_PD7_IN();
	if(AMBE_PD0_GET()){val|=0x01;}
	if(AMBE_PD1_GET()){val|=0x02;}
	if(AMBE_PD2_GET()){val|=0x04;}
	if(AMBE_PD3_GET()){val|=0x08;}
	if(AMBE_PD4_GET()){val|=0x10;}
	if(AMBE_PD5_GET()){val|=0x20;}
	if(AMBE_PD6_GET()){val|=0x40;}
	if(AMBE_PD7_GET()){val|=0x80;}
	return(val);
}

void AMBEReset(void)
{
	uint16_t i;
	AMBE_RST_SET();
	for(i=0;i<200;i++){Delay();}
	AMBE_RST_CLR();
	for(i=0;i<3000;i++){Delay();}
	AMBE_RST_SET();
}

void AMBEInit(void)
{
	uint8_t i;
	PinInit();
	AMBEReset();
	AMBESetGain();
	FrameWrData[0]=0x13;
	FrameWrData[1]=0xEC;
	for(i=0;i<32;i++)
	{
		FrameWrData[i+2]=0x00;
	}
}

char*AMBERdData(void)
{
	uint8_t i;
	for(i=0;i<34;i++)
	{
		AMBE_PRD_CLR();
		FrameRdData[i]=ParalleGet();
		Delay();
		AMBE_PRD_SET();
		Delay();
	}
	return(&FrameRdData[10]);
}

void AMBEWrData(char*pdata)
{
	uint8_t i;

	while (!AMBEGetWrStatus())
	{
		if(waitcnt++ > 65530)
			return;
	}
	
	
	for(i=0;i<6;i++)
	{
		FrameWrData[i+10]=*pdata++;
	}
	for(i=0;i<34;i++)
	{
		ParallelSet(FrameWrData[i]);
		AMBE_PWR_CLR();
		Delay();
		AMBE_PWR_SET();
		Delay();
	}
}

char AMBEGetWrStatus(void)
{
	return(AMBE_DPE_GET());
}

char AMBEGetRdStatus(void)
{
	return(AMBE_EPR_GET());
}

void AMBESetSine(void)
{
  uint8_t i;
	
	while (!AMBEGetWrStatus())
	{
		if(waitcnt++ > 65530)
			return;
	}
	
  for(i=0;i<34;i++)
  {
    ParallelSet(SINE_DATA[i]);
    AMBE_PWR_CLR();
		Delay();
    AMBE_PWR_SET();
		Delay();
  }
//	char dat[6];
//	uint8_t tmpi;
//	for (tmpi=0;tmpi<6;tmpi++)
//		dat[tmpi]=SINE_DATA1[tmpi];
//	
//	AMBEWrData(dat);
}	

void AMBESetMute(void)
{
  uint8_t i;
  for(i=0;i<34;i++)
  {
    ParallelSet(MUTE_DATA[i]);
    AMBE_PWR_CLR();
		Delay();
    AMBE_PWR_SET();
		Delay();
  }
}

void AMBESetGain(void)
{
  uint8_t i;
	uint32_t timeout;
	timeout=0xfffffff;
	while(!AMBEGetWrStatus())
	{
		if (timeout==0)
		{
			timeout=0x50000;
			AMBEReset();
            break;
		}
		timeout--;
	}
  for(i=0;i<34;i++)
  {
    ParallelSet(GAIN_DATA[i]);
    AMBE_PWR_CLR();
		Delay();
    AMBE_PWR_SET();
		Delay();
  }
}


void AMBESetSleep(void)
{
  uint8_t i;
  for(i=0;i<34;i++)
  {
    ParallelSet(SLEEP_DATA[i]);
    AMBE_PWR_CLR();
		Delay();
    AMBE_PWR_SET();
		Delay();
  }
}

void AMBESetWake(void)
{
  uint8_t i;
  for(i=0;i<34;i++)
  {
    ParallelSet(WAKE_DATA[i]);
    AMBE_PWR_CLR();
		Delay();
    AMBE_PWR_SET();
		Delay();
  }
}

