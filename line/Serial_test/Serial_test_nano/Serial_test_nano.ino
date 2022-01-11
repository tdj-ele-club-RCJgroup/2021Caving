void setup(){
  Serial.begin(9600);
}

void loop(){
}

void serialEvent(){
  while (Serial.available() > 0)Serial.read();
  Serial.write(120);
  while(Serial.available() < 0);
  while (Serial.available() > 0)Serial.read();
  Serial.write(95);
}