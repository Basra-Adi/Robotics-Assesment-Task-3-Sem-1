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

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

class Scroll_Menu {
  private:
    //menu system
    int selected_line;
    int selected_player;
    int scroll_page;
    int item_count;
    int y_direction;
    int y_count;
    int move;
    int btn_press;
    int item_amount;
    char _menu_type;
    char menu_type;
    bool prop_array_init = false;
    //sub_menus
    String Player_Sub_Menu[3] = {"Properties", "Bank", "Back"};
    String Property_Sub_Menu[3] = {"Buy", "Owned", "Back"};
    String Bank_Sub_Menu[3] = {"Loan", "Balance", "Back"};
    String Loan_Sub_Menu[3];
    String Balance_Sub_Menu[4] = {"Balance", "Increase", "Decrease", "Back"};
    int Increase_Val[8] = {0, 1, 5, 10, 20, 50, 100, 500};
    int Balance_Val[8];
    int house_cost = 50;
    int Property_Info[28][5] = { //10 Property Groups, 2-4 Properties in each group, Owner and Value of Property 
      //format {property_value, base_rent, house_cost, number of houses, ownership}
      {60, 5},
      {60, 10}, //brown
      {100, 10}, 
      {100, 10}, 
      {120, 15}, //light blue
      {140, 14},
      {140, 14},
      {160, 20}, //pink
      {180, 18},
      {180, 18},
      {200, 25}, //orange
      {220, 22},
      {220, 22},
      {240, 30}, //red
      {260, 26},
      {260, 26},
      {280, 35}, //yellow
      {300, 30},
      {300, 30},
      {320, 40}, //green
      {350, 50},
      {400, 80}, //dark blue
      {200, 25},
      {200, 25},
      {200, 25},
      {200, 25}, //station
      {150},
      {150}  //utilities
    };

