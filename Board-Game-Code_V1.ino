const int joy_x = A1;
const int joy_y = A0;
const int joy_btn = 9;
int btn_state;     
int lastButtonState = LOW;
int btn_press = LOW; 
unsigned long lastDebounceTime = 0; 
unsigned long debounceDelay = 50;
int nav = 0;
int move_x;
int move_y;
int num_players;
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
  if (nav == 0){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(13, 20);
    display.print("New, Game!");
    display.display();
    if (btn_press == HIGH){
      nav = nav + 1;
      btn_press = LOW;
    }
  }
  else if (nav == 1){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 0);
    display.println("Number of ");
    display.println("Players: ");
    display.display();
    if (btn_press == HIGH){
      nav = nav + 1;
      btn_press = LOW;
    }
  }
  else if (nav == 2){
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(0, 0);
    display.println("Number of ");
    display.println("Players: ");
    display.setTextColor(WHITE, BLACK);
    if (move_y == "UP"){
      num_players = num_players + 1;
    }
    if (move_y == "DOWN"){
      num_players = num_players - 1;
    }
    display.print(num_players);
    if (btn_press == HIGH){
      nav = nav - 1;
      btn_press = LOW;
    }
 }
}

void joy_val() {
  int val_x = analogRead(joy_x);
  Serial.print("X: ");
  Serial.print(val_x);
  Serial.print(" ");

  int val_y = analogRead(joy_y);
  Serial.print("Y: ");
  Serial.print(val_y);
  Serial.print(" ");

  int val_btn = digitalRead(joy_btn);
  Serial.print("btn: ");
  Serial.print(val_btn);

  if (val_x > 900) {
    Serial.print(" RIGHT ");
    move_x = 1;
  }
  else if (val_x < 100){
    Serial.print(" LEFT ");
    move_x = 0;
  }

  if (val_y > 900) {
    Serial.print(" DOWN ");
    move_y = 0;
  }
  else if (val_y < 100){
    Serial.print(" UP ");
    move_y = 1;
  }

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
  sys_menu();
  joy_val();
}
