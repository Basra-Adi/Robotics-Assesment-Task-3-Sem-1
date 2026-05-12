const int joy_x = A1;
const int joy_y = A0;
const int joy_btn = 9;

//Button Debounce
byte btn_state;     
byte lastButtonState = LOW;
byte btn_press = LOW; 
unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 50;

// Information and nav variables
byte nav = 0;
byte nav_sub = 0;
byte num_players = 2;
byte text_color = HIGH;
byte text_color_bg = LOW;
long player_count[8][3];

//Joystick Debounce
// x value deb
byte move_x;
byte x_state;
byte x_count;
unsigned long x_deb;

// y value deb
byte move_y;
byte y_state;
byte y_count;
unsigned long y_deb;

//init of game
long player_money = 1500000;

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

void start_page() {
  // display init
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);

  // display info
  display.setCursor(13, 20);
  display.print("New, Game!");
  display.display();
}

void page_2() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);

  //display info
  display.setCursor(0, 0);
  display.println("Number of ");
  display.print("Players: ");
  display.print(num_players);
  display.display();
}

void page_2_1(){
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

  if (btn_press == HIGH && y_count == HIGH){
    if (move_y == 2 && nav == 1 && nav_sub == 1){
      // update player count
        num_players = num_players - 1;
    }
    else if (move_y == 1 && nav == 1 && nav_sub == 1){
      //update player count
      num_players = num_players + 1;
    }
    y_count = LOW;
  }
    
}

void player_menu(){
  // display init
  display.clearDisplay();
  display.setTextSize(1);

  //array for player account
  byte player_num[num_players-1][1];

  for (byte i = 0; i < num_players-1; i++){
    player_num[i][0] = i + 1;
  }


  // scroll down sys
  if (move_y == 2){
    int count_init;
    for (byte i = 0; i < num_players-1; i++){
      if (player_num[i][1] != 0) {
        count_init = player_num[i][1];
      }
      else if (count_init == 1){
        player_num[i][1] = 0;
        player_num[i + 1][1] = 1;
      }
      else{
        player_num[0][1] = 1;
      }
    }
  }
  else if (move_y == 1){
    for (byte i = 0; i < num_players-1; i++){
      if (player_num[i][1] == 1 && player_num[0][1] != 1) {
        player_num[i - 1][1] = 1;
        player_num[i][1] = 0;
      }  
    }
  }

  for (byte i = 0; i < num_players-1; i++){
    if (player_num[i][1] == 1){
      display.setTextColor(!text_color, !text_color_bg);
      display.print("Player: ");
      display.println(player_num[i][0]);
      display.display();
    }
    else if (player_num[i][1] == 0){
      display.setTextColor(BLACK);
      display.print("Player: ");
      display.println(player_num[i][0]);
      display.display();
    }
  }

  display.setCursor(0, 40);
  display.setTextColor(text_color, text_color_bg);
  display.println("Back");
  display.display();

  if (x_state == 2){
    text_color = LOW;
    text_color_bg = HIGH;
  }
  else if (x_state == 1){
    text_color = HIGH;
    text_color_bg = LOW;
  }

  if (btn_press == HIGH && text_color == LOW){
      nav = nav - 1;
      btn_press = LOW;
  }
}

void player_info(){
  display.clearDisplay();
  display.print("Amount: ");
  display.println(player_count[0][1]);
  display.display();
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
          if (nav == 1){
            nav_sub = nav_sub + 1;
          } 
        }
        else if (move_x == 2){
          // page nav
          if (nav == 1 && btn_press == LOW){
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
  for (byte i = 0; i < num_players; i++) {
      player_count[i][0] = i + 1;
    }
  if (nav == 1){
    for (byte i = 0; i < 3; i++){
      player_count[i][1] = player_money;
    }
  }
}

void loop() {
  joy_val();
  variable_conditions();
  player_information();

  if (nav == 0){
    start_page();
  }
  else if (nav == 1 && nav_sub == 0){
    page_2();
  }
  else if (nav == 1 && nav_sub == 1){
    page_2_1();
  }
  else if (nav == 2){
    player_menu();
  }
  else if (nav == 3){
    
  }
  if (btn_press == HIGH && nav_sub == 0){
    nav = nav + 1;
    btn_press = LOW;
  }

  Serial.print(nav_sub);
}
