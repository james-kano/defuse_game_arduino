/*
  Auto-generated wiring program template for wires game.

  Reads against 3 states:
   - High rail connected
   - Low rail connected
   - Disconnected

  High rail should be connected to digital pin 5.
  Low rail should be connected to digital pin 6.
  All input wires should have 100K pull-down to GND, then conncted to inputs specified below. 
*/


const int win_sig_out = 10;
const int lose_sig_out = 11;
const int lose_sig_in = 12;

const int high_rail = 7;
const int low_rail = 8;

int seed_pin = A6;
int ee_seed = 0;

const int red = A0;
const int yellow = A1;
const int orange = A2;
const int white = A3;
const int blue = A4;
const int green = A5;

const int wires[6] = {red, yellow, orange, white, blue, green};

int curr_pin_states[6] = {0, 0, 0, 0, 0, 0};
int allowed_mistakes[6] = {true, true, true, true, true, true};

//Start light pin declarations
const int top = 2;
const int middle = 3;
const int bottom = 4;

bool wire_change = false;
int progress = 0;
int mistake_count = 0;
int lose_count = 3;


int get_curr_pin_state(int pin) {
  // Gets the current mapped state of a given pin
  // Maps the input value against uneven thresholds for 3-level mapping
  // open = 0, low = 1, high = 2
  int val_in = 0;
  digitalWrite(high_rail, HIGH);
  if (digitalRead(pin) == HIGH) {
    val_in = 2;
  }
  digitalWrite(high_rail, LOW);
  digitalWrite(low_rail, HIGH);
  if (digitalRead(pin) == HIGH) {
    val_in = 1;
  }
  digitalWrite(low_rail, LOW);
  return val_in;
}


void safe() {
  // Actions upon game won
  digitalWrite(win_sig_out, HIGH);
}


void lose () {
  // Actions upon game lose
  digitalWrite(lose_sig_out, HIGH);
  digitalWrite(top, LOW);
  digitalWrite(middle, LOW);
  digitalWrite(bottom, LOW);
  digitalWrite(high_rail, LOW);
  digitalWrite(low_rail, LOW);
  
}


void check_progress() {
  // Checks the progress and increments if successful
  bool correct = false;
  switch (progress) {
    case 0:
      if (curr_pin_states[1] == 2 && curr_pin_states[0] == 1) {
        correct = true;
      }
      else {
        if ((curr_pin_states[1] == 1 && allowed_mistakes[1] == false) || (curr_pin_states[0] == 2 && allowed_mistakes[0] == false)) {
          mistake_count++;
          Serial.println("Oops!");
        }
      }
      break;
    case 1:
      if (curr_pin_states[1] == 2 && curr_pin_states[0] == 1 && curr_pin_states[3] == 1) {
        correct = true;
      }
      else {
        if ((curr_pin_states[1] == 1 && allowed_mistakes[1] == false) || (curr_pin_states[0] == 2 && allowed_mistakes[0] == false) || (curr_pin_states[3] == 2 && allowed_mistakes[3] == false)) {
          mistake_count++;
          Serial.println("Oops!");
        }
      }
      break;
    case 2:
      if (curr_pin_states[1] == 2 && curr_pin_states[0] == 1 && curr_pin_states[3] == 1 && curr_pin_states[2] == 2) {
        correct = true;
      }
      else {
        if ((curr_pin_states[1] == 1 && allowed_mistakes[1] == false) || (curr_pin_states[0] == 2 && allowed_mistakes[0] == false) || (curr_pin_states[3] == 2 && allowed_mistakes[3] == false) || (curr_pin_states[2] == 1 && allowed_mistakes[2] == false)) {
          mistake_count++;
          Serial.println("Oops!");
        }
      }
      break;
    case 3:
      if (curr_pin_states[1] == 2 && curr_pin_states[0] == 2 && curr_pin_states[3] == 1 && curr_pin_states[2] == 2 && curr_pin_states[5] == 1) {
        correct = true;
      }
      else {
        if ((curr_pin_states[1] == 1 && allowed_mistakes[1] == false) || (curr_pin_states[0] == 1 && allowed_mistakes[0] == false) || (curr_pin_states[3] == 2 && allowed_mistakes[3] == false) || (curr_pin_states[2] == 1 && allowed_mistakes[2] == false) || (curr_pin_states[5] == 2 && allowed_mistakes[5] == false)) {
          mistake_count++;
          Serial.println("Oops!");
        }
      }
      break;
    case 4:
      if (curr_pin_states[1] == 2 && curr_pin_states[0] == 1 && curr_pin_states[3] == 0 && curr_pin_states[2] == 2 && curr_pin_states[5] == 1) {
        correct = true;
      }
      else {
        if ((curr_pin_states[1] == 1 && allowed_mistakes[1] == false) || (curr_pin_states[0] == 2 && allowed_mistakes[0] == false) || (curr_pin_states[2] == 1 && allowed_mistakes[2] == false) || (curr_pin_states[5] == 2 && allowed_mistakes[5] == false)) {
          mistake_count++;
          Serial.println("Oops!");
        }
      }
      break;
  }
  
  if (correct == true) {
      // set curr_pin_states
      for (int i=0; i<6; i++) {
        int curr_pin_state = get_curr_pin_state(wires[i]);
        int prev_pin_state = curr_pin_states[i];
      }
      progress ++;
  }
  Serial.print("Progress: ");
  Serial.println(progress);
}


