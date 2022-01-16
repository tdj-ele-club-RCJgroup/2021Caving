int rawData[8] = {0};
void sen_line();

void setup(){
  pinMode(2, OUTPUT);
  pinMode(12, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);
  digitalWrite(12,HIGH);
}

void loop(){
  sen_line();
  delay(1000);
}

void sen_line(){
  int upperData = 0;
  int lowerData = 0;

  Serial1.write(1);
  Serial1.flush();
  do{
    lowerData = Serial1.read();
  }while(lowerData == -1);
  Serial1.write(1);
  Serial1.flush();
  do{
    upperData = Serial1.read();
  }while(upperData == -1);

  Serial.println(lowerData);
  Serial.println(upperData);//*/

  for(int i=0; i<8; i++){
    rawData[i] = lowerData % 2;
    rawData[i+8] = upperData % 2;
    lowerData = ( lowerData - rawData[i] ) / 2;
    upperData = ( upperData - rawData[i+8] ) / 2;
  }

  for(int i=0; i<16; i++){
    Serial.print(rawData[i]);
    Serial.println();//*/
  }
}
