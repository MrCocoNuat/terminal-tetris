#include "interface.h"

#include <ncurses.h>

WINDOW *interfaceWin;
WINDOW *timeWin;

void initInterface(void){
  interfaceWin = newwin(12,13,11,16); //create a window for the preview
  wborder(interfaceWin,
	  ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD,
	  ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD); //draw a box around it
}

void initTime(void){
  timeWin = newwin(6,14,6,16); //create a window for the preview
  wborder(timeWin,
	  ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD,
	  ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD); //draw a box around it
}

void drawTime(int micros){ //shows minutes, seconds, centiseconds
  mvwprintw(timeWin, 2, 5, "TIME");
  mvwprintw(timeWin, 3, 3, "%02d:%02d.%02d",
	    micros/60000000, micros/1000000 % 60, micros / 10000 % 100);
}

int scoreShowing = 0; //whether score is being shown, not technique
void drawScore(int score){
  if (!scoreShowing)//blank out the score rows
    for(int row = 2; row < 4; row++)
      mvwprintw(interfaceWin, row, 1, "           ");
    
  mvwprintw(interfaceWin, 2, 4, "SCORE");
  mvwprintw(interfaceWin, 3, 2, "%09d", score);
  scoreShowing = 1;
}

void drawTechnique(int btb, int tspin, int lines, int pclear, int combo){
//blank out the score rows
  for(int row = 2; row < 4; row++)
    mvwprintw(interfaceWin, row, 1, "           ");
    
  if (btb > 1){
    if (pclear) mvwprintw(interfaceWin, 2, 1, "B/B PERFECT");
    else mvwprintw(interfaceWin, 2, 2, "BACK/BACK");
  }
  else if (pclear) mvwprintw(interfaceWin, 2, 3, "PERFECT");
  
  static char *techTable[] = {"SPIN...","SINGLE!","DOUBLE!","TRIPLE!","TETRIS!"};
  int comboShift = 0;
  if (lines > 0 && combo - 1 > 0){
    comboShift = 1;
    wattrset(interfaceWin, COLOR_PAIR(12));
    mvwprintw(interfaceWin, 3, 9+tspin, "+%d", combo - 1); //doesn't really work for combo > 10...
    wattrset(interfaceWin, COLOR_PAIR(0));
  }

  if (tspin)
    mvwprintw(interfaceWin, 3, 2-comboShift, "T-%s", techTable[lines]);
  else if (lines > 0)
    mvwprintw(interfaceWin, 3, 3-comboShift,  "%s", techTable[lines]);

  scoreShowing = 0;
} //draws into where the score would be

int levelShowing = 0;
void drawLevel(int level){
  if (!levelShowing) //blank out the level rows
    for(int row = 5; row < 7; row++)
      mvwprintw(interfaceWin, row, 1, "           ");
  
  mvwprintw(interfaceWin, 5, 4, "LEVEL");
  mvwprintw(interfaceWin, 6, 5, "L%02d", level);
  levelShowing = 1;
}

void drawLevelup(int levelBefore, int levelAfter){
  //blank out the level rows
  for(int row = 5; row < 7; row++)
    mvwprintw(interfaceWin, row, 1, "           ");
  mvwprintw(interfaceWin, 5, 2, "LEVEL UP!");
  mvwprintw(interfaceWin, 6, 2, "L%02d > L%02d", levelBefore, levelAfter);
  levelShowing = 0;
}

void drawLines(int lines){
  mvwprintw(interfaceWin, 8, 4, "LINES");
  mvwprintw(interfaceWin, 9, 4, "%05d", lines);
}

void refreshInterface(void){
  wnoutrefresh(timeWin);
  wnoutrefresh(interfaceWin);
}
