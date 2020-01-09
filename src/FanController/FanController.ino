unsigned long toRpm(unsigned long pulseTime) {
  if(pulseTime < 15000) return 0;
  return 60000/(pulseTime*0.002);
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
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if(lastInterrupt + 1000 < millis()) pulseTime = 0;
  Serial.println(toRpm(pulseTime));
}
