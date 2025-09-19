#include <SoftwareSerial.h>

int bluetoothTx = 2;
int bluetoothRx = 3;

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

char data; 

const int PIN13 = 13;
const int PIN12 = 12;
const int PIN11 = 11;
const int PIN10 = 10;
const int PIN9 = 9; //enable


const int pin6 = 6;  
//Deadmann switch const int pin4 = 4; //kanon

const int PINA0 = A0;
const int PINA1 = A1;
const int PINA3 = A3;

bool lastBlockedLeft = false;
bool lastBlockedRight = false;

unsigned long driveStartTime = 0;
bool timedDriveActive = false;

// ---- Function prototypes ----

// ---- Motor functions ----
void hoejreDrejTil() {
  digitalWrite(PIN10, LOW);
  digitalWrite(PIN11, HIGH);
  analogWrite(pin6,170);
}
void venstreDrejTil() {
  digitalWrite(PIN12, LOW);
  digitalWrite(PIN13, HIGH);
  analogWrite(PIN9,110);
}
void hoejreStop() {
  analogWrite(pin6,255);
  digitalWrite(PIN10, HIGH);
  digitalWrite(PIN11, HIGH);
}
void venstreStop() {
  analogWrite(PIN9, 255);
  digitalWrite(PIN12, HIGH);
  digitalWrite(PIN13, HIGH);
}

void hoejreBaglens() {
  analogWrite(pin6,170);
  digitalWrite(PIN10, HIGH);
  digitalWrite(PIN11, LOW);
  
}
void venstreBaglens() {
  analogWrite(PIN9, 90);
  digitalWrite(PIN12, HIGH);
  digitalWrite(PIN13, LOW);
}

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
  // pinMode(pin4, OUTPUT);

  analogWrite(PIN9, 250);
  digitalWrite(pin6, HIGH);

  pinMode(PINA1, INPUT);
  pinMode(PINA0, INPUT);
  pinMode(PINA3, INPUT);

  pinMode(armButtonPin, INPUT_PULLUP); // momentary push button
  pinMode(LED_BUILTIN, OUTPUT);        // indicator LED
}

unsigned long turnStartTime = 0;
bool turningActive = false;
bool turnRight = false; // true = right, false = left

void loop() {
    bool buttonState = digitalRead(armButtonPin);

    // ---- Handle arming button ----
    if (lastButtonState == HIGH && buttonState == LOW) {
        armed = !armed;
        digitalWrite(LED_BUILTIN, armed ? HIGH : LOW);
        delay(200);
    }
    lastButtonState = buttonState;

    // ---- Read sensors ----
    int senH = analogRead(PINA0); // Left
    int senV = analogRead(PINA1); // Right
    int senM = analogRead(PINA3);

    if (armed) {
        bool blockedRight = (senV < 350);
        bool blockedLeft  = (senH < 500);
        bool blockedMidle = (senM > 200);

        if (blockedMidle) {
            if (blockedRight && !blockedLeft) {
                lastBlockedRight = true;
                lastBlockedLeft = false;
            } else if (blockedLeft && !blockedRight) {
                lastBlockedLeft = true;
                lastBlockedRight = false;
            }
        }

        // ---- Both side sensors blocked ----
        if (blockedRight && blockedLeft && !turningActive) {
            turningActive = true;
            turnStartTime = millis();
            turnRight = lastBlockedRight;
            if (turnRight) {
                hoejreDrejTil();
                venstreStop();
            } else {
                venstreDrejTil();
                hoejreStop();
            }
        }

        // ---- Handle active turning ----
        if (turningActive) {
            if (millis() - turnStartTime >= 300) {
                turningActive = false;
                hoejreStop();
                venstreStop();
            }
        }
        // ---- Normal behavior if not turning ----
        else if (!(blockedRight && blockedLeft)) {
            if (blockedRight) hoejreBaglens(); else hoejreDrejTil();
            if (blockedLeft) venstreBaglens(); else venstreDrejTil();
        }
    } else {
        // Disarmed → stop
        hoejreStop();
        venstreStop();
    }
}
