const int motorPin[8] = {7,8,10,11,12,13,5,6};

void setup(){
  Serial.begin(9600);
  for(int i = 0; i < 8; i++){
    pinMode(motorPin[i], OUTPUT);
  }
}

void loop(){
//  for(int i = 0; i < 8; i = i + 2){
    analogWrite(motorPin[4], 255);
    Serial.println(motorPin[4]);
//  }
  Serial.println();
}
