/**************************************************************************
 Decoder for defuse game. This is intended for use with the DFRobot LCD 
 KeyPad Shield. The decoder attaches via I2C devices to the 
 Author: James Kano (2025)

 LCD KeyPad Shield For Arduino SKU DFR0009:
 https://wiki.dfrobot.com/LCD_KeyPad_Shield_For_Arduino_SKU__DFR0009

 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.

/**************************************************************************/


#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define button_read_pin A0

void clear_display() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}


int button_val = 1023;
String button = " ";
String button_default = " ";
bool actioned_button = false;

bool can_read_buttons() {
  /*
  Logic for handling single press only, thenrelease bfore next press
  */
  output_buttons();
  if (button == button_default) {
    actioned_button = false;
  }
  else {
    if (actioned_button == false) return true;
  }
  return false;
}


void output_buttons() {
  button_val = analogRead(button_read_pin);
  button = button_default;
  if (button_val < 100) {
    button = "r";
  }
  if (button_val > 100 && button_val < 300) {
    button = "u";
  }
  if (button_val > 300 && button_val < 500) {
    button = "d";
  }
  if (button_val > 500  && button_val < 700) {
    button = "l";
  }
  if (button_val > 700 && button_val < 900) {
    button = "s";
  }
}


int cursor_x = 0;
int cursor_y = 0;

void set_cursor() {
  if (button == "l") cursor_x -= 1;
  if (button == "r") cursor_x += 1;
  if (button == "d") cursor_y += 1;
  if (button == "u") cursor_y -= 1;
  
  // prevent curosor moving off screen
  if (cursor_x < 0) cursor_x = 0;
  if (cursor_y < 0) cursor_y = 0;
  if (cursor_x > 15) cursor_x = 15;
  if (cursor_y > 1) cursor_y = 1;
}


void setup(){
    lcd.begin(16, 2);
}


void loop() {
  if (can_read_buttons() == true) {
    clear_display();
    set_cursor();
    lcd.setCursor(cursor_x, cursor_y);
    lcd.print(button);

    actioned_button = true;
  }
}
