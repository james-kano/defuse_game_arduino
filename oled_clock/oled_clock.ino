/**************************************************************************
 Game countdown clock based upon example code sampled from the below source
 Author: James Kano (2025)

 Adafruit official splash screen has been retained and can be displayed by
 uncommenting display lines in the setup.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 
/**************************************************************************
 Example for Monochrome OLEDs based on SSD1306 drivers code info:
  Written by Limor Fried/Ladyada for Adafruit Industries,
  with contributions from the open source community.
  BSD license

  http://www.adafruit.com/category/63_98

  Example code written for 128x32 pixel display using I2C to communicate

  Adafruit invests time and resources providing this open
  source code, please support Adafruit and open-source
  hardware by purchasing products from Adafruit!
**************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     -1 // sharing Arduino reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] = {
  0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000
};


#define DECODER_ADDRESS 0x1A
#define ping_in_pin A3 // this pin allows I2C slave to signal for data request
String i2c_data_in = ""

// pins for win / lose effects (outbound signals)
#define safe_led_pin 2
#define explode_led_pin 3

#define lose_sig_pin 11
bool lost = false;

// pins for inbound win signals from game modules
//  - extend this array when adding more game modules
const int win_sig_in_pins[] = {4, 5};
int num_win_pins = sizeof(win_sig_in_pins) / sizeof(win_sig_in_pins[0]);
bool all_won = false;

// set the time allowed to complete the game
#define start_secs 120 // 2 min
// #define start_secs = 180; // 3 min
// #define start_secs = 20; // 20 seconds


void win() {
  /*
  Win game conditions
  */
  delay(5000);
  display.clearDisplay();
  display.display();

  digitalWrite(safe_led_pin, HIGH);
}


void lose() {
  /*
  Lose game conditions
  */
  display.clearDisplay();
  display.display();

  pinMode(lose_sig_pin, OUTPUT);
  digitalWrite(lose_sig_pin, HIGH);

  digitalWrite(explode_led_pin, HIGH);

}


void send_to_decoder(int _data_num) {
  /*
  Transmit data to the decoder
  */
  Wire.beginTransmission(DECODER_ADDRESS);
  Wire.write(_data_num); 
  Wire.endTransmission();
}


String request_decode_event() {
  /*
  Request data from the decoder
  */
  String _data_in = "";
  Wire.requestFrom(DECODER_ADDRESS);
  while(Wire.available()) {   // slave may send less than requested
    char c = Wire.read();    // receive a byte as character
    _data_in.concat(c);
  }
  return _data_in;
}


int cursor_vert_2 = 20;
int x_offset = 3;

void draw_time(int _mins, int _secs_t, int _secs_u) {
  /*
  Display the remaining time on screen

  :param _mins: integer of mins (supports single digit)
  :param _secs_t: integer of tens of seconds (supports single digit)
  :param _secs_u: integer of units of seconds (supports single digit)
  */
  display.setTextColor(SSD1306_WHITE); // Draw white text

  display.setTextSize(4);      // Normal 1:1 pixel scale
  int test = 48;
  display.setCursor(0 + x_offset, cursor_vert_2);
  display.write(48);
  display.setCursor(25 + x_offset, cursor_vert_2);
  display.write(_mins + 48);
  display.setCursor(50 + x_offset, cursor_vert_2);
  display.write(':');
  display.setCursor(75 + x_offset, cursor_vert_2);
  display.write(_secs_t + 48);
  display.setCursor(100 + x_offset, cursor_vert_2);
  display.write(_secs_u + 48);
}


int proportion = 0;
int prop_lock = 0;

void draw_remaining_bar(int remaining) {
  /*
  Draws bars above the time that get shorter as time runs out

  :param remaining: remaining time expressed as total seconds
  */
  proportion = (remaining / float(start_secs)) * float(SCREEN_WIDTH);

  // simple bars
  // display.fillRoundRect(0, 0, proportion, 5, display.height()/4, SSD1306_INVERSE);
  // display.fillRoundRect(display.width() - proportion, 7, proportion, 5, display.height()/4, SSD1306_INVERSE);

  // converging bars
  if (remaining / float(start_secs) >= 0.5) {
    display.fillRoundRect(0, 0, proportion, 5, display.height()/4, SSD1306_INVERSE);
    display.fillRoundRect(display.width() - proportion, 7, proportion, 5, display.height()/4, SSD1306_INVERSE);
    prop_lock = proportion;
  } 
  else {
    display.fillRoundRect(prop_lock - proportion, 0, proportion, 5, display.height()/4, SSD1306_INVERSE);
    display.fillRoundRect(prop_lock, 7, proportion, 5, display.height()/4, SSD1306_INVERSE);
  } 
}


void setup() {
  pinMode(ping_in_pin, INPUT);

  // set up the pins for win / lose inbound signals
  for (int i=0; i<num_win_pins; i++) {
    pinMode(win_sig_in_pins[i], INPUT);
  }
  pinMode(lose_sig_pin, INPUT);

  // set up the pins for win / lose effects
  pinMode(explode_led_pin, OUTPUT);
  pinMode(safe_led_pin, OUTPUT);
  digitalWrite(explode_led_pin, LOW);
  digitalWrite(safe_led_pin, LOW);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Uncomment to display the splash screen
  // display.display();
  // delay(2000);

  // Clear the buffer
  display.clearDisplay();

}


int curr_secs = 0;
int time_mins = 0;
int time_secs = 0;
int time_secs_tens = 0;
int time_secs_units = 0;

int num_won = 0;

void loop() {

  // check that all win_sig_in_pins are in win state
  num_won = 0;
  for (int i=0; i<num_win_pins; i++) {
    if (digitalRead(win_sig_in_pins[i]) == HIGH) {
      num_won++;
    }
  }
  // if all game modules won, win condition and exit loop
  if (num_won == num_win_pins) {
    win();
    return;
  }

  // see if data available on the I2C bus
  if (digitalRead(ping_in_pin) == HIGH) {
    i2c_data_in = request_decode_event();
  }

  // calculate time remaining
  curr_secs = start_secs - (millis() / 1000);

  // if time runs out or lose signal, lose condition
  if ((curr_secs < 0) || (digitalRead(lose_sig_pin) == HIGH)) {
    lose();
  }
  // otherwise, update clock
  else {
    time_mins = curr_secs / 60;
    time_secs = curr_secs % 60;
    time_secs_tens = (curr_secs % 60) / 10;
    time_secs_units = (curr_secs % 60) % 10;

    display.clearDisplay();
    draw_remaining_bar(curr_secs);
    draw_time(time_mins, time_secs_tens, time_secs_units);

    display.display();
    delay(100);

  }
}

