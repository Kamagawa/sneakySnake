#ifndef MAIN
#define MAIN

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <lpc17xx.h>
#include <cmsis_os2.h>
#include <lpc17xx.h>
#include "uart.h"
#include "random.h"
#include "GLCD.h"


int LCDinit();
int LCDdraw();
int LCDoops();

// finish everything later maybe score? maybe edge border? 
int LCDborder();
int score(int score);


int POTinit();
int POTvalue();
void game();

// Joy stick 
void JOYvalue();


#endif


#define EDGE_R 48
#define EDGE_C 64

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

