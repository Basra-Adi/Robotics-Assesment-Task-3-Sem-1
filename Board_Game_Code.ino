#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Stepper.h>

const int joy_x = A1;
const int joy_y = A0;
const int joy_btn = 9;

//motor pins
int steps = 2048;
int rpm = 10;
int motorLoop = 0;
int complete = 1;
Stepper myStepper(steps, 13, 11, 12, 10);

//light sensor pins
const int S0 = 2;
const int S1 = 8;
const int S2 = A4;
const int S3 = A5;
const int SOut = A3;
int r_f;
int g_f;
int b_f;

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
    String Bank_Sub_Menu[3] = {"Loan", "Balance", "Back"};
    String Loan_Sub_Menu[3] = {"Pending Amount", "New Loan", "Reduce"};
    String Balance_Sub_Menu[4] = {"Balance", "Increase", "Decrease", "Back"};
    int Increase_Val[8] = {0, 1, 5, 10, 20, 50, 100, 500};
    int Loan_Val[3] = {250, 500, 1000};
    volatile int Loan_Amount[9]; 
    volatile int Balance_Val[9];
    int house_cost = 50;
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

    if (Loan_Amount[8] != 0){
      for (int i = 0; i < 9; i++) {
        Loan_Amount[i] = 0;
      }
    }

    //checks which menu to load
    if (_menu_type == 'P' && btn_press == HIGH) {
      _menu_type = 'B';
    }
    if (btn_press == HIGH){
      switch(selected_line){
        case 1:
        switch(_menu_type){
          case 'B':
          _menu_type = 'L';
          break;

          case 'I':
          _menu_type = 'A';
          break;

          case 'D':
          _menu_type = 'A';
          break;

          case 'L':
          _menu_type = 'B';
          break;

          case 'N':
          _menu_type = 'L';
          break;

          case 'R':
          _menu_type = 'L';
          break;
        }
        break;

        case 2:
        switch(_menu_type){
          case 'B':
          _menu_type = 'A';
          break;

          case 'A':
          _menu_type = 'I';
          break;

          case 'L':
          _menu_type = 'N';
          break;
        }
        break;

        case 3:
        switch(_menu_type) {
          case 'B':
          _menu_type = 'P';
          break;
          
          case 'A':
          _menu_type = 'D';
          break;

          case 'L':
          _menu_type = 'R';
          break;
        }
        case 4:
        switch(_menu_type) {
          case 'A':
          _menu_type = 'B';
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

      case 'L':
      item_count = 3;
      break;

      case 'N':
      item_count = 4;
      break;

      case 'R':
      item_count = 9;
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
              else if (_menu_type == 'L') {
                if (p == 0){
                  display.print(Loan_Sub_Menu[p]);
                  display.print(": ");
                  display.println(Loan_Amount[selected_player - 1]);
                }
                else{
                  display.println(Loan_Sub_Menu[p]);
                }
              }
              else if (_menu_type == 'N') {
                if (p == 0){
                  display.println("Loan Amount: ");
                }
                else{
                  display.println(Loan_Val[p-1]);
                  if (btn_press == HIGH){
                    switch(selected_line){
                      case 2:
                      Loan_Amount[selected_player - 1] = Loan_Val[p-1] + 100;
                      break;
                      case 3:
                      Loan_Amount[selected_player - 1] = Loan_Val[p-1] + 150;
                      break;
                      case 4:
                      Loan_Amount[selected_player - 1] = Loan_Val[p-1] + 250;
                      break;
                    }
                  }
                }
              }
              else if (_menu_type == 'R') {
                if (p == 0){
                  display.print("Loan Amount: ");
                  display.println(Loan_Amount[selected_player - 1]);
                }
                else{
                  display.println(Increase_Val[p - 1]);
                  if (btn_press == HIGH){
                    switch(selected_line) {
                      case 2:
                      Loan_Amount[selected_player - 1] = Loan_Amount[selected_player-1] - Increase_Val[p - 1];
                      break;
                      case 3:
                      Loan_Amount[selected_player - 1] = Loan_Amount[selected_player-1] - Increase_Val[p - 1];
                      break;
                      case 4:
                      Loan_Amount[selected_player - 1] = Loan_Amount[selected_player-1] - Increase_Val[p - 1];
                      break;
                      case 5:
                      Loan_Amount[selected_player - 1] = Loan_Amount[selected_player-1] - Increase_Val[p - 1];
                      break;
                      case 6:
                      Loan_Amount[selected_player - 1] = Loan_Amount[selected_player-1] - Increase_Val[p - 1];
                      break;
                      case 7:
                      Loan_Amount[selected_player - 1] = Loan_Amount[selected_player-1] - Increase_Val[p - 1];
                      break;
                      case 8:
                      Loan_Amount[selected_player - 1] = Loan_Amount[selected_player-1] - Increase_Val[p - 1];
                      break;
                      case 9:
                      Loan_Amount[selected_player - 1] = Loan_Amount[selected_player-1] - Increase_Val[p - 1];
                      break;
                      }
                  }
                }
              }
            }
            else{//hi
              display.setTextColor(BLACK);
              if (_menu_type == 'P') {
                display.print("Player: ");
                display.println(p + 1);
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
              else if (_menu_type == 'L'){
                if (p == 0){
                  display.print(Loan_Sub_Menu[p]);
                  display.print(": ");
                  display.println(Loan_Amount[selected_player - 1]);
                }
                else{
                  display.println(Loan_Sub_Menu[p]);
                }
              }
              else if (_menu_type == 'N') {
                if (p == 0){
                  display.println("Loan Amount: ");
                }
                else{
                  display.println(Loan_Val[p - 1]);
                }
              }
              else if (_menu_type == 'R') {
                if (p == 0){
                  display.print("Loan Amount: ");
                  display.println(Loan_Amount[selected_player - 1]);
                }
                else{
                  display.println(Increase_Val[p - 1]);
                }
              }
            }
          }
        }
      }
      display.display();
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
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(SOut, INPUT);
  //frequency scaling  = 100%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);
  myStepper.setSpeed(rpm);
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
  display.println(num_players);

  display.setTextColor(BLACK);
  display.println("Detect No of Players");
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

  display.setTextColor(BLACK);
  display.println("Detect No of Players");
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

