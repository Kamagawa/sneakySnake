#ifndef LCD
#define LCD
int LCDinit();
int LCDdraw(int position[64][48]);
int LCDoops();

// finish everything later maybe score? maybe edge border? 
int LCDborder();
int score(int score);
#endif /* LCD */
