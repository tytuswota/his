#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DEBUG
#define NTC

// constants
#define NTC_RESISTANCE 10000

#define LCD_REFRESH_TIME 1000

#define SET_TEMP_MIN 10
#define SET_TEMP_MAX 70

#define FAN_PIN 2
#define TEMPSENSOR_PIN A0
#define TEMPSENSOR_VCC 4
#define POTMETER_PIN A1
#define FAN_OUT 5

///
///   FUNCTIONS
///

// takes value from 0 - 1023 and returns a value between SET_TEMP_MIN and SET_TEMP_MAX
// this is the temperature chosen by the user
unsigned int getPotmeterTemp() {
  return map(analogRead(POTMETER_PIN), 0, 1023, SET_TEMP_MIN, SET_TEMP_MAX);
}

// takes a pulse time in microseconds and returns the revolutions per minute
unsigned long toRpm(unsigned long pulseTime) {
  // pulsetime = microseconds per half rev
  // millis per half rev = pulsetime * 0.001
  // millis per rev = pulsetime * 0.002
  // sec per rev = (pulsetime * 0.002) / 1000
  // rev per sec = 1000 / (pulsetime * 0.002)
  // rev per min = 60000 / (pulsetime * 0.002)

  // if the pulsetime is lower than 15000 microseconds, there is probably an error (this would mean an rpm above 2000)
  if(pulseTime < 15000) return 0;
  return 60000/(pulseTime*0.002);
}

// takes the voltage read at the ntc and returns the temperature in celsius
int readNtcTemp(int ntcVout){
  
  float logResistanceOfNtc,resistanceOfNtc, kelvin, celsius;
  const float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;
  resistanceOfNtc = NTC_RESISTANCE * (1023.0 / (float)ntcVout - 1.0);
  logResistanceOfNtc = log(resistanceOfNtc);
  //Steinhart–Hart equation
  //1/T = A + B*ln(R2) + C(ln(R2))^3
  
  kelvin = (1.0 / (A + B*logResistanceOfNtc + C*logResistanceOfNtc*logResistanceOfNtc*logResistanceOfNtc));//in kelvin
  celsius = kelvin - 273.15;//naar celsius
  return celsius;
}

// returns the value read from the sensor at TEMPSENSOR_PIN converted to celsius
int readTemp() {
  int temp;

  // the sensor is only powered when a value is being read to prevent it from heating up too much
  digitalWrite(TEMPSENSOR_VCC, 1);
  delay(5);

  #ifdef LM35DZ
    // in = 0 - 1023        V = in / (1023 / 5)      mV = V * 1000       deg = mV / 10
    // deg = read/2.046
    temp = analogRead(TEMPSENSOR_PIN) / 2.046;
  #else // NTC
    temp = readNtcTemp(analogRead(TEMPSENSOR_PIN));
  #endif
  digitalWrite(TEMPSENSOR_VCC, 0);
  return temp;
}

void isr();

///
///   GLOBALS
///

unsigned long pulseTime, lastInterrupt, lcdRefreshTime = 0;
char lcdBuffer[2][16];
LiquidCrystal_I2C lcd(0x38, 16, 2);

///
///   SETUP
///

void setup() {
  pinMode(TEMPSENSOR_PIN, INPUT);
  pinMode(TEMPSENSOR_VCC, OUTPUT);
  pinMode(FAN_PIN, INPUT_PULLUP);
  pinMode(POTMETER_PIN, INPUT);
  pinMode(FAN_OUT, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(FAN_PIN), isr, CHANGE);
  Serial.begin(9600);
  lcd.init();
}

///
///   LOOP
///

void loop() {
  // if the fan pin stays at the same voltage for more than one second assume it stopped spinning
  if(lastInterrupt + 1000 < millis()) pulseTime = 0;

  // turn on the fan if the temperature from the sensor is higher than the temperature set by the user
  digitalWrite(FAN_OUT, (readTemp() > getPotmeterTemp()));

  #ifdef DEBUG
    Serial.println("fan rpm: " + (String)toRpm(pulseTime) + " temperature: " + (String)readTemp() + " potmeter: " + (String)getPotmeterTemp());
  #endif

  // lcd is not refreshed on every iteration to prevent flickering
  if(millis() > lcdRefreshTime) {
    snprintf(lcdBuffer[0], 16, "cur. temp: %-3dC", readTemp());
    snprintf(lcdBuffer[1], 16, "set temp: %-4dC", getPotmeterTemp());
    
    lcdRefreshTime += LCD_REFRESH_TIME;
    lcd.setCursor(0, 0);
    lcd.print(lcdBuffer[0]);
    lcd.setCursor(0, 1);
    lcd.print(lcdBuffer[1]);
  }
}

///
///   ISRs
///

// this isr gets called when the sensor from the fan changes
// this function starts timing when the pin goes low
// when the pin goes high the elapsed time will be stored in pulseTime
void isr() {
  static unsigned long start = 0;
  if(!digitalRead(FAN_PIN)) start = micros();
  if(digitalRead(FAN_PIN)) pulseTime = micros()-start;
  lastInterrupt = millis();
}