void page_2_2(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);

  //display info
  display.setCursor(0, 0);
  display.println("Number of ");
  display.print("Players: ");
  display.println(num_players);

  display.setTextColor(WHITE, BLACK);
  display.println("Detect No. of Players: ");
  display.display();
}

void variable_conditions(){
  //restrictions
  if (nav == 1 && nav_sub > 2){
    nav_sub = 2;
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
  else if (val_x < 200){
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
  else if (val_y < 200){
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
}

void loop() {
  joy_val();
  Serial.println(analogRead(joy_x));
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
  else if (nav == 1 && nav_sub == 2){
    page_2_2();
    if (btn_press == HIGH){
      complete = 1;
      // red color filter
        digitalWrite(S2,LOW);
        digitalWrite(S3,LOW);
        r_f = pulseIn(SOut, LOW);
        r_f = map(r_f, 25, 70, 255, 0);
        delay(100);

        // green color filter
        digitalWrite(S2,HIGH);
        digitalWrite(S3,HIGH);
        g_f = pulseIn(SOut, LOW);
        g_f = map(g_f, 25, 70, 255, 0);
        delay(100);

        // blue color filter
        digitalWrite(S2,LOW);
        digitalWrite(S3,HIGH);
        b_f = pulseIn(SOut, LOW);
        b_f = map(b_f, 25, 70, 255, 0);
        delay(100);
        int prevR_F = r_f;
        int prevG_F = g_f;
        int prevB_F = b_f;
        int player_amount = 1;

      while (complete > 0 && complete < 6){
        if (complete == 1){
          myStepper.step(512);
          complete = 2;
          // red color filter
          digitalWrite(S2,LOW);
          digitalWrite(S3,LOW);
          r_f = pulseIn(SOut, LOW);
          r_f = map(r_f, 25, 70, 255, 0);
          delay(100);

          // green color filter
          digitalWrite(S2,HIGH);
          digitalWrite(S3,HIGH);
          g_f = pulseIn(SOut, LOW);
          g_f = map(g_f, 25, 70, 255, 0);
          delay(100);

          // blue color filter
          digitalWrite(S2,LOW);
          digitalWrite(S3,HIGH);
          b_f = pulseIn(SOut, LOW);
          b_f = map(b_f, 25, 70, 255, 0);
          delay(100);

          if(r_f <= prevR_F * 1.2  && r_f >= prevR_F * 0.8 && g_f <= prevG_F * 1.2 && g_f >= prevG_F * 0.8 && b_f <= prevB_F * 1.2 && b_f >= prevB_F * 0.8){
            player_amount;
          }
          else{
            player_amount++;
          }
          prevR_F = r_f;
          prevG_F = g_f;
          prevB_F = b_f;

          Serial.println(player_amount);
        }
        else if (complete == 2){
          myStepper.step(512);
          complete = 3;
          // red color filter
          digitalWrite(S2,LOW);
          digitalWrite(S3,LOW);
          r_f = pulseIn(SOut, LOW);
          r_f = map(r_f, 25, 70, 255, 0);
          delay(100);

          // green color filter
          digitalWrite(S2,HIGH);
          digitalWrite(S3,HIGH);
          g_f = pulseIn(SOut, LOW);
          g_f = map(g_f, 25, 70, 255, 0);
          delay(100);

          // blue color filter
          digitalWrite(S2,LOW);
          digitalWrite(S3,HIGH);
          b_f = pulseIn(SOut, LOW);
          b_f = map(b_f, 25, 70, 255, 0);
          delay(100);

          if(r_f <= prevR_F * 1.2  && r_f >= prevR_F * 0.8 && g_f <= prevG_F * 1.2 && g_f >= prevG_F * 0.8 && b_f <= prevB_F * 1.2 && b_f >= prevB_F * 0.8){
            player_amount;
          }
          else{
            player_amount++;
          }

          prevR_F = r_f;
          prevG_F = g_f;
          prevB_F = b_f;
          Serial.println(player_amount);
        }
        else if (complete == 3){
          myStepper.step(-1536);
          complete = 4;
          // red color filter
          digitalWrite(S2,LOW);
          digitalWrite(S3,LOW);
          r_f = pulseIn(SOut, LOW);
          r_f = map(r_f, 25, 70, 255, 0);
          delay(100);

          // green color filter
          digitalWrite(S2,HIGH);
          digitalWrite(S3,HIGH);
          g_f = pulseIn(SOut, LOW);
          g_f = map(g_f, 25, 70, 255, 0);
          delay(100);

          // blue color filter
          digitalWrite(S2,LOW);
          digitalWrite(S3,HIGH);
          b_f = pulseIn(SOut, LOW);
          b_f = map(b_f, 25, 70, 255, 0);
          delay(100);

          if(r_f <= prevR_F * 1.2  && r_f >= prevR_F * 0.8 && g_f <= prevG_F * 1.2 && g_f >= prevG_F * 0.8 && b_f <= prevB_F * 1.2 && b_f >= prevB_F * 0.8){
            player_amount;
          }
          else{
            player_amount++;
          }
          prevR_F = r_f;
          prevG_F = g_f;
          prevB_F = b_f;
          Serial.println(player_amount);
        }
        else if (complete == 4){
          myStepper.step(-512);
          complete = 5;
          // red color filter
          digitalWrite(S2,LOW);
          digitalWrite(S3,LOW);
          r_f = pulseIn(SOut, LOW);
          r_f = map(r_f, 25, 70, 255, 0);
          delay(100);

          // green color filter
          digitalWrite(S2,HIGH);
          digitalWrite(S3,HIGH);
          g_f = pulseIn(SOut, LOW);
          g_f = map(b_f, 25, 70, 255, 0);
          delay(100);

          // blue color filter
          digitalWrite(S2,LOW);
          digitalWrite(S3,HIGH);
          b_f = pulseIn(SOut, LOW);
          b_f = map(b_f, 25, 70, 255, 0);
          delay(100);

          if(r_f <= prevR_F * 1.2  && r_f >= prevR_F * 0.8 && g_f <= prevG_F * 1.2 && g_f >= prevG_F * 0.8 && b_f <= prevB_F * 1.2 && b_f >= prevB_F * 0.8){
            player_amount;
          }
          else{
            player_amount++;
          }
          prevR_F = r_f;
          prevG_F = g_f;
          prevB_F = b_f;
          Serial.println(player_amount);
        }
        else if (complete == 5){
          myStepper.step(1024);
          complete = 0;
          // red color filter
          digitalWrite(S2,LOW);
          digitalWrite(S3,LOW);
          r_f = pulseIn(SOut, LOW);
          r_f = map(r_f, 25, 70, 255, 0);
          delay(100);

          // green color filter
          digitalWrite(S2,HIGH);
          digitalWrite(S3,HIGH);
          g_f = pulseIn(SOut, LOW);
          g_f = map(g_f, 25, 70, 255, 0);
          delay(100);

          // blue color filter
          digitalWrite(S2,LOW);
          digitalWrite(S3,HIGH);
          b_f = pulseIn(SOut, LOW);
          b_f = map(b_f, 25, 70, 255, 0);
          delay(100);

          if(r_f <= prevR_F * 1.2  && r_f >= prevR_F * 0.8 && g_f <= prevG_F * 1.2 && g_f >= prevG_F * 0.8 && b_f <= prevB_F * 1.2 && b_f >= prevB_F * 0.8){
            player_amount;
          }
          else{
            player_amount++;
          }
          prevR_F = r_f;
          prevG_F = g_f;
          prevB_F = b_f;
          Serial.println(player_amount);
        }
      }
      num_players = player_amount;
      btn_press = LOW;
    }
  }
  else if (nav == 2){
    Scroll_Menu(num_players, y_direction, y_count, btn_press);
  }
  if (nav > 2){
    nav = 2;
  }
  if (btn_press == HIGH && nav_sub == 0){
    nav = nav + 1;
    btn_press = LOW;
  }
}
