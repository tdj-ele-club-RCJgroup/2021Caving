#define LED 12
#define ballCapture 3

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);
}

void loop()
{
  Serial.println(analogRead(ballCapture));
}