  public:
    Arrays_Init(){
      if (prop_array_init == false) {
        for (int i = 0; i < 8; i++) {
          Balance_Val[i] = 1500; 
        }
        for (int i = 0; i < 28; i++) {
          if (i <= 4){
            Property_Info[i][2] = house_cost;
            Property_Info[i][3] = 0;
            Property_Info[i][4] = 0;
          }
          else if (5 <= i <= 10){
            Property_Info[i][2] = house_cost * 2;
            Property_Info[i][3] = 0;
            Property_Info[i][4] = 0;
          }
          else if (11 <= i <= 16){
            Property_Info[i][2] = house_cost * 3;
            Property_Info[i][3] = 0;
            Property_Info[i][4] = 0;
          }
          else if (17 <= i <= 21){
            Property_Info[i][2] = house_cost * 4;
            Property_Info[i][3] = 0;
            Property_Info[i][4] = 0;
          }
          else if (22 <= i <= 25){
            Property_Info[i][2] = house_cost/2;
            Property_Info[i][4] = 0;
          }
          else if (26 <= i <= 27){
            Property_Info[i][4] = 0;
          }
        }
        prop_array_init = true;
      }
    }
    Scroll_Menu(int item_amount, int y_direction, int y_count, int btn_press) {
      this->item_amount = item_amount;
      this->y_direction = y_direction;
      this->y_count = y_count;
      this->btn_press = btn_press;
      this->menu_type = 'P';
      
      if (_menu_type != 'P' && _menu_type != 'p' && _menu_type != 'S' && _menu_type != 'B' && _menu_type != 'A' && _menu_type != 'I' && _menu_type != 'D'){
        this->_menu_type = menu_type;
      }
      if (_menu_type == 'P' && btn_press == HIGH) {
        btn_press = LOW;
        this->_menu_type = 'p';
      }
      if (_menu_type == 'p' && btn_press == HIGH && selected_line == 1) {
        btn_press = LOW;
        this->_menu_type = 'S';
      }
      else if (_menu_type == 'p' && btn_press == HIGH && selected_line == 2) {
        btn_press = LOW;
        this->_menu_type = 'B';
      }
      else if (_menu_type == 'p' && btn_press == HIGH && selected_line == 3) {
        btn_press = LOW;
        this->_menu_type = 'P';
      }
      else if (_menu_type == 'S' && btn_press == HIGH && selected_line == 1) {
        btn_press = LOW;
        this->_menu_type = 'b';
      }
      else if (_menu_type == 'S' && btn_press == HIGH && selected_line == 2) {
        btn_press = LOW;
        this->_menu_type = 'O';
      }
      else if (_menu_type == 'S' && btn_press == HIGH && selected_line == 3) {
        btn_press = LOW;
        this->_menu_type = 'p';
      }
      else if (_menu_type == 'B' && btn_press == HIGH && selected_line == 1) {
        btn_press = LOW;
        this->_menu_type = 'L';
      }
      else if (_menu_type == 'B' && btn_press == HIGH && selected_line == 2) {
        btn_press = LOW;
        this->_menu_type = 'A';
      }
      else if (_menu_type == 'B' && btn_press == HIGH && selected_line == 3) {
        btn_press = LOW;
        this->_menu_type = 'p';
      }
      else if (_menu_type == 'A' && btn_press == HIGH && selected_line == 2) {
        btn_press = LOW;
        this->_menu_type = 'I';
      }
      else if (_menu_type == 'A' && btn_press == HIGH && selected_line == 3) {
        btn_press = LOW;
        this->_menu_type = 'D';
      }
      else if (_menu_type == 'A' && btn_press == HIGH && selected_line == 4) {
        btn_press = LOW;
        this->_menu_type = 'B';
      }
      else if (_menu_type == 'I' && btn_press == HIGH && selected_line == 1) {
        btn_press = LOW;
        this->_menu_type = 'A';
      }
      else if (_menu_type == 'D' && btn_press == HIGH && selected_line == 1) {
        btn_press = LOW;
        this->_menu_type = 'A';
      }
      delay(50);
      if (y_count == HIGH){
        if (y_direction == 1){
          move = 1;
        }
        else if (y_direction == 2) {
          move = 2;
        }
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
      }
      else{
        move = 0;
      }
      scroll_page = 1;

      Serial.print(" ");
      Serial.print(selected_line);
      Serial.print(" ");
      Serial.print(_menu_type);
      Serial.print(" ");
      Serial.print(item_count);

      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextSize(1);

      if (_menu_type == 'P') {
        item_count = item_amount;
      }
      else if (_menu_type == 'p') {
        item_count = 3;
      }
      else if (_menu_type == 'S') {
        item_count = 3;
      }
      else if (_menu_type == 'B') {
        item_count = 3;
      }
      else if(_menu_type == 'A') {
        item_count = 4;
      }
      else if(_menu_type == 'D') {
        item_count = 8;
      }
      else if(_menu_type == 'I') {
        item_count = 8;
      }

      int total_pages = ceil(item_count/4.0);

      if(selected_line > scroll_page * 4){
        scroll_page = scroll_page + 1;
      }
      else if(ceil(selected_line/4.0) < scroll_page){
        scroll_page = scroll_page - 1;
      }

      for (int i = 0; i < total_pages; i++) {
        if(scroll_page == i + 1){
          for (int p = 0 + i * 4; p < scroll_page * 4; p++){
            if (item_count > p){
              if (selected_line == p + 1) {
                display.setTextColor(WHITE, BLACK);
                if (_menu_type == 'P') {
                  display.print("Player: ");
                  display.println(p + 1);
                  selected_player = selected_line;
                }
                else if (_menu_type == 'p') {
                  display.println(Player_Sub_Menu[p]);
                }
                else if (_menu_type == 'S') {
                  display.println(Property_Sub_Menu[p]);
                }
                else if (_menu_type == 'B') {
                  display.println(Bank_Sub_Menu[p]);
                }
                else if (_menu_type == 'A') {
                  if(p == 0){
                    display.print(Balance_Sub_Menu[p]);
                    display.print(": ");
                    display.println(Balance_Val[selected_player - 1]);
                  }
                  else{
                    display.println(Balance_Sub_Menu[p]);
                  }
                }
                else if (_menu_type == 'I') {
                  if(p == 0){
                    display.print(Balance_Sub_Menu[p]);
                    display.print(": ");
                    display.println(Balance_Val[selected_player - 1]);
                  }
                  else{
                    display.println(Increase_Val[p]);
                    if (btn_press == HIGH){
                      int new_balance = Balance_Val[selected_player - 1] + Increase_Val[p];
                      Serial.print(" ");
                      Serial.print(new_balance);
                      Balance_Val[selected_player - 1] = new_balance;
                    }
                  }
                }
                else if (_menu_type == 'D') {
                  if(p == 0){
                    display.print(Balance_Sub_Menu[p]);
                    display.print(": ");
                    display.println(Balance_Val[selected_player - 1]);
                  }
                  else{
                    display.println(Increase_Val[p]);
                    if (btn_press == HIGH){
                      int new_balance = Balance_Val[selected_player - 1] - Increase_Val[p];
                      Serial.print(" ");
                      Serial.print(new_balance);
                      Balance_Val[selected_player - 1] = new_balance;
                    }
                  }
                }
              }
              else{
                display.setTextColor(BLACK);
                if (_menu_type == 'P') {
                  display.print("Player: ");
                  display.println(p + 1);
                }
                else if (_menu_type == 'p') {
                  display.println(Player_Sub_Menu[p]);
                }
                else if (_menu_type == 'S') {
                  display.println(Property_Sub_Menu[p]);
                }
                else if (_menu_type == 'B') {
                  display.println(Bank_Sub_Menu[p]);
                }
                else if (_menu_type == 'A') {
                  if(p == 0){
                    display.print(Balance_Sub_Menu[p]);
                    display.print(": ");
                    display.println(Balance_Val[selected_player - 1]);
                  }
                  else{
                    display.println(Balance_Sub_Menu[p]);
                  }
                }
                else if (_menu_type == 'I') {
                  if(p == 0){
                    display.print(Balance_Sub_Menu[p]);
                    display.print(": ");
                    display.println(Balance_Val[selected_player - 1]);
                  }
                  else{
                    display.println(Increase_Val[p]);
                  }
                }
                else if (_menu_type == 'D') {
                  if(p == 0){
                    display.print(Balance_Sub_Menu[p]);
                    display.print(": ");
                    display.println(Balance_Val[selected_player - 1]);
                  }
                  else{
                    display.println(Increase_Val[p]);
                  }
                }
              }
            }
          } 
        }
      }
      display.display();
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

void loop() {
  joy_val();
  variable_conditions();
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
    Scroll_Menu(num_players, y_direction, y_count, btn_press);
  }
  if (btn_press == HIGH && nav_sub == 0){
      nav = nav + 1;
      btn_press = LOW;
    }

  if (nav > 2){
    nav = 2;
  }
}
