#include "e154.h"
// #include "font.h"

unsigned char GDOControl[4]={0x01,(yDot-1)%256,(yDot-1)/256,0x00}; //for 1.54inch
unsigned char softstart[4]={0x0c,0xd7,0xd6,0x9d};
unsigned char Rambypass[2] = {0x21,0x8f};		// Display update
unsigned char MAsequency[2] = {0x22,0xf0};		// clock 
unsigned char GDVol[2] = {0x03,0x66};	// Gate voltage +18V/-18V
unsigned char SDVol[2] = {0x04,0x04};	// Source voltage +12V/-12V
unsigned char VCOMVol[2] = {0x2c,0x7f};	// VCOM 7c
unsigned char BOOSTERFB[2] = {0xf0,0x1f};	// Source voltage +15V/-15V
unsigned char DummyLine[2] = {0x3a,0x1a};	// 4 dummy line per gate
unsigned char Gatetime[2] = {0x3b,0x08};	// 2us per line
unsigned char BorderWavefrom[2] = {0x3c,0x33};	// Border
unsigned char RamDataEntryMode[2] = {0x11,0x01};	// Ram data entry mode

const unsigned char LUTDefault_part[31] = 
{
 0x32,	// command
 0x10,
 0x18,0x18,0x28,0x18,0x18,0x18,0x18,0x08,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x13,0x11,0x22,0x63,0x11,
 0x00,0x00,0x00,0x00,0x00,
};

const unsigned char LUTDefault_full[31] =
{
 0x32,	  // command
 0x66,   //machine LUT
 0x66,0x44,0x66,0xAA,0x11,0x80,0x08,0x11,
 0x18,0x81,0x18,0x11,0x88,0x11,0x88,0x11,
 0x88,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x5F,
 0xAF,0xFF,0xFF,0x2F,0x00,
};

static void sw_delay_ms(uint16_t x) 
{  
	uint16_t i,j= 0;
	i = x;
	while(i>0)
	{
		for(j=0;j<8000;j++)
			nop();

		i--;
	} 
}	

static void SPI_Write(unsigned char value)                                    
{                                                           
    unsigned char i;

    for(i=0; i<8; i++)   
    {
		nop();nop();nop();nop();nop();
		nop();nop();nop();nop();nop();
		nop();nop();nop();nop();nop();
		nop();nop();nop();nop();nop();
		nop();nop();nop();nop();nop();
        E154_CLK_0;
		nop();nop();nop();nop();nop();
		nop();nop();nop();nop();nop();
        if(value & 0x80)
        	E154_MOSI_1;
        else
        	E154_MOSI_0;		
        value = (value << 1);
		nop();nop();nop();nop();nop();
		nop();nop();nop();nop();nop();
		nop();nop();nop();nop();nop();
		nop();nop();nop();nop();nop();
		nop();nop();nop();nop();nop(); 
        E154_CLK_1;  
    }
}

unsigned char ReadBusy(void)
{
    uint16_t i=0;
    for(i=0;i<40;i++)
    {
        if(E154_BUSY==0) {
        return 1;
        }
        sw_delay_ms(10);
    }
    return 0;
}

void EPD_W21_WriteCMD(unsigned char command)
{
    E154_CS_0;                   
	E154_DC_0;		// command write
	SPI_Write(command);
	E154_CS_1;
}

void EPD_W21_WriteCMD_p1(unsigned char command,unsigned char para)
{
	ReadBusy();	

    E154_CS_0;                   
	E154_DC_0;		// command write
	SPI_Write(command);
	E154_DC_1;		// command write
	SPI_Write(para);
	E154_CS_1;
}

void EPD_W21_Write(unsigned char *value, unsigned char datalen)
{
	unsigned char i = 0;
	unsigned char *ptemp;
	ptemp = value;
    E154_CS_0;                   	
	E154_DC_0;		// command write
	SPI_Write(*ptemp);
	ptemp++;
	E154_DC_1;		// data write
	for(i= 0;i<datalen-1;i++)	// sub the command
	{
		SPI_Write(*ptemp);
		ptemp++;
	}
	E154_CS_1;
}


