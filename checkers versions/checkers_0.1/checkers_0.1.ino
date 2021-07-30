//Programmer: Andrew J Burgess

//Just making checkers before chess

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;      // hard-wired for UNO shields anyway
#include "TouchScreen.h"

char *name = "Please Calibrate."; //edit name of shield
const int16_t XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9341
const int16_t TS_LEFT = 133, TS_RT = 921, TS_TOP = 79, TS_BOT = 888;

/*
  PORTRAIT  CALIBRATION     240 x 320
  x = map(p.x, LEFT=133, RT=921, 0, 240)
  y = map(p.y, TOP=79, BOT=888, 0, 320)

  LANDSCAPE CALIBRATION     320 x 240
  x = map(p.y, LEFT=79, RT=888, 0, 320)
  y = map(p.x, TOP=921, BOT=133, 0, 240)
*/

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 200
#define MAXPRESSURE 1000

//int16_t BOXSIZE;
int16_t PENRADIUS = 1;
int16_t ID, oldcolor, currentcolor;
int8_t Orientation = 0; //PORTRAIT

//--------- Game Global Variables
int gameMoves = 1;
int checkerBoard[8][8] = {};
int oldRow = 0;
int oldCol = 0;
int row_right = 0;
int row_left = 0;
int add_col = 0;

int16_t eatCoordL[] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
int16_t eatCoordR[] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
int pickMove = 0;

  int16_t arrSize = 8;
  int16_t rrow1 = -1;
  int16_t rcol1 = -1;
  int16_t rrow2 = -1;
  int16_t rcol2 = -1;
  int16_t rrow3 = -1;
  int16_t rcol3 = -1;
  int16_t rrow4 = -1;
  int16_t rcol4 = -1;
  
  int16_t lrow1 = -1;
  int16_t lcol1 = -1;
  int16_t lrow2 = -1;
  int16_t lcol2 = -1;
  int16_t lrow3 = -1;
  int16_t lcol3 = -1;
  int16_t lrow4 = -1;
  int16_t lcol4 = -1;

//Human-readable names to common 16-bit color values:
#define BLACK     0x0000
#define BLUE      0x001F
#define RED       0xF800
#define GREEN     0x07E0
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define WHITE     0xFFFF

//putting the colors into an array to make them easier to access.
//There are two arrays, reversed so that they can be accessed with
//the isOdd() function
int16_t colorsOdd[] = {
  BLACK, WHITE
};
int16_t colorsEven[] = {
  WHITE, BLACK
};

// You can create a grid of 64 individual squares with this size
// on a 320 x 240 display
int16_t BOXSIZE = 30;

//Returns true or false
boolean isOdd(int cntr) {
  int result = cntr % 2;
  if (result != 0) {
    return true;
  } else {
    return false;
  }
}

//there is a global variable 'gamemoves' that increases by 1 every time a piece is moved.
//This function returns the current active player according to turn value.
int getPlayer() {
  bool result = isOdd(gameMoves);
  switch (result) {
    case true:
      return 1;
    case false:
      return 2;
  }
}

/*-------------------------------------------------------------------------------------
   ---------- CONVERTING RAW PIXEL INPUT INTO 8 X 8 COORDINATES -----------------------
   ------------------------------------------------------------------------------------
*/

//convertPosCoordinate function converts "raw" pixel coordinates into board location
int16_t convertXposCoordinate(int xpos) {
  int16_t row;

  //converting x coordinate
  if (xpos > 0 && xpos < 30) {
    row = 0;
  } else if (xpos > 30 && xpos < 60) {
    row = 1;
  } else if (xpos > 60 && xpos < 90) {
    row = 2;
  } else if (xpos > 90 && xpos < 120) {
    row = 3;
  } else if (xpos > 120 && xpos < 150) {
    row = 4;
  } else if (xpos > 150 && xpos < 180) {
    row = 5;
  } else if (xpos > 180 && xpos < 210) {
    row = 6;
  } else if (xpos > 210 && xpos < 240) {
    row = 7;
  } else {
    row = -1;
  }
  return row;
}

