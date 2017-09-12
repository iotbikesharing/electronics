//-------------------------------------------------------------------------------------------------------------
//This part provide Project Description, key functions, and other project-related information
//Project     : Lab 3 - Interrupt - Keypad
//Author      : Uong Kim Thuan - Vo Minh Hoang - Nguyen Minh
//Date        : 
//Board       : Nu-LB-NUC140
//MCU         : NUC140VE3CN
//Functions   :
//
//
//-------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Font8x16.h"
#include "string.h"

#define LCD_Xmax 128 // Maximum X number pixel
#define LCD_Ymax 64	 // Maximum X number pixel

// Global variable declaration
char DisplayBuffer[128*8]; // Display pixel


uint8_t  Timer_flag =0; // Flag to update display
uint32_t Timer_count =0; // Variable to use for timing

/* Functions declaration */

void SYS_config(void);
void SPI_Init(void);
void LCD_command(unsigned char temp);
void LCD_data(unsigned char temp);
void init_LCD(void);
void clear_LCD(void);
void TMR2_IRQHandler(void);
void Init_TimerCapture(void);
void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr);
void printC(int16_t x, int16_t y, unsigned char ascii_code);
void print_Line(int8_t line, char text[]);


int main(void)
{
    float distance;
    char Text[16];
	
	/* System Configuration */
    SYS_config();
		SPI_Init();
    init_LCD();
    clear_LCD();
	
	/* Pin configuration */
		PC->PMD |= (0x55<<24);
		PB->PMD |= (0x01<<8);
		PA->PMD |= (0x15<<24);
	
	/* Simple Introduction on LCD */
		print_Line(0, "Team SASER");
		print_Line(1, "Ultrasound SR04");
			
    PB->DOUT &= ~(1<<4);
    	
    Init_TimerCapture();	
	  
    while(1) {
			uint8_t i;	
      PB->DOUT |=   (1<<4);
		  CLK_SysTickDelay(10);
      PB->DOUT &=  ~(1<<4);			
      TIMER2->TEXCON |= (0x01<<4);    // set RSTCAPSEL to 1 to reset counter
			TIMER2->TCSR |= (1<<30);
			CLK_SysTickDelay(30000);
      TIMER2->TCSR &= ~(1<<30);
			
      if (Timer_flag==1) {
          distance = Timer_count * 340 /20000;
          sprintf(Text,"T=%5d", Timer_count);
          print_Line(2,Text);				
					sprintf(Text,"D=%5d cm", (int) distance);	
          print_Line(3,Text);
				if(distance>30){
					for(i=0;i<2;i++){
					PA->DOUT ^= (0x07<<12);
					PC->DOUT ^= (1<<15);
					CLK_SysTickDelay(20000);
					}
				}
				if(distance>20 && distance<=30){
				for(i=0;i<2;i++){
					PA->DOUT ^= (0x06<<12);
					PC->DOUT ^= (1<<14);
					CLK_SysTickDelay(40000);
					}
				}
				if(distance>10 && distance<=20){
				for(i=0;i<2;i++){
					PA->DOUT ^= (0x05<<12);
					PC->DOUT ^= (1<<13);
					CLK_SysTickDelay(60000);
					}
				}
				if(distance>2 && distance<=10){
				for(i=0;i<2;i++){
					PA->DOUT ^= (0x03<<12);
					PC->DOUT ^= (1<<12);
					CLK_SysTickDelay(80000);
					}
				}
          Timer_flag=0;
      }
    }
}

void printC(int16_t x, int16_t y, unsigned char ascii_code)
{
  int8_t i;
  unsigned char temp;	    
  for(i=0;i<8;i++) {
	LCD_SetAddress((y/8),(LCD_Xmax+1-x-i));
	temp=Font8x16[(ascii_code-0x20)*16+i];	 
	LCD_data(temp);
	}

  for(i=0;i<8;i++) {
	LCD_SetAddress((y/8)+1,(LCD_Xmax+1-x-i));	 
	temp=Font8x16[(ascii_code-0x20)*16+i+8];
	LCD_data(temp);
  }
}
void print_Line(int8_t line, char text[])
{
	int8_t i;
	for (i=0;i<strlen(text);i++) 
		printC(i*8,line*16,text[i]);
}



void SYS_config(void)
{
	SYS_UnlockReg();
  CLK->PWRCON |= (1<<0)|(1<<1)|(1<<2)|(1<<3); //Enable all clock
 
  while(!(CLK->CLKSTATUS & 1<<0)); // Check status

  CLK->PLLCON &= ~(1ul<<18); // PLL FOUT enable
  CLK->PLLCON &= ~(1ul<<16); // PLL is normal mode
  CLK->PLLCON &= ~(0x1FF<<0); // Clear Feedback Divider value
  CLK->PLLCON |= (48<<0); // Set Feedback Divider value to 48
	 
  while(!(CLK->CLKSTATUS & 1<<2)); // Check status

  CLK->CLKSEL0 &= ~(0x07<<0); // Cleaer all value
  CLK->CLKSEL0 |= 0x02 <<0; // Choose HCLK source to PLL
 	
  CLK->CLKDIV &= ~0x0F; // Clear HCLK divide value
  CLK->CLKDIV |= 0; // Set divide value to 0

	CLK->CLKSEL1 &= ~(0x07<<16);
	 
	CLK->APBCLK |= 1ul << 4;										// Enable CLK for TIMER 2
  CLK->APBCLK |= 1ul << 15;
	SYS_LockReg();
 }
