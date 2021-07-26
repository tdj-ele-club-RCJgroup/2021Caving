const int motorPin[8] = {7,8,10,11,12,13,5,6};

void setup(){
  for(int i = 0; i < 8; i++){
    pinMode(motorPin[i], OUTPUT);
  }
}

void loop(){
  for(int i = 0; i < 8; i = i + 2){
    analogWrite(motorPin[i], 255);
  }
}
