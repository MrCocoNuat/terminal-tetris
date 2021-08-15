#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#define matrixRows 22
#define matrixCols 10

enum mino {RESERVED, I, O, T, S, Z, J, L, BLANK, GREY};
//determines color of blocks in matrix and current mino

//static const int PIECES_Y[8][4] =
//  {{0,0,0,0},{1,1,1,1},{1,1,2,2},{1,1,1,2},
//   {1,1,2,2},{1,1,2,2},{1,1,1,2},{1,1,1,2}};
//standard starting positions for each piece
//static const int PIECES_X[8][4] =
//  {{0,0,0,0},{1,2,3,4},{2,3,2,3},{1,2,3,2},
//   {1,2,2,3},{2,3,1,2},{1,2,3,1},{1,2,3,3}};
//in the standard order null, I, O, T, S, Z, J, L

static const int PIECES_Y[8][4] =
  {{0,0,0,0},{1,1,1,1},{1,1,2,2},{1,1,2,1},
   {1,1,2,2},{1,1,2,2},{1,1,1,2},{1,1,1,2}};
//standard starting positions for each piece, first block is true center
static const int PIECES_X[8][4] =
  {{0,0,0,0},{2,1,3,4},{2,3,2,3},{2,3,2,1},
   {2,1,2,3},{2,3,1,2},{2,1,3,1},{2,1,3,3}};
//in the standard order null, I, O, T, S, Z, J, L

void initMatrix(void); //initialize the matrix and draw the border
void initPreview(int n); //initialize preview and set previews to N minos
void initHold(void); //duh

void drawMatrix(void); //draws the matrix
void drawMino(void); //draws current piece
void drawGhost(void); //draws a colored ghost piece
void drawPreview(void); //draws the next n pieces
void drawHold(int canHold); //draws hold queue
void refreshMatrix(void); //pushes all drawings to terminal

void newMino(int force);
//create a new piece if not already existing
//picks from the bag of 7 unless force is specified as not 0,
//then picks that without removing from the bag

int gravity(int locking);
//resolve any pending lineclears by calling lineclear(1)
//drop the piece by one unit if possible,
//if can't drop anymore and locking is enabled, then lock it
//returns whether the piece dropped

int tspin(int update);
//updates by gravity() on every lock,
//returns whether the last lock was a tspin, by the "immobile" rule
//IMPORTANT, tspin minis do not exist under this rule!
//can only change values after a call with update enabled
//IMPORTANT, does not consider at all what inputs led to that state
//if using 3corner, calling function should also know the inputs and score accordingly

int lineclear(int clearing);
//handle lineclears and return how many there are (1..4)
//only actually clears if clearing is enabled,
//else just greys out full lines and counts them

int perfectClear(void);
//returns whether the matrix is totally empty

int topout(void);
//returns whether the matrix has overflowed (you lose)

int shift(int dx); //shift the piece left/right if possible, returns success

int rotate(int dir, int floorkick);
//rotate the piece dir = 1:CW -1:antiCW if possible, returns success. Uses Super Rotation System standard
//if floorkick is disabled, rotate will refuse to floorkick the mino

int floorkick();
//updated by rotate()
//returns whether the last rotation was a floorkick


int hold(void); //swaps mino on matrix with mino in hold. Spawns new mino, returns success

#endif
