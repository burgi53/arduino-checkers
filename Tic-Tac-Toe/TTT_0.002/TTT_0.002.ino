//Programmer: Andrew J Burgess
//version 0.002

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;      // hard-wired for UNO shields anyway
#include "TouchScreen.h"
#include "BoardTile.h"

//Human-readable names to common 16-bit color values:
#define BLACK     0x0000
#define BLUE      0x001F
#define RED       0xF800
#define GREEN     0x07E0
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define WHITE     0xFFFF

//these three boardtile arrays are our gameboards
static BoardTile tictactoe[3][3];
//static BoardTile checkers[8][8];
//static BoardTile chess[8][8];

//------------------------------------------------------------------------------------------------------
//------------------------------------- TTT TERMINAL GAME -----------------------------------------------
//------------------------------------------------------------------------------------------------------

//This method takes in one bool argument; if true board is a 3x3(tictactoe), else board is 8x8(chess/checkers)
//method also checks for a winner
void print_board_to_terminal(bool ttt) {

  //these variables determine loop iteration size
  int x = 7;
  int y = 7;

  //these variables hold the player board total
  int p1 = 0;
  int p2 = 0;

  //assigning iteration size for nested loop
  if (ttt) {
    x = 2;
    y = 2;
  }

  //assigning values to board positions to check the win function
  //tictactoe[0][2].setPieceValue(1);
  //tictactoe[1][1].setPieceValue(1);
  //tictactoe[2][0].setPieceValue(1);

  //using a nested loop to print out the game board to the terminal
  for (int i = 0; i <= x; i++) {
    for (int j = 0; j <= y; j++) {

      int val = tictactoe[i][j].getPieceValue();

      //this if else statement finds player total value for the board
      if (val > 0) {
        p1 += val;
      }
      else if (val < 0)
      {
        p2 += val;
      }
      Serial.print(val);

    }
    //using an endle to index to the next line
    Serial.println();
  }
}

//check_ttt_winner simply adds up rows and columns, then checks for a total value of 3 or -3 to then call the
//declare winner function.
int win = -5;
void check_ttt_winner() {
  //declaring variables that will be used for winning value verification
  int x0 = 0;
  int x1 = 0;
  int x2 = 0;
  int y123 = 0;

  //using nested loop to check gameboard values
  for (int i = 0; i <= 2; i++) {
    for (int j = 0; j <= 2; j++) {
      //checking vertical values for game win
      if (j == 0) {
        x0 += tictactoe[i][j].getPieceValue();
        if (abs(x0) == 3) {
          win = 3;
        }
      }
      else if (j == 1) {
        x1 += tictactoe[i][j].getPieceValue();
        if (abs(x1) == 3) {
          Serial.print("x1: ");
          Serial.println(x1);
          win = 4;
        }
      }
      else if (j == 2) {
        x2 += tictactoe[i][j].getPieceValue();
        if (abs(x2) == 3) {
          win = 5;
        }
      }
      //checking horizontal values for a game win
      y123 += tictactoe[i][j].getPieceValue();
      if (y123 == 3) {
        Serial.println("player 1 wins!");
        win = i;
      }
      else if (y123 == -3) {
        Serial.println("player 2 wins!");
        win = i;
      }
//      Serial.println("x0-2:");
//      Serial.println(x0);
//      Serial.println(x1);
//      Serial.println(x2);
    }
    y123 = 0;
  }
  //checking x variables if there is a win for either player
  if (x0 == 3 || x1 == 3 || x2 == 3) {
    Serial.println("player 1 wins!");
  }
  else if (x0 == -3 || x1 == -3 || x2 == -3) {
    Serial.println("player 2 wins!");
    //win = true;
  }
  //resetting variables
  x0 = 0;
  x1 = 0;
  x2 = 0;

  //if there are no horizontal or vertical winners, I reset x0 and x1 to check for diagnal winners
  //first checks top left to bottom right
  x0 = tictactoe[0][0].getPieceValue() + tictactoe[1][1].getPieceValue() + tictactoe[2][2].getPieceValue();
  //second checks bottom left to top right
  x1 = tictactoe[0][2].getPieceValue() + tictactoe[1][1].getPieceValue() + tictactoe[2][0].getPieceValue();

  if (abs(x0) == 3) {
    win = 6;
  } else if (abs(x1) == 3) {
    win = 7;
  }

  //checking for if diagnals produce a win for either player
  if (x0 == 3 || x1 == 3) {
    Serial.println("player 1 wins!");
  }
  else if (x0 == -3 || x1 == -3) {
    Serial.println("player 2 wins!");
  }
}


//ttt() method initializes the tictactoe gameboard
void init_ttt() {
  //using nested for loops to initialize empty gameboard with tictacktoe tile values.
  for (int x = 0; x <= 2; x++) {
    for (int y = 0; y <= 2; y++) {
      tictactoe[x][y] = BoardTile(79, 79, 1, x, y, 0);
    }
  }
  print_board_to_terminal(true);
  check_ttt_winner();

}

//---------------------------------------------------------------------------------------------------------
//-------------------------------- END OF TERMINAL GAME -----------------------------------------------------
//---------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------
//-------------------------------- USER INPUT PORTION OF GAME ---------------------------------------------
//---------------------------------------------------------------------------------------------------------

//global player variables counts number of turns to decipher whos turn it is and thus which value to assign to tile

int turns = 1;
int x = 0;
int y = 0;

