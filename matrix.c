#include "matrix.h"

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

WINDOW *matrixWin;
WINDOW *previewWin;
WINDOW *holdWin;

int matrix[matrixRows][matrixCols];
//the tetris matrix is 22 tall and 10 wide
//only the bottom 20 rows should ever be displayed

int minoNow = BLANK;
//the current mino
int minoY[4] = {0,0,0,0};
int minoX[4] = {0,0,0,0};
//a list of the coordinates of blocks making the mino
//in rotation 0, blocks are guaranteed to be stored in left-to-right within down-to-up row order
//when rotated, they keep the same relative order
int minoRotation = 0;
//the rotation of the mino, 0 is spawn position, +1 for each clockwise 90deg rotation,

int bag[] = {1,2,3,4,5,6,7,1,2,3,4,5,6,7,1,2,3,4,5,6,7}; //sequence of pieces
//storing 3 bags is necessary because 0-6 is the bag, 7-13 (nextbag) can be previewed, and 14-20 (nextnextbag) is storage
int bagIndex = 7; //if >=7, time to refill bag, then set index to 0

int previewCount = 0; //how many minos in preview, no more than 5

int holdNow = BLANK; //which mino is in hold. starts at BLANK

void initMatrix(void){
  time_t t;
  srand((unsigned) time(&t)); //seed the PRNG for bag refilling

  for (int b = 0; b < 3; b ++){ //initialize the bag(s)
    for (int x = 0; x < 7; x++){
      int temp = bag[b*7 + x];
      int ran = b*7 + rand() % (7-x) + x;
      bag[b*7 + x] = bag[ran];
      bag[ran] = temp;
    }
  }
  
  for(int i = 0; i < matrixRows; i++){ //initialize the matrix
    for(int j = 0; j < matrixCols; j++){
      matrix[i][j] = BLANK; //interior
    }
  }
  
  matrixWin = newwin(22,22,1,29); //create a window for the matrix

  wborder(matrixWin,
	  ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD,
	  ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD); //draw a box around it
}

void initPreview(int n){
  if (n <= 0) return; //do nothing
  previewCount = (n > 5)? 5 : n; //constrain previews to <= 5 
  
  previewWin = newwin(2 + 4*previewCount,14,1,50); //create a window for the preview

  wborder(previewWin,
	  ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD,
	  ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD); //draw a box around it
  mvwprintw(previewWin, 0, 5, "NEXT"); //label it
}

void initHold(void){
  holdWin = newwin(6,14,1,16); //create a window for the preview
  wborder(holdWin,
	  ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD,
	  ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD, ACS_CKBOARD); //draw a box around it
  mvwprintw(holdWin, 0, 5, "HOLD"); //label it
}

void drawMatrix(void){
  for (int row = 0; row < matrixRows - 2; row++){ //don't draw top 2 rows
    wmove(matrixWin,20 - row,1);
    for (int col = 0; col < matrixCols; col++){
      wattrset(matrixWin, COLOR_PAIR(matrix[row][col])); //for invisible mode change here...
      if (matrix[row][col] == GREY){ //grey lineclears are drawn as white instead for contrast
	wprintw(matrixWin, "  ");
      }
      else{
	waddch(matrixWin, ACS_CKBOARD);
	waddch(matrixWin, ACS_CKBOARD);
      }
      //each block is two side-by-side characters wide
    }
  }
}

void drawMino(void){
  if (minoNow == BLANK) return;
  
  wattrset(matrixWin, COLOR_PAIR(minoNow));
  for (int block = 0; block < 4; block++){
    if (minoY[block] > 19) continue; //don't draw top 2 rows
    mvwprintw(matrixWin,20 - minoY[block], 2*minoX[block] + 1, "  ");
    //each block is two side-by-side characters wide
  }
}

