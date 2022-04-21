//Programmer: Andrew J Burgess
//version 0.001 implements Boardtile.h and logic to manually assign values
//to the gameboard then check for a winner. purely terminal based paired with
//basic touch input capabilities to draw a green line.

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
//------------------------------------- TTT BACKEND WORK -----------------------------------------------
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
      else if(val < 0)
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
      }
      else if (j == 1) {
        x1 += tictactoe[i][j].getPieceValue();
      }
      else if (j == 2) {
        x2 += tictactoe[i][j].getPieceValue();
      }
      //checking horizontal values for a game win
      y123 += tictactoe[i][j].getPieceValue();
      if (y123 == 3) {
        Serial.println("player 1 wins!");
      }
      else if (y123 == -3) {
        Serial.println("player 2 wins!");
      }
    }
    y123 = 0;
  }
  //checking x variables if there is a win for either player
  if (x0 == 3 || x1 == 3 || x2 == 3) {
    Serial.println("player 1 wins!");
  }
  else if (x0 == -3 || x1 == -3 || x2 == -3) {
    Serial.println("player 2 wins!");
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
//-------------------------------- TTT END OF BACKEND -----------------------------------------------------
//---------------------------------------------------------------------------------------------------------

char *name = "Please Calibrate."; //edit name of shield
const int16_t XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9341
const int16_t TS_LEFT = 133, TS_RT = 921, TS_TOP = 79, TS_BOT = 888;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint tp;

#define MINPRESSURE 200
#define MAXPRESSURE 1000

//int16_t BOXSIZE;
int16_t PENRADIUS = 1;
int16_t ID, oldcolor, currentcolor;
int8_t Orientation = 0; //PORTRAIT

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
  tft.drawLine(0,40,79,119,BLACK);
  tft.drawLine(0,119,79,40,BLACK);
  tft.drawCircle(120,80,39,BLACK);

  init_ttt();
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t xpos, ypos;    //screen coordinates
  tp = ts.getPoint();     //ts.x, tp.y are ADC values
  int16_t curRow, oldRow, curCol, oldCol;
  boolean screenPress = false;



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

    tft.fillCircle(xpos, ypos, 1, GREEN);
  }else;

}
