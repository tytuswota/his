#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DEBUG

#define FAN_PIN 2
#define TEMPSENSOR_PIN A0
#define TEMPSENSOR_VCC 4
#define POTMETER_PIN A1

unsigned int getPotmeterTemp() {
  // TO DO: calibrate
  return analogRead(POTMETER_PIN);
}

unsigned long toRpm(unsigned long pulseTime) {
  if(pulseTime < 15000) return 0;
  return 60000/(pulseTime*0.002);
}

int readTemp() {
  int temp;
  digitalWrite(TEMPSENSOR_VCC, 1);
  delay(5);

  // in = 0 - 1023        V = in / (1023 / 5)      mV = V * 1000       deg = mV / 10
  // deg = read/2.046
  temp = analogRead(A0) / 2.046;
  digitalWrite(TEMPSENSOR_VCC, 0);
  return temp;
}

unsigned long pulseTime;
unsigned long lastInterrupt;
char lcdBuffer[2][16];
LiquidCrystal_I2C lcd(0x38, 16, 2);

void isr() {
  static unsigned long start = 0;
  if(!digitalRead(FAN_PIN)) start = micros();
  if(digitalRead(FAN_PIN)) pulseTime = micros()-start;
  lastInterrupt = millis();
}

void setup() {
  pinMode(TEMPSENSOR_PIN, INPUT);
  pinMode(TEMPSENSOR_VCC, OUTPUT);
  pinMode(FAN_PIN, INPUT_PULLUP);
  pinMode(POTMETER_PIN, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(FAN_PIN), isr, CHANGE);
  Serial.begin(9600);
  lcd.init();
}

void loop() {
  if(lastInterrupt + 1000 < millis()) pulseTime = 0;

  #ifdef DEBUG
    Serial.println("fan rpm: " + (String)toRpm(pulseTime) + " temperature: " + (String)readTemp() + " potmeter: " + (String)getPotmeterTemp());
  #endif
  
  snprintf(lcdBuffer[0], 16, "cur. temp: %-4d", readTemp());
  snprintf(lcdBuffer[1], 16, "set temp: %-5d", getPotmeterTemp());

  lcd.setCursor(0, 0);
  lcd.print(lcdBuffer[0]);
  lcd.setCursor(0, 1);
  lcd.print(lcdBuffer[1]);

  delay(100);
}
