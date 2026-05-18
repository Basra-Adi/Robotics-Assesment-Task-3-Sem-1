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
    bool prop_array_init = false;
    //sub_menus
    String Player_Sub_Menu[3] = {"Properties", "Bank", "Back"};
    String Property_Sub_Menu[3] = {"Buy", "Owned", "Back"};
    String Bank_Sub_Menu[3] = {"Loan", "Balance", "Back"};
    String Loan_Sub_Menu[3];
    String Balance_Sub_Menu[4] = {"Balance", "Increase", "Decrease", "Back"};
    int Increase_Val[8] = {0, 1, 5, 10, 20, 50, 100, 500};
    volatile int Balance_Val[9];
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
    Scroll_Menu(int item_amount, int y_direction, int y_count, int btn_press) {
      this->item_amount = item_amount;
      this->y_direction = y_direction;
      this->y_count = y_count;
      this->btn_press = btn_press;
      //init of array for bank balance
      if (Balance_Val[8] != 1500){
        for (int i = 0; i < 9; i++) {
          Balance_Val[i] = 1500;
        }
      }
      //init of array for property val etc.
      if (prop_array_init == false) {
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

      //checks which menu to load
      if (_menu_type == 'P' && btn_press == HIGH) {
        _menu_type = 'p';
      }
      if (btn_press == HIGH){
        switch(selected_line){
          case 1:
          switch(_menu_type){
            case 'p':
            _menu_type = 'S';
            break;

            case 'S':
            _menu_type = 'b';
            break;

            case 'B':
            _menu_type = 'L';
            break;

            case 'I':
            _menu_type = 'A';
            break;

            case 'D':
            _menu_type = 'A';
            break;
          }
          break;

          case 2:
          switch(_menu_type){
            case 'p':
            _menu_type = 'B';
            break;

            case 'S':
            _menu_type = 'O';
            break;

            case 'B':
            _menu_type = 'A';
            break;

            case 'A':
            _menu_type = 'I';
            break;
          }
          break;

          case 3:
          switch(_menu_type) {
            case 'p':
            _menu_type = 'P';
            break;

            case 'S':
            _menu_type = 'p';
            break;

            case 'B':
            _menu_type = 'p';
            break;
            
            case 'A':
            _menu_type = 'D';
            break;
          }
          case 4:
          switch(_menu_type) {
            case 'A':
            _menu_type = 'p';
            break;
          }
        }
      }

      //delay to prevent the double scroll
      delay(50);

      //nav system for scroll lines
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
      }
      else{
        move = 0;
      }
      if (selected_line > 1 && selected_line > item_count) { 
        selected_line = 1;
      }
      else if (selected_line < 1 && selected_line < item_count) {
        selected_line = item_count;
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


      //checks menu size and init first menu
      switch (_menu_type){
        case 'P':
        item_count = item_amount;
        break;

        case 'p':
        item_count = 3;
        break;

        case 'S':
        item_count = 3;
        break;

        case 'B':
        item_count = 3;
        break;

        case 'A':
        item_count = 4;
        break;

        case 'D':
        item_count = 8;
        break;

        case 'I':
        item_count = 8;
        break;

        case 'b':
        item_count = 29;
        break;

        default: 
        _menu_type = 'P';
        item_count = item_amount;
        break;
      }


      //calculates total number of pages
      int total_pages = ceil(item_count/4.0);


      //checks for unwarranted values and corrects them
      if(selected_line > scroll_page * 4){
        scroll_page = scroll_page + 1;
      }
      else if(ceil(selected_line/4.0) < scroll_page){
        scroll_page = scroll_page - 1;
      }


      //rendering the menus
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
                      this->Balance_Val[selected_player - 1] = new_balance;
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
                      this->Balance_Val[selected_player - 1] = new_balance;
                    }
                  }
                }
                else if (_menu_type == 'b'){
                  switch(selected_line){
                    case 1:
                    display.print("Old Kent Rd: ");
                    break;
                    case 2:
                    display.print("Whitechapel: ");
                    break;
                    case 3:
                    display.print("The Angel: ");
                    break;
                    case 4:
                    display.print("Euston Rd: ");
                    break;
                    case 5:
                    display.print("Pento Rd: ");
                    break;
                    case 6:
                    display.print("Pall Mall: ");
                    break;
                    case 7:
                    display.print("Whitehall: ");
                    break;
                    case 8:
                    display.print("North Av: ");
                    break;
                    case 9:
                    display.print("Bow St: ");
                    break;
                    case 10:
                    display.print("Marl St: ");
                    break;
                    case 11:
                    display.print("Vine St: ");
                    break;
                    case 12:
                    display.print("Strand: ");
                    break;
                    case 13:
                    display.print("Fleet St: ");
                    break;
                    case 14:
                    display.print("Traf Sq: ");
                    break;
                    case 15:
                    display.print("Leic St: ");
                    break;
                    case 16:
                    display.print("Cov St: ");
                    break;
                    case 17:
                    display.print("Picadilly: ");
                    break;
                    case 18:
                    display.print("Regent St: ");
                    break;
                    case 19:
                    display.print("Oxford St: ");
                    break;
                    case 20:
                    display.print("Bond St: ");
                    break;
                    case 21:
                    display.print("Park Lane: ");
                    break;
                    case 22:
                    display.print("Mayafair: ");
                    break;
                    case 23:
                    display.print("Kings Cross: ");
                    break;
                    case 24:
                    display.print("Marlye: ");
                    break;
                    case 25:
                    display.print("Fenchurch: ");
                    break;
                    case 26:
                    display.print("Liverpool: ");
                    break;
                    case 27:
                    display.print("Electric: ");
                    break;
                    case 28:
                    display.print("Waterworks: ");
                    break;
                  }
                  display.print(Property_Info[p][0]);
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
                else if (_menu_type == 'b'){
                  switch(selected_line){
                    case 1:
                    display.print(F("Old Kent Rd: "));
                    break;
                    case 2:
                    display.print(F("Whitechapel: "));
                    break;
                    case 3:
                    display.print(F("The Angel: "));
                    break;
                    case 4:
                    display.print(F("Euston Rd: "));
                    break;
                    case 5:
                    display.print(F("Pento Rd: "));
                    break;
                    case 6:
                    display.print(F("Pall Mall: "));
                    break;
                    case 7:
                    display.print(F("Whitehall: "));
                    break;
                    case 8:
                    display.print(F("North Av: "));
                    break;
                    case 9:
                    display.print(F("Bow St: "));
                    break;
                    case 10:
                    display.print(F("Marl St: "));
                    break;
                    case 11:
                    display.print(F("Vine St: "));
                    break;
                    case 12:
                    display.print(F("Strand: "));
                    break;
                    case 13:
                    display.print(F("Fleet St: "));
                    break;
                    case 14:
                    display.print(F("Traf Sq: "));
                    break;
                    case 15:
                    display.print(F("Leic St: "));
                    break;
                    case 16:
                    display.print(F("Cov St: "));
                    break;
                    case 17:
                    display.print(F("Picadilly: "));
                    break;
                    case 18:
                    display.print(F("Regent St: "));
                    break;
                    case 19:
                    display.print(F("Oxford St: "));
                    break;
                    case 20:
                    display.print(F("Bond St: "));
                    break;
                    case 21:
                    display.print(F("Park Lane: "));
                    break;
                    case 22:
                    display.print(F("Mayafair: "));
                    break;
                    case 23:
                    display.print(F("Kings Cross: "));
                    break;
                    case 24:
                    display.print(F("Marlye: "));
                    break;
                    case 25:
                    display.print(F("Fenchurch: "));
                    break;
                    case 26:
                    display.print(F("Liverpool: "));
                    break;
                    case 27:
                    display.print(F("Electric: "));
                    break;
                    case 28:
                    display.print(F("Waterworks: "));
                    break;
                  }
                  display.print(Property_Info[p][0]);
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
