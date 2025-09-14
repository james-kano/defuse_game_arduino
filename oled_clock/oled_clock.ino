/**************************************************************************
 Game countdown clock based upon example code sampled from the below source
 Author: James Kano (2025)

 Adafruit official splash screen has been retained and can be displayed by
 uncommenting display lines in the setup.

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

const int win_sig_pin = ;
const int lose_sig_pin = 11;
bool lost = false;


void setup() {
  pinMode(win_sig_pin, INPUT);
  pinMode(lose_sig_pin, INPUT);
  pinMode(12, OUTPUT);

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

int start_secs = 180; // 3 min
// int start_secs = 20; // 20 seconds

void loop() {

  if (digitalRead(win_sig_pin) == HIGH) {
    delay(5000)
    display.clearDisplay();
    display.display();
    return;
  }
  
  if (digitalRead(lose_sig_pin) == HIGH) {
    lost = true;
  }

  curr_secs = start_secs - (millis() / 1000);

  if ((curr_secs < 0) || (lost == true)) {
    display.clearDisplay();
    display.display();
    lose();

  }
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

void lose() {
  pinMode(lose_sig_pin, OUTPUT);
  digitalWrite(lose_sig_pin, HIGH);
  digitalWrite(12, HIGH);
}


int cursor_vert_2 = 20;
int x_offset = 3;

void draw_time(int _mins, int _secs_t, int _secs_u) {

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

