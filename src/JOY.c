#include <lpc17xx.h>

// 1 - left
// 2 - right
// 3 - down
// 4 - up
// may need to re-orientate in the game as right is down relative to screen
int JOYvalue(){ 
	long val = LPC_GPIO1->FIOPIN;
	
	for (int i =1; i <=4; i++) {
			if(!(val&1<<(22+i))) return i;
	}
	return 0;
}