void EPD_W21_WriteDispRam(unsigned char XSize,unsigned int YSize,
							unsigned char *Dispbuff)
{
    int i = 0,j = 0;

    if(XSize%8 != 0)
    {
        XSize = XSize+(8-XSize%8);
    }
    XSize = XSize/8;

    ReadBusy();		
    
    E154_CS_0;                   
    E154_DC_0;		//command write
    SPI_Write(0x24);
    
    E154_DC_1;		//data write
    for(i=0;i<YSize;i++)
    {
        for(j=0;j<XSize;j++)
        {
            SPI_Write(*Dispbuff);
            Dispbuff++;
        }
    }
    
    E154_CS_1;
}

void EPD_W21_WriteDispRamMono(unsigned char XSize,unsigned int YSize,
							unsigned char dispdata)
{
	int i = 0,j = 0;

	if(XSize%8 != 0)
	{
		XSize = XSize+(8-XSize%8);
	}
	XSize = XSize/8;
	ReadBusy();	

    E154_CS_0;                   
	E154_DC_0;		// command write
	SPI_Write(0x24);
	
	E154_DC_1;		// data write
	for(i=0;i<YSize;i++)
	{
		for(j=0;j<XSize;j++)
		{
		 SPI_Write(dispdata);
		}
	}
	
	E154_CS_1;
}

void EPD_W21_POWERON(void)
{
	EPD_W21_WriteCMD_p1(0x22,0xc0);
	EPD_W21_WriteCMD(0x20);
}

void EPD_W21_SetRamArea(unsigned char Xstart,unsigned char Xend, unsigned char Ystart, unsigned char Ystart1, unsigned char Yend,unsigned char Yend1)
{
    unsigned char RamAreaX[3];	// X start and end
	unsigned char RamAreaY[5]; 	// Y start and end
	RamAreaX[0] = 0x44;	// command
	RamAreaX[1] = Xstart;
	RamAreaX[2] = Xend;
	RamAreaY[0] = 0x45;	// command
	RamAreaY[1] = Ystart;
	RamAreaY[2] = Ystart1;
	RamAreaY[3] = Yend;
    RamAreaY[4] = Yend1;
	EPD_W21_Write(RamAreaX, sizeof(RamAreaX));
	EPD_W21_Write(RamAreaY, sizeof(RamAreaY));
}

void EPD_W21_SetRamPointer(unsigned char addrX,unsigned char addrY,unsigned char addrY1)
{
    unsigned char RamPointerX[2];	// default (0,0)
	unsigned char RamPointerY[3]; 	
	RamPointerX[0] = 0x4e;
	RamPointerX[1] = addrX;
	RamPointerY[0] = 0x4f;
	RamPointerY[1] = addrY;
	RamPointerY[2] = addrY1;
	
	EPD_W21_Write(RamPointerX, sizeof(RamPointerX));
	EPD_W21_Write(RamPointerY, sizeof(RamPointerY));
}

void part_display(unsigned char RAM_XST,unsigned char RAM_XEND,unsigned char RAM_YST,unsigned char RAM_YST1,unsigned char RAM_YEND,unsigned char RAM_YEND1)
{    
	EPD_W21_SetRamArea(RAM_XST,RAM_XEND,RAM_YST,RAM_YST1,RAM_YEND,RAM_YEND1);  	/*set w h*/
    EPD_W21_SetRamPointer (RAM_XST,RAM_YST,RAM_YST1);		    /*set orginal*/
}

