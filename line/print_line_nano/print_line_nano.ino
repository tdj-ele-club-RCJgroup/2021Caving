//ライン制御
#define intPin 2
const uint8_t linePin[16] = {11,18,19,16,17,14,15,4,3,6,5,8,7,10,9,12};
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
  digitalWrite(intPin,HIGH);
}

void loop(){
  BFifLine = ifLine;
  getData();
  if(!BFifLine && ifLine){
    digitalWrite(intPin,LOW);
    while (!Serial.available()); //arduinoが反応するまで待つ
    sendData();
  }
  if(BFifLine && !ifLine){
    digitalWrite(intPin,HIGH);
  }
}

void serialEvent(){
  sendData();
}

 //読み取り
void getData(){
  uint8_t rawdata[16];
  ifLine = false;
  for(int i=0; i<16; i++){
    rawdata[i] = !digitalRead(linePin[i]);
    if(rawdata[i]) ifLine = true;
    //Serial.println((String)i + rawdata[i]);
  }
  for(int i=0; i<8; i++){
    lowerData += rawdata[i] * 2^i;
    upperData += rawdata[i+8] * 2^(i+8);
  }
}

void sendData(){
  while (Serial.available()) Serial.read(); //バッファを消す
  Serial.write(lowerData);
  while (!Serial.available()); //arduinoから送られてくるまで待つ
  while (Serial.available()) Serial.read(); //バッファを消す
  Serial.write(upperData);
}
