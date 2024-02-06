/******************************************************************/
//Seg_LCD.c	Rev 1.0	8/20/2014 EPH Initial version									//
//Author: Ethan Hettwer																						//
//Purpose:  Allow for easy control of Seg_LCD											//
/******************************************************************/

#include "MKL46Z4.h"
#include "Seg_LCD.h"

const static uint8_t LCD_Frontplane_Pin[LCD_NUM_FRONTPLANE_PINS] = {LCD_FRONTPLANE0, LCD_FRONTPLANE1, LCD_FRONTPLANE2, LCD_FRONTPLANE3, LCD_FRONTPLANE4, LCD_FRONTPLANE5, LCD_FRONTPLANE6, LCD_FRONTPLANE7};
const static uint8_t LCD_Backplane_Pin[LCD_NUM_BACKPLANE_PINS] = {LCD_BACKPLANE0, LCD_BACKPLANE1, LCD_BACKPLANE2, LCD_BACKPLANE3};

/******************************************************************/
//						Function for Initialization of LCD									//
/******************************************************************/

void SystemClockConfiguration(void)
{
    /* OUTDIV4 = 2, other is 0*/
    SIM->CLKDIV1 = (uint32_t)0x00020000UL; /* Update system prescalers */
    
    /* Switch to FEI Mode */
    /* C1 Field: 7-6: CLKS,    5-3: FRDIV,     2: REFS,    1: CLKEN,   0: EFSTEN*/ 
    /* MCG->C1: CLKS=0,FRDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0  => 00000110b ~ 0x06*/ 
    MCG->C1 = (uint8_t)0x06U;
    
    /* C2 Field: 7: LOCRE0,    6: FCFTRIM,     5-4: RANGE0,    3: HGO0,   2: EREFS0,    1: LP,      0: IRCS*/ 
    /* MCG_C2: LOCRE0=0,FCFTRIM=1,RANGE0=0,HGO0=0,EREFS0=0,LP=0,IRCS=0 => 01000000*/
    MCG->C2 &= (uint8_t)0x40;
    
    /* Frequency Selection is refered at page 388 in Ref. Manual */
    /* C4 Field: 7: DMX32,      6-5: DRST_DRS,      4-1: FCTRIM: xxxx,        0: SCFTRIM */
    /* xxxx is undefined at reset */
    /* FCTIM will load value xxxx at reset => they can't be touch at logical bitwise operation*/
    /* MCG->C4: DMX32=0, DRST_DRS=1 => output frequency is 40 - 50 MHz*/
    MCG->C4 = (uint8_t)(MCG->C4 & 0x20U);
    
    /*OSC0->CR:ERCLKEN=1, EREFSTEN=0, SC2P=0,SC4P=0,SC8P=0,SC16P=0 */
    OSC0->CR = (uint8_t)0x80U;
    
    /* C5 Field: 7: x,      6: PLLCLKEN0,       5: PLLSTEN0,        4-0: PRDIV0 */
    /* MCG->C5: x=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=0 */
    MCG->C5 = (uint8_t)0x00U;
    
    /* C6 Field: 7: LOLIE0,    6: PLLS,    5: CME0,      4-0: VDIV0 */
    /* MCG->C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=0 */
    MCG->C6 = (uint8_t)0x00U;
    
    while((MCG->S & MCG_S_IREFST_MASK) == 0x00U)
    {
        /* Check that the source of the FLL reference clock is the internal reference clock. */
    }
    
    while((MCG->S & 0x0CU) != 0x00U)
    {
        /* Wait until output of the FLL is selected */
    }
}



