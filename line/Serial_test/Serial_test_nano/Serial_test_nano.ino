void setup(){
  Serial.begin(9600);
}

void loop(){
}

void serialEvent(){
  while (Serial.available()) Serial.read(); //バッファを消す
  Serial.write(120);
  while (!Serial.available()); //arduinoから送られてくるまで待つ
  while (Serial.available()) Serial.read(); //バッファを消す
  Serial.write(95);
}