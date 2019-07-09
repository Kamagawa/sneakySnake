#include "main.h"


// global variables declarations
// dimension: 320 * 240, 1 blocks per 5 pixels : 64 * 48
// within the position:
// 0 - empty
// 1 - body 
// 10 - food
int position[48][64] = {0}; 
int speed = 10;
int direction = 4;
int h_r = 22, h_c = 32, t_r = 22, t_c = 33; 

int init(){ 
	position[22][32]=1;
	position[22][33]=1;
	speed = 10;
  direction = 4;
  h_r = 22, h_c = 32; 
	return 0;
}

int main () { 
	// osKernelInitialize();                 // Initialize CMSIS-RTOS
	// init kernel, and then init queues
	// test cases here:
	printf("cute \n");
	
	position[22][32]=1;
	position[22][33]=1;
	
	LCDinit();
	LCDdraw();
	LCDoops();
	
	POTinit();
	
	while(1) { 
		/* POT works
		int obtained = POTvalue();
		printf("POT: %d \n", obtained);
		*/ 
		
		/* JOY works */
		int dir = JOYvalue();
		printf("DIR: %d \n", dir);
	}
	
	// init os Kernel 
	
	/*
	osThreadNew(client, (void*)clientargs, NULL);    // Create application main thread
  osThreadNew(monitor, NULL, NULL);    // Create application main thread
	osThreadNew(serverA, (void*)&q1, NULL);    // Create application main thread
	osThreadNew(serverB, (void*)&q2, NULL);    // Create application main thread
  osKernelStart();                      // Start thread execution
	*/
  for (;;) {}
	return 0;
}




int snakeTraverser(int h_r, int h_c, int c_r, int c_c) {
	bool changes = false;
	// find in four direction, excluding previous to find tail
	for(int i=-1; i<=1;i++) { 
		for(int j=-j;i<=j;j++) { 
			if (i||j || i*i+j*j==2 || (c_r+i==h_r && c_c+j ==h_c)) { 
				// skip 1. (0,0) 2. corners like (1,1) 3. equals last block
				continue;
			} else if (position[c_r+i][c_r+j] > 0) { 
				changes = true;
				snakeTraverser(c_r, c_c, c_r+i, c_c+j);
			}
		}
	}
	// tail here 
	if (!changes){
		position[c_r][c_c] = 0;
	}
	return 0;
}

int moveHead(int unit){ 
	switch(direction){ 
			case 4: h_c-=unit; break;
			case 2: h_c+=unit; break;
			case 1: h_r-=unit; break;
			case 3: h_r+=unit; break;
		}
	return unit;
}
int game(){ 
	init();
	
	while(true) { 
		 // 1. first find new head new position: 4, 2. 1 3
		int prev_h_r = h_r, prev_h_c = h_c;
		moveHead(1);
		// 2. move head
		if(h_r>=0 && h_r<48 && h_c >=0 && h_c<64)
			position[h_r][h_c] = 1;
		// 3. move tail
		snakeTraverser(prev_h_c, prev_h_r, h_r, h_c);
		// check if food is eaten, if eaten update size and food
		
	}
	
	
	
	// check error
	return 0;
}

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
		case 1: return Blue;
		case 10: return Cyan;
		// add more colors here:
		default: return 0;
	}
}

int LCDdraw() { 
	// iterate matrix
	for (int i=0; i <48; i++) { 
		for (int j=0; j<64; j++) { 
			
			// fill up square
			if(position[i][j]) {
				int off_r = i*5, off_c = j*5;
				GLCD_SetTextColor(codeToColor(position[i][j]));
				for (int a=0; a <5; a++) { 
					for (int b=0; b<5; b++) { 
						GLCD_PutPixel(off_c+b, off_r+a);
					}
				}
			}
		}
	}
	return 0;
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


// JoyStick 
int JOYvalue(){ 
	long val = LPC_GPIO1->FIOPIN;
	
	for (int i =1; i <=4; i++) {
			if(!(val&1<<(22+i))) return i;
	}
	return 0;
}

