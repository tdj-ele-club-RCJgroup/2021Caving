//ライン制御
#define intPin 2
const uint8_t linePin[16] = {4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
byte upperData = 0;
byte lowerData = 0;
bool ifLine = false;
bool BFifLine = false;
void getData();
void sendData();

void setup(){
  Serial.begin(9600);
  pinMode(intPin,OUTPUT);
  for(int i=0; i<16; i++){
   pinMode(linePin[i],INPUT_PULLUP);
  }
}

void loop(){
  BFifLine = ifLine;
  getData();
  if(!BFifLine && ifLine){
    digitalWrite(intPin,HIGH);
    while(Serial.available() < 0);
    sendData();
  }
  if(BFifLine && !ifLine){
    digitalWrite(intPin,LOW);
  }
}

void serialEvent(){
  sendData();
}

 //読み取り
void getData(){
  uint8_t rawdata[16];

  for(int i=0; i<16; i++){
    rawdata[i] = !digitalRead(linePin[i]);
    if(rawdata[i]) ifLine = true;
  }
  for(int i=0; i<8; i++){
    lowerData += rawdata[i] * 2^i;
    upperData += rawdata[i+8] * 2^(i+8);
  }
}

void sendData(){
  while (Serial.available() > 0)Serial.read();
  Serial.write(lowerData);
  while(Serial.available() < 0);
  while (Serial.available() > 0)Serial.read();
  Serial.write(upperData);
}
