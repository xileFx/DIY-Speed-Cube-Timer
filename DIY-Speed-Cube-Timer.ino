#include "DigitLedDisplay.h"

#define DIN 2
#define CS 3
#define CLK 4

#define PIN_PAD_LEFT 8
#define PIN_PAD_RIGHT 9

#define PIN_INSPECTION_TIME 10

DigitLedDisplay ld = DigitLedDisplay(DIN, CS, CLK);

void setup() {
  ld.on();
  ld.setBright(12);
  ld.setDigitLimit(8);
  ld.clear();
  pinMode(PIN_PAD_LEFT, INPUT);
  pinMode(PIN_PAD_RIGHT, INPUT);
  pinMode(PIN_INSPECTION_TIME, INPUT);
}

int padL;
int padR;
int prevPads;
short padsSwitched;

boolean useInspectionTime = false;
boolean plusTwo = false;

boolean isInspecting = false;
boolean isRunning = false;
boolean isReady = false;


short digits[8];
uint32_t startTimer = 0;
uint32_t t = 0;

void loop() {
    t = millis()-startTimer;
    padL = digitalRead(PIN_PAD_LEFT);
    padR = digitalRead(PIN_PAD_RIGHT);
    padsSwitched = !((prevPads|(padR&padL))&(!(prevPads&(padR&padL)))) + ((!prevPads)|(padR&padL));
    prevPads = padR&padL;
    
    useInspectionTime = digitalRead(PIN_INSPECTION_TIME) == HIGH;
    
  if (isInspecting) {
    if (!isReady) {
      if (padsSwitched == 1) {
        isReady = true;
      }
    } else if (padsSwitched == 0) {
      isInspecting = false;
      isRunning = true;
      isReady = false;
      startTimer = millis();
    }
    if (t >= 17000) {
      dnf();
      return;
    }
    if (t >= 15000) {
      plusTwo = true;
      displayPlusTwo();
      return;
    }
    digits[0] = 0;
    digits[1] = 0;
    digits[2] = 0;
    digits[3] = (int)floor((16000-t)/10000)%6;
    digits[4] = (int)floor((16000-t)/1000)%10;
    digits[5] = 0;
    digits[6] = 0;
    digits[7] = 0;
  } else if (isRunning) {
    if (padsSwitched == 1) {
      isRunning = false;
      t += plusTwo ? 2000 : 0;
      plusTwo = false;
    }
    digits[0] = (int)floor(t/600000)%10;
    digits[1] = (int)floor(t/600000)%6;
    digits[2] = (int)floor(t/60000)%10;
    digits[3] = (int)floor(t/10000)%6;
    digits[4] = (int)floor(t/1000)%10;
    digits[5] = (int)floor(t/100)%10;
    digits[6] = (int)floor(t/10)%10;
    digits[7] = (int)t%10;
    
  } else if (!isReady) {
    if (padsSwitched == 1) {
      isReady = true;
    }
  } else if (padsSwitched == 0) {
    if (useInspectionTime) {
      isInspecting = true;
    } else {
      isRunning = true;
    }
    isReady = false;
    startTimer = millis();
  }
  updateDisplay();
}

byte chars[] = {B00111101, B00010101, B01000111, B01100111, B00001110, B00111110, B01011011};
byte numbers[] = {B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,B01111111,B01111011};

void dnf() {
  ld.clear();
  ld.write(5, chars[0]);
  ld.write(4, chars[1]);
  ld.write(3, chars[2]);
  delay(2000);
  ld.clear();
  padL = 0;
  padR = 0;
  prevPads = 0;
  padsSwitched = 2;
  isInspecting = false;
  isRunning = false;
  isReady = false;
  plusTwo = false;
  for (int i = 0; i < 8; i++) {
    digits[i] = 0;
  }
}

void displayPlusTwo() {
  ld.write(8, B00000000);
  ld.write(7, chars[3]);
  ld.write(6, chars[4]);
  ld.write(5, chars[5]);
  ld.write(4, chars[6]);;
  ld.write(3, B00000000);
  ld.write(2, numbers[2]);
  ld.write(1, B00000000);
}

void updateDisplay() {
  for (int i = 0; i < 8; i++) {
    ld.write(i+1, numbers[digits[7-i]] + (i==3 || i==5 || i==7? B10000000:0));
  }
}
