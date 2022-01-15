void setup(){
  Serial.begin(9600);
  pinMode(12, INPUT_PULLUP);
}

void loop(){
  Serial.println(digitalRead(12));
}