int ghostValid = 0; //don't recalculate ghost if it is still valid, i.e. no moves or new minos, only gravity and drops
int ghostdY = 0; //how much lower should the ghost be drawn relative to the mino? nonnegative values
void drawGhost() {
  if (minoNow == BLANK) return; //don't draw that
  if (!ghostValid){ //must recalculate ghost
    int collided = 0;
    for (ghostdY = 0; !collided; ghostdY++){
      for (int block = 0; block < 4; block++){
	collided |= (minoY[block] - ghostdY < 0) || (matrix[minoY[block] - ghostdY][minoX[block]] != BLANK);
      }
    }
    ghostValid = 1;
    ghostdY-= 2; //for loop sets this two too far by its nature
    //printw("%d",ghostdY);
  }

  wattrset(matrixWin, COLOR_PAIR(10 + minoNow)); //uses colored-foreground pairs, 11-18
  for (int block = 0; block < 4; block++){ //draw ghost
    if (minoY[block] - ghostdY > 19) continue; //don't draw top 2 rows
    mvwprintw(matrixWin,20 - (minoY[block] - ghostdY), 2*minoX[block] + 1, "[]");
  }
}

int previewValid = 0; //is preview drawing still valid?
void drawPreview(void){
  if (previewCount == 0) return;
  if (!previewValid){
    wattrset(previewWin, COLOR_PAIR(0));
    for (int line = 1; line < 4*previewCount + 1; line++){ //blank preview window
      mvwprintw(previewWin, line, 1, "            ");
    }
    for (int preview = 0; preview < previewCount; preview++){ //draw previews
      int previewMino = bag[bagIndex + preview];
      wattrset(previewWin, COLOR_PAIR(previewMino));
      for (int block = 0; block < 4; block++){
	mvwprintw(previewWin,4*preview + 4 - PIECES_Y[previewMino][block], 2*PIECES_X[previewMino][block] + 1, "  ");
      }
    }
  }
}

int holdValid = 0; //is hold drawing still valid?
void drawHold(int canHold){
  if (!holdValid){ //if drawing is no longer valid
    wattrset(holdWin, COLOR_PAIR(0));
    for(int line = 1; line < 5; line++){ //blank hold window
      mvwprintw(holdWin, line, 1, "            ");
    }
    if (holdNow != BLANK){ //redraw
      wattrset(holdWin, canHold? COLOR_PAIR(holdNow) : COLOR_PAIR(GREY)); //color pair GREY is white background
      for (int block = 0; block < 4; block++){
	if (canHold) mvwprintw(holdWin,4 - PIECES_Y[holdNow][block], 2*PIECES_X[holdNow][block] + 1, "  ");
	else{ //if cannot hold (pressed hold and hasn't locked yet), draw the hold piece greyed out
	  mvwaddch(holdWin, 4 - PIECES_Y[holdNow][block], 2*PIECES_X[holdNow][block] + 1, ACS_CKBOARD);
	  waddch(holdWin, ACS_CKBOARD);
	} 
      }
    }

    holdValid = 1;
  }
}

void refreshMatrix(void){
  wnoutrefresh(matrixWin);
  wnoutrefresh(holdWin);
  wnoutrefresh(previewWin);
}

void newMino(int force){
  if (minoNow != BLANK) return; //can't create mino if one already exists
 
  if (bagIndex == 7){
    //rotate nextbag into bag, nextnextbag into nextbag, etc, then permute nextnextbag
    //this enables piece previews even when the 7-piece bag is emptied
    //2 full bags should be ready at any moment for the previews to be correct
    
    for (int x = 0; x < 7; x++){
      int temp = bag[x];
      bag[x] = bag[x+7];
      bag[x+7] = bag[x+14];
      bag[x+14] = temp;
    }
    for (int x = 0; x < 7; x++){
      int temp = bag[14 + x];
      int ran = 14 + rand() % (7-x) + x;
      bag[14 + x] = bag[ran];
      bag[ran] = temp;
    }
    bagIndex = 0;
  }

  if (force == BLANK) //has a specific mino not been requested?
    minoNow = bag[bagIndex++]; //pick from the bag
  else minoNow = force; //requested specific mino, don't pick from bag
  
  memcpy(minoY, PIECES_Y[minoNow], 4 * sizeof(int)); //copy positions into current position
  memcpy(minoX, PIECES_X[minoNow], 4 * sizeof(int));
  for (int block = 0; block < 4; block++){ //apply offsets to position pieces correctly in the matrix
    minoY[block] += 20; //20 up from standard position
    minoX[block] += 2; //2 right from standard position
  }
  minoRotation = 0; //set initial rotation state

  ghostValid = 0; //invalidate ghost
  previewValid = 0; //invalidate preview
  gravity(0); //by standards the new piece should immediately drop once if able to

  /*static int hi = 1;
  if (hi) mvprintw(25,--hi,"");
  printw("bag: ");
  for (int i = 0; i < 21; i++){
    printw("%d ",bag[i]);
  }
  printw("index: %d\n",bagIndex);
  */
}