int16_t convertYposCoordinate(int16_t ypos) {
  int16_t column;

  //converting y coordinate
  if (ypos > 40 && ypos < 70) {
    column = 0;
  } else if (ypos > 70 && ypos < 100) {
    column = 1;
  } else if (ypos > 100 && ypos < 130) {
    column = 2;
  } else if (ypos > 130 && ypos < 160) {
    column = 3;
  } else if (ypos > 160 && ypos < 190) {
    column = 4;
  } else if (ypos > 190 && ypos < 220) {
    column = 5;
  } else if (ypos > 220 && ypos < 250) {
    column = 6;
  } else if (ypos > 250 && ypos < 280) {
    column = 7;
  } else {
    column = -1;
  }
  return column;
}

/*-------------------------------------------------------------------------------------
   ------ CONVERTING 8 X 8 COORDINATES INTO TOP LEFT X AND Y COORDINATES FOR TILE -
   ------------------------------------------------------------------------------------
*/

//get box coord functions return the x and y start position for drawrectangle
//you can then use the variable BOXSIZE to complete the rest of the board square.
int16_t getBoxCoordx(int16_t row) {
  if (row >= 0 && row <= 7) {
    switch (row) {
      case 0:
        return 0;
      case 1:
        return 30;
      case 2:
        return 60;
      case 3:
        return 90;
      case 4:
        return 120;
      case 5:
        return 150;
      case 6:
        return 180;
      case 7:
        return 210;
    }
  }
}

int16_t getBoxCoordy(int16_t column) {
  if (column >= 0 && column <= 7) {
    switch (column) {
      case 0:
        return 40;
      case 1:
        return 70;
      case 2:
        return 100;
      case 3:
        return 130;
      case 4:
        return 160;
      case 5:
        return 190;
      case 6:
        return 220;
      case 7:
        return 250;
    }
  }
}


//initializes the graphics for the gameboard. This includes gameboard, pieces, and player side
//identification.
void initBoard() {

  int16_t cntrc = 0;
  boolean odd;

  //These two lines create a nice border on the top and bottom to polish the gameboard
  tft.fillRect(0, 39, 240, 1, WHITE);
  tft.fillRect(0, 280, 240, 1, WHITE);

  //Nested loop to create squares that fill the 240 x 320 display
  //switch statement creates the difference in black and white colors for the board
  //if statements allow for placement of pieces on only black squares
  for (int16_t cntry = 0; cntry <= 7; cntry++) {
    odd = isOdd(cntry);
    for (int16_t cntrx = 0; cntrx <= 7; cntrx++) {
      switch (odd) {
        case true:
          tft.fillRect(BOXSIZE * cntrx, BOXSIZE * cntry + 40, BOXSIZE, BOXSIZE, colorsOdd[cntrc]);
          //if(cntrc == 0)
          //tft.fillCircle((BOXSIZE * cntrx)+(BOXSIZE / 2), (BOXSIZE * cntry+40) +
          //(BOXSIZE / 2)-1, BOXSIZE/2-3 , RED);
          break;
        case false:
          tft.fillRect(BOXSIZE * cntrx, BOXSIZE * cntry + 40, BOXSIZE, BOXSIZE, colorsEven[cntrc]);
          //if(cntrc == 1)
          //tft.fillCircle((BOXSIZE * cntrx)+(BOXSIZE / 2)-1, (BOXSIZE * cntry+40) +
          //(BOXSIZE / 2), BOXSIZE/2-3 , BLUE);
          break;
      }
      //adding an if statement to reset the color index
      if (cntrc < 1) {
        cntrc++;
      } else {
        cntrc = 0;
      }
    }
  }
};

/*------------------------------------------------------------------------------------------
   drawBoard() will draw checker pieces in their represented places from the gameBoard array
   -----------------------------------------------------------------------------------------
*/

