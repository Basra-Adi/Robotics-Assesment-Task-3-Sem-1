const int joy_x = A1;
const int joy_y = A0;
const int joy_btn = 9;

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
  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(13, 20);
  display.print("New, Game!");
  display.display();
}

void joy_val() {
  int val_x = analogRead(joy_x);
  Serial.print("X: ");
  Serial.print(val_x);
  Serial.print(" ");
  //int val_y = analogRead(joy_y);
  //Serial.print("Y: ")
  //Serial.println(val_y);
  //Serial.print(" ");
  int val_btn = analogRead(joy_btn);
  Serial.print("btn: ");
  Serial.println(val_btn);
}

void loop() {
  sys_menu();
  joy_val();
}

