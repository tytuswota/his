unsigned long toRpm(unsigned long pulseTime) {
  if(pulseTime < 15000) return 0;
  return 60000/(pulseTime*0.002);
}

int toDeg(float r) {
  // read = 0 - 1023
  // V = read / (1023 / 5)
  // mV = V * 1000
  // deg = mV / 10

  // deg = read/2.046
  return r/2.046;
}

unsigned long pulseTime;
unsigned long lastInterrupt;

void isr() {
  static unsigned long start = 0;
  if(!digitalRead(2)) start = micros();
  if(digitalRead(2)) pulseTime = micros()-start;
  lastInterrupt = millis();
}

void setup() {
  attachInterrupt(digitalPinToInterrupt(2), isr, CHANGE);
  pinMode(A0, INPUT);
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if(lastInterrupt + 1000 < millis()) pulseTime = 0;
  Serial.println("fan rpm: " + (String)toRpm(pulseTime) + " temperature: " + (String)toDeg(analogRead(A0)));
}
