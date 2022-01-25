const int motorPin[8] = {6,7,8,9,10,11,4,5};

void setup(){
  Serial.begin(9600);
  for(int i = 0; i < 8; i++){
    pinMode(motorPin[i], OUTPUT);
  }
}

void loop() {
    for(int i=0; i<8; i=i+2){
        analogWrite(motorPin[i],255);
        analogWrite(motorPin[i+1],0);
    }
    delay(1000);
    for(int i=0; i<8; i=i+2){
        analogWrite(motorPin[i],255);
        analogWrite(motorPin[i+1],255);
    }
    delay(500);
    for(int i=0; i<8; i=i+2){
        analogWrite(motorPin[i],0);
        analogWrite(motorPin[i+1],255);
    }
    delay(1000);
    for(int i=0; i<8; i=i+2){
        analogWrite(motorPin[i],255);
        analogWrite(motorPin[i+1],255);
    }
    delay(500);
}
