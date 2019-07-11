#include "main.h"
#include "peripheral.h"

// global variables declarations
// dimension: 320 * 240, 1 blocks per 5 pixels : 64 * 48
// within the position:
// 0 - empty
// 1 - body 
// 10 - food

// food size: 1,2,4,5 no more 
unsigned char position[EDGE_R][EDGE_C] = {0}; 
int speed = 10, direction = 4, size = 4, nSize=1;
int h_r = 22, h_c = 32, t_r = 22, t_c = 33, f_r=22, f_c=10; 
bool loose = false;

int init(){ 
	loose = false;
	position[22][32]='P';
	position[22][33]='P';
	position[22][9]='Y';
	position[22][11]='Y';
	position[21][10]='Y';
	position[23][10]='Y';
	lights(nSize);
	f_r=22; 
	f_c=10; 
	speed = 10;
  direction = 4;
  h_r = 22, h_c = 32; 
	return 0;
}

int gibRando(int lower, int upper) { 
		// srand(time(0));
    return (rand() % (upper - lower + 1)) + lower; 
}

int main () { 
	osKernelInitialize();                 // Initialize CMSIS-RTOS
	// init kernel, and then init queues
	printf("cute \n");
	LCDinit();	
	POTinit();
	lightInit();
	init();
	
	
	
	// init os Kernel 
	
	
	osThreadNew(game, NULL, NULL);    
	osThreadNew(JOYvalue, NULL, NULL);
/*	
	osThreadNew(monitor, NULL, NULL); 
	osThreadNew(serverB, (void*)&q2, NULL);   
	*/
  osKernelStart();                      
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

// imagine the food as 3*3 blocks
// dangerous but do random
// size is global
bool validFood(int a, int b){ 
	if(a == -1 || b==-1){
		return false;
	} else if(size>2){
			return !position[a][b] && !position[a-1][b] && !position[a+1][b]
				&& !position[a][b+1] && !position[a][b-1];
	} else if (size ==2){
		return !position[a][b+1];
	} 
	return !position[a][b];
}

int processfood(int a, int b, bool action){
	char m = (action)? 'Y': 1;
	if(size==1) {
		position[a][b] = m;
	} else if(size==2){
		position[a][b] = m;
		position[a][b+1] = m;
	} else if (size == 4 || size ==5) {
		position[a][b+1] = m;
		position[a][b-1] = m;
		position[a+1][b] = m;
		position[a-1][b] = m;
		if (size==5) position[a][b] = m;
	} 
	return 0;
}
	
int generateFood(){ 
	// getNext Size and compute next next size. 
	size = nSize; 
	printf("new light size %d \n", size);
	nSize = gibRando(1, 5);
	// lights(nSize); // set light
	if (nSize == 3) nSize=1;
	printf("foodsize: %d %d\n", size, nSize);
	lights(nSize);
	// do shit to the LED
	
	
	
	int row = -1, col = -1;
	while (!validFood(row, col)){
		row = gibRando(1, EDGE_R-2);
		col = gibRando(1, EDGE_C-2);
	}
	f_r = row, f_c=col;
	processfood(row,col, true);
	return 0;
}

void game(){ 
	
	while(true) { 
		// A. move
		int prev_h_r = h_r, prev_h_c = h_c; // 1. get new head
		moveHead(1);		// 2. move head
		char suppose = position[h_r][h_c];
		if(h_r>=0 && h_r<48 && h_c >=0 && h_c<64)
		position[h_r][h_c] = 'P';
		// printf("%c \n", suppose);
		snakeTraverser();		// 3. move tail
		
		// B. eat
		// check if food is eaten, if eaten update size and food
		
		if(suppose == 'X'){
			
			printf("EATEN\n");
			moveHead(size);
			printf("finishMove\n");
			//1. clear first
			processfood(f_r, f_c, false);
			// maybe refresh UI?
			// LCDdraw();
			printf("processedFood\n");
			int a = MIN(h_r, prev_h_r),b = MAX(h_r, prev_h_r);
			for (int i =a; i <b; i++){
				if(i<EDGE_R && i>=0)position[i][h_c] = 'P';
			}		
			printf("realmoveR\n");
			a = MIN(h_c, prev_h_c),b = MAX(h_c, prev_h_c);
			for (int i =a; i <b; i++){
				if(i<EDGE_C && i>=0) position[h_r][i] = 'P';
			}
			
			printf("realmoveL\n");
			//refresh? screen
			LCDdraw();
			generateFood();
			printf("generatefood\n");
				//1. add length of coresponding block
				//2. set all food to blank
				//3. set corresponding snake to block
		}
		
		
		if(h_r<0 || h_c<0 || h_r>=EDGE_R || h_c >=EDGE_C){
			 loose = true;
			 LCDoops();
		}
		
		LCDdraw();
		
		osDelay(osKernelGetTickFreq()/10);
		int man = 1;
		while(man>0){
			if(loose) man = 100;
			man--;
		}
	}	
}

int LCDdraw() { 
	// iterate matrix
	for (int i=0; i <EDGE_R; i++) { 
		for (int j=0; j<EDGE_C; j++) {
			if (position[i][j]==1){
				position[i][j] = 0;
				tool(i,j, White);
			} else if (position[i][j]=='P'){
				position[i][j] = 'O';
				tool(i,j, Blue);
			} else if (position[i][j]=='Y'){
				position[i][j] = 'X';
				tool(i,j, Red);
			}
		}	
			
	}
	return 0;	
}

// JoyStick 
void JOYvalue(){ 
	printf("INTO\n");
	long val = LPC_GPIO1->FIOPIN;
	while (true){
		val = LPC_GPIO1->FIOPIN;
		for (int i =1; i <=4; i++) {
			if(!(val&1<<(22+i))) {
				// direction = i;
				// printf("d: %d, i: %d\n", direction, i);
				if (i==direction || i+2==direction || i-2==direction){
						// printf("REAP\n");
				} else {
					printf("YOLO %d \n", i);
						direction = i;
				}
			}
		}
	}
}