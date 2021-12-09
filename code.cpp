#include <LiquidCrystal.h>
#define LENGTH_X_MAX 15
#define LENGTH_X_MIN 2
#define LENGTH_Y_MAX 1
#define LENGTH_Y_MIN 0
#define POSITION_UP   0
#define POSITION_DOWN 1
#define CHAR_X 6
#define ANIM_CHAR1 6
#define ANIM_CHAR2 7
#define INIT_FLY_SPEED 100
#define BUZZER 10
#define FX_B7 1976

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

byte cc10x1[] = {
  B00000,B00000,B00000,B00000,B01110,B01010,B01010,B11111
};
byte cc10x2[] = {
  B00000,B00000,B00000,B00000,B01110,B11011,B11011,B11111
};
byte cc11x1[] = {
  B10101,B00100,B00100,B01110,B11011,B00000,B00000,B00000
};
byte cc11x2[] = {
  B00100,B00100,B00100,B01110,B11011,B00000,B00000,B00000
};
byte characterRun1[] = {
  B01110,B01010,B01011,B11111,B10100,B00100,B01110,B11011
};
byte characterRun2[] = {
  B01110,B01010,B11010,B11111,B00101,B00100,B01110,B11011
};
byte middleBullet[] = {
  B00000,B00100,B01110,B11011,B01110,B00100,B00000,B00000
};
byte rightBullet[] = {
  B00000,B10000,B11000,B01100,B11000,B10000,B00000,B00000
};
byte leftBullet[] = {
  B00000,B00000,B00001,B00011,B00001,B00000,B00000,B00000
};

class Bullet{
    private:
        int xLocation;
        int yLocation;
        int flySpeed;
        bool isBetween;
        bool isGone;
    public: 
        Bullet(){
    }
            
     int getX()        {    return xLocation;}
    int getY()        {    return yLocation;}
    bool getIsBetween()  {    return isBetween;}
    
    void setX(int xLocation)          {    this->xLocation = xLocation;}    
    void setY(int yLocation)          {    this->yLocation = yLocation;}
    void setIsBetween(bool isBetween) {    this->isBetween = isBetween;} 
};

class MainCharacter{
    private:
        int xLocation;
        int yLocation;  
    public:
      MainCharacter() {}
      
      int getX() {    return xLocation;}    
    int getY() {    return yLocation;}
      
        void setX(int xLocation){    this->xLocation = xLocation;}
    void setY(int yLocation){    this->yLocation = yLocation;}
};

class Bullet spawnBullet(int locationX, int locationY)
{
  Bullet bl = Bullet();
  bl.setY(locationY); 
  bl.setX(locationX);
  bl.setIsBetween(true);
  return bl;
}

static bool buttonPressed = false;
static int controlBullet = false;
static bool stateOfChar = true;
static bool isStarted = false;
static bool isPlaying = false;
static MainCharacter mc = MainCharacter();
static Bullet theBullet = Bullet();
static int score = 0;
static int mcControl = 0;
static int bulletControl = 0;
static bool scoreControl = false;
static int flySpeed = INIT_FLY_SPEED;

void drawBullet(){
  lcd.setCursor(theBullet.getX()-1,theBullet.getY());
  lcd.print(" ");
  if(!theBullet.getIsBetween())
  {
     lcd.setCursor(theBullet.getX()+1,theBullet.getY());
     lcd.print(" ");
     lcd.setCursor(theBullet.getX(),theBullet.getY());
     lcd.write((byte)4);
  }
  else
  {   
    lcd.setCursor(theBullet.getX()+1,theBullet.getY());
    lcd.write((byte)5);
    lcd.setCursor(theBullet.getX(),theBullet.getY());
    lcd.write((byte)3);
  }
    
}

void bulletMovement()
{
  if(theBullet.getIsBetween())
        {
            theBullet.setIsBetween(false);
        }
        else
        {
          theBullet.setIsBetween(true);
          theBullet.setX(theBullet.getX()-1);       
        }
}

void drawScore()
{
  lcd.setCursor(0,0);
  lcd.print("YOU");
  lcd.setCursor(0,1);;
  lcd.print(score);
}

