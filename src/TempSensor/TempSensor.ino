float toDeg(float r) {
  // read = 0 - 1023
  // V = read / (1023 / 5)
  // mV = V * 1000
  // deg = mV / 10

  // deg = read/2.046
  return r/2.046;
}

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop() {
  Serial.println(toDeg(analogRead(A0)));
}
