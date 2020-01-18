#define NTCPin A0
const int vout;
//als er een weerstand van 100k wordt aangesloten
//moet r1 10000 zijn als je een weerstand van 15k neemt
//moet r1 gedeelt door 10 anders komen er rare waardes uit
const float R1 = 10000;
float logR2, R2, T, Tc, Tf;
//Steinhart–Hart coefficients
float A = 1.009249522e-03, B = 2.378405444e-04, C = 2.019202697e-07;

void setup() {
Serial.begin(9600);
}

void loop() {

  vout = analogRead(NTCPin);
  R2 = R1 * (1023.0 / (float)vout - 1.0);
  
  logR2 = log(R2);
  
  //Steinhart–Hart equation
  //1/T = A + B*ln(R2) + C(ln(R2))^3
  
  T = (1.0 / (A + B*logR2 + C*logR2*logR2*logR2));//in kelvin
  Tc = T - 273.15;//naar celsius
   
  Serial.print("Resistance R2: "); 
  Serial.print(R2);
  Serial.print(" C: ");
  Serial.print(Tc);
  
  delay(500);
}