void SegLCD_Init(void)
{
    //Initializes all components of SLCD on the FRDM-KL46Z
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK |    //Enable Clock to ports B
                  SIM_SCGC5_PORTC_MASK |    //Enable Clock to ports C
                  SIM_SCGC5_PORTD_MASK |    //Enable Clock to ports D
                  SIM_SCGC5_PORTE_MASK |    //Enable Clock to ports E
                  SIM_SCGC5_SLCD_MASK;      //Enable Clock to ports SegLCD Peripheral
    
    LCD->GCR   |= LCD_GCR_PADSAFE_MASK;     //Set PADSAFE to disable LCD while configuring: LCD_BASE is 0x40053000, PADSAFE Register is 15th bit => offset mask is 0x8000 
    LCD->GCR   &= ~LCD_GCR_LCDEN_MASK;      //Clear LCDEN (LCD Enable) while configuring:   Force LCDEN bit to be 0, offset mask is 0x80
    
    
    //Configure pins, set pins to MUX 0 for normal LCD display operation
    PORTC->PCR[17] = PORT_PCR_MUX(0u);      //Set PTC17 to LCD_P37   (Front Plane Pin 0)
    PORTB->PCR[21] = PORT_PCR_MUX(0u);      //Set PTB21 to LCD_P17   (Front Plane Pin 1)
    PORTB->PCR[7]  = PORT_PCR_MUX(0u);      //Set PTB7 to LCD_P7     (Front Plane Pin 2)
    PORTB->PCR[8]  = PORT_PCR_MUX(0u);      //Set PTB8 to LCD_P8     (Front Plane Pin 3)
    PORTE->PCR[5]  = PORT_PCR_MUX(0u);      //Set PTE5 to LCD_P53    (Front Plane Pin 4)
    PORTC->PCR[18] = PORT_PCR_MUX(0u);      //Set PTC18 to LCD_P38   (Front Plane Pin 5)
    PORTB->PCR[10] = PORT_PCR_MUX(0u);      //Set PTB10 to LCD_P10   (Front Plane Pin 6)
    PORTB->PCR[11] = PORT_PCR_MUX(0u);      //Set PTB11 to LCD_P11   (Front Plane Pin 7)
    PORTD->PCR[0]  = PORT_PCR_MUX(0u);      //Set PTD0 to LCD_P40    (COM0)
    PORTE->PCR[4]  = PORT_PCR_MUX(0u);      //Set PTE4 to LCD_P52    (COM1)
    PORTB->PCR[23] = PORT_PCR_MUX(0u);      //Set PTB23 to LCD_P19   (COM2)
    PORTB->PCR[22] = PORT_PCR_MUX(0u);      //Set PTB22 to LCD_P18   (COM3)
    
   	//Configure LCD Registers: Reference: Table 44-43. Initialization register value for example 1
    //Configure LCD_GCR - General Control Register, controls most options in LCD Peripheral
 LCD->GCR = LCD_GCR_RVEN(0x00)      |       //Clear LCD_GCR_RVEN, disable voltage regulator.
            LCD_GCR_RVTRIM(0x00)    |       //Set RVTRIM to 0, irrelevant as voltage regulator is disabled, but setting it to a known state.
            LCD_GCR_CPSEL(0x00)     |       //Set LCD_GCR_CPSEL to use capacitor charge pump.
            LCD_GCR_LADJ(0x03)      |       //Set LCD_GCR_LADJ to 11, slow clock rate = lower power, but higher load capacitance on the LCD requires higher clock speed.
            LCD_GCR_VSUPPLY(0x01)   |       //Clear LCD_GCR_VSUPPLY, drive VLL3 externally.
            LCD_GCR_PADSAFE_MASK    |       //Set LCD_GCR_PADSAFE, leave enabled during configuration process.
            LCD_GCR_ALTDIV(0x00)    |       //Set LCD_GCR_ALTDIV to 0 (no divide) because external frequency is 32768Hz (is among the active range of LCD: 30 -39.063 kHz)
            LCD_GCR_ALTSOURCE(0x00) |       //Set LCD_GCR_ALTSOURCE, Part of setting clock source to OSCERCLK, or external oscillator.
            LCD_GCR_FFR(0x00)       |       //Set LCD_GCR_FFR, allow an LCD Frame Frequency of 46.6Hz to 146.2Hz.  Disable to change range to 23.3Hz to 73.1Hz.
            LCD_GCR_LCDDOZE_MASK    |       //Clear LCD_GCR_LCDDOZE, allows LCD peripheral to run even in doze mode.  Set to disable LCD in doze mode.
            LCD_GCR_SOURCE(0x05)    |       //Set LCD_GCR_SOURCE, Part of setting clock source to OSCERCLK, or external oscillator.
            LCD_GCR_LCLK(0x07)      |       //Set LCD_GCR_LCLK to 111, LCD Clock prescaler where LCD controller frame frequency = LCD clock/((DUTY  |  1) x 8 x (4 | LCLK[2:0]) x Y), where Y = 2, 2, 3, 3, 4, 5, 8, 16 chosen by module duty cycle config
            LCD_GCR_DUTY(0x03);             //Set LCD_GCR_DUTY to 011, Have 4 backplane pins, so need a 1/4 duty cycle.
            
       
							
    //Configure LCD_SEG_AR  - Auxiliary Register, controls blinking of LCD
	LCD->AR = LCD_AR_BRATE(0x00);			//Set LCD_SEG_AR_BRATE to 000.  Frequency of blink is determined by LCD clock/(2^(12 + BRATE))
						
    
    
    //Configure LCD_SEG_FDCR - Fault Detect Control Register, controls use of Fault Detect features of SLCD.
    LCD->FDCR = 0x00000000; //Clear all bits in FDCR. As this will not be covering use of fault detect, this register is cleared.
    
    
    //Configure LCD_PENn - Pin Enable Register, controls which of the possible LCD pins are used
    //LCD->PEN[0] contains bits 0-31, while LCD->PEN[1] contains bits 32-63.
    LCD->PEN[0] = LCD_PEN_PEN(1u << 7)  | //LCD_P7
                  LCD_PEN_PEN(1u << 8)  | //LCD_P8
                  LCD_PEN_PEN(1u << 10) | //LCD_P10
                  LCD_PEN_PEN(1u << 11) | //LCD_P11
                  LCD_PEN_PEN(1u << 17) | //LCD_P17
                  LCD_PEN_PEN(1u << 18) | //LCD_P18
                  LCD_PEN_PEN(1u << 19);  //LCD_P19
                  
    LCD->PEN[1] = LCD_PEN_PEN(1u << 5)   | //LCD_P37
                  LCD_PEN_PEN(1u << 6)   | //LCD_P38
                  LCD_PEN_PEN(1u << 8)   | //LCD_P40
                  LCD_PEN_PEN(1u << 20)  | //LCD_P52
                  LCD_PEN_PEN(1u << 21);   //LCD_P53
    
    
    //Configure LCD_SEG_BPENn - Back Plane Enable Register, controls which pins in LCD->PEN are Back Plane (commons)
    //LCD->BPEN[0] contains bits 0-31, while LCD->BPEN[1] contains bits 32-63.
    LCD->BPEN[0] = LCD_BPEN_BPEN(1u << 18) | //LCD_P18 COM3
                   LCD_BPEN_BPEN(1u << 19);  //LCD_P19, COM2
                   
    LCD->BPEN[1] = LCD_BPEN_BPEN(1u << 8)  | //LCD_P40, COM0
                   LCD_BPEN_BPEN(1u << 20);  //LCD_P52, COM1
                   
                
    //Configure LCD_WFyTOx - Configures 4 Waveform signals, LCD_WF[z] is defined such that x = (z*4) and y = 3 | (z*4)
    //Where x is the n index value of WFn on the least significant byte (bits 7-0) and y is the n index value of WFn on the most significant byte (bits 31-24)
    //Note that "Disabled" is used for pins that are not set as LCD pins, where "Off" is used for pins that are set as LCD, but are just inactive.
    LCD->WF[0]  = LCD_WF_WF0(0x00) |      //WF Pin 0 Disabled
                  LCD_WF_WF1(0x00) |      //WF Pin 1 Disabled
                  LCD_WF_WF2(0x00) |      //WF Pin 2 Disabled
                  LCD_WF_WF3(0x00);       //WF Pin 3 Disabled
                  
    LCD->WF[1]  = LCD_WF_WF4(0x00) |      //WF Pin 4 Disabled
                  LCD_WF_WF5(0x00) |      //WF Pin 5 Disabled
                  LCD_WF_WF6(0x00) |      //WF Pin 6 Disabled
                  LCD_WF_WF7(0x00);       //WF Pin 7 Off
                  
    LCD->WF[2]  = LCD_WF_WF8(0x00) |      //WF Pin 8 Off
                  LCD_WF_WF9(0x00) |      //WF Pin 9 Disabled
                  LCD_WF_WF10(0x00)|      //WF Pin 10 Off
                  LCD_WF_WF11(0x00);      //WF Pin 11 Off
                  
    LCD->WF[3]  = LCD_WF_WF12(0x00)|      //WF Pin 12 Disabled
                  LCD_WF_WF13(0x00)|      //WF Pin 13 Disabled
                  LCD_WF_WF14(0x00)|      //WF Pin 14 Disabled
                  LCD_WF_WF15(0x00);      //WF Pin 15 Disabled
                  
    LCD->WF[4]  = LCD_WF_WF16(0x00)|      //WF Pin 16 Disabled
                  LCD_WF_WF17(0x00)|      //WF Pin 17 Off
                  LCD_WF_WF18(0x88)|      //WF Pin 18 (COM3) is enabled on Phases D and H
                  LCD_WF_WF19(0x44);      //WF Pin 19 (COM2) is enabled on Phases C and G
                  
    LCD->WF[5]  = LCD_WF_WF20(0x00)|      //WF Pin 20 Disabled
                  LCD_WF_WF21(0x00)|      //WF Pin 21 Disabled
                  LCD_WF_WF22(0x00)|      //WF Pin 22 Disabled
                  LCD_WF_WF23(0x00);      //WF Pin 23 Disabled
                  
    LCD->WF[6]  = LCD_WF_WF24(0x00)|      //WF Pin 24 Disabled
                  LCD_WF_WF25(0x00)|      //WF Pin 25 Disabled
                  LCD_WF_WF26(0x00)|      //WF Pin 26 Disabled
                  LCD_WF_WF27(0x00);      //WF Pin 27 Disabled
                  
    LCD->WF[7]  = LCD_WF_WF28(0x00)|      //WF Pin 28 Disabled
                  LCD_WF_WF29(0x00)|      //WF Pin 29 Disabled
                  LCD_WF_WF30(0x00)|      //WF Pin 30 Disabled
                  LCD_WF_WF31(0x00);      //WF Pin 31 Disabled
                  
    LCD->WF[8]  = LCD_WF_WF32(0x00)|      //WF Pin 32 Disabled
                  LCD_WF_WF33(0x00)|      //WF Pin 33 Disabled
                  LCD_WF_WF34(0x00)|      //WF Pin 34 Disabled
                  LCD_WF_WF35(0x00);      //WF Pin 35 Disabled
                  
    LCD->WF[9]  = LCD_WF_WF36(0x00)|      //WF Pin 36 Disabled
                  LCD_WF_WF37(0x00)|      //WF Pin 37 Off
                  LCD_WF_WF38(0x00)|      //WF Pin 38 Off
                  LCD_WF_WF39(0x00);      //WF Pin 39 Disabled
                  
    LCD->WF[10] = LCD_WF_WF40(0x11)|      //WF Pin 40 (COM0) is enabled on Phases A and E
                  LCD_WF_WF41(0x00)|      //WF Pin 41 Disabled
                  LCD_WF_WF42(0x00)|      //WF Pin 42 Disabled
                  LCD_WF_WF43(0x00);      //WF Pin 43 Disabled
                  
    LCD->WF[11] = LCD_WF_WF44(0x00)|      //WF Pin 44 Disabled
                  LCD_WF_WF45(0x00)|      //WF Pin 45 Disabled
                  LCD_WF_WF46(0x00)|      //WF Pin 46 Disabled
                  LCD_WF_WF47(0x00);      //WF Pin 47 Disabled
                  
    LCD->WF[12] = LCD_WF_WF48(0x00)|      //WF Pin 48 Disabled
                  LCD_WF_WF49(0x00)|      //WF Pin 49 Disabled
                  LCD_WF_WF50(0x00)|      //WF Pin 50 Disabled
                  LCD_WF_WF51(0x00);      //WF Pin 51 Disabled
                  
    LCD->WF[13] = LCD_WF_WF52(0x22)|      //WF Pin 52 (COM1) is enabled on Phases B and F
                  LCD_WF_WF53(0x00)|      //WF Pin 53 Off
                  LCD_WF_WF54(0x00)|      //WF Pin 54 Disabled
                  LCD_WF_WF55(0x00);      //WF Pin 55 Disabled
                  
    LCD->WF[14] = LCD_WF_WF56(0x00)|      //WF Pin 56 Disabled
                  LCD_WF_WF57(0x00)|      //WF Pin 57 Disabled
                  LCD_WF_WF58(0x00)|      //WF Pin 58 Disabled
                  LCD_WF_WF59(0x00);      //WF Pin 59 Disabled
                  
    LCD->WF[15] = LCD_WF_WF60(0x00)|      //WF Pin 60 Disabled
                  LCD_WF_WF61(0x00)|      //WF Pin 61 Disabled
                  LCD_WF_WF62(0x00)|      //WF Pin 62 Disabled
                  LCD_WF_WF63(0x00);      //WF Pin 63 Disabled
    
    //Disable GCR_PADSAFE and Enable GCR_LCDEN
    LCD->GCR &= ~LCD_GCR_PADSAFE_MASK;    //Clear PADSAFE to unlock LCD pins
    LCD->GCR |= LCD_GCR_LCDEN_MASK;       //Set LCDEN to enable operation of LCD
}
//End SegLCD_Init


