int lowerData;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(2,INPUT);
  attachInterrupt(digitalPinToInterrupt(2),sen_line,FALLING);
}

void loop(){
  Serial.println(digitalRead(2));
}

void sen_line(){
  Serial.print("wow");
  noInterrupts();
    Serial1.write(1);
    Serial1.flush();
    interrupts();
    Serial.print(Serial1.available());
}
