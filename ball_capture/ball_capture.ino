#define ballCapture 3
#define capture_rate 300

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int cap_rawData;
  cap_rawData = analogRead(ballCapture);
  Serial.print((String)"rawData" + cap_rawData);
  if(cap_rawData > capture_rate){
    Serial.println(" ball");
    //return 1;
  }else{
    Serial.println(" noball");
    //return 0;
  }
}
