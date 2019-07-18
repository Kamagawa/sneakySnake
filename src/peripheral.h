#ifndef PER
#define PER

#include "data.h"
#include <lpc17xx.h>
#include "uart.h"
#include "GLCD.h"


// LCD stuff
int LCDinit();

// set color for body or food or other shits 
int codeToColor(int xd);

void tool(int, int, int);
int LCDdraw();

int LCDoops();


// POT
int POTinit();
void setPOT (void *);


// JoyStick 
void JOYvalue(void *);
void lights (int);
void lightInit();

#endif