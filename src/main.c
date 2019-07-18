#include "main.h"
#include "data.h"
#include "peripheral.h"

// global variables declarations
// dimension: 320 * 240, 1 blocks per 5 pixels : 64 * 48
// within the pos:
// 0 - empty
// 1 - body 
// 10 - food

// food size: 1,2,4,5 no more 
unsigned char pos[EDGE_R][EDGE_C] = {0}; 
int speed = 0, direction = 4, size = 4, nSize=1;
int h_r = 22, h_c = 32, t_r = 22, t_c = 33, f_r=22, f_c=10; 
int gas= 0;
bool loose = false;

int init(){ 
	loose = false;
	pos[22][32]='P';
	pos[22][33]='P';
	pos[22][9]='Y';
	pos[22][11]='Y';
	pos[21][10]='Y';
	pos[23][10]='Y';
	lights(nSize);
	f_r=22; 
	f_c=10; 
	speed = 0;
  direction = 4;
  h_r = 22, h_c = 32; 
	return 0;
}

void rip(){
			loose = true;
			LCDoops();
}

int main () { 
	printf("cute \n");
	osKernelInitialize(); // Initialize CMSIS-RTOS	
	LCDinit();	
	POTinit();
	lightInit();
	init(); // init os Kernel 
	Tupleptr t = {&direction, &gas};
	osThreadNew(game, NULL, NULL);    
	osThreadNew(JOYvalue, &t, NULL);
	osThreadNew(setPOT, (void*)&speed, NULL); 
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
			} else if (pos[t_r+i][t_c+j] == 'O') { 
				pos[t_r][t_c]= 1;
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
			return !pos[a][b] && !pos[a-1][b] && !pos[a+1][b]
				&& !pos[a][b+1] && !pos[a][b-1];
	} else if (size ==2){
		return !pos[a][b+1];
	} 
	return !pos[a][b];
}

int processfood(int a, int b, bool action){
	char m = (action)? 'Y': 1;
	if(size==1) {
		if(pos[a][b]!='P' && pos[a][b]!='O')pos[a][b] = m;
	} else if(size==2){
		if(pos[a][b]!='P' &&pos[a][b]!='O')pos[a][b] = m;
		if(pos[a][b+1]!='P'&&pos[a][b+1]!='O')pos[a][b+1] = m;
	} else if (size == 4 || size ==5) {
		if(pos[a][b+1]!='P'&&pos[a][b+1]!='O')pos[a][b+1] = m;
		if(pos[a][b-1]!='P'&&pos[a][b-1]!='O')pos[a][b-1] = m;
		if(pos[a+1][b]!='P'&&pos[a+1][b]!='O')pos[a+1][b] = m;
		if(pos[a-1][b]!='P'&&pos[a-1][b]!='O')pos[a-1][b] = m;
		if (size==5) {
			if(pos[a][b]!='P'&& pos[a][b]!='O')pos[a][b] = m;
		}
	}		
	return 0;
}

int generateFood(){ 
	// getNext Size and compute next next size. 
	size = nSize; 
	printf("new light size %d \n", size);
	nSize = gibRando(1, 5, gas);
	// lights(nSize); // set light
	if (nSize == 3) nSize=1;
	printf("foodsize: %d %d\n", size, nSize);
	lights(nSize);
	// do shit to the LED
	
	
	
	int row = -1, col = -1;
	while (!validFood(row, col)){
		row = gibRando(1, EDGE_R-1, gas);
		col = gibRando(1, EDGE_C-1, gas);
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
		char suppose = pos[h_r][h_c];
		if(h_r>=0 && h_r<EDGE_R && h_c >=0 && h_c<EDGE_C && pos[h_r][h_c] != 'O') pos[h_r][h_c] = 'P';
		else rip();
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
				if (i==prev_h_r){}
				else if(i<EDGE_R && i>=0 && pos[i][h_c]!='O')pos[i][h_c] = 'P';
				else rip();
			}		
			printf("realmoveR\n");
			a = MIN(h_c, prev_h_c),b = MAX(h_c, prev_h_c);
			for (int i =a; i <b; i++){
				if (i==prev_h_c){}
				else if(i<EDGE_C && i>=0 &&  pos[h_r][i]!='O') pos[h_r][i] = 'P';
				else rip();
			}
			
			printf("realmoveL\n");
			LCDdraw();
			generateFood();
			printf("generatefood\n");
		}
		
		
		if(h_r<0 || h_c<0 || h_r>=EDGE_R || h_c >=EDGE_C)rip(); // still need? 
		LCDdraw();
		printf("Speed %d\n", (osKernelGetTickFreq()/speed));
		osDelay(osKernelGetTickFreq()/speed);
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
			if (pos[i][j]==1){
				pos[i][j] = 0;
				tool(i,j, White);
			} else if (pos[i][j]=='P'){
				pos[i][j] = 'O';
				tool(i,j, Blue);
			} else if (pos[i][j]=='Y'){
				pos[i][j] = 'X';
				tool(i,j, Red);
			}
		}	
			
	}
	return 0;	
}