void drawBoard() {

  int16_t tileValue = 0;
  int16_t xpix = 0;
  int16_t ypix = 0;
  int16_t color;

  for (int16_t i = 0; i <= 7; i++) {
    for (int16_t j = 0; j <= 7; j++) {

      int16_t tileValue = checkerBoard[i][j];
      xpix = getBoxCoordx(i);
      ypix = getBoxCoordy(j);

      switch (tileValue) {
        case 0:
          break;
        case 1:
          color = BLUE;
          tft.fillCircle(xpix + (BOXSIZE / 2), ypix + (BOXSIZE / 2), (BOXSIZE / 2) - 4, color);
          break;
        case 2:
          color = RED;
          tft.fillCircle(xpix + (BOXSIZE / 2), ypix + (BOXSIZE / 2), (BOXSIZE / 2) - 4, color);
          break;
        case 3:
          color = MAGENTA;
          tft.fillCircle(xpix + (BOXSIZE / 2), ypix + (BOXSIZE / 2), (BOXSIZE / 2) - 4, color);
          break;
        case 4:
          color = YELLOW;
          tft.fillCircle(xpix + (BOXSIZE / 2), ypix + (BOXSIZE / 2), (BOXSIZE / 2) - 4, color);
          break;
      };
    };
  };
};


//This method will return a 0 if the selected gametile is black
//and 1 if the gametile is white. Black gametiles are the only
//usable tiles in checkers.

//the reason that black is 0 and white is 1 is due to the light
//spectrum. black is lack of color while white is all color
//thus {0,0,0}, {255,255,255}
int16_t tile_color(int16_t row, int16_t column) {
  bool row_is_odd = false;
  bool column_is_odd = false;

  row_is_odd = isOdd(row);
  column_is_odd = isOdd(column);

  if (row_is_odd == false && column_is_odd == true) {
    return 0;
  }
  else if (row_is_odd == true && column_is_odd == false) {
    return 0;
  }
  else {
    return 1;
  }
}

//This method takes in an empty array (gameBoard) and assigns checker
//piece values to them
void initCheckerBoard() {
  int16_t tcolor;

  for (int16_t i = 0; i <= 7; i++) {
    for (int16_t j = 0; j <= 7; j++) {
      tcolor = tile_color(i, j);
      if (tcolor == 0 && i <= 2) {
        checkerBoard[j][i] = 1; // P1 piece location
      }
      else if (tcolor == 0 && i >= 5) {
        checkerBoard[j][i] = 2; // P2 piece location
      }
      else {
        checkerBoard[j][i] = 0; //board location contains no pieces
      }
    }
  }
}

//function prints current gameBoard to terminal.
void printBoard() {

  int16_t m = 0;

  for (int16_t k = 0; k <= 7; k++) {
    for (int16_t l = 0; l <= 7; l++) {
      m = checkerBoard[l][k];
      Serial.print(int16_t(m));
    }
    Serial.println();
  }
  Serial.println("");
}

//
//        Serial.print("jumpLeftI, jumpLeftJ: ");
//        Serial.println(jumpRightI, jumpRightJ);// This was the first function that I wrote - I believe I can write something a little less complicated.
//void checkEatable(int row, int col){
//  int16_t player = getPlayer();
//  int16_t tileVal = (-1);
//  int16_t tileValLeft = (-1);
//  int16_t tileValRight = (-1);
//  int16_t jumpTileLeft = 0;
//  int16_t jumpTileRight = 0;
//  int16_t jumpRightI = 0;
//  int16_t jumpRightJ = 0;
//  int16_t jumpLeftI = 0;
//  int16_t jumpLeftJ = 0;
//
//  switch(player){
//    //player 1 scans for eatable pieces from top to bottom
//    case 1:
//    //looping through all tiles starting from 0,0
//    for(int16_t i = 0; i < 7; i++){
//      for(int16_t j = 0; j < 7; j++){
//        tileVal = checkerBoard[i][j]; //assigning tileVal to each tile value
//        if(tileVal == 1){
//          tileValLeft = checkerBoard[i+1][j+1]; //finding value of tiles in front of current tile
//          tileValRight = checkerBoard[i+1][j-1];
//          if(tileValLeft == 2){ //if either right or left tile contain an enemy piece, check if next tile is empty
//            jumpTileLeft = checkerBoard[i+1][j+1];
//            if(jumpTileLeft == 0){ //assign values of where gamepiece can jump to
//              jumpLeftI = i;
//              jumpLeftJ = j;
//            }
//          }else if(tileValRight == 2){
//            jumpTileRight = checkerBoard[i+1][j-1];
//            if(jumpTileRight == 0){
//              jumpRightI = i;
//              jumpRightJ = j;
//            }
//          }
//        }
//        break;
//      }
//    }
//    break;
//    //player 2 scans for eatable pieces from bottom to top
//    case 2:
//    for(int16_t i = 7; i >= 0; i--){
//      for(int16_t j; j < 7; j++){
//        break;
//      }
//    }
//    break;
//  }
//}


