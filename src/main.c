#include "main.h"


// global variables declarations
// dimension: 320 * 240, 1 blocks per 5 pixels : 64 * 48
// within the position:
// 0 - empty
// 1 - body 
// 10 - food
int position[64][48] = {0}; 
int speed;
int direction;
int main () { 
	// osKernelInitialize();                 // Initialize CMSIS-RTOS
	// init kernel, and then init queues
	// test cases here:
	printf("cute \n");
	
	position[0][0]=1;
	position[4][4]=1;
	position[4][5]=1;
	position[4][6]=1;
	position[63][47]=1;
	LCDinit();
	LCDdraw(position);
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

