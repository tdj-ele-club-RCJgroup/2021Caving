byte upperData = 0;
byte lowerData = 0;
int rawData[8] = {0};
void sen_line();

void setup(){
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop(){
  sen_line();
  delay(1000);
}

void sen_line(){
    Serial1.write(1);
    while(Serial1.available() < 1) Serial.println("waiting...");
    lowerData = Serial1.read();
    Serial1.write(1);
    while(Serial1.available() < 1);
    upperData = Serial1.read();

    Serial.println(upperData);
    Serial.println(lowerData);
    
  for(int i=0; i<8; i++){
    rawData[i] = lowerData % 2;
    rawData[i+8] = upperData % 2;
    lowerData = ( lowerData - rawData[i] ) / 2;
    upperData = ( upperData - rawData[i+8] ) / 2;
  }
}
