#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdbool.h>
#include<conio.h>

struct stats{           //game stats structure
    int MATCHES;        //no. of matches played
    int MOVES;          //total moves throughout game(AVG=MOVES/MATCHES)
    long int TIME;      //total time throughout game(AVG=TIME/MATCHES)
    int MINMOVES;       //minimum moves to completion
    int MINTIME;        //minimum time to completion
}stored;

short int board[4][4];  //game board

clock_t START,CURR;     //clock time count variables
int moveCount=0;        //no. of moves for current game
bool firstHit=false;    //first input(clock starts)
bool readSuccess;       //data file read status

bool readStats();       //all function declarations to avoid implicit declaration warning
BOOL cursorLocation(const WORD,const WORD);
void menuoutline(char*,char*,char*,char*,char*,char*,char*);
void createboard();
int checksolvable();
char input(char);
void displayboardFrame();
int displayboard();
void reset();
void pausemenu();
void stopwatch(int);
void displayStopwatch();
bool inputhandler(int);
int checkboard();
void gameOn();
void updateStats();
void statsView();
void titlescreen();
void swap(short int*,short int*);
void windowSetup();

void main()                                 //entire running sequence(moved this up to resolve some warnings)
{
    windowSetup();
    while(1)
    {
        readSuccess=readStats();
        titlescreen();
        gameOn();
        updateStats();
        cursorLocation(3,14);
        printf("SOLVED!!!-----------------------------------------------------------------");
        cursorLocation(3,16);
        system("pause");
        reset();
    }
}

void windowSetup()
{
    srand((unsigned)time(NULL));                        //for rand(), placing here requires less time to restart
    COORD coord={80,30};
    SMALL_RECT Rect={0,0,79,29};
    CONSOLE_CURSOR_INFO lpCursor = {1, false};          //invisible cursor
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTitleA("oneFIVE");
    SetConsoleWindowInfo(console, TRUE, &Rect);         //set window size
    SetConsoleScreenBufferSize(console, coord);         //set console buffer size
    SetConsoleCursorInfo(console, &lpCursor);           //disable cursor
}

bool readStats()                            //loading statistics data
{
   FILE *data;
   data=fopen("file0","rb");                //stats file(in binary mode,delete "file0" in program path to reset)
   if(data==NULL)                           //no file found(fresh/reset game)
   {
       stored.MATCHES=stored.MOVES=stored.TIME=stored.MINMOVES=stored.MINTIME=0;
       return false;                        //no file found, thus fresh game
   }
   fread((char*)&stored,sizeof(stored),1,data);     //stored in global variables for use in current session
   fclose(data);
   return true;                             //file found and data loaded
}

BOOL cursorLocation(const WORD x,const WORD y)              //similar to gotoxy(), places cursor at the specified location for printing
{
    COORD coordinates;                                      //return function only accepts COORD(structure containing WORD/short int variables)
    coordinates.X=x;
    coordinates.Y=y;
    return SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coordinates);
}

void menuoutline(char *main,char *op1,char *sel1,char* op2,char *sel2,char *op3,char *sel3)     //for menu display, op for option key, sel for selected action
{
    cursorLocation(3,12);
    printf("%s",main);
    cursorLocation(3,14);
    printf("%s\t%s",op1,sel1);
    cursorLocation(3,16);
    printf("%s\t%s",op2,sel2);
    cursorLocation(3,18);
    printf("%s\t%s",op3,sel3);
}

void createboard()                                      //creating game board
{
    short int value=1,posI,posJ;                        //value to be put at random location and positions
    for(posI=0;posI<4;posI++)
    {
        for(posJ=0;posJ<4;posJ++)
            board[posI][posJ]=0;                        //initializing all board elements to zero
    }
    while(value<16)
    {
        posI=rand()%4;
        posJ=rand()%4;                                  //choosing a random position
        if(posI*posJ!=9)                                //position should not be (3,3)/occupied
        {
            if(board[posI][posJ]==0)
            {
                board[posI][posJ]=value;                //value in random position
                value++;
            }
        }
    }
}

int checksolvable()                                             //check if board is solvable, uses inverse counting
{
    int invcount=0,greater,boardlist[16]={ 0 },num1,posI,posJ;
    for(posI=0;posI<4;posI++)
    {
        for(posJ=0;posJ<4;posJ++)
        {
            if(board[posI][posJ])
            {
                greater=0;
                for(num1=0;boardlist[num1];num1++)
                {
                    if(board[posI][posJ]>boardlist[num1]&&boardlist[num1])
                        greater++;
                }
                invcount+=board[posI][posJ]-greater-1;
                boardlist[num1]=board[posI][posJ];
            }
        }
    }
    return(invcount%2);                                         //solvable if even
}