void checkEatable() {

  bool canEatR = false;
  bool canEatL = false;
  int16_t currentTile = 10;
  int16_t left1 = 10;
  int16_t left2 = 10;
  int16_t right1 = 10;
  int16_t right2 = 10;
  int16_t coordCntr = 0;
  int16_t player = getPlayer();
  Serial.print("player: ");
  Serial.println(player);
  switch (player) {
    case 1:
      for (int16_t i = 0; i < 8; i++) {
        for (int16_t j = 0; j < 8; j++) {
         // Serial.println(i);
          currentTile = checkerBoard[i][j];
          left1 = checkerBoard[i + 1][j + 1];
          left2 = checkerBoard[i + 2][j + 2];
          right1 = checkerBoard[i - 1][j + 1];
          right2 = checkerBoard[i - 2][j + 2];
          if (currentTile == player && left1 == 2 && left2 == 0 && (j + 2) <= 7) {
            canEatL = true;
            for (int16_t x = 0; x <= 1; x++) {
              switch (x) {
                case 0:
                  eatCoordL[coordCntr] = i + 2;
                  Serial.print("eatCoordL1: ");
                 Serial.println(eatCoordL[coordCntr]);
                  coordCntr++;
                  break;
                case 1:
                  eatCoordL[coordCntr] = j + 2;
                  Serial.print("eatCoordL2: ");
                Serial.println(eatCoordL[coordCntr]);
                  break;
              }
            }
          }
          if(currentTile == player && right1 == 2 && right2 == 0 && (j-2) >= 0){
            canEatR = true;
            for (int16_t x = 0; x <= 1; x++) {
              switch (x) {
                case 0:
                  eatCoordR[coordCntr] = i + 2;
                  Serial.print("eatcoordR1: ");
                  Serial.println(eatCoordR[coordCntr]);
                  coordCntr++;
                  break;
                case 1:
                  eatCoordR[coordCntr] = j + 2;
                  Serial.print("eatcoordR2: ");
                  Serial.println(eatCoordR[coordCntr]);
                  break;
              }
            }
          }
        }
      }

      if (canEatR == true || canEatL == true) {
        if(canEatR == true){
          Serial.println("Right is true");
        }else if(canEatL == true){
          Serial.println("Right is true");
        }
        eatPiece(canEatR, canEatL); 
      }
  }
}