int gravity(int locking){
  if (minoNow == BLANK) return 0;
  
  int canDrop = 1;
  for (int block = 0; block < 4; block++){
    canDrop &= (minoY[block] > 0) && (matrix[minoY[block] - 1][minoX[block]] == BLANK);
  }
  if (canDrop){ //drop the mino
    for (int block = 0; block < 4; block++){
      minoY[block]--;
    }
    ghostdY--; //the mino moves one closer to the ghost
  }
  else if (locking){ //lock it if locking is enabled
    tspin(1); //update tspin()
    for (int block = 0; block < 4; block++){
      matrix[minoY[block]][minoX[block]] = minoNow;
    }
    minoNow = BLANK;
    holdValid = 0; //invalidates because now hold can be pressed again
  }
  return canDrop;
}

int tspin(int update){ //uses "immobile" tspin check, which doesn't have the problems of 3corner
  static int tspinType = 0;

  if (update){
    if (minoNow == T){
      int immobile = 1;
      int dy[] = {1,0,-1,0};
      int dx[] = {0,1,0,-1};
      for (int direction = 0; direction < 4; direction++){
	int directionMobile = 1;
	for (int block = 0; block < 4; block++){
	  directionMobile &= ((minoY[block] + dy[direction] >= 0)
			      && (minoY[block] + dy[direction] < matrixRows)
			      && (minoX[block] + dx[direction] >= 0)
			      && (minoX[block] + dx[direction] < matrixCols)
			      && (matrix[minoY[block] + dy[direction]][minoX[block] + dx[direction]] == BLANK)
			      );
	}
	immobile &= (!directionMobile);
	if (!immobile) break;
      }
      return tspinType = immobile;
    }
    else tspinType = 0;
  }
  
  return tspinType;
}

int lineclear(int clearing){
  int lines = 0;
  for (int line = 0; line < matrixRows; line++){
    int cleared = 1;
    for (int block = 0; block < matrixCols; block++){ 
      cleared &= (matrix[line][block] != BLANK); 
    }
    if (cleared){ //if clearing is enabled, all higher lines must fall one unit, just copy from line above
      lines++;
      if (clearing){
	for (int newLine = line--; newLine < matrixRows - 1; newLine++){ //decrement line!!
	  for (int block = 0; block < matrixCols; block++){
	    matrix[newLine][block] = matrix[newLine + 1][block];
	  }
	}
	for (int block = 0; block < matrixCols; block++){ //blank the uppermost row 
	  matrix[matrixRows - 1][block] = BLANK;
	}
      }
      else{ //if not, grey out the cleared lines only
	for (int block = 0; block < matrixCols; block++){
	  matrix[line][block] = GREY;
	}
      }
    }
  }
  return lines;
}

int perfectClear(void){
  int perfect = 1;
  for (int block = 0; block < matrixCols; block++){
    perfect &= (matrix[0][block] == BLANK);
  }
  return perfect;
}

int topout(void){ //a single locked block remaining in line 21+ is game over
  int topped = 0;
  for (int block = 0; block < matrixCols; block++)
    topped |= (matrix[21][block] != BLANK);
  return topped;
}

