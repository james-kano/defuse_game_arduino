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

#define num_menu_items 4
String menu_items[num_menu_items] = {
//"                " allowed max length
  "1 Wire sequencer",
  "2 Repair display",
  "3 Quash overload",
  "4 Passcode      "
};


  // -----------------------------//
 //         SCREEN FUNCS         //
// -----------------------------//


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


  // -----------------------------//
 //            GAMES             //
// -----------------------------//


void wire_sequencer() {
  /*
  Provides instructions for the wiring interface
  Logic: instructions match the defusing sequence produced by the Generate_wires_game
  notebook.
  */
}


void repair_display() {
  /*
  Fixes a glitching clock display.
  Logic: press the 'select' button
  */
  if (button == "s") {

  }
}


int qo_start_millis = 0;

void quash_overload() {
  /*
  Prevents overload causing early device triggering.
  Logic: hold the select button for the required length of time
  */
  if (rd_start_millis == 0) rd_start_millis = millis();
  curr_secs = start_secs - (millis() / 1000);

}


void passcode() {
/*
Input the directional passcode to unlock the device
Logic: Match the directional sequence on screen
*/

}


  // -----------------------------//
 //          MAIN LOOP           //
// -----------------------------//


void setup(){
    lcd.begin(16, 2);
}


bool in_menu = true;
int menu_pos = 0;
bool splash = true;

void loop() {
  if (splash == true) {
    lcd.setCursor(0, 0);
    lcd.print("--- Decoder ---");
  }
  if (can_read_buttons() == false) return;
  clear_display();

  if (in_menu == true) {
    // action if up / down button
    if (button == "u") menu_pos -= 1;
    if (menu_pos < 0) menu_pos = 0;
    if (button == "d") menu_pos += 1;
    if (menu_pos > num_menu_items - 1) menu_pos = num_menu_items - 1;
    if (splash == true) {
      splash = false;
      menu_pos = 0;
    }

    // display the menu item
    lcd.setCursor(0, 0);
    lcd.print("Menu:");
    lcd.setCursor(0, 1);
    lcd.print(menu_items[menu_pos]);

    if (button == "s") in_menu = false;
  }
  else {
    switch (menu_pos) {
      case 0:
        wire_sequencer();
        break;
      case 1:
        repair_display();
        break;
      case 2:
        quash_overload();
        break;
      case 3:
        passcode();
        break;
    }
  }

  actioned_button = true;
}
