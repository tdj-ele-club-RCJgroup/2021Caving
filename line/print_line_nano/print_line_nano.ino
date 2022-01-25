//ライン制御
#define intPin 2
const uint8_t linePin[16] = {11,18,19,16,17,14,15,4,3,6,5,8,7,10,9,12};
byte upperData = 0;
byte lowerData = 0;
uint8_t rawdata[16][4] = {0};
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
  uint8_t culdata[16] = {0};
  upperData = 0;
  lowerData = 0;
  ifLine = false;
  for(int i=0; i<4; i++){
    for(int j=0; j<16; j++){
      rawdata[j][i] = rawdata[j][i+1];
    }
  }//配列更新

  for(int i=0; i<16; i++){
    rawdata[i][3] = !digitalRead(linePin[i]);
    if((rawdata[i][0] + rawdata[i][1] + rawdata[i][2] + rawdata[i][3]) >= 3){
      culdata[i] = 1;
    }
    if(culdata[i]) ifLine = true;
    //Serial.println((String)i + "\t" + rawdata[i][4]);
  }
  for(int i=0; i<8; i++){
    lowerData += culdata[i] * (1<<i);
    upperData += culdata[i+8] * (1<<i);
  }
  /*Serial.println((String)"lowerData" + lowerData);
  Serial.println((String)"upperData" + upperData);//*/
}

void sendData(){
  while (Serial.available()) Serial.read(); //バッファを消す
  Serial.write(lowerData);
  while (!Serial.available()); //arduinoから送られてくるまで待つ
  while (Serial.available()) Serial.read(); //バッファを消す
  Serial.write(upperData);
}
