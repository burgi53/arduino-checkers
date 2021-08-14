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

int16_t eatCoordL[10] = {};
int16_t eatCoordR[10] = {};
int pickMove = 0;


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
  if (result == true) {
    return 1;
  } else {
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
    row = 70;
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
    column = 70;
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

void checkEatable() {

  bool canEatR = false;
  bool canEatL = false;
  int16_t currentTile = 10;
  int16_t left1 = 10;
  int16_t left2 = 10;
  int16_t right1 = 10;
  int16_t right2 = 10;
  int16_t coordCntrR = 0;
  int16_t coordCntrL = 0;
  int16_t player = getPlayer();

  int16_t kingLeft1 = 0;
  int16_t kingLeft2 = 0;
      
  Serial.print("player: ");
  Serial.println(player);
  if (player == 1) {
    for (int16_t i = 0; i < 8; i++) {
      for (int16_t j = 0; j < 8; j++) {
        // Serial.println(i);
        if (i + 1 <= 7 && j + 1 <= 7 && i + 2 <= 7 && j + 2 <= 7 ) {
          currentTile = checkerBoard[i][j];
          left1 = checkerBoard[i + 1][j + 1];
          left2 = checkerBoard[i + 2][j + 2];
          
          kingLeft1 = checkerBoard[i-1][j+1];
          kingLeft2 = checkerBoard[i-2][j+2];

          if (currentTile == player && left1 == 2 && left2 == 0 && (j + 2) <= 7) {

            for (int16_t x = 0; x <= 1; x++) {
              Serial.print("p1 left coord iterations: ");
              Serial.println(x);
              if(x == 0 && currentTile == 1){
                  Serial.print("p1 left coord iterations: ");
                  Serial.println(x);
                  Serial.print("eatCoordL[coordCntr] - x: ");
                  Serial.println(int(eatCoordL[coordCntrL]));
                    eatCoordL[coordCntrL] = i + 2;
                    Serial.print("eatCoordL1: ");
                    Serial.println(eatCoordL[coordCntrL]);
                    coordCntrL++;
              }else if(x == 1 && currentTile == 1){
                  Serial.print("eatCoordL[coordCntr] - y: ");
                  Serial.println(int(eatCoordL[coordCntrL]));
                    eatCoordL[coordCntrL] = j + 2;
                    Serial.print("eatCoordL2: ");
                    Serial.println(eatCoordL[coordCntrL]);
                    coordCntrL++;
              }
            }
            canEatL = true;
          }//else if(currentTile == 4 && kingLeft1 == 2 || kingLeft == 
        }
      }
    }
    coordCntrL = 0;
    //player 1 eat right check and coordinates assigned to eatR array
    for (int16_t i = 0; i < 8; i++) {
      for (int16_t j = 0; j < 8; j++) {
        
        if (i - 1 >= 0 && j + 1 <= 7 && i - 2 >= 0 && j + 2 <= 7) {
          currentTile = checkerBoard[i][j];
          right1 = checkerBoard[i - 1][j + 1];
          right2 = checkerBoard[i - 2][j + 2];
          if (currentTile == player && right1 == 2 && right2 == 0 && (j - 2) >= 0) {

            for (int16_t x = 0; x <= 1; x++) {
              Serial.print("p1 right coord iterations: ");
              Serial.println(x);
              if(x == 0){
                    eatCoordR[coordCntrR] = i - 2;
                    Serial.print("eatcoordR1: ");
                    Serial.println(eatCoordR[coordCntrR]);
                    coordCntrR++;
              }else if(x == 1){
                    eatCoordR[coordCntrR] = j + 2;
                    Serial.print("eatcoordR2: ");
                    Serial.println(eatCoordR[coordCntrR]);
                    coordCntrR++;
              }
            }
            canEatR = true;
          }
        }
      }
    }
    coordCntrR = 0;
  }
  if (player == 2) {
    for (int16_t i = 7; i >= 0; i--) {
      for (int16_t j = 7; j >= 0; j--) {

        if (i - 1 >= 0 && j - 1 >= 0 && i - 2 >= 0 && j - 2 >= 0) {
          currentTile = checkerBoard[i][j];
          left1 = checkerBoard[i - 1][j - 1];
          left2 = checkerBoard[i - 2][j - 2];

          if (currentTile == player && left1 == 1 && left2 == 0 && (j - 2) >= 0) {
            Serial.println("player 2 can eat left");

            for (int16_t x = 0; x <= 1; x++) {

              Serial.print("p2 left coord iterations: ");
              Serial.println(x);
              if (x == 0) {
                if (eatCoordL[coordCntrL] >= 0) {
                  eatCoordL[coordCntrL] = i - 2;
                  Serial.print("eatCoordL1: ");
                  Serial.println(eatCoordL[coordCntrL]);
                  coordCntrL++;
                }
              } else if (x == 1) {
                if (eatCoordL[coordCntrL] >= 0) {
                  eatCoordL[coordCntrL] = j - 2;
                  Serial.print("eatCoordL2: ");
                  Serial.println(eatCoordL[coordCntrL]);
                  coordCntrL++;
                }
              }
            }
            canEatL = true;
          }
        }
      }
    }
    coordCntrL = 0;
    
    for (int16_t i = 7; i >= 0; i--) {
      for (int16_t j = 7; j >= 0; j--) {
        
        if (i + 1 <= 7 && j - 1 >= 0 && i + 2 <= 7 && j - 2 >= 0) {
          //Serial.print("conditions were met for jump right p2 - i: ");
          //Serial.println(i);
          currentTile = checkerBoard[i][j];
          right1 = checkerBoard[i + 1][j - 1];
          right2 = checkerBoard[i + 2][j - 2];

          if (currentTile == player && right1 == 1 && right2 == 0 && (j + 2) <= 7) {
            Serial.println("Player 2 can eat right");

            for (int16_t x = 0; x <= 1; x++) {
              Serial.print("p2 right coord iterations: ");
              Serial.println(x);
              if (x == 0) {
                //if (eatCoordR[coordCntrR] <= 7) {
                  eatCoordR[coordCntrR] = i + 2;
                  Serial.print("eatcoordR1: ");
                  Serial.println(eatCoordR[coordCntrR]);
                  coordCntrR++;
                //}
              } else if (x == 1) {
                //if (eatCoordR[coordCntrR] >= 0) {
                  eatCoordR[coordCntrR] = j - 2;
                  Serial.print("eatcoordR2: ");
                  Serial.println(eatCoordR[coordCntrR]);
                  coordCntrR++;
                //}
              }
            }
            canEatR = true;
          }
        }
      }
    }
    coordCntrR = 0;
  }


  if (canEatR == true || canEatL == true) {
    if (canEatR == true) {
      Serial.println("Right is true");
    }
    if (canEatL == true) {
      Serial.println("Left is true");
    }
    eatPiece(canEatR, canEatL , player);
  }
  canEatR = false;
  canEatL = false;

}

void eatPiece(bool eatR, bool eatL, int player) {

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

  Serial.print("arraySize: ");
  Serial.println(arrSize);

  //assigning coordinates to variables. I am already thining of a better way to do this but we have a program to finish. will refactor later.
  //currently set up to be dynamic with the size of the array with coordinates. thinking I might be able to make it static for simplicity.
  for (int x = 0; x <= 3; x++) {
    //if (eatL == true) {
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
        //        case 4:
        //          lrow3 = eatCoordL[x];
        //          break;
        //        case 5:
        //          lcol3 = eatCoordL[x];
        //          break;
        //        case 6:
        //          lrow4 = eatCoordL[x];
        //          break;
        //        case 7:
        //          lcol4 = eatCoordL[x];
        //          break;
    }
    //}
    //if (eatR == true) {
    switch (x) {
      case 0:
        rrow1 = eatCoordR[x];
        Serial.print("rrow1: ");
        Serial.println(rrow1);
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
        //        case 4:
        //          rrow3 = eatCoordR[x];
        //          break;
        //        case 5:
        //          rcol3 = eatCoordR[x];
        //          break;
        //        case 6:
        //          rrow4 = eatCoordR[x];
        //          break;
        //        case 7:
        //          rcol4 = eatCoordR[x];
        //          break;
    }
    //}
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

    if (player == 1) {
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
      } else if (xRow == rrow1 && xCol == rcol1 && eatR == true || xRow == rrow2 && xCol == rcol2 && eatR == true) {
        checkerBoard[xRow][xCol] = 1;
        checkerBoard[xRow + 2][xCol - 2] = 0;
        checkerBoard[xRow + 1][xCol - 1] = 0;
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
    } else if (player == 2) {
      if (xRow == lrow1 && xCol == lcol1 && eatL == true || xRow == lrow2 && xCol == lcol2 && eatL == true) {
        checkerBoard[xRow][xCol] = 2;
        checkerBoard[xRow + 2][xCol + 2] = 0;
        checkerBoard[xRow + 1][xCol + 1] = 0;
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
      }
      //with this statement - the program enters a loop in which the given coordinates to match - the x coord
      //will become a number that is impossible to reach by using the touch screen.
      else if (xRow == rrow1 && xCol == rcol1 && eatR == true || xRow == rrow2 && xCol == rcol2 && eatR == true) {
        checkerBoard[xRow][xCol] = 2;
        checkerBoard[xRow - 2][xCol + 2] = 0;
        checkerBoard[xRow - 1][xCol + 1] = 0;
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
  if (checkerBoard[row + 1][col - 1] == player && col == add_col && row == row_right && player == 1 && checkerBoard[row][col] != 2 && checkerBoard[row + 1][col - 1] != 0 || checkerBoard[row + 1][col - 1] == 3) {
    Serial.println("P1 moved right");
    if(checkerBoard[row + 1][col - 1] == player){
    checkerBoard[row + 1][col - 1] = 0;
    checkerBoard[row][col] = 1;
    }else if(checkerBoard[row + 1][col - 1] == 3){
    checkerBoard[row + 1][col - 1] = 0;
    checkerBoard[row][col] = 3;
    }
    //If a piece is moved, row and col are reset because otherwise it makes it possible to move with
    //only one input rather than the required 2 with current game structure. no values are contained on 0,0.
    row = 0;
    col = 0;
    gameMoves++;
  } else if (checkerBoard[row - 1][col - 1] == player && col == add_col && row == row_left && player == 1 && checkerBoard[row][col] != 2 && checkerBoard[row - 1][col - 1] != 0 || checkerBoard[row - 1][col - 1] == 3) {
    Serial.println("P1 moved left");
    if(checkerBoard[row - 1][col - 1] == player){
    checkerBoard[row - 1][col - 1] = 0;
    checkerBoard[row][col] = 1;
    }else if(checkerBoard[row - 1][col - 1] == 3){
    checkerBoard[row - 1][col - 1] = 0;
    checkerBoard[row][col] = 3;
    }
    row = 0;
    col = 0;
    gameMoves++;
  }
  //king piece move if conditionals for p1
  else if(checkerBoard[row + 1][col + 1] == 3 && player == 1 && col == add_col - 2 && row == row_right && checkerBoard[row][col] != 1 && checkerBoard[row + 1][col + 1] != 0 && checkerBoard[row][col] != 2){
    Serial.println("P1 king moved back to the right");
    checkerBoard[row + 1][col + 1] = 0;
    checkerBoard[row][col] = 3;
    row = 0;
    col = 0;
    gameMoves++;
  }else if(checkerBoard[row - 1][col + 1] == 3 && player == 1 && col == add_col - 2 && row == row_left && checkerBoard[row][col] != 1 && checkerBoard[row - 1][col + 1] != 0 && checkerBoard[row][col] != 2) {
    Serial.println("P2 moved right");
    checkerBoard[row - 1][col + 1] = 0;
    checkerBoard[row][col] = 3;
    row = 0;
    col = 0;
    gameMoves++;
  }
  //start of p2 board movements
  if (checkerBoard[row - 1][col + 1] == player && player == 2 && col == add_col - 2 && row == row_left && checkerBoard[row][col] != 1 && checkerBoard[row - 1][col + 1] != 0 || checkerBoard[row - 1][col + 1] == 4) {
    Serial.println("P2 moved right");
    if(checkerBoard[row - 1][col + 1] == player){
    checkerBoard[row - 1][col + 1] = 0;
    checkerBoard[row][col] = 2;
    }else if (checkerBoard[row - 1][col + 1] == 4){
    checkerBoard[row - 1][col + 1] = 0;
    checkerBoard[row][col] = 4;
    }
    row = 0;
    col = 0;
    gameMoves++;
  } else if (checkerBoard[row + 1][col + 1] == player && player == 2 && col == add_col - 2 && row == row_right && checkerBoard[row][col] != 1 && checkerBoard[row + 1][col + 1] != 0 || checkerBoard[row + 1][col + 1] == 4) {
    Serial.println("P2 moved left");
    if(checkerBoard[row + 1][col + 1] == player){
    checkerBoard[row + 1][col + 1] = 0;
    checkerBoard[row][col] = 2;
    }else if(checkerBoard[row + 1][col + 1] == 4){
    checkerBoard[row + 1][col + 1] = 0;
    checkerBoard[row][col] = 4;  
    }
    row = 0;
    col = 0;
    gameMoves++;
  }
  //start of p2 king movement if conditionals
  else if(checkerBoard[row + 1][col - 1] == 4 && col == add_col && row == row_right && player == 2 && checkerBoard[row][col] != 2 && checkerBoard[row + 1][col - 1] != 0 && checkerBoard[row][col] != 1) {
    Serial.println("P2 king moved back and left");
    checkerBoard[row + 1][col - 1] = 0;
    checkerBoard[row][col] = 4;
    //If a piece is moved, row and col are reset because otherwise it makes it possible to move with
    //only one input rather than the required 2 with current game structure. no values are contained on 0,0.
    row = 0;
    col = 0;
    gameMoves++;
  }else if(checkerBoard[row - 1][col - 1] == 4 && col == add_col && row == row_left && player == 2 && checkerBoard[row][col] != 2 && checkerBoard[row - 1][col - 1] != 0 && checkerBoard[row][col] != 1) {
    Serial.println("P1 king moved back and right");
    checkerBoard[row - 1][col - 1] = 0;
    checkerBoard[row][col] = 4;
    row = 0;
    col = 0;
    gameMoves++;
  }

  initBoard();
  drawBoard();

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

//checkKing is called in the main loop. it checks for opposing players piece on "home" row and switches
// the value on checkerBoard[][] to the respective players king value - 3 for p1, 4 for p2.
void checkKing(){
  
int player = getPlayer();
int tile = 0;
  for(int x = 0; x <= 1; x++){
    for(int y = 0; y <= 7; y++){
      if(x == 0){
      tile = checkerBoard[y][0];
      if(tile == 2){
        checkerBoard[y][0] = 4;
        initBoard();
        drawBoard();
      }
      }else if(x == 1){
        tile = checkerBoard[y][7];
        if(tile == 1){
          checkerBoard[y][7] = 3;
          initBoard();
          drawBoard();
        }
      }
    }
  }
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
    checkKing();
    movePiece(curRow, curCol);

    printBoard();
    //drawBoard();
    delay(250);
  } else;
}