/******************************************************************/
//						End Function for Initialization of LCD							//
/******************************************************************/

/******************************************************************/
//						Functions for Manipulation of LCD										//
/******************************************************************/

void SegLCD_Set(uint8_t Value,uint8_t Digit){//Sets a value from 0-F to a specified Digit, with 1 being the leftmost, 4 being the rightmost.  Will not display error is Value is outside of 0-F, but display will not update
	int k;
	if(Digit > 4){
		SegLCD_DisplayError(0x01);
	}		//Display "Err" if trying to access a digit that does not exist
	else{
		if(Value==0x00)			{LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_D | LCD_SEG_E |LCD_SEG_F); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = (LCD_SEG_A | LCD_SEG_B | LCD_SEG_C);}
		else if(Value==0x01){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_CLEAR); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = (LCD_SEG_B | LCD_SEG_C);}
		else if(Value==0x02){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_D | LCD_SEG_E | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = (LCD_SEG_A | LCD_SEG_B);}
		else if(Value==0x03){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_D | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = (LCD_SEG_A | LCD_SEG_B | LCD_SEG_C);}
		else if(Value==0x04){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_F | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = (LCD_SEG_B | LCD_SEG_C);}
		else if(Value==0x05){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_D | LCD_SEG_F | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = (LCD_SEG_A | LCD_SEG_C);}
		else if(Value==0x06){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = (LCD_SEG_A | LCD_SEG_C);}
		else if(Value==0x07){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_CLEAR); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = (LCD_SEG_A | LCD_SEG_B | LCD_SEG_C);}
		else if(Value==0x08){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = (LCD_SEG_A | LCD_SEG_B | LCD_SEG_C);}
		else if(Value==0x09){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_F | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = (LCD_SEG_A | LCD_SEG_B | LCD_SEG_C);}
		else if(Value==0x0A){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_E | LCD_SEG_F | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = (LCD_SEG_A | LCD_SEG_B | LCD_SEG_C);}
		else if(Value==0x0B){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = LCD_SEG_C;}
		else if(Value==0x0C){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_D | LCD_SEG_E | LCD_SEG_F); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = LCD_SEG_A;}
		else if(Value==0x0D){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_D | LCD_SEG_E | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = LCD_SEG_B | LCD_SEG_C;}
		else if(Value==0x0E){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = LCD_SEG_A;}
		else if(Value==0x0F){LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-2)]] = (LCD_SEG_E | LCD_SEG_F | LCD_SEG_G); LCD->WF8B[LCD_Frontplane_Pin[((2*Digit)-1)]] = LCD_SEG_A;}
	}
}//End SegLCD_Set

