
/*
  Basic wiring program (POC) for wires game.
  This verifies that the wires are reading correctly.

  Reads against 3 states:
   - High rail connected
   - Low rail connected
   - Disconnected

  High rail sould be connected to 3V3 terminal.
  Low rail should be connected by 10K to 3V3 and 10K to GND (sits between two 10K resistors).
  All input wires should have 100K pull-down to GND, then conncted to inputs specified below. 
*/
const int red_pin = A0;
const int yellow_pin = A1;
const int orange_pin = A2;
const int white_pin = A3;
const int blue_pin = A4;
const int green_pin = A5;

const int wires[6] = {red_pin, yellow_pin, orange_pin, white_pin, blue_pin, green_pin};

int curr_pin_states[6] = {0, 0, 0, 0, 0, 0};
int allowed_opens[6] = {0, 0, 0, 0, 0, 0};

const int low_thresh = 100;
const int up_thresh = 500;

bool wire_change = false;
int progress = 0;


int mapPinValue(int input) {
  // Maps the input value against uneven thresholds for 3-level mapping
  // open = 0, low = 1, high = 2
  int output = 0;
  if (input > up_thresh) {
    output = 2;
  }
  else {
    if (input > low_thresh) {
      output = 1;
    }
  }
  return output;
}


int get_curr_pin_state(int pin) {
  // Gets the current mapped state of a given pin
  int val_in = analogRead(pin);
  val_in = mapPinValue(val_in);
  return val_in;
}


void safe() {
  // actions upon game won

}


void lose () {
  // actions upon game lose

}


bool check_allowed_opens() {
// Checks the progress and increments if successful
  bool _allowed_opens = false;
  switch (progress) {
    case 1:
      // allowed opens should be [1 for wire in wires if allowed_open else 0]
      break;
    case 2:
      // statements
      break;
    default:
      // statements

      break;
  }
}

void check_progress() {
  // Checks the progress and increments if successful
  switch (progress) {
    case 1:
      // compare against required
      // if correct:
        // set curr_pin_states
        // progress ++
      // else:
        // check allowed_opens
      break;
    case 2:
      // statements
      break;
    default:
      // statements

      break;
  }
}

void setup() {
  // set the current wire conditions (and allowed opens to starting opens)
  for (int i=0; i<6; i++) {
    int curr_pin_state = get_curr_pin_state(wires[i]);
    curr_pin_states[i] = curr_pin_state;
    if (curr_pin_state == 0) {
      allowed_opens[i] = 1;
    }

  }


}


void loop() {
  // run the game logic
  wire_change = false;
  // check for wire change
  for (int i=0; i<6; i++) {
    int curr_pin_state = get_curr_pin_state(wires[i])
    int prev_pin_state = curr_pin_states[i]
    if (curr_pin_state != prev_pin_state) {
      wire_change = true;
    }
  }

  if (wire_change == true) {
    check_progress()
  }

  if (progress > 6) {
    safe();
  }
  else {
    if (progress < 0) {
      lose();
    }
  }

}