char input(char Low)
{
    return (Low>90)?Low-=32:Low;                                //similar to toupper(), converts lowercase only to uppercase
}

void displayboardFrame(){                                       //also clears the screen
    short int ypos = 2, posX = 29, posY = 2;
    system("cls");
    for( ; posY < 10 ; ++posY){
        cursorLocation(posX, posY++);
        printf("+----+----+----+----+");                        //upper horizontal line in the 4x4 box
        for( ; posX < 50 ; posX += 5){
            cursorLocation(posX, posY);
            printf("|");
        }
        posX = 29;   
    }
    cursorLocation(posX, posY);
    printf("+----+----+----+----+");  
}

int displayboard()                                              //board display during game
{
    short int zero, posI = 0, posJ = 0;                         //empty space location
    short int posY=3, posX = 30;                                //for use in printing the board
    for( ; posY < 10 ; posY += 2, posI++){
        for( ; posX < 50 ; posX += 5, posJ++){
            cursorLocation(posX, posY);
            if(board[posI][posJ]){
                printf("%4d", board[posI][posJ]);
            }
            else{
                printf("    ");
                zero = (10 * posI) + posJ;
            }
        }
        posJ = 0;
        posX = 30;
    }
    cursorLocation(27, 12);
    printf("MOVES %d",moveCount);              //displaying the no. of moves done in current game
    cursorLocation(3, 14);
    printf("Esc\tPAUSE");                           //controls help
    cursorLocation(3, 16);
    printf("W A S D  MOVE");
    return zero;                                    //empty position returned
}

void displayStopwatch(){
    cursorLocation(40,12);
    printf("TIME %d : %d.%d     ", ((CURR-START)/1000)/60, ((CURR-START)/1000)%60, ((CURR-START)%1000)/100); //time display in current game
}

void reset()                                       //reset game variables
{
    moveCount=0;
    firstHit=false;
}

void pausemenu()
{
    char pauseop;
    do{
        menuoutline("PAUSED","Esc","RESUME","N","NEW GAME","E","EXIT");
        pauseop=input(getch());
        if(pauseop=='N')               //new game
        {
            reset();
            cursorLocation(3,20);
            printf("\tRESTARTING...");
            reset();
            gameOn();                   //relaunch function, break loop on exit
            break;
        }
        else if(pauseop=='E')          //exit to main menu
        {
            reset();
            main();
        }
    }while(pauseop!=27);                //resume
}

void stopwatch(int toggle)  //diff=START-CURR to be got
{
    if(toggle==0)           //game ongoing
        CURR=clock();       //change CURR according to time
    else if(toggle==1)      //game (paused) at beg(no input)
        START=clock();      //change START according to time
    else                    //game paused during
    {
        pausemenu();            //exit out of this function to resume
        START+=(clock()-CURR);  //advancing START to maintain difference
    }
}

void swap(short int *x,short int *y)
{
    short int temp=(*x);
    (*x)=(*y);
    (*y)=temp;
}
bool inputhandler(int zero)
{
    int oI=zero / 10;
    int oJ=zero % 10;                   //extracting empty position from displayboard
    bool keyHit = false;
    char move;                          //W A S D
    if(kbhit())                         //keyboard hit
    {
        move=input(getch());
        firstHit=true;                  //first hit(input)
        keyHit = true;
    }
    stopwatch(0);                       //continue stopwatch
    if(!firstHit)                       //if false, stop changing START according to time
        stopwatch(1);                   //no first hit(input)
    if((move=='W')&&(oI!=0))            //forward
    {
        swap(&board[oI-1][oJ],&board[oI][oJ]);
        moveCount++;
    }
    else if((move=='A')&&(oJ!=0))       //left
    {
        swap(&board[oI][oJ-1],&board[oI][oJ]);
        moveCount++;
    }
    else if((move=='S')&&(oI!=3))       //right
    {
        swap(&board[oI+1][oJ],&board[oI][oJ]);
        moveCount++;
    }
    else if((move=='D')&&(oJ!=3))       //down
    {
        swap(&board[oI][oJ+1],&board[oI][oJ]);
        moveCount++;
    }
    else if(move==27)                   //pause(Esc)
    {
        cursorLocation(3, 16);
        printf("     ");                //to overwrite 'move' prompt
        stopwatch(-1);                  //pausemenu and stopwatch pause
        displayboardFrame();            //clear screen and redisplay board
    }
    return keyHit;
}

