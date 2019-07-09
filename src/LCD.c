#include "GLCD.h"

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
		case 1: return Blue;
		case 10: return Cyan;
		// add more colors here:
		default: return 0;
	}
}

int LCDdraw(int position[64][48]){ 
	// iterate matrix
	for (int i=0; i <64; i++) { 
		for (int j=0; j<48; j++) { 
			
			// fill up square
			if(position[i][j]) {
				int off_r = i*5, off_c = j*5;
				GLCD_SetTextColor(codeToColor(position[i][j]));
				for (int a=0; a <5; a++) { 
					for (int b=0; b<5; b++) { 
						GLCD_PutPixel(off_r+a,off_c+b);
					}
				}
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
