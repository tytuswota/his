unsigned long toRpm(unsigned long msphr) {
  return 60000/(msphr*0.002);
}

void setup() {
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  Serial.println(toRpm(pulseIn(2, LOW)));
}