void eatPiece(bool eatR, bool eatL) {

  Serial.print("arraySize: ");
  Serial.println(arrSize);
  
  //assigning coordinates to variables. I am already thining of a better way to do this but we have a program to finish. will refactor later.
  //currently set up to be dynamic with the size of the array with coordinates. thinking I might be able to make it static for simplicity.
  for (int x = 0; x <= 3; x++) {
      if(eatL == true){
        switch (x) {
      case 0:
        lrow1 = eatCoordL[x];
        break;
      case 1:
        lcol1 = eatCoordL[x];
        break;
      case 2:
        lrow2 = eatCoordL[x];
        break;
      case 3:
        lcol2 = eatCoordL[x];
        break;
        case 4:
        lrow3 = eatCoordL[x];
        break;
      case 5:
        lcol3 = eatCoordL[x];
        break;
      case 6:
        lrow4 = eatCoordL[x];
        break;
      case 7:
        lcol4 = eatCoordL[x];
        break;
      }
      }
      if(eatR == true){
      switch (x) {
      case 0:
        rrow1 = eatCoordR[x];
        break;
      case 1:
        rcol1 = eatCoordR[x];
        break;
      case 2:
        rrow2 = eatCoordR[x];
        break;
      case 3:
        rcol2 = eatCoordR[x];
        break;
        case 4:
        rrow3 = eatCoordR[x];
        break;
      case 5:
        rcol3 = eatCoordR[x];
        break;
      case 6:
        rrow4 = eatCoordR[x];
        break;
      case 7:
        rcol4 = eatCoordR[x];
        break;
      }
    }
  }

//Serial.print("eat coordx: ");
//Serial.println(row1);
//Serial.print("eat coordy: ");
//Serial.println(col1);

  while (pickMove == 0) {
  uint16_t xpos, ypos;    //screen coordinates
  tp = ts.getPoint();     //ts.x, tp.y are ADC values
  if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
    //switch statement gives proper orientation for touch sensor
    
Serial.println("im stuck in an infinite loop");

    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);

    switch (Orientation) {
      case 0:
        xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
        ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
        break;
      case 1:
        xpos = map(tp.y, TS_TOP, TS_BOT, 0, tft.width());
        ypos = map(tp.x, TS_RT, TS_LEFT, 0, tft.height());
        break;

      case 2:
        xpos = map(tp.x, TS_RT, TS_LEFT, 0, tft.width());
        ypos = map(tp.y, TS_BOT, TS_TOP, 0, tft.height());
        break;
      case 3:
        xpos = map(tp.y, TS_BOT, TS_TOP, 0, tft.width());
        ypos = map(tp.x, TS_LEFT, TS_RT, 0, tft.height());
        break;
    }
  }
  
  int xRow = convertXposCoordinate(xpos);
  int xCol = convertYposCoordinate(ypos);

Serial.println(xRow);
Serial.println(rrow1);
Serial.println(xCol);
Serial.println(rcol1);
  
//its clunky but it works - compares first four variables assigned to eatCoord for eating left pieces and last four for eating right
  if (xRow == lrow1 && xCol == lcol1 && eatL == true || xRow == lrow2 && xCol == lcol2 && eatL == true) {
    checkerBoard[xRow][xCol] = 1;
    checkerBoard[xRow - 2][xCol - 2] = 0;
    checkerBoard[xRow - 1][xCol - 1] = 0;
    for (int z = 0; z <= 3; z++) {
    eatCoordL[z] = (10);
    }
   lrow1 = 10;
   lcol1 = 10;
   lrow2 = 10;
   lcol2 = 10;
   //Serial.println(row1);
    gameMoves++;
    break;
  }else if(xRow == rrow1 && xCol == rcol1 && eatR == true || xRow == rrow2 && xCol == rcol2 && eatR == true){
    checkerBoard[xRow][xCol] = 1;
    checkerBoard[xRow + 2][xCol + 2] = 0;
    checkerBoard[xRow + 1][xCol + 1] = 0;
    for (int z = 0; z <= 3; z++) {
    eatCoordR[z] = (10);
    }
    rrow1 = 10;
    rcol1 = 10;
    rrow2 = 10;
    rcol2 = 10;
    gameMoves++;
    break;
  }
  delay(250);
  }
   drawBoard();
   printBoard();

}

