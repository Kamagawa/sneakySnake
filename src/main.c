#include "main.h"


// global variables declarations
// dimension: 320 * 240, 1 blocks per 5 pixels : 64 * 48
// within the position:
// 0 - empty
// 1 - body 
// 10 - food

// food size: 1,2,4,5 no more 
unsigned char position[EDGE_R][EDGE_C] = {0}; 
int speed = 10, direction = 4, size = 2, nSize=1;
int h_r = 22, h_c = 32, t_r = 22, t_c = 33; 
bool loose = false;

int init(){ 
	loose = false;
	position[22][32]='P';
	position[22][33]='P';
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
	
	
	LCDinit();	
	POTinit();
	
	
	game();
	
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




int snakeTraverser() {
	
	// find in four direction, excluding previous to find tail
	for(int i=-1; i<=1;i++) { 
		for(int j=-1;j<=1;j++) { 
			if ((!i&&!j) || i*i+j*j==2) { 
				// skip 1. (0,0) 2. corners like (1,1) 3. equals last block
				continue;
			} else if (position[t_r+i][t_c+j] == 'O') { 
				position[t_r][t_c]= 1;
				t_r = t_r+i;
				t_c = t_c+j;
			}
		}
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
		// 2. move head
		moveHead(1);
		if(h_r>=0 && h_r<48 && h_c >=0 && h_c<64)
			position[h_r][h_c] = 'P';
		// 3. move tail
		snakeTraverser();
		
		
		
		// check if food is eaten, if eaten update size and food
		if(position[h_r][h_c] == 'X'){
				//1. add length of coresponding block
				//2. set all food to blank
				//3. set corresponding snake to block
				
		}
		
		if(h_r<0 || h_c<0 || h_r>=EDGE_R || h_c >=EDGE_C){
			 loose = true;
			 LCDoops();
		}
		
		LCDdraw();
		
		osDelay(osKernelGetSysTimerFreq()*100);
		int man = 1;
		while(man>0){
			if(loose) man = 100;
			man--;
			printf("R");
		}
		printf("\n");
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

int LCDdraw() { 
	// iterate matrix
	for (int i=0; i <30; i++) { 
		for (int j=0; j<52; j++) {
			if (position[i][j]==1){
				position[i][j] = 0;
				tool(i,j, White);
				// GLCD_DisplayString(i,j,0, &blank);
			} else if (position[i][j]=='P'){
				position[i][j] = 'O';
				tool(i,j, Blue);
				//GLCD_DisplayString(i,j,0, &position[i][j]);
			} else if (position[i][j]=='Y'){
				position[i][j] = 'X';
				tool(i,j, Red);
				//GLCD_DisplayString(i,j,0, &position[i][j]);
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

