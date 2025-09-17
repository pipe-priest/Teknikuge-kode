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

bool lastBlockedLeft = false;
bool lastBlockedRight = false;

unsigned long driveStartTime = 0;
bool timedDriveActive = false;

// ---- Function prototypes ----

// ---- Motor functions ----
void hoejreDrejTil() {
  digitalWrite(PIN11, HIGH);
  digitalWrite(PIN10, LOW);
  analogWrite(pin6,170);


}
void venstreDrejTil() {
  digitalWrite(PIN13, HIGH);
  digitalWrite(PIN12, LOW);
  analogWrite(PIN9,90);
 
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
  pinMode(pin4, OUTPUT);

  analogWrite(PIN9, 250);
  digitalWrite(pin6, HIGH);

  pinMode(PINA1, INPUT);
  pinMode(PINA0, INPUT);

  pinMode(armButtonPin, INPUT_PULLUP); // momentary push button
  pinMode(LED_BUILTIN, OUTPUT);        // indicator LED
}

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
  Serial.println(senH);
  Serial.println(senV);

  if (armed) {
  bool blockedRight = (senV < 350);
  bool blockedLeft  = (senH < 500);

  // Track last blocked
  if (blockedRight && !blockedLeft) {
    lastBlockedRight = true;
    lastBlockedLeft = false;
  } else if (blockedLeft && !blockedRight) {
    lastBlockedLeft = true;
    lastBlockedRight = false;
  }

  // ---- Both sensors blocked ----
  if (blockedRight && blockedLeft) {
    if (lastBlockedRight) {
      hoejreBaglens();
      venstreStop();
      delay(100);       // run for 100 ms
      hoejreStop();     // then stop
    } else if (lastBlockedLeft) {
      venstreBaglens();
      hoejreStop();
      delay(100);       // run for 100 ms
      venstreStop();    // then stop
    }
  } 
  // ---- Normal behavior ----
  else {
    if (blockedRight) {
      hoejreBaglens();
    } else {
      hoejreDrejTil();
    }

    if (blockedLeft) {
      venstreBaglens();
    } else {
      venstreDrejTil();
    }
  }
} else {
  // Disarmed → stop
  hoejreStop();
  venstreStop();
  digitalWrite(pin4, LOW);
}
}