#include "peripheral.h"

// LCD stuff
int LCDinit(){ 
	// init screen 
	GLCD_Init();
	GLCD_Clear(White);	
	GLCD_SetBackColor(White);		
	GLCD_SetTextColor(Blue);
	return 0;
}

// set color for body or food or other shits 
int codeToColor(int xd) { 
	switch(xd){
		case 0: return White;
		case 1: return Blue;
		case 10: return Cyan;
		// add more colors here:
		default: return 0;
	}
}

void tool(int i, int j, int color) { 
	int off_r = i*5, off_c = j*5;
	GLCD_SetTextColor(color);
	for (int a=0; a <5; a++) { 
		for (int b=0; b<5; b++) { 
			GLCD_PutPixel(off_c+b, off_r+a);
		}
	}
}



int LCDoops(){ 
	GLCD_SetTextColor(Red);
	GLCD_DisplayString (4,7,1, "OOPS");
	return 0;
}


// POT
int POTinit() { 
	LPC_PINCON->PINSEL1 &= ~(0x03<<18);
	LPC_PINCON->PINSEL1 |= (0x01<<18);
	LPC_SC->PCONP |= (0x01<<12);
	LPC_ADC->ADCR = (1 << 2) | // select AD0.2 pin
									(4 << 8) | // ADC clock is 25MHz/5
									(1 << 21); // enable ADC
	return 0;
}

// current range 2 - 4092, maybe 10 levels of speed? should there be 0 speed?
int POTvalue(){ 
	  LPC_ADC->ADCR |= (0x01<<24);
		while(! (LPC_ADC-> ADGDR & 0x80000000));
		return (LPC_ADC-> ADGDR & 0xfff0) >> 4;
}




void lightInit(){
	LPC_GPIO1->FIODIR |= 0xB0000000;
		LPC_GPIO2->FIODIR |= 0x0000007C;
}

void lights (int number){ 
		LPC_GPIO2->FIOCLR |= 0x0000007C;
	
		switch(number) {
			case 0: LPC_GPIO2->FIOSET |= 0x00000000; break;
			case 1: LPC_GPIO2->FIOSET |= 0x00000040; break;
			case 2: LPC_GPIO2->FIOSET |= 0x00000060; break;
			case 3: LPC_GPIO2->FIOSET |= 0x00000070; break;
			case 4: LPC_GPIO2->FIOSET |= 0x00000078; break;
			case 5: LPC_GPIO2->FIOSET |= 0x0000007C; break;
		}
		
}