void drawMan(int xLocation, int yLocation){
  
    lcd.setCursor(xLocation,yLocation);
    if(stateOfChar)
    {
        lcd.write((byte)ANIM_CHAR1);
        stateOfChar = false;
    }
    else
    {
      lcd.write((byte)ANIM_CHAR2);
      stateOfChar = true;
  }
}

void game(){
     noTone(BUZZER);
     drawScore();
     //print character at 5*10 interval
     if(mcControl == 5)
     {
         drawMan(mc.getX(),mc.getY());
         mcControl = 0;
     }
     else
     {
         mcControl++;
     }
     //change character position if pressed button
     if (buttonPressed)
     {    
         lcd.setCursor(mc.getX(),mc.getY());
         lcd.print(" ");
         if (mc.getY() == 0)
         {
            mc.setY(1); 
         }
         else
         {
            mc.setY(0);
         }
         buttonPressed = false;
     }
     //all about the bullet
   if(bulletControl == flySpeed/10)
   {
     bulletControl = 0;
     
     //print the bullet & check bullet touch the character
     if(theBullet.getX()>LENGTH_X_MIN)
     { 
        drawBullet();
        if((mc.getX()==theBullet.getX()) && (mc.getY() != theBullet.getY()) && scoreControl == false)
        {
           score++;
           scoreControl = true;
           tone(BUZZER, FX_B7);
        }
        else if ((mc.getX()==theBullet.getX()) && (mc.getY() == theBullet.getY()))
        {
           isPlaying = false;
        }
        bulletMovement();
     }
     else if(theBullet.getX() == LENGTH_X_MIN)
     {    
         lcd.setCursor(theBullet.getX()+1, theBullet.getY());
         lcd.print(" ");
         theBullet = spawnBullet(LENGTH_X_MAX, random(0,2));
         scoreControl = false;
       //increase flyspeed every 2 scores
         if((score!=0) && ((score%2)==0))
         {
             if(flySpeed != 10)
             {
                 flySpeed = flySpeed - 10;
             }
             else
             {
                 flySpeed = 10;
             }
         }
     }
   }else{bulletControl++;}
   delay(10);  
}

void printTitle(){
  lcd.setCursor(3,0);
  lcd.print("The Adventure");
  
  lcd.setCursor(5,1);
  lcd.print("*-start-*");
  
  lcd.createChar(1, cc11x1);
  lcd.setCursor(1,1);
  lcd.write((byte)1);
       
  lcd.createChar(0, cc10x1);
  lcd.setCursor(1,0);
  lcd.write((byte)0);
  delay(500);
  
  lcd.setCursor(5,1);
  lcd.print("<!start!>");
    
  lcd.createChar(1, cc11x2);
  lcd.setCursor(1,1);
  lcd.write((byte)1);
    
  lcd.createChar(0, cc10x2);
  lcd.setCursor(1,0);
  lcd.write((byte)0); 
  delay(500);

  
}


void changeState(){
   buttonPressed = true;
}

void setup() {
  lcd.createChar(3,leftBullet);
  lcd.createChar(4,middleBullet);
  lcd.createChar(5,rightBullet);
  lcd.createChar(ANIM_CHAR1,characterRun1);
  lcd.createChar(ANIM_CHAR2,characterRun2);
  pinMode(2, INPUT);
  pinMode(BUZZER, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), changeState, RISING); 
  randomSeed(analogRead(0));
  mc.setX(CHAR_X);
  mc.setY(POSITION_DOWN);
  theBullet = spawnBullet(LENGTH_X_MAX, random(0,2));
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
}

void loop() {
    if(!isStarted){
        printTitle();
        if(buttonPressed){
            lcd.clear();
          buttonPressed = false;
          isStarted = true;
          isPlaying = true;
      }
        return;
  }
  while(isPlaying)
  {
      game(); 
    
    
  }
  lcd.setCursor(6,0);
  lcd.print("You lost!");
  lcd.setCursor(4,1);
  lcd.print("Let's restart");
  
}