int shift(int dx){
  int canShift = 1;
  for (int block = 0; block < 4; block++){
    canShift &= (minoX[block] + dx >= 0 && minoX[block] + dx < matrixCols) && (matrix[minoY[block]][minoX[block] + dx] == BLANK);
  }
  if (canShift){
    for (int block = 0; block < 4; block++){
      minoX[block] += dx;
    }
    ghostValid = 0; //invalidate ghost
  }
  //printw("%d",canShift);
  return canShift;
}


int floorkicked = 0; //whether the last rotation was a floorkick
/* rotation is performed under the SRS standard, allowing minoes to kick off walls/floors when rotating*/
int rotate(int dir, int floorkick){ //the first block in minoY/X must be true center 
  if (minoNow == O) return 1;
  if (minoNow == BLANK) return 0;

  //the new value of minoRotation if rotation succeeds
  int newRotation = (minoRotation + dir + 4) % 4;
  
  // perform the pure rotation, centered on minoY/X[0] 
  int newMinoY[] = {0,0,0,0};
  int newMinoX[] = {0,0,0,0};
  
  for (int block = 0; block < 4; block++){
    newMinoY[block] = minoY[0] + dir*(minoX[0] - minoX[block]);
    newMinoX[block] = minoX[0] + dir*(minoY[block] - minoY[0]);
  } //at this point the mino still has to be kicked
  
  //establish the rotation offset tables 
  static int JLSTZ_OffsetTableY[4][5] =
    {{0,0,0,0,0},{0,0,-1,2,2},{0,0,0,0,0},{0,0,-1,2,2}};
  static int JLSTZ_OffsetTableX[4][5] =
    {{0,0,0,0,0},{0,1,1,0,1},{0,0,0,0,0},{0,-1,-1,0,-1}};
  static int I_OffsetTableY[4][5] =
    {{0,0,0,0,0},{0,0,0,1,-2},{1,1,1,0,0},{1,1,1,-1,2}};
  static int I_OffsetTableX[4][5] =
    {{0,-1,2,-1,2},{-1,0,0,0,0},{-1,1,-2,1,-2},{0,0,0,0,0}};
  
  for (int test = 0; test < 5; test++){
    //calculate the kick values
    int kY, kX;
    if (minoNow == I){
      kY = I_OffsetTableY[minoRotation][test] - I_OffsetTableY[newRotation][test];
      kX = I_OffsetTableX[minoRotation][test] - I_OffsetTableX[newRotation][test];
    }
    else{
      kY = JLSTZ_OffsetTableY[minoRotation][test] - JLSTZ_OffsetTableY[newRotation][test];
      kX = JLSTZ_OffsetTableX[minoRotation][test] - JLSTZ_OffsetTableX[newRotation][test];
    }

    //check rotation possibility
    int canRotate = 1;
    for(int block = 0; block < 4; block++){
      int newY = newMinoY[block] + kY;
      int newX = newMinoX[block] + kX;
      canRotate &= (newY >= 0 && newY < matrixRows) && (newX >= 0 && newX <  matrixCols) && (matrix[newY][newX] == BLANK);
    }
    if (canRotate){
      if (!floorkick && kY > 0){ //refuse to floorkick if it is disabled
	floorkicked = 0;
	return 0;
      }
      for (int block = 0; block < 4; block++){
	minoY[block] = newMinoY[block] + kY;
	minoX[block] = newMinoX[block] + kX;
      }
      minoRotation = newRotation;
      ghostValid = 0; //invalidate ghost
      floorkicked = kY > 0; //set the floorkicked flag
      return 1;
    }
  }
  return 0;
}

int floorkick(){
  return floorkicked;
}

int hold(void){
  if (minoNow == BLANK) return 0; //no holding
  int held = holdNow;
  holdNow = minoNow;
  minoNow = BLANK;
  newMino(held); //force this mino back

  holdValid = 0; //invalidate hold drawing
  return 1;
}
