#include <Wire.h> 

// Set the LCD address to 0x27 or 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
}

void start_menu() {
  lcd.clear();
  lcd.setCursor(2, 0); 
  lcd.print("1. New Game!");
  lcd.setCursor(2, 0);
  lcd.noBacklight(); // Turn off backlight
  delay(500);
  lcd.backlight();   // Turn on backlight
  delay(500);
}

void loop() {
  start_menu();
}