void setup() {
  // setup random seed
  ee_seed = analogRead(seed_pin);
  randomSeed(ee_seed);
  Serial.begin(9600);
  // set the current wire conditions
  for (int i=0; i<6; i++) {
    int curr_pin_state = get_curr_pin_state(wires[i]);
    curr_pin_states[i] = curr_pin_state;
  }
  // setup the win / lose signal wires
  pinMode(win_sig_out, OUTPUT);
  digitalWrite(win_sig_out, LOW);
  pinMode(lose_sig_out, OUTPUT);
  digitalWrite(lose_sig_out, LOW);
  pinMode(lose_sig_in, INPUT);
  // setup the rail pins;
  pinMode(high_rail, OUTPUT);
  digitalWrite(high_rail, LOW);
  pinMode(low_rail, OUTPUT);
  digitalWrite(low_rail, LOW);
  // set the sequence indicator LEDs
  pinMode(top, OUTPUT);
  digitalWrite(top, HIGH);
  pinMode(middle, OUTPUT);
  digitalWrite(middle, HIGH);
  pinMode(bottom, OUTPUT);
  digitalWrite(bottom, LOW);
  // set the wire pins to be digital
  pinMode(red, INPUT);
  pinMode(yellow, INPUT);
  pinMode(orange, INPUT);
  pinMode(white, INPUT);
  pinMode(blue, INPUT);
  pinMode(green, INPUT);

}


void loop() {
  // Game loop
  // check if lost elsewhere
  if (lose_sig_in == HIGH) {
    mistake_count == lose_count;
  }
  // check for wire change
  wire_change = false;
  for (int i=0; i<6; i++) {
    allowed_mistakes[i] = true;
  }
  if (mistake_count < lose_count) {
    for (int i=0; i<6; i++) {
      int curr_pin_state = get_curr_pin_state(wires[i]);
      int prev_pin_state = curr_pin_states[i];
      if (curr_pin_state != prev_pin_state) {
        wire_change = true;
        curr_pin_states[i] = curr_pin_state;
        allowed_mistakes[i] = false;
        Serial.print("A");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(curr_pin_state);
        delay(100);
      }
    }
  }  if (wire_change == true && mistake_count < lose_count) {
    check_progress();
  }

  if (progress == 5) {
    safe();
  }
  else {
    if (mistake_count == lose_count) {
      lose();
    }
  }
}
