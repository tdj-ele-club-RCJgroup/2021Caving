void setup(){
    Serial.begin(9600);
    pinMode(22, INPUT_PULLUP);
    pinMode(2, OUTPUT);
}

void loop()
{
    digitalWrite(2,HIGH);
    Serial.println(digitalRead(22));
    //Serial.println(analogRead(0));
}