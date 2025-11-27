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
//"              " allowed max length (14)
  "Wire sequencer",
  "Repair display",
  "Quash overload",
  "Passcode      "
};



  // -----------------------------//
 //         SCREEN FUNCS         //
// -----------------------------//


void clear_display() {
  /*
  Clears the display
  */
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}


void display_menu(int _menu_pos, bool _clear_display = false) {
  /*
  displays the menu in required item position
  */
  if (_clear_display == true) {
    clear_display();
  }

  lcd.setCursor(0, 0);
  lcd.print("Menu:");
  lcd.setCursor(0, 1);
  lcd.print(String(_menu_pos+1) + " ");
  lcd.print(menu_items[_menu_pos]);
}


int button_val = 1023;
String button = " ";
String button_default = " ";
bool actioned_button = false;
bool force_read = false;

bool can_read_buttons(bool _force_read) {
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
  if (_force_read == true) return true;
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


String binary_string = "";

void repair_display() {
  /*
  Fixes a glitching clock display.
  Logic: press the 'select' button
  */
  randomSeed(millis());

  lcd.setCursor(0, 0);
  lcd.print("Compiling...");
  for (int l=0; l<50; l++) {
    for (int i=0; i<16; i++) {
      binary_string += String(random(0,2));
    }
    lcd.setCursor(0, 1);
    lcd.print(binary_string);
    delay(100);
    binary_string = "";
  }
  clear_display();
  lcd.setCursor(0, 0);
  lcd.print("Uploading...");
  delay(2000);
  lcd.print("Done");
  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ToDo: add success comms here!
  delay(1500);
}


int qo_start_secs = 0;
int elapsed_secs = 0;
bool qo_success = false;

bool quash_overload() {
  /*
  Prevents overload causing early device triggering.
  Logic: hold the select button for the required length of time
  */
  lcd.setCursor(0, 0);
  lcd.print("Hold select!");

  if (qo_start_secs == 0) qo_start_secs = millis() / 1000;
  if (button == "s") {
    elapsed_secs = (millis() / 1000) - qo_start_secs;
    lcd.setCursor(0, 1);
    // create loading bar
    for (int i=0; i<elapsed_secs; i++) {
      lcd.print(char(0));
      if (i == 16) {
        //success state
        // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< ToDo: add success comms here!
        return true;
      }
    }
  }
  else {
    qo_start_secs = 0;
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
  // use char(1) or char(0)
  delay(500);

  return false;
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
bool splash = true;
bool hold_clear_display = false;
int menu_pos = 0;

void loop() {
  if (splash == true) {
    lcd.setCursor(0, 0);
    lcd.print("--- Decoder ---");
  }
  if (can_read_buttons(force_read) == false) return;
  if (hold_clear_display == false) clear_display();

  if (in_menu == true) {
    clear_display();
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
    display_menu(menu_pos);

    if (button == "s") in_menu = false;
  }
  else {
    switch (menu_pos) {
      case 0:
        wire_sequencer();
        break;
      case 1:
        repair_display();
        in_menu = true;
        display_menu(menu_pos, true);
        break;
      case 2:
        hold_clear_display = true;
        force_read = true;
        in_menu = quash_overload();
        if (in_menu == true) {
          hold_clear_display = false;
          force_read = false;
          display_menu(menu_pos, true);
        }
        break;
      case 3:
        passcode();
        break;
      default:
        lcd.setCursor(0, 0);
        lcd.print("Unsupported");
        lcd.setCursor(0, 1);
        lcd.print("menu item!");
    }
  }

  actioned_button = true;
}
