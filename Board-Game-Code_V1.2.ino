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
int nav = 0;
int nav_sub = 0;
byte num_players = 2;
byte text_color = HIGH;
byte text_color_bg = LOW;
byte count_init;
byte first_array_init = LOW;
long player_count[8][3];
byte player_num[8][2];

//Joystick Debounce
// x value deb
int move_x;
int x_state;
int x_count;
unsigned long x_deb;

// y value deb
int move_y;
int y_state;
int y_count;
int y_direction;
unsigned long y_deb;

//init of game
long player_money = 1500000;

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

class Scroll_Menu {
  private:
    int selected_line;
    int item_count;
    int y_direction;
    int y_count;
    int move;
    char _menu_type;
  public:
    Scroll_Menu(int item_amount, char menu_type, int y_direction, int y_count) {
      this->item_count = item_amount;
      this->_menu_type = menu_type;
      this->y_direction = y_direction;
      this->y_count = y_count;

      if (y_count == HIGH){
        if (y_direction == 1){
          move = 1;
        }
        else if (y_direction == 2) {
          move = 2;
        }
      }
      else{
        move = 0;
      }
      
      delay(50);

      if (move == 1){
        selected_line = selected_line - 1;
      }
      else if(move == 2){
        selected_line = selected_line + 1;
      }

      if (selected_line != 1 && selected_line > item_count) { 
          selected_line = 1;
      }
      else if (selected_line < 1 && selected_line < item_count) {
        selected_line = item_count;
      }
      

      if (_menu_type == 'P'){
        for (int i = 0; i < item_count; i++) {
          if (i <= 3 && selected_line <= 4){
            if (selected_line == i + 1) {
              display.setTextColor(WHITE, BLACK);
              display.print("Player: ");
              display.println(i + 1);
            }
            else if (i <= 3 && selected_line <= 4) {
              display.setTextColor(BLACK);
              display.print("Player: ");
              display.println(i + 1);
            } 
          }
          else if (i >= 4 && selected_line >= 5){
            if (selected_line == i + 1){
              display.setTextColor(WHITE, BLACK);
              display.print("Player: ");
              display.println(i + 1); 
            }
            else {
              display.setTextColor(BLACK);
              display.print("Player: ");
              display.println(i + 1);
            }
          }
        }
      }
    }
};

void setup() {
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

  first_array_init = LOW;
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
    if (y_direction == 2 && nav == 1 && nav_sub == 1){
      // update player count
        num_players = num_players - 1;
    }
    else if (y_direction == 1 && nav == 1 && nav_sub == 1){
      //update player count
      num_players = num_players + 1;
    }
    y_count = LOW;
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
  if (nav == 1 && nav_sub > 1){
    nav_sub = 1;
  }
  else if (nav == 1 && nav_sub < 0){
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
  byte val_btn = digitalRead(joy_btn);


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
      if (y_count == HIGH){
        if (move_y == 1){
          y_direction = 1;
        }
        else if (move_y == 2){
          y_direction = 2;
        }
      }
    }
    else{
      y_count = LOW;
      y_direction = 0;
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
  Serial.print(y_direction);
  Serial.print(" ");
  Serial.print(move_y);
  Serial.print(" ");
  Serial.print(y_count);
  Serial.print(" ");
  Serial.print(y_state);
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
    display.clearDisplay();
    Scroll_Menu(num_players, 'P', y_direction, y_count);
    display.display();
  }
  else if (nav == 3){
    player_information();
  }
  if (btn_press == HIGH && nav_sub == 0){
    nav = nav + 1;
    btn_press = LOW;
  }
}
