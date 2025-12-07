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
#include <Wire.h>

// liquid crystal
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define button_read_pin A0

// communications
#define ping_pin A3 // additional wire allows slave to signal the master for data collection.
#define DECODER_ADDRESS 0x1A
String data_to_send = "  ";
int data_received = -1;

// logic
#define num_menu_items 5
String menu_items[num_menu_items] = {
//"              " allowed max length (14)
  "Wire sequencer",
  "Repair display",
  "Quash overload",
  "Passcode      ",
  "Show all ascii"
};


  // -----------------------------//
 //            COMMS             //
// -----------------------------//


void receive_event(int howMany) {
  /*
  Handler for receiving data from I2C master
  */
  // reset the ping line
  digitalWrite(ping_pin, LOW);
  pinMode(ping_pin, INPUT);

  while(Wire.available()) { 
    data_received = Wire.read();
  }
}

void request_event() {
  /*
  Handler for sending data to I2C master upon request
  */
  Wire.write(data_to_send.c_str());
}

void ping_data_ready(String _data) {
  /*
  Readies the data to be sent and signals I2C master to collect data
  */
  data_to_send = _data;
  pinMode(ping_pin, OUTPUT);
  digitalWrite(ping_pin, HIGH);
}


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
      binary_string += String(random(0, 2));
    }
    lcd.setCursor(0, 1);
    lcd.print(binary_string);
    delay(100);
    binary_string = "";
  }
  clear_display();
  lcd.setCursor(0, 0);
  lcd.print("Uploading...");
  ping_data_ready("2w "); //send back to connected device

  delay(2000);
  lcd.print("Done");
  delay(1500);
}


int qo_start_secs = 0;
int elapsed_half_secs = 0;
bool qo_success = false;

bool quash_overload() {
  /*
  Prevents overload causing early device triggering.
  Logic: hold the select button for the required length of time
  */
  lcd.setCursor(0, 0);
  lcd.print("Hold select!");

  if (qo_start_secs == 0) qo_start_secs = millis() / 500;
  if (button == "s") {
    elapsed_half_secs = (millis() / 500) - qo_start_secs;
    lcd.setCursor(0, 1);
    // create loading bar
    for (int i=0; i<elapsed_half_secs; i++) {
      lcd.print(char(0));
      if (i == 16) {
        //success state
        ping_data_ready("3w ");  //send back to connected device
        lcd.setCursor(0, 0);
        lcd.print("Done            ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        delay(1500);
        //reset the function to run again if selected
        qo_start_secs = 0;
        elapsed_half_secs = 0;
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


int password_len = 16;
String password = "";
String password_buttons = "";
String arrows[4] = {"<", ">", "^", "v"};
String arrow_buttons[4] = {"l", "r", "u", "d"};
int progress = -1;

bool passcode() {
/*
Input the directional passcode to unlock the device
Logic: Match the directional sequence on screen
*/
  // generate password sequence if not generated
  if (progress == -1) {
    randomSeed(millis());
    for (int i=0; i<password_len; i++) {
      int password_val = random(0, 4);
      password += arrows[password_val];
      password_buttons += arrow_buttons[password_val];
    }
    progress = 0;
    button = " ";
    // display start code
    lcd.setCursor(0, 0);
    lcd.print("Decrypting...   ");
    // animation for whole line
    for (int i=0; i<16; i++) {
      lcd.setCursor(i, 1);
      lcd.print(String(char(random(33, 48))));
      delay(200);
    }
    // animation for decrypting each symbol
    for (int i=0; i<16; i++) { // second loop scrolls individual letters and moves along
      for (int j=0; j<5; j++) {
        lcd.setCursor(i, 1);
        lcd.print(String(char(random(33, 48))));
        delay(100);
      }
      lcd.setCursor(i, 1);
      lcd.print(password[i]);
    }
    delay(200);
  }
  lcd.setCursor(0, 0);
  lcd.print(password);
  // check the progress
  if (String(button) == String(password_buttons[progress])) progress++;
  else {
    if (button != " " & button != "s") {
      lcd.setCursor(0, 1);
      lcd.print("Error!          ");
      delay(1500);
      lcd.setCursor(0, 0);
      lcd.print(password);
      progress = 0;
    }
  }
  // display the progress
  String progress_string = "";
  for (int i=0; i<password_len+1; i++) {
    if (i < progress) progress_string += char(255);
    else {
      if (i == progress) progress_string += char(95);
      else progress_string += " ";
    }
  }
  lcd.setCursor(0, 1);
  delay(100);
  lcd.print(progress_string);

  if (progress == password_len){
    ping_data_ready("4w ");  //send back to connected device
    delay(500);
    lcd.setCursor(0, 0);
    lcd.print("Success!        ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    delay(1500);
    //reset function
    progress = -1;
    password = "";
    password_buttons = "";
    return true;
  }
  return false;
}


int place = 0;

void show_ascii() {
  /*
  Displays all ascii characters showable by the display
  */
String ascii = "";
  for (int i=place; i<place+16; i++) {
    ascii += char(i);
  }
  lcd.setCursor(0, 0);
  lcd.print(place);
  lcd.setCursor(0, 1);
  lcd.print(ascii);

  if (button == "d" & place < 256) {
    place += 16;
  }
  if (button == "u"){
    place -= 16;
    if (place < 0) place = 0;
  }
}


  // -----------------------------//
 //          MAIN LOOP           //
// -----------------------------//


void setup(){
    lcd.begin(16, 2);
    pinMode(ping_pin, INPUT);
    Wire.begin(DECODER_ADDRESS);
    Wire.onReceive(receive_event);
    Wire.onRequest(request_event);
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

  if (in_menu == true & button != "s") {
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
  }
  else {
    in_menu = false;
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
        hold_clear_display = true;
        in_menu = passcode();
        if (in_menu == true) {
          hold_clear_display = false;
          force_read = false;
          display_menu(menu_pos, true);
        }
        break;
      case 4:
        show_ascii();
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
