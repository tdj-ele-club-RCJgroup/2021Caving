const uint8_t linePin[16] = {11,18,19,16,17,14,15,4,3,6,5,8,7,10,9,12};

void setup(){
  Serial.begin(9600);
  for(int i=0; i<16; i++){
    pinMode(linePin[i],INPUT_PULLUP);
  }
  //pinMode(12, INPUT_PULLUP);
}

void loop(){
  for(int i=0; i<16; i++){
    Serial.println(!digitalRead(linePin[i]));
  }
}