/**************************************************************************
 Set of 3 games playable on 8-segment TM2638 module.

 Copyright (C) 2025  James Kano
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

#include <TM1638.h>

//choose digital pins compatibles with your board
#define STB 10 // Strobe digital pin
#define CLK 11 // clock digital pin
#define DIO 12 // data digital pin

#define lose_sig 4
#define win_sig 2

#define seed_pin A1 // uses an analogue signal to set the random seed

TM1638 tm(CLK,DIO,STB);


uint8_t low_brightness=PULSE1_16;
uint8_t high_brightness=PULSE14_16;
int delay_speed = 100;


int s = 109;
int a = 119;
int f = 113;
int e = 121;
int r = 80;
int o_low = 92;
int o_up = 63;

const int ints[10] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 103};


const int safe[8] = {64, 0, s, a, f, e, 0, 64};
const int error[8] = {e, r, r, o_low, r, 0, 0, 0};


void display_line(int _segs[]) {
  for (int p=0; p<8; p++) {
      tm.displayDig(7-p, _segs[p]);
  }
}

byte flip_byte(byte c) {
  char r=0;
  for(byte i = 0; i < 8; i++){
    r <<= 1;
    r |= c & 1;
    c >>= 1;
  }
  return r;
}

  // -----------------------------//
 //          ANIMATIONS          //
// -----------------------------//

void roll(int _delay) {
  // roll animation (math)
  int current_bit = 1;
  for (int a=1; a<=5; a++) {
    for (uint8_t p=0; p<8; p++) {
      tm.displayDig(7-p, current_bit);
    }
    delay(_delay);
    current_bit = current_bit << 1;
  }
}


void wave(int _delay) {
  // wave animation (math)
  int current_bit = 1;
  for (int a=1; a<=6; a++) {
    int digit = current_bit;
    
    for (int p=0; p<8; p++) {
      tm.displayDig(7-p, digit);
      digit = digit << 1;
      if (digit == 64) {
        digit = 1;
      }
    }
    current_bit = current_bit << 1;
    delay(_delay);
  }
}


void load (int _delay, bool _with_line) {
  // loads with dots
  int blank = 0;
  int dot = 128;
  if (_with_line == true) {
    dot = 136;
  }
  for (int i=0; i<=8; i++) {
    for (int p=0; p<8; p++) {
      if (p <= i) {
        tm.displayDig(7-p, dot);
      }
      else {
        tm.displayDig(7-p, blank);
      }
      delay(_delay);
    }
  }
}

void unload (int _delay) {
  // loads with dots
  int blank = 0;
  int dot = 128;
  for (int i=8; i>=0; i--) {
    for (int p=0; p<8; p++) {
      if (p <= i) {
        tm.displayDig(7-p, dot);
      }
      else {
        tm.displayDig(7-p, blank);
      }
      delay(_delay);
    }
  }
}

void lose() {
  //lights all segments and leds in exploding pattern
  int lose[8] = {ints[8],ints[8],ints[8],ints[8],ints[8],ints[8],ints[8],ints[8]};
  display_line(lose);
  tm.writeLeds(255);
  pinMode(lose_sig, OUTPUT);
  digitalWrite(lose_sig, HIGH);
}

  //-------------------------//
 //          GAMES          //
//-------------------------//
// Notes:
//  - progress will always start at -1 to allow game setup.
//  - games must be stateful as control interfacing is handled in main loop.
// Game template for turn-based games:
// void game (int difficulty, int input) {
//   if (progress == -1) {
//     //do any setup here
//   }
//   else {
//     if (input == correct) {
//       // correct input action & progress here
//     }
//     else {
//       if (lives > 0) {
//         // incorrect entry response here 
//       }
//       else {
//         alive = false;
//       }
//       lives--;
//     }
//   }
// }

int progress = -1;
int lives = 2;
bool alive = true;


// --- memory game --- //
int sequence[8] = {};
int progress_display[8] = {0,0,0,0,0,0,0,0};
bool await_input = false;

void play_memory(int seq_len, int _button) {
  // Game to repeat a given sequence
  if (progress == -1) {
    // setup game sequence and seg display
    for (int i=0; i<seq_len; i++) {
      sequence[i] = 1 << random(8);
      progress_display[i] = 64; //middle line
      tm.writeLeds(sequence[i]);
      delay(500);
      tm.writeLeds(0);
      delay(250);
    }    
    display_line(progress_display);
    tm.writeLeds(0);
    progress++; //progress=0
  }
  else {
    // main game logic
    if (_button == sequence[progress]) {
      //correct answer
      progress_display[progress] = 0;
      display_line(progress_display);
      progress++;
    }
    else {
      //incorrect answer
      if (lives > 0) {
        display_line(error);
        delay(1000);
        for (int p=0; p<progress; p++) {
          progress_display[progress] = e;
        }
        display_line(progress_display);
      }
      else {
        alive = false;
      }
      lives--;
    }
  }  
}


// --- mathematics game --- //
int answer_num = 0;
int led_num = 0;
int answer_ints[3] = {0, 0, 0};
int digit_selection[8] = {};
int digit_ints[8] = {};
int place_1 = 10;
int place_2 = 10;

void play_math(int difficulty, int input) {
  if (progress == -1) {
    answer_num = random(1, 257);
    int itoa_num = answer_num;
    for (int i=0; i<3; i++) {
      answer_ints[2-i] = itoa_num % 10;
      itoa_num = itoa_num - answer_ints[2-i];
      itoa_num = int(itoa_num / 10);
    }
    load(20, true);
    delay(500);
    unload(20);
    // setup digit display array
    for (int i=0; i<8; i++) {
      int rand_digit = random(0, 10);
      digit_ints[i] = rand_digit;
      digit_selection[i] = ints[rand_digit];
    }
    led_num = flip_byte(answer_num);
    tm.writeLeds(led_num);
    // add answer digits in random places
    for (int i=0; i<3; i++) {
      int answer_place = random(8);
      while ((answer_place == place_1) || (answer_place == place_2)) {
        answer_place = random(8);
      }
      if (place_1 == 10) {
        place_1 = answer_place;
      }
      else {
        if (place_2 == 10) {
          place_2 = answer_place;
        }
      }
      digit_ints[answer_place] = answer_ints[i];
      digit_selection[answer_place] = ints[answer_ints[i]];
    }
    // display the digit option set
    display_line(digit_selection);
    progress++;
    while (answer_ints[progress] == 0) {
      progress++;
    }
  }

  else {
    // main game logic
    int input_place = get_input_place(input);
    if (digit_ints[input_place] == answer_ints[progress]) {
      // correct input action & progress here
      tm.writeLeds(input);
      delay(500);
      tm.writeLeds(led_num);
      progress++;
    }
    else {
      if (lives > 0) {
        // incorrect entry response here
        display_line(error);
        delay(1000);
        display_line(digit_selection);
        progress = 0; 
      }
      else {
        alive = false;
      }
      lives--;
    }
  }
}


// --- spatial reasoning game --- //
int broken_digits[4] = {0, 0, 0, 0};
int frag_map[4] = {0, 0, 0, 0}; // shows where a broken_digit's fragment is in fragments
int fragments[4] = {0, 0, 0, 0};
int frag_display[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int used_frag_places[4] = {10, 10, 10, 10};

void play_spatial (int difficulty, int input) {
  if (progress == -1) {
    //create the fragments, separate, store and map
    for (int i=0; i<4; i++) {
      unload(10);

      int fragment = 0;
      int frag_lines = random(7);
      for (int i=0; i<frag_lines; i++) {
        int frag_shift = random(7);
        int frag_line = 1 << frag_shift;
        fragment += frag_line;
      }
      // place fragment in a non-repeating random location
      int frag_place = 0;
      bool place_taken = true;
      while (place_taken == true) {
        place_taken = false;
        frag_place = random(4);
        for (int i=0; i<4; i++) {
          if (frag_place == used_frag_places[i]) {
            place_taken = true;
          }
        }
      }
      used_frag_places[i] = frag_place;

      int broken_digit = ints[8] - fragment; // 8 - fragment
      broken_digits[i] = broken_digit;
      frag_display[i] = broken_digit;
      frag_map[i] = frag_place;
      fragments[frag_place] = fragment;

    }
    // complete the display by adding the fragments
    for (int i=4; i<8; i++) {
      frag_display[i] = fragments[i-4];
    }
    display_line(frag_display);
    tm.writeLeds(1);
    progress++;
  }

  else {
    // main game logic
    int input_place = get_input_place(input);
    int correct = frag_map[progress] + 4;
    if (input_place == correct) {
      // correct input action & progress here
      frag_display[progress] = ints[8];
      display_line(frag_display);
      tm.writeLeds(1 << progress);
      progress++;
    }
    else {
      if (lives > 0) {
        display_line(error);
        delay(1000);
        display_line(frag_display);
      }
      else {
        alive = false;
      }
      lives--;
    }
  }
}

int exec_game(int _game_select, int _win_len, int _buttons) {
  switch (_game_select) {
    case 0:
      _win_len = 5;
      play_memory(_win_len, _buttons);
    break;
    case 1:
      _win_len = 4;
      play_spatial(_win_len, _buttons);
    break;
    case 2:
      _win_len = 3;
      play_math(_win_len, _buttons);
    break;
  }
  return _win_len;
}

  //--------------------------------------//
 //          CONTROL INTERFACING         //
//--------------------------------------//

bool pressed = false;
int give_button_num() {
  //prevents double-registering button presses
  int _buttons = tm.getButtons();
  if (_buttons != 0) {
    if (pressed == false) {
      pressed = true;
      Serial.print("Button: ");
      Serial.println(_buttons);
      return _buttons;
    }
  }
  else {
    pressed = false;
  }
  return 0;
}

int get_input_place(int _input) {
  // convert the button input back to useful array place
  int _input_place = 0;
  int _shift_input = _input;
  for (int i=0; i<8; i++) {
    if (_shift_input == 1) {
      _input_place++;
    }
    else {
      _shift_input = _shift_input >> 1;
    }
  }
  _input_place = 8 - _input_place;

  return _input_place;
}


  //------------------------//
 //          MAIN          //
//------------------------//
int ee_address = 0;
int ee_seed = 100;

int buttons = 0;
int win_len = 0; // set in exec_game() fore each game.
int final_display = 0;

int num_games = 3;
int game_select = 0;
int game_select_leds[3] = {1, 3, 7};

int standby = 0;


void setup() {
  // setup win outbound signal and communal lose signal
  // pinMode(seed_pin, INPUT);
  pinMode(lose_sig, INPUT);
  pinMode(win_sig, OUTPUT);
  digitalWrite(win_sig, LOW);

  Serial.begin(9600);
  tm.reset();
  tm.displaySetBrightness(low_brightness);
  ee_seed = analogRead(seed_pin);
  randomSeed(ee_seed);
  Serial.println(ee_seed);
  // tm.test();
  for (int rep=0; rep<5; rep++) {
    roll(50);
  }
  game_select = random(num_games);
  Serial.print("game select: ");
  Serial.println(game_select);

  //standby mode sub-loop
  while (standby < 2) {
    if (alive == true) {
      tm.writeLeds(game_select_leds[game_select]);
      buttons = give_button_num();
    }
    if (buttons != 0 || digitalRead(lose_sig) == HIGH) {
      if (buttons == 64 && digitalRead(lose_sig) == LOW) {
        standby++;
      }
      else {
        lose();
        final_display++;
        alive = false;
      }
    }
  }
  delay(1000);
  //setup the selected game
  win_len = exec_game(game_select, win_len, buttons);
}


void loop() {
  //main loop
  // check if loste elsewhere
  if (digitalRead(lose_sig) == HIGH) {
    alive = false;
    final_display=0;
  }
  //continue game
  buttons = give_button_num();

  if ((progress < win_len) && (alive == true)) {

    // only go into game logic if input registered!
    if (buttons !=0) {
      win_len = exec_game(game_select, win_len, buttons);
    }
  }
  else {
    if (alive == true) {
      if (final_display == 0) {
        tm.writeLeds(0);
        // WIN STATE
        for (int rep=0; rep<3; rep++) {
          wave(75);
        }
        display_line(safe);
        final_display++;
        digitalWrite(win_sig, HIGH);
      }
    }
    else {
      if (final_display == 0) {
        // LOSE STATE
        lose();
        final_display++;
      }
    }
  }
    
}