#define FANPIN 2
#define TEMPSENSORIN A0
#define TEMPSENSORVCC 4

unsigned long toRpm(unsigned long pulseTime) {
  if(pulseTime < 15000) return 0;
  return 60000/(pulseTime*0.002);
}

int readTemp() {
  int temp;
  digitalWrite(TEMPSENSORVCC, 1);
  delay(5);

  // in = 0 - 1023
  // V = in / (1023 / 5)
  // mV = V * 1000
  // deg = mV / 10

  // deg = read/2.046
  temp = analogRead(A0) / 2.046;
  digitalWrite(TEMPSENSORVCC, 0);
  return temp;
}

unsigned long pulseTime;
unsigned long lastInterrupt;

void isr() {
  static unsigned long start = 0;
  if(!digitalRead(FANPIN)) start = micros();
  if(digitalRead(FANPIN)) pulseTime = micros()-start;
  lastInterrupt = millis();
}

void setup() {
  attachInterrupt(digitalPinToInterrupt(FANPIN), isr, CHANGE);
  pinMode(TEMPSENSORIN, INPUT);
  pinMode(TEMPSENSORVCC, OUTPUT);
  pinMode(FANPIN, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  if(lastInterrupt + 1000 < millis()) pulseTime = 0;
  Serial.println("fan rpm: " + (String)toRpm(pulseTime) + " temperature: " + (String)readTemp());
}