void EPD_W21_DispInit(void)
{
	EPD_W21_Write(GDOControl, sizeof(GDOControl));	// Pannel configuration, Gate selection	
    EPD_W21_Write(softstart, sizeof(softstart));	// X decrease, Y decrease
	EPD_W21_Write(VCOMVol, sizeof(VCOMVol));		// VCOM setting
	EPD_W21_Write(DummyLine, sizeof(DummyLine));	// dummy line per gate
	EPD_W21_Write(Gatetime, sizeof(Gatetime));		// Gage time setting
	EPD_W21_Write(RamDataEntryMode, sizeof(RamDataEntryMode));	// X increase, Y decrease
	EPD_W21_SetRamArea(0x00,(xDot-1)/8,(yDot-1)%256,(yDot-1)/256,0x00,0x00);	// X-source area,Y-gage area
    EPD_W21_SetRamPointer(0x00,(yDot-1)%256,(yDot-1)/256);	// set ram

}

void EPD_W21_Init(void)
{
	E154_RST_0;		// Module reset
	sw_delay_ms(100);
	E154_RST_1;
	sw_delay_ms(100); 

	EPD_W21_DispInit();		// pannel configure
}

void EPD_W21_Update(void)
{
	EPD_W21_WriteCMD_p1(0x22,0xc7);
	EPD_W21_WriteCMD(0x20);
	EPD_W21_WriteCMD(0xff);
}

void EPD_W21_Update_Part(void)
{
	EPD_W21_WriteCMD_p1(0x22,0x04);
	EPD_W21_WriteCMD(0x20);
	EPD_W21_WriteCMD(0xff);
}

void EPD_W21_WirteLUT(unsigned char *LUTvalue,unsigned char Size)
{	
	EPD_W21_Write(LUTvalue, Size);
}

void EPD_init_Full(void)
{		
	EPD_W21_Init();			// display
    EPD_W21_WirteLUT((unsigned char *)LUTDefault_full,sizeof(LUTDefault_full));
	EPD_W21_POWERON();
	sw_delay_ms(300);		
}

void EPD_init_Part(void)
{		
	EPD_W21_Init();			// display
	EPD_W21_WirteLUT((unsigned char *)LUTDefault_part,sizeof(LUTDefault_part));
	EPD_W21_POWERON();
	sw_delay_ms(300);
}

void EPD_Dis_Full(unsigned char *DisBuffer,unsigned char Label)
{
    EPD_W21_SetRamPointer(0x00,(yDot-1)%256,(yDot-1)/256);	// set ram
	if(Label==0)
	{
		EPD_W21_WriteDispRamMono(xDot, yDot, 0xff);	// white	
	}
	else
	{
		EPD_W21_WriteDispRam(xDot, yDot, (unsigned char *)DisBuffer);	// white
	}	
	EPD_W21_Update();	
	
}

void EPD_Dis_Part(unsigned char xStart,unsigned char xEnd,unsigned long yStart,unsigned long yEnd,unsigned char *DisBuffer,unsigned char Label)
{
	if(Label==0)
	{
		part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);
		EPD_W21_WriteDispRamMono(xEnd-xStart, yEnd-yStart+1, DisBuffer[0]);	// black
 		EPD_W21_Update_Part();
		sw_delay_ms(200);
		part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);	
		EPD_W21_WriteDispRamMono(xEnd-xStart, yEnd-yStart+1, DisBuffer[0]);	// black
	}
	else if(Label==1)
	{
		part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);	// set ram	
		EPD_W21_WriteDispRam(xEnd-xStart, yEnd-yStart+1,DisBuffer);
		EPD_W21_Update_Part();
		sw_delay_ms(300);
		part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);
		EPD_W21_WriteDispRam(xEnd-xStart, yEnd-yStart+1,DisBuffer);
	}
	else
	{
		part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);	// set ram	
		EPD_W21_WriteDispRam(xEnd-xStart, yEnd-yStart+1,DisBuffer);
		EPD_W21_Update_Part();
		sw_delay_ms(300);
		part_display(xStart/8,xEnd/8,yEnd%256,yEnd/256,yStart%256,yStart/256);
		EPD_W21_WriteDispRam(xEnd-xStart, yEnd-yStart+1,DisBuffer);
	}
}
