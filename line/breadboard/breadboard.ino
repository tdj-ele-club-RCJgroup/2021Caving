void setup(){
    Serial.begin(9600);
    //pinMode(22, INPUT_PULLUP);
    pinMode(12, OUTPUT);
}

void loop()
{
    digitalWrite(12,HIGH);
    //Serial.println(digitalRead(22));
    //Serial.println(analogRead(0));
}
