
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
const int green_pin = A0;
const int blue_pin = A1;
const int white_pin = A2;
const int orange_pin = A3;
const int yellow_pin = A4;
const int red_pin = A5;

const int low_thresh = 100;
const int up_thresh = 500;

const bool debug = true;
const int raw_debug = false;

int mapPinValue(int input) {
  // Maps the input value against uneven thresholds for 3-level mapping
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

void printWireValue(int val_pin, String col) {
  // Prints the mapped value of a colour to Serial for debugging
  int val_in = analogRead(val_pin);
  if (raw_debug == false) {
    val_in = mapPinValue(val_in);
  }
  Serial.print(col + ": ");
  Serial.println(val_in);
}

void setup() {

  if (debug == true) {
    Serial.begin(9600);
  }

}

void loop() {
  // put your main code here, to run repeatedly:
if (debug == true) {
  printWireValue(green_pin, "Green");
  printWireValue(blue_pin, "Blue");
  printWireValue(white_pin, "White");
  printWireValue(orange_pin, "Orange");
  printWireValue(yellow_pin, "Yellow");
  printWireValue(red_pin, "Red");
}

Serial.println("");
delay(1000);

}