void SegLCD_DisplayDecimal(uint16_t Value){//Displays a 4 Digit number in decimal
	if(Value > 9999){
		SegLCD_DisplayError(0x10); //Display "Err" if value is greater than 4 digits
	}
	else{
		SegLCD_Set(Value/1000,1);
		SegLCD_Set((Value - (Value/1000)*1000)/100,2);
		SegLCD_Set((Value - (Value/100)*100)/10,3);
		SegLCD_Set(Value - (Value/10)*10,4);
	}
}//End SegLCD_Display4Digit

void SegLCD_DisplayHex(uint16_t Value){ //Displays a 16 bit number in Hex Format
	SegLCD_Set((Value & 0xF000)>>12, 1);
	SegLCD_Set((Value & 0x0F00)>>8 , 2);
	SegLCD_Set((Value & 0x00F0)>>4 , 3);
	SegLCD_Set((Value & 0x000F)>>0 , 4);
}//End SegLCD_DisplayHex

void SegLCD_DisplayTime(uint8_t Value1, uint8_t Value2){//Displays 2 values separated by a colon
	if((Value1 > 99) | (Value2 > 99)){
		SegLCD_DisplayError(0x10); //Display "Err" if either value is greater than 2 digits
	}
	else{
		SegLCD_Set(Value1/10, 1);
		SegLCD_Set(Value1 % 10, 2);
		SegLCD_Set(Value2/10, 3);
		SegLCD_Set(Value2 % 10, 4);
		SegLCD_Col_On();
	}
}//End SegLCD_DisplayTime