/*------------------------------------------------------------------------------------------
   movePiece() is an integral part of the logic to this program. This function is how gamepieces
   move from tile to tile AFTER the board has been checked for opposing pieces to be eaten.
   -----------------------------------------------------------------------------------------
*/
void movePiece(int row, int col) {

  int player = getPlayer();
//  Serial.print("player: ");
//  Serial.println(player);

  //logic does not allow for the spontaneous generation of new pieces, and verifies that a valid
  //piece is contained on tile before selected piece is moved to new tile. Logic also verifies
  //that a move is valid (add_col and row_right/row_left are cooresponding variables)
  if (checkerBoard[row + 1][col - 1] == player && col == add_col && row == row_right && player == 1 && checkerBoard[row][col] != 2 && checkerBoard[row + 1][col - 1] != 0) {
    Serial.println("P1 moved right");
    checkerBoard[row + 1][col - 1] = 0;
    checkerBoard[row][col] = 1;
    //If a piece is moved, row and col are reset because otherwise it makes it possible to move with
    //only one input rather than the required 2 with current game structure.
    row = 0;
    col = 0;
    gameMoves++;
  } else if (checkerBoard[row - 1][col - 1] == player && col == add_col && row == row_left && player == 1 && checkerBoard[row][col] != 2 && checkerBoard[row - 1][col - 1] != 0) {
    Serial.println("P1 moved left");
    checkerBoard[row - 1][col - 1] = 0;
    checkerBoard[row][col] = 1;
    row = 0;
    col = 0;
    gameMoves++;
  }

  //keep in mind row right is in perspective of P1 so row left is moving piece right.
  if (checkerBoard[row - 1][col + 1] == player && player == 2 && col == add_col - 2 && row == row_left && checkerBoard[row][col] != 1 && checkerBoard[row - 1][col + 1] != 0) {
    Serial.println("P2 moved right");
    checkerBoard[row - 1][col + 1] = 0;
    checkerBoard[row][col] = 2;
    row = 0;
    col = 0;
    gameMoves++;
  } else if (checkerBoard[row + 1][col + 1] == player && player == 2 && col == add_col - 2 && row == row_right && checkerBoard[row][col] != 1 && checkerBoard[row + 1][col + 1] != 0) {
    Serial.println("P2 moved left");
    checkerBoard[row + 1][col + 1] = 0;
    checkerBoard[row][col] = 2;
    row = 0;
    col = 0;
    gameMoves++;
  }

  initBoard();
  drawBoard();

  oldRow = row;
  oldCol = col;

  row_right = row - 1;
  row_left = row + 1;
  add_col = col + 1;

//
//  Serial.print("game Move: ");
//  Serial.println(gameMoves);
//
//  Serial.print("Old coords:  ");
//  Serial.print(oldRow);
//  Serial.print(" , ");
//  Serial.println(oldCol);
//
//  Serial.print("move right coord:  ");
//  Serial.print(row_right);
//  Serial.print(" , ");
//  Serial.println(add_col);
//
//  Serial.print("move left coord:  ");
//  Serial.print(row_left);
//  Serial.print(" , ");
//  Serial.println(add_col);
//
//  Serial.println();
}

//This code runs once
void setup() {
  tft.reset();
  //ID = tft.readID();
  ID = 0x9341; //this one works the same as tft.readID()
  //ID = 0x8357; this one does some inverted colors which is neat
  tft.begin(ID);
  Serial.begin(9600);
  tft.setRotation(Orientation);
  tft.fillScreen(BLACK);
  Serial.println("BEGIN");

  initBoard();  //init graphics
  initCheckerBoard(); //init backend
  printBoard(); //print board to terminal
  drawBoard(); //draw checker piece graphics
  //tft.reset();
  //tft.drawRect(0, 0, BOXSIZE, BOXSIZE, GREEN);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t xpos, ypos;    //screen coordinates
  tp = ts.getPoint();     //ts.x, tp.y are ADC values
  int16_t curRow, oldRow, curCol, oldCol;
  boolean screenPress = false;

  

  if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
    //switch statement gives proper orientation for touch sensor

    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);

    switch (Orientation) {
      case 0:
        xpos = map(tp.x, TS_LEFT, TS_RT, 0, tft.width());
        ypos = map(tp.y, TS_TOP, TS_BOT, 0, tft.height());
        break;
      case 1:
        xpos = map(tp.y, TS_TOP, TS_BOT, 0, tft.width());
        ypos = map(tp.x, TS_RT, TS_LEFT, 0, tft.height());
        break;

      case 2:
        xpos = map(tp.x, TS_RT, TS_LEFT, 0, tft.width());
        ypos = map(tp.y, TS_BOT, TS_TOP, 0, tft.height());
        break;
      case 3:
        xpos = map(tp.y, TS_BOT, TS_TOP, 0, tft.width());
        ypos = map(tp.x, TS_LEFT, TS_RT, 0, tft.height());
        break;

    }

    tft.fillCircle(xpos, ypos, 1, GREEN);

    curRow = convertXposCoordinate(xpos);
    curCol = convertYposCoordinate(ypos);

//    Serial.println("Input Coordinates: ");
//    Serial.print("row ");
//    Serial.print(curRow);
//    Serial.print(" column ");
//    Serial.println(curCol);

    checkEatable();
    movePiece(curRow, curCol);

    printBoard();
    //drawBoard();
    delay(250);
  } else;
}