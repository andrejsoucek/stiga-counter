#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "pitches.h"

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const int PIN_BUTTON = 2;

const int PIN_HOME_LED = 11;
const int PIN_HOME_GOAL = 3;

const int PIN_AWAY_LED = 10;
const int PIN_AWAY_GOAL = 4;

const int INTERVAL_LED = 5000;

unsigned int goalsHome = 0;
unsigned int goalsAway = 0;

unsigned long previousMillisTimer = 0;
unsigned long previousMillisLed = 0;

int minutes = 5;
int seconds = 0;

bool isLedOn = false;
bool isGameOn = false;

void setup() {
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_HOME_LED, OUTPUT);
  pinMode(PIN_HOME_GOAL, INPUT);
  pinMode(PIN_AWAY_LED, OUTPUT);
  //pinMode(PIN_AWAY_GOAL, OUTPUT);

  Serial.begin(9600);

  initLcd();
}

void loop() {
  unsigned long currentMillis = millis();

  if (digitalRead(PIN_BUTTON) == LOW) {
    restartGame();
  }

  if (isGameOn && (currentMillis - previousMillisTimer >= 1000)) {
    previousMillisTimer = currentMillis;

    decreaseTimer();
  }

  if (isGameOn && isLedOn && currentMillis - previousMillisLed >= INTERVAL_LED) {
    ledOff();
  }

  if (isGameOn && !isLedOn && digitalRead(PIN_HOME_GOAL) == LOW) {
    scoreHome();
    previousMillisLed = currentMillis;
  }

  //if (isGameOn && !isLedOn && digitalRead(PIN_AWAY_GOAL) == LOW) {
  //  scoreAway();
  //  previousMillisLed = currentMillis;
  //}
}

void initLcd() {
  lcd.init();
  lcd.backlight();
  resetScoreAndTime();
}

void decreaseTimer() {
  seconds--;
  if (seconds < 0) {
    minutes--;
    if (minutes <= 0) {
      minutes = 4;
    }
    seconds = 59;
  }
  lcd.setCursor(11, 1);
  lcd.print(minutes);
  if (seconds < 10) {
    lcd.setCursor(13, 1);
    lcd.print(0);
    lcd.setCursor(14, 1);
    lcd.print(seconds);
  } else {
    lcd.setCursor(13, 1);
    lcd.print(seconds);
  }
}

void scoreHome() {
  digitalWrite(PIN_HOME_LED, HIGH);
  tone(8, NOTE_AS3, 500);
  lcd.setCursor(11, 0);
  lcd.print(++goalsHome);
  isLedOn = true;
}

void scoreAway() {
  digitalWrite(PIN_AWAY_LED, HIGH);
  tone(8, NOTE_AS3, 500);
  lcd.setCursor(13, 0);
  lcd.print(++goalsAway);
  isLedOn = true;
}

void ledOff() {
  digitalWrite(PIN_HOME_LED, LOW);
  digitalWrite(PIN_AWAY_LED, LOW);
  isLedOn = false;
}

void restartGame() {
  resetScoreAndTime();
  int melody[] = {
    NOTE_AS3, NOTE_F3, NOTE_G3, NOTE_A3,
    NOTE_B3, NOTE_FS3, NOTE_GS3, NOTE_AS3,
    NOTE_C4, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_C4,
    NOTE_C3, NOTE_E3, NOTE_G3, NOTE_G3, NOTE_E3, NOTE_G3
  };
  int noteDurations[] = {
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 4, 4, 2,
    8, 8, 8, 4, 8, 2
  };
  const int notesCount = sizeof(melody) / sizeof(melody[0]);

  for (int i = 0; i < notesCount; i++) {
    int noteDuration = 1000 / noteDurations[i];
    tone(8, melody[i], noteDuration);
    delay(noteDuration * 1.30);
  }
  isGameOn = true;
}

void resetScoreAndTime() {
  minutes = 5;
  seconds = 0;
  goalsHome = 0;
  goalsAway = 0;
  lcd.setCursor(0, 0);
  lcd.print("SKORE:");
  lcd.setCursor(11, 0);
  lcd.print("0:0");

  lcd.setCursor(0, 1);
  lcd.print("CAS:");
  lcd.setCursor(10, 1);
  lcd.print("05:00");
}