int checkboard()                        //returns 1 to end game
{
    short int checkValue=1;
    for(short int i=0;i<4;i++)
    {
        for(short int j=0;j<4;j++)
        {
          if(!(i==3&&j==3))
          {
              if(board[i][j]==checkValue)       //sequential position checking except at (3,3)
                checkValue++;                   //true, check next value at next position
              else
                return 0;                       //false, exit
          }
        }
    }
    return 1;                                   //true, exit
}

void gameOn()                           //ongoing game
{
    do{
            createboard();
    }while(checksolvable());        //create a board and check if it is solvable
    displayboardFrame();
    int zero = displayboard();      //Empty position
    do{
        if(inputhandler(zero)){
            zero = displayboard();  //refresh board when input is obtained
        }
        displayStopwatch();
    }while(!checkboard());          //to exit(end) game, goes false
}

void updateStats()                          //update statistics and write to file
{
    if(readSuccess)                         //file was present earlier to compare the stored values
    {
        if(moveCount<stored.MINMOVES)       //comparing with stored value
            stored.MINMOVES=moveCount;
        if((CURR-START)<stored.MINTIME)
            stored.MINTIME=(CURR-START);
    }
    else
    {
        stored.MINMOVES=moveCount;          //fresh game
        stored.MINTIME=(CURR-START);
    }
    stored.MATCHES++;                       //one match completed
    stored.TIME+=(CURR-START);              //time used in current game
    stored.MOVES+=moveCount;
    FILE *writ=fopen("file0","wb");         //creating a new file and writing in it(binary)
    fwrite((char*)&stored,sizeof(stored),1,writ);
    fclose(writ);
}

void statsView()                            //statistics display in menu
{
  char option;
  system("cls");
  menuoutline("STATISTICS","\0","\0","\0","\0","\0","\0");
  cursorLocation(3,14);
  printf("TOTAL GAMES PLAYED\t%-10d",stored.MATCHES);
  cursorLocation(3,16);
  printf("TOTAL TIME PLAYED\t%d : %d.%d",((stored.TIME)/1000)/60, ((stored.TIME)/1000)%60, ((stored.TIME)%1000)/100);
  cursorLocation(3,18);
  printf("TOTAL MOVES\t%12d",stored.MOVES);
  cursorLocation(3,20);
  printf("MINIMUM TIME IN ONE GAME\t%d : %d.%d",((stored.MINTIME)/1000)/60, ((stored.MINTIME)/1000)%60, ((stored.MINTIME)%1000)/100);
  cursorLocation(3,22);
  printf("MINIMUM MOVES IN ONE GAME\t%d",stored.MINMOVES);
  cursorLocation(3,24);
  if(readSuccess)                           //to avoid 0/0 problem
  {
      printf("AVERAGE TIME%10d : %d.%d",((stored.TIME/stored.MATCHES)/1000)/60, ((stored.TIME/stored.MATCHES)/1000)%60, ((stored.TIME/stored.MATCHES)%1000)/100);
      cursorLocation(3,26);
      printf("AVERAGE MOVES\t%d",(stored.MOVES/stored.MATCHES));
      cursorLocation(3,28);
  }
  printf("B\tBACK");
  do{
    option=input(getch());
  }while(option!='B');
}

void titlescreen()                          //title screen
{
    char option;
    title:
    system("cls");
    menuoutline("15PUZZLE (1.0.0731)","N","NEW GAME","S","STATISTICS","Q","QUIT");       //15 puzzle
    option=input(getch());
    if(option=='N')                         //new game
    {
        cursorLocation(8,20);
        printf("STARTING...");
        return;                             //returns to main
    }
    else if(option=='S')                    //statistics
    {
        statsView();
        goto title;
    }
    else if(option=='Q')                    //quit
    {
        cursorLocation(8,20);
        printf("Y\tCONFIRM");
        option=input(getch());
        if(option=='Y'){
            system("cls");
            exit(0);                        //will still be paused after compiling in some IDEs
        }
        else
            goto title;
    }
    else
        goto title;
}
