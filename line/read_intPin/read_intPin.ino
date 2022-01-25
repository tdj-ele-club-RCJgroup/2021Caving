void setup() {
  Serial.begin(9600);
  pinMode(2,INPUT_PULLUP);
  pinMode(12,OUTPUT);
  digitalWrite(12,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(digitalRead(2));
}