//Returns true or false
boolean isOdd(int cntr) {
  int result = cntr % 2;
  if (result != 0) {
    return true;
  } else {
    return false;
  }
}

void draw_ttt_board() {
  //vertical lines
  tft.drawLine(80, 40, 80, 280, BLACK);
  tft.drawLine(160, 40, 160, 280, BLACK);
  //horizontal lines
  tft.drawLine(1, 120, 240, 120, BLACK);
  tft.drawLine(1, 200, 240, 200, BLACK);
}

//----- converting "raw" pixel data into 0-8 coordinates------
void convert_pixel_in(int xpos, int ypos) {
  //int coord, x, y;

  //assigning x value
  if (xpos >= 0 && xpos <= 79) {
    x = 0;
  } else if (xpos >= 81 && xpos <= 160) {
    x = 1;
  } else if (xpos >= 162 && xpos <= 240) {
    x = 2;
  }

  //assigning y value
  if (ypos >= 40 && ypos <= 119) {
    y = 0;
  } else if (ypos >= 121 && ypos <= 200) {
    y = 1;
  } else if (ypos >= 202 && ypos <= 281) {
    y = 2;
  }
  //return x, y;
  Serial.print("x: ");
  Serial.println(x);
  Serial.print("y: ");
  Serial.println(y);
}


//this function draws a circle at given x,y location
void draw_piece() {

  boolean player = false;
  int x_center = 0;
  int y_center = 0;
  int x1 = 0;
  int x2 = 0;
  int y1 = 0;
  int y2 = 0;

  player = isOdd(turns);

  if (player == true) {
    if (x == 0) {
      x_center = 40;
    } else if (x == 1) {
      x_center = 120;
    } else if (x == 2) {
      x_center = 200;
    }

    if (y == 0) {
      y_center = 80;
    } else if (y == 1) {
      y_center = 160;
    } else if (y == 2) {
      y_center = 240;
    }
    Serial.println(x);
    Serial.println(y);
    tft.drawCircle(x_center, y_center, 38, RED);
  } else {

    if (x == 0) {
      x1 = 0;
      x2 = 79;
    } else if (x == 1) {
      x1 = 81;
      x2 = 160;
    } else if (x == 2) {
      x1 = 162;
      x2 = 241;
    }
    if (y == 0) {
      y1 = 40;
      y2 = 119;
    } else if (y == 1) {
      y1 = 121;
      y2 = 200;
    } else if (y == 2) {
      y1 = 202;
      y2 = 281;
    }
    tft.drawLine(x1, y1, x2, y2, BLUE);
    tft.drawLine(x1, y2, x2, y1, BLUE);
  }
}

void display_end_screen(int player) {
  Serial.print(player);
  boolean lock = true;
  //tft.reset();
  if (turns == 10) {
    tft.fillScreen(MAGENTA);
  } else if (player == 1) {
    tft.fillScreen(BLUE);
  } else if (player == -1) {
    tft.fillScreen(RED);
  }
  while (lock == true) {

  }
}


//---------------------------------------------------------------------------------------------------------
//----------------------------- EVERYTHING YOU NEED FOR A TFT SCREEN --------------------------------------
//---------------------------------------------------------------------------------------------------------

//DECLARING VARIABLES FOR TOUCH SCREEN FUNCTIONALITY. You will need to run a screen calibration so that your
//touch screen can run the code.
char *name = "Please Calibrate."; //edit name of shield
const int16_t XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9341
const int16_t TS_LEFT = 133, TS_RT = 921, TS_TOP = 79, TS_BOT = 888;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

// max-min pressure are ADC values to verify that the screen has recieved user input
#define MINPRESSURE 200
#define MAXPRESSURE 1000

int16_t ID;
int8_t Orientation = 0; //PORTRAIT

//---------------------------------------------------------------------------------------------------------
//----------------------------- END OF TFT SCREEN ESSENTIALS ------- --------------------------------------
//---------------------------------------------------------------------------------------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  Serial.print("found ID = 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3) ID = 0x9481; //force ID if write-only display
  tft.begin(ID);
  tft.setRotation(0);


  tft.fillScreen(WHITE);

  init_ttt();
  draw_ttt_board();
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t xpos, ypos;    //screen coordinates
  tp = ts.getPoint();     //ts.x, tp.y are ADC values
  int16_t curBoardX, oldX, curBoardY, oldY;
  boolean screenPress = false;
  boolean p1 = false;
  int piece = 0;
  int player = 0;

  if (tp.z > MINPRESSURE && tp.z < MAXPRESSURE) {
    //switch statement gives proper orientation for touch sensor

    //without these two lines your arduino will white screen due to pull up
    // and down pins confusing the board.
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

    p1 = isOdd(turns);
    if (p1) {
      player = 1;
    } else {
      player = -1;
    }

    convert_pixel_in(xpos, ypos);
    //Serial.print("x: ");
    //Serial.println(x);
    //Serial.print("y: ");
    //Serial.println(x);

    piece = tictactoe[y][x].getPieceValue();

    if (piece == 0) {
      tictactoe[y][x].setPieceValue(player);
      print_board_to_terminal(true);
      check_ttt_winner();
      turns += 1;
      draw_piece();
    }
      if (turns == 10 || win >= 0) {
    Serial.print("turns: ");
    Serial.println(turns);
    Serial.print("win: ");
    Serial.println(win);
    Serial.print("end screen function called. player: ");
    Serial.println(player);
    display_end_screen(player);
  }

    //tft.fillCircle(xpos, ypos, 1, GREEN);

    delay(200);
  } else;

}