void SPI_Init(void)
{
	SYS->GPD_MFP |= 1ul << 11; //1: PD11 is configured for alternative function
	SYS->GPD_MFP |= 1ul << 9; //1: PD9 is configured for alternative function
	SYS->GPD_MFP |= 1ul << 8; //1: PD8 is configured for alternative function

	SPI3->CNTRL &= ~(1ul << 23); //0: disable variable clock feature
	SPI3->CNTRL &= ~(1ul << 22); //0: disable two bits transfer mode
	SPI3->CNTRL &= ~(1ul << 18); //0: select Master mode
	SPI3->CNTRL &= ~(1ul << 17); //0: disable SPI interrupt

	SPI3->CNTRL |= 1ul << 11; //1: SPI clock idle high

	SPI3->CNTRL &= ~(1ul << 10); //0: MSB is sent first

	SPI3->CNTRL &= ~(3ul << 8); //00: one transmit/receive word will be executed in one data transfer

	SPI3->CNTRL &= ~(31ul << 3);
	SPI3->CNTRL |= 9ul << 3;

	SPI3->CNTRL |= (1ul << 2); //1: Transmit at negative edge of SPI CLK

	SPI3->DIVIDER = 24; // SPI clock divider. SPI clock = HCLK / ((DIVIDER+1)*2)
 }
void LCD_command(unsigned char temp)
{
	SPI3->SSR |= 1ul << 0;
	SPI3->TX[0] = temp;
	SPI3->CNTRL |= 1ul << 0;
	while(SPI3->CNTRL & (1ul << 0));
	SPI3->SSR &= ~(1ul << 0);
}
void LCD_data(unsigned char temp)
{
	SPI3->SSR |= 1ul << 0;
	SPI3->TX[0] = 0x0100+temp;
	SPI3->CNTRL |= 1ul << 0;
	while(SPI3->CNTRL & (1ul << 0));
	SPI3->SSR &= ~(1ul << 0);
}

void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr)
{
 LCD_command(0xB0 | PageAddr); // Set PageAddr
 LCD_command(0x10 | ((ColumnAddr>>4)&0xF)); // Set ColumnAddr MSB
 LCD_command(0x00 | (ColumnAddr & 0xF)); // Set ColumnAddr LSB
}

void init_LCD(void)
{
	LCD_command(0xEB); 
	LCD_command(0x81); 
	LCD_command(0xA0);  
	LCD_command(0xC0);  
	LCD_command(0xAF); // Set Display Enable 
}

void clear_LCD(void)
{
	int16_t i,j;
	for (j=0;j<LCD_Ymax;j++){
	  for (i=0;i<LCD_Xmax;i++){
	     DisplayBuffer[i+j/8*LCD_Xmax]=0;
		}
	}
		LCD_SetAddress(0x0, 0x0);		

	for (i = 0; i < 132 *8; i++)
	{
		LCD_data(0x00);
	}
	LCD_data(0x0f);
}

void TMR2_IRQHandler(void)
{
		TIMER2->TEXCON &= ~(0x01<<4);
		Timer_count = TIMER2->TCAP;
		Timer_flag = 1;
		TIMER2 -> TEXISR |= (0x01<<0);
}

void Init_TimerCapture(void)
{
		SYS-> GPB_MFP |= 	(1ul << 2);				
		SYS-> ALT_MFP &= ~(1ul <<11);
		SYS-> ALT_MFP &= ~(1ul <<13);
		SYS-> ALT_MFP |= 	(1ul <<26);
		
		TIMER2->TCSR  |=  	  (1 <<26);
		TIMER2->TCSR  |=  (0x03 <<27);
		TIMER2->TCSR  &= ~(0x01 <<24);
		TIMER2->TCSR  &= ~(0xFF << 0);
		TIMER2->TCSR  |= 	(0x0B <<0);
		
		TIMER2->TEXCON	&= ~(1<<4);
		TIMER2->TEXCON	|= (0x02<<1);
		TIMER2->TEXCON	|= (1<<3);
   	TIMER2->TEXISR 	|= (1<<0);	
		TIMER2->TEXCON	|= (1<<5);
				
		NVIC->ISER[0] 	|= (1<<10);
	
	  TIMER2->TCMPR		|= (0xFFFFFF<<0);
}
