int lowerData;
#define intPin_line 3
#define Line_interupts() (attachInterrupt(digitalPinToInterrupt(intPin_line),int_line,FALLING))
#define Line_noInterrupts() (detachInterrupt(digitalPinToInterrupt(intPin_line)))

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(2,INPUT);
  pinMode(37,OUTPUT);
  Line_interupts();
}

void loop(){
  Serial.println(digitalRead(2));
  Line_noInterrupts();
  digitalWrite(37,LOW);
  delay(1000);
  Line_interupts();
  delay(1000);
}

void int_line(){
  digitalWrite(37,HIGH);
}
