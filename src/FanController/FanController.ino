unsigned long toRpm(unsigned long pulseTime) {
  return 60/(pulseTime*0.002);
}

unsigned long pulseTime;

void isr() {
  static unsigned long start = 0;
  if(!digitalRead(2)) start = millis();
  if(digitalRead(2)) pulseTime = millis()-start;
}

void setup() {
  attachInterrupt(digitalPinToInterrupt(2), isr, CHANGE);
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  Serial.println(toRpm(pulseTime));
}
