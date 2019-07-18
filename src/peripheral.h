#ifndef PER
#define PER

#include <lpc17xx.h>
#include "uart.h"
#include "random.h"
#include "GLCD.h"


// LCD stuff
int LCDinit();

// set color for body or food or other shits 
int codeToColor(int xd);

void tool(int i, int j, int color);
int LCDdraw();

int LCDoops();


// POT
int POTinit();




// JoyStick 

void lights (int number);
void lightInit();

#endif