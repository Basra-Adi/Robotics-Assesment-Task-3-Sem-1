const int joy_x = A1;
const int joy_y = A0;
const int joy_btn = 9;

//Button Debounce
int btn_state;     
byte lastButtonState = LOW;
byte btn_press = LOW; 
unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 50;

// Information and nav variables
int nav = 0;
int nav_sub = 0;
int num_players;

//Joystick Debounce
// x value deb
int move_x;
int x_state;
int x_count;
unsigned long x_deb;

// y value deb
int move_y;
int y_state;
byte y_count;
unsigned long y_deb;

//init of game
long player_money = 1500000;
byte game_start = LOW;

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

void setup()   {
  Serial.begin(9600);
  display.begin();
  display.setContrast(60);
  display.clearDisplay();
  pinMode(joy_x, INPUT);
  pinMode(joy_y, INPUT);
  pinMode(joy_btn, INPUT_PULLUP);
}

void sys_menu() {
  if (nav == 0 && nav_sub == 0){
    // display init
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);

    // display info
    display.setCursor(13, 20);
    display.print("New, Game!");
    display.display();

    if (btn_press == HIGH){
      nav = nav + 1;
      btn_press == LOW;
    }
  }
  else if (nav == 1 && nav_sub == 0){
    // display init
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);

    //display info
    display.setCursor(0, 0);
    display.println("Number of ");
    display.print("Players: ");
    display.print(num_players);
    display.display();

    if (btn_press == HIGH){
      nav = nav + 1;
      game_start = HIGH;
      btn_press = LOW;
    }
  }
  else if (nav_sub == 1 && nav == 1){
    // display init
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);

    // display info
    display.setCursor(0, 0);
    display.println("Number of ");
    display.print("Players: ");
    display.setTextColor(WHITE, BLACK);
    display.println(num_players);
    display.display();

    //Serial.print(num_players);
    //Serial.print(" "); 
  }
}

void variable_conditions(){
  //restrictions
  if (nav == 1 && nav_sub >1){
      nav_sub = 1;
  }
  else if (nav_sub < 0){
    nav_sub = 0;
  }
  // restrictions on player count
  if (num_players < 2){
    num_players = 2;
  }
  else if (num_players > 8){
    num_players = 8;
  }
}

void joy_val() {
  int val_x = analogRead(joy_x);
  int val_y = analogRead(joy_y);
  int val_btn = digitalRead(joy_btn);


  // joy x val read
  if (val_x > 700) {
    move_x = 1;
  }
  else if (val_x < 100){
    move_x = 2;
  }
  else{
    move_x = 0;
  }

  // joy x val deb 
  if (millis() - x_deb > 100){
    x_deb = millis();
    if (x_state != move_x){
      x_count = HIGH;
      x_state = move_x;
      if (x_count == HIGH){
        if (move_x == 1){
          // page nav
          if(nav == 1){
            nav_sub = nav_sub + 1;
          } 
        }
        else if (move_x == 2){
          // page nav
          if (nav == 1){
            nav_sub = nav_sub - 1;
          }

        }
      }
      x_count = LOW;
    }
    else{
    x_count = LOW;
    }
  }
  // joy y val read
  if (val_y > 700) {
    move_y = 2;
  }
  else if (val_y < 100){
    move_y = 1;
  }
  else{
    move_y = 0;
    y_state = move_y;
  }

  // joy y val deb
  if (millis() - y_deb > 100){
    y_deb = millis();
    if (y_state != move_y){
      y_count = HIGH;
      y_state = move_y;
      if (y_count == HIGH){
        if (move_y == 2){
          // update player count
          if (nav == 1 && nav_sub == 1){
            num_players = num_players - 1;
          }
        }
        else if (move_y == 1){
          //update player count
          if (nav == 1 && nav_sub == 1){
            num_players = num_players + 1;
          }
        }
      }
      y_count = LOW;
    }
    else{
    y_count = LOW;
    }
  }
  
  // btn deb
  if (val_btn != lastButtonState) {
      lastDebounceTime = millis();
    }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (val_btn != btn_state) {
      btn_state = val_btn;
      if (btn_state == LOW){
        btn_press = !btn_press;
      }
    }
  }
  lastButtonState = val_btn;

  Serial.println("");
}

void player_information() {
  int player_count[num_players-1][3];
  for (byte i = 0; i < num_players; i = i + 1) {
      player_count[i][3] = i + 1;
    }
  if (game_start == LOW){
    for (byte i = 0; i < 3; i = i + 1){
      player_count[0][i] = player_money;
      Serial.print(player_count[0][i]);
    }
  }
}

void loop() {
  sys_menu();
  joy_val();
  variable_conditions();
  player_information();
}