void SegLCD_DisplayError(uint8_t ErrorNum){//Displays Err# on screen, where # is a value 0-F.  If ErrorNum is outside of that range, just displays Err
	LCD->WF8B[LCD_FRONTPLANE0] = (LCD_SEG_D | LCD_SEG_E | LCD_SEG_F | LCD_SEG_G);
	LCD->WF8B[LCD_FRONTPLANE1] = (LCD_SEG_A);
	LCD->WF8B[LCD_FRONTPLANE2] = (LCD_SEG_E | LCD_SEG_G);
	LCD->WF8B[LCD_FRONTPLANE3] = (LCD_CLEAR);
	LCD->WF8B[LCD_FRONTPLANE4] = (LCD_SEG_E | LCD_SEG_G);
	LCD->WF8B[LCD_FRONTPLANE5] = (LCD_CLEAR);
	if(ErrorNum < 0x10){
		SegLCD_Set(ErrorNum,4); //Display ErrorNum in digit 4 if within valid range.  If not, leave blank.
	}
	else{
		LCD->WF8B[LCD_FRONTPLANE6] = (LCD_CLEAR);
		LCD->WF8B[LCD_FRONTPLANE7] = (LCD_CLEAR);
	}
}//End SegLCD_DisplayError

//SegLCD_DP1_On() defined as macro in Seg_LCD.h, Turns on leftmost decimal without disturbing rest of display
//SegLCD_DP1_Off() defined as macro in Seg_LCD.h, Turns off leftmost decimal without disturbing rest of display
//SegLCD_DP2_On() defined as macro in Seg_LCD.h, Turns on center decimal without disturbing rest of display
//SegLCD_DP2_Off() defined as macro in Seg_LCD.h, Turns off center decimal without disturbing rest of display
//SegLCD_DP3_On() defined as macro in Seg_LCD.h, Turns on rightmost decimal without disturbing rest of display
//SegLCD_DP3_Off() defined as macro in Seg_LCD.h, Turns off rightmost decimal without disturbing rest of display
//SegLCD_Col_On() defined as macro in Seg_LCD.h, Turns on colon without disturbing rest of display
//SegLCD_Col_Off() defined as macro in Seg_LCD.h, Turns off colon without disturbing rest of display

/******************************************************************/
//						End Functions for Manipulation of LCD								//
/******************************************************************/
