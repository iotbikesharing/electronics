//This is the codes used for EEET2492-Project- Distance measurement system from Team Saser
//Project     : Distance measurement
//Author      : Vo Minh Hoang - s3575983
//	       		  Nguyen Minh - s3574915
//	       		  Uong Kim Thuan - s3574935
//Board       : Nu-LB-NUC140
//Extra component: Ultrasonic ranging module HC - SR04
//MCU         : NUC140VE3CN
//Functions   : Measure the distance between the ultrasonic sensor and any other object 
//							(preferably flat surfaced objects) and then display the distance on the 
//							LCD screen installed on the Nuvoton board. In addition, a blinking LED  
//							is also used in this program to indicate the range of between the object
//							and the sensor. The frequency at which the LED blinks will be determined 
//							by the distance measured.
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "font8x16.h"
#include <string.h>

#define LCD_Xmax 128
#define LCD_Ymax 64

uint32_t Timer_count =0;
uint8_t  Timer_flag =0;
 uint32_t D =0;
void initialize_LCD (void);
void LCD_clear (void);
void print_Line(int8_t line, char text[]);

int main(void)
{
 //--------------------------------
 //System initialization
 //--------------------------------
 SYS_UnlockReg();
 // Clock source configurations
 CLK -> PWRCON |= (0x01 << 0) | (0x01<<1) | (0x01<<2) | (0x01 <<3);				// Enable all the clocks on the board
	
 while (!( CLK->CLKSTATUS & (0x01 <<0)));																	// Check High external clk for stability
	
 // CLK source select configurations
 CLK-> CLKSEL0 &=~ (0x07 << 0);							// Clear the system CLK source selection

 CLK->CLKSEL0 |= 0x00 <<0;									// Chooses CLK Source	for the system
																						// 000 : High XT 12 MHz (4~24)
																						// 001 : Low XT 32.768 kHz
																						// 010 : PLL CLK
																						// 011 : Low internal 10 kHz
																						// 111 : High internal 22.1184
	
 CLK->CLKSEL1 &= ~(7ul << 16);							// Clear TIMER 2 CLK source (000: High external)
 CLK->CLKSEL1 |= 0x02 << 16;								// Set TIMER 3 CLK src
																						// 000 : High XT  12 MHz (4~24)
																						// 001 : Low XT 32.768 kHz
																						// 010 : CLK SRC = HCLK
																						// 111 : High internal 22.1184 MHz
																						
 CLK->APBCLK |= 1ul << 4;										// Enable CLK source for TIMER 2
 CLK-> APBCLK |= 1ul << 15;									// Enable CLK source for SPI 3
 
 SYS_LockReg();
 // END SYSTEM INITIALIZATION
 ///////////////////////////////////////////////////////////////////////////////////
 
 //LCD CONFIGURATION
	initialize_LCD ();												// Set up the LCD
	LCD_clear ();															// Clear out the previous display on the LCD		
	print_Line(0, "Team SASER-SR04");					// Print out the title on the LCD
		
	PB->PMD &= ~(0x3<< 8);										// Clear out the mode control bits for PB4 (SR04 Trigger)		
	PB->PMD |= (0x01 << 8);										// Set PB4 to output mode
		
	PB-> DOUT &=~ (0x01<<4);									// Set the value of PB4 to 0
		
	PC-> PMD &=~ (0x03<<24);									// Clear out the mode control bits for PC12 (LED2)
	PC-> PMD |= (0x01<<12);										// Set PC12 to be in output mode

 
// TIMER 2 EXTERNAL CONFIGURATION
 //Enable the Timer 2 external (T2EX) function for the GPIO_B pin 2
 SYS-> GPB_MFP |= 1ul << 2;				
 SYS-> ALT_MFP &=~(1ul << 11);
 SYS-> ALT_MFP &=~ (1ul <<13);
 SYS-> ALT_MFP |= (1ul <<26);
 
 //	TIMER 2 configuration
 TIMER2->TCSR |= 1ul << 26;  								// Reset the timer counter, internal 24 bit up timer and CEN bit

 
 TIMER2->TCSR |= (0x03 << 27); 							// Set mode [28:27]
																						// 00 : One shot mode
																						// 01 : Periodic mode
																						// 10 : Toggle mode
																						// 11 : continuous mode
	
 TIMER2 -> TCSR &=~ (0xFF<<0);							// Clear out the prescale value
 TIMER2 -> TCSR |= (0x0B <<0);							// Set the prescale value to 11
																	
 // TEX2 capture function configuration
 TIMER2 -> TEXCON &=~ (0x01<< 4);						// Enable timer external capture function
 TIMER2 -> TEXCON |= (0x02<< 1);						// Both falling and rising is detected

 TIMER2-> TEXCON |= (0x01<<3);							// Enable Timer 2 external pin
																	
 //Timer 2 interrupt configuration 
 TIMER2 -> TEXISR |= (0x01<<0);							// Clear TIF flag before enabling timer interrupt
																	
 TIMER2 -> TEXCON |= (0x01 <<5);						// Enable timer external interrupt

 NVIC-> ISER[0] |= (0x01<<10);		 					// 	Enable interrupt for Timer 2 in the NVIC
 
 TIMER2->TCMPR = 0xFFFFFF;									// Set timer compare value

 // END TIMER 2 EXTERNAL CONFIGURATION
 /////////////////////////////////////////////////////////////////////////////////////
 
	
//-------------------------------------------------------------------------------------------------------------
//Main program
//-------------------------------------------------------------------------------------------------------------
 while(1){	 
	 char Text1[16];
	 char Text2[16];
	 
	 PB-> DOUT |= (0x01<<4);											// Set the trigger pin to high state
	 CLK_SysTickDelay (10);												// in 10us
	 PB-> DOUT &=~ (0x01<<4);											// then turn off
	 TIMER2-> TEXCON |= (0x01<<4);								// Reset the TDR --> 0
	 
	 TIMER2->TCSR |= (0x01 << 30); 								// Enable the Timer 2(start counting)
	 CLK_SysTickDelay (30000);										// Count for 30us 
	 TIMER2-> TCSR &=~ (0x01<<30);								// Disable the Timer 2 (stop counting)
	 
	 
	 if (Timer_flag == 1) {												// Condition for when the timer_flag variable is 1
		 D= Timer_count*170/10000;									// Calculating the distance based on the captured time
		 sprintf (Text1, "T=%5d", Timer_count);			 
		 print_Line (1,Text1);											// Print out the Timer count value on the LCD
		 sprintf (Text2, "D=%5d cm", D);		
		 print_Line (2,Text2);											// Print out the distance value on the LCD
		 Timer_flag=0;															// Set the timer flag to 0
	 }
	 
	 if (D>30) {																	// Condition for when the distance is larger 
		PC->DOUT &=~ (1<<13);												// than 30 cm, LED2 will blink with a 300us delay
		CLK_SysTickDelay (300000);
		PC->DOUT |= (1<<13);
		CLK_SysTickDelay (300000);
	 }
	 else if (D<30 && D>=20) {										// Condition for when the distance is <30cm and 
		PC->DOUT &=~ (1<<13);												// >=20cm LED2 will blink with a 150us delay
		CLK_SysTickDelay (150000);
		PC->DOUT |= (1<<13);
		CLK_SysTickDelay (150000); 
	 }
	 else if (D<20 && D>=10){											// Condition for when the distance is <20cm and
		PC-> DOUT &=~ (1<<13);											// >=10cm LED2 will blink with a 50us delay
		CLK_SysTickDelay (50000);
		PC->DOUT |= 1<<13;
		CLK_SysTickDelay (50000);
	 }
	 else if (D<10 && D>=2) {											// Condition for when the distance is <10cm
		PC->DOUT &=~ (1<<13);												// and >= 2cm LED2 will blink with a 10us delay
		CLK_SysTickDelay (10000);
		PC->DOUT |= 1<<13;
		CLK_SysTickDelay (10000);
	 }
 }
}
 void TMR2_IRQHandler (void) {
		TIMER2-> TEXCON &=~ (0x01<< 4);							// set RSTCAPSEL to 0 to read counter
		Timer_count = TIMER2-> TCAP;								// Store captured value (TCAP) to Timer_count
		Timer_flag =1;
		TIMER2-> TEXISR |= (0x01<<0);								// Clear interrupt flag
 }

 
 void initialize_SPI3 (void) {
		SYS->GPD_MFP |= 1ul << 11; 									// 1: Select the MOSI0 function for PD11
	  SYS->GPD_MFP |= 1ul << 9; 									// 1: Select SPI3 SPICLK function to PD9
		SYS->GPD_MFP |= 1ul << 8; 									// 1: Select SPI3 SS30 function for PD8

		SPI3->CNTRL &= ~(1ul << 23); 								// 0: Serial output clk = fixed and only decicded by the 
																								//		value of DIVIDER 
																								// 1: Enable variable clk feat, output freq can be
																								// 		decided by the value of VARCLK, DIVIDER and DIVIDER2
																			
		SPI3->CNTRL &= ~(1ul << 22); 								// 0: disable two bits transfer mode
																		
		SPI3->CNTRL &= ~(1ul << 18); 								// 0: Master mode
																								// 1: Slave mode
		 
		SPI3->CNTRL &= ~(1ul << 17); 								// 0: disable SPI interrupt

		SPI3->CNTRL |= 1ul << 11; 									// 1: SPI clock idle high

		SPI3->CNTRL &= ~(1ul << 10); 								// 0: MSB is sent first

		SPI3->CNTRL &= ~(3ul << 8); 								// # of Trans/Rec word
																								// 00: 1 trans/rec word will be executed in 1 transfer
																								// 01: 2 trans/rec words will be execurted in 1 transfer
																
		SPI3->CNTRL &= ~(31ul << 3);								// Clear the transmite bit length
		SPI3->CNTRL |= 9ul << 3;										// Transmit bit length min: 8 bits, max: 32 bits

		SPI3->CNTRL |= (1ul << 2); 									// 1: Transmit at falling edge of SPI CLK

		SPI3->DIVIDER = 24; 												// SPI clock divider. SPI clock = HCLK /((DIVIDER+1)*2)
 }
 
 void LCD_command(unsigned char temp)						// Function to send commands to the LCD via SPI3
{
 SPI3->SSR |= 1ul << 0;													// Set the SPISSx0/1 line to an active state
	
 SPI3->TX[0] = temp;														// Store the data to be transmitted in the transfer

 SPI3->CNTRL |= 1ul << 0;												// 1: Master mode = start SPI data transfer
																								//   	Slave mode = ready to communicate/receive data
																								// 0: stop data transfer if SPI is transfering
	
 while(SPI3->CNTRL & (1ul << 0));								// Check if SPI have finished transfering
	
 SPI3->SSR &= ~(1ul << 0);											// Set the SPISSx0/1 to inactive 
}
void LCD_data(unsigned char temp)	{							// Function to write data on the LCD screen
 SPI3->SSR |= 1ul << 0;							
	
 SPI3->TX[0] = 0x0100+temp;											// Set the data value which result in displaying on the LCD
	
 SPI3->CNTRL |= 1ul << 0;
	
 while(SPI3->CNTRL & (1ul << 0));		
	
 SPI3->SSR &= ~(1ul << 0);					
}

