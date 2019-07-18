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
int LCDborder();

int score(int score);
int POTinit();
int POTvalue();
void game();


#endif



