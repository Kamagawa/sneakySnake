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
unsigned int marker[EDGE_R][EDGE_C] = {0}; 
int speed = 0, direction = 4, size = 4, nSize=1;
int h_r = 22, h_c = 32, t_r = 22, t_c = 33, f_r=22, f_c=10; 
int gas= 0, snakeSize = 3;
bool loose = false;

int init(){ 
	loose = false;
	//body
	pos[22][32]='P';
	pos[22][33]='P';
	
	//body marker
	marker[22][32]=2;
	marker[22][33]=1;
	
	//food
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
	t_r = 22, t_c = 33;
	return 0;
}

void rip(){
			loose = true;
			LCDoops();
}

void display(){
	for (int i=0; i <EDGE_R; i++) { 
		for (int j=0; j<EDGE_C; j++) {
			printf("%d ", marker[i][j]);
		}
		printf("|| \n");
	}
	
	printf("______\n");
	printf("\n");
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
	osThreadNew(pushDetector, NULL, NULL);
  osKernelStart();                      
  for (;;) {}
	return 0;
}



int snakeTraverser() {
	// assume the last place is  1
	pos[t_r][t_c]= 1;
	marker[t_r][t_c]= 0;
	if(within(t_r, t_c+1)&&marker[t_r][t_c+1]==2) {
		t_c+=1;
	} else if (within(t_r,t_c-1)&&marker[t_r][t_c-1]==2) {
		t_c-=1;
	} else if (within(t_r+1,t_c)&&marker[t_r+1][t_c]==2) {
		t_r+=1;
	} else if (within(t_r-1,t_c)&&marker[t_r-1][t_c]==2) {
		t_r-=1;
	}
	snakeSize--;
	for (int i=0; i <EDGE_R; i++) { 
		for (int j=0; j<EDGE_C; j++) {
			if(marker[i][j]>0) marker[i][j]--;
		}
	}
	return 0;
}

Tuple director(){
	Tuple p = {0,0};
	switch(direction){ 
			case 4: p.y=-1; break;
			case 2: p.y=1; break;
			case 1: p.x=-1; break;
			case 3: p.x=1; break;
		}
	return p;
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
	if (nSize == 3) nSize=1;
	printf("foodsize: %d %d\n", size, nSize);
	lights(nSize); // light up LED to show new size
	
	int row = -1, col = -1; // put food on the table
	while (!validFood(row, col)){
		row = gibRando(1, EDGE_R-1, gas);
		col = gibRando(1, EDGE_C-1, gas);
	}
	f_r = row, f_c=col;
	processfood(row,col, true);
	return 0;
}

void pushDetector(){ 
	bool prevState, curState;
	prevState = false;
	while(true) { 
		curState = !(LPC_GPIO2->FIOPIN & (1 << 10));
		if(curState && !prevState){
			printf("PUSHED YUGE BUTTON");
			processfood(f_r,f_c, false);
			generateFood();
		}
		prevState = curState;
	}
}


void game(){ 
	while(true) { 
		//display();
		int prev_h_r = h_r, prev_h_c = h_c; // 1. get new head
		Tuple t = director();
		h_r+=t.x; h_c+=t.y;
		marker[h_r][h_c] = snakeSize++;
		char suppose = pos[h_r][h_c];
		if(within(h_r,h_c)&& pos[h_r][h_c] != 'O') pos[h_r][h_c] = 'P';
		else rip();
		// printf("%c \n", suppose);
		snakeTraverser();		// 3. move tail
		
		// B. eat
		// check if food is eaten, if eaten update size and food
		
		if(suppose == 'X'){
			printf("EATEN\n");
			processfood(f_r, f_c, false); //1. clear first
			printf("processedFood\n");
			int a = MIN(h_r, prev_h_r),b = MAX(h_r, prev_h_r);
			for (int i =0; i <size; i++){
				h_r+=t.x; h_c+=t.y;
				
				if(within(h_r,h_c) && pos[h_r][h_c]!='O') {
					marker[h_r][h_c] = snakeSize++;
					pos[h_r][h_c] = 'P';
				}
				else rip();
			}		
			printf("realmoveR\n");
			LCDdraw();
			generateFood();
			printf("generatefood\n");
		}
		
		
		if(!within(h_r,h_c))rip(); // lose after all is done but out
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