void LCD_SetAddress(uint8_t PageAddr, uint8_t ColumnAddr){			// Function to set the place where 
																																// the data will appear on the screen
 LCD_command(0xB0 | PageAddr);									// Set page address can vary from 0 ~ 8 

 // Set collumn address: Val range from 0~131 
 LCD_command(0x10 | (ColumnAddr>>4)&0xF);				// Set collumn address MSB
 LCD_command(0x00 | (ColumnAddr & 0xF));				// Set collumn address LSB 
}


void LCD_clear(void){														// Function to clear out the existing data 
																								// displayed on the LCD screen
 int16_t i;
LCD_SetAddress(0x0, 0x0);												// The address is set at 0,0

for (i = 0; i < 132 *8; i++){										// Then the data on every address is then
 LCD_data(0x00);																// clear out the data in every address on the LCD		
	}
}

void initialize_LCD (void) {										// Function to initialize the whole LCD
																								// including the intialization of the SPI3
	initialize_SPI3();	
	LCD_command(0xEB); 														// Set the LCD bias ratio to 9
	LCD_command(0x81);														// Set V-Bias potentiometer 
	LCD_command(0xA0);  													// Set the frame rate to 80 fps
	LCD_command(0xC0);  													// Set the LCd mapping control
	LCD_command(0xAF); 														// Enable the LCD display
 }

 void printC(int16_t x, int16_t y, unsigned char ascii_code){			// Function used to print 
																																  // characters based on ASCII code
	 
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

 void print_Line(int8_t line, char text[]){				// Function to print out text on the LCD 
																									// based on the print C function
	int8_t i;
	for (i=0;i<strlen(text);i++) 
		printC(i*8,line*16,text[i]);
}
