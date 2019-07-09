#include <lpc17xx.h>

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
