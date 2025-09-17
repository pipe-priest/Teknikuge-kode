#include <SoftwareSerial.h>

int bluetoothTx = 2;
int bluetoothRx = 3;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

char data; 

const int PIN13 = 13;
const int PIN12 = 12;
const int PIN11 = 11;
const int PIN10 = 10;
const int PIN9 = 9;



const int pin6 = 6;
const int pin4 = 4;

const int PINA0 = A0;
const int PINA1 = A1;


// ---- Arming switch setup ----
const int armButtonPin = 7;   // Button pin (connect to GND when pressed)
bool armed = false;           // System state
bool lastButtonState = HIGH;  // Track button state

void setup() {
  Serial.begin(9600);
  bluetooth.begin(9600);

  pinMode(PIN13, OUTPUT);
  pinMode(PIN12, OUTPUT);
  pinMode(PIN11, OUTPUT);
  pinMode(PIN10, OUTPUT);
  pinMode(pin4, OUTPUT);

  analogWrite(PIN9, 250);
  digitalWrite(pin6, HIGH);

  pinMode(PINA1, INPUT);
  pinMode(PINA0, INPUT);

  pinMode(armButtonPin, INPUT_PULLUP); // momentary push button
  pinMode(LED_BUILTIN, OUTPUT);        // indicator LED
}

void loop() {
  // ---- Handle arming button with delay debounce ----
  bool buttonState = digitalRead(armButtonPin);

  // Detect press (transition HIGH -> LOW)
  if (lastButtonState == HIGH && buttonState == LOW) {
    armed = !armed; // Toggle armed state
    digitalWrite(LED_BUILTIN, armed ? HIGH : LOW);
    delay(200); // debounce + simple press delay
  }

  lastButtonState = buttonState;

  // ---- Bluetooth receive ----
//  if (bluetooth.available()) {
//    data = bluetooth.read();
//    Serial.println(data);
//  }

  // ---- Read joysticks ----
  int senH = analogRead(PINA0);
  int senV = analogRead(PINA1);
 Serial.println(senH);
 Serial.println(senV);

  // ---- Only move if armed ----
  if (armed) {
    if (senV < 350) {
      hoejreStop();
    } else {
      hoejreDrejTil();
    }

    if (senH < 500) {
      venstreStop();
    } else {
      venstreDrejTil();
    }

    if (senH < 500 && senV < 500) {
      digitalWrite(pin4, HIGH);
    }
  } else {
    // Disarmed: stop everything
    hoejreStop();
    venstreStop();
    digitalWrite(pin4, LOW);
  }
}

// ---- Motor functions ----
void hoejreDrejTil() {
  digitalWrite(PIN11, HIGH);
  digitalWrite(PIN10, LOW);
  analogWrite(pin6,190);


}
void venstreDrejTil() {
  digitalWrite(PIN13, HIGH);
  digitalWrite(PIN12, LOW);
  analogWrite(PIN9,110);
 
}
void hoejreStop() {
  analogWrite(pin6,255);
  digitalWrite(PIN10, LOW);
  digitalWrite(PIN11, LOW);
}
void venstreStop() {
  analogWrite(PIN9, 255);
  digitalWrite(PIN12, LOW);
  digitalWrite(PIN13, LOW);
}
