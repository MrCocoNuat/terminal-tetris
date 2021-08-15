#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

void initInterface(void); //draws an interface window
void initTime(void); //draws a time window


void drawTime(int micros); //pushes these values to screen
void drawScore(int score);
void drawTechnique(int btb, int tspin, int lines, int pclear, int combo);
void drawLevel(int level);
void drawLevelup(int levelBefore, int levelAfter);
void drawLines(int lines); 
//these are all calculated in main.c, since the interface
//is meant to be more flexible than the matrix, for example
void refreshInterface(void);

#endif
