//座標
typedef struct {
  float X; //直交座標のⅹ座標 [cm]
  float Y; //直交座標のＹ座標 [cm]
  float R; //　極座標のｒ（中心からの距離）[cm]
  float T; //　極座標のθ（正面0,-180~180）...度
} Coordinate;
void XYtoRT(Coordinate *Data){
  Data->R = sqrt(pow(Data->X, 2.0) + pow(Data->Y, 2.0));
  Data->T = atan2(Data->Y, Data->X) * 180 / M_PI  -  90;
  if(-275 <= Data->T && Data->T <= -180){
    Data->T = Data->T + 360;
  }
};   //座標の変換


//ライン制御
#define intPin 2
const uint8_t linePin[16] = {11,18,19,16,17,14,15,4,3,6,5,8,7,10,9,12};
const float lineLocate_t[16] = {0,  22.5, 45, 67.5, 90, 112.5,  135,  157.5,  180,  -157.5, -135, -112.5, -90,  -67.5,  -45,  -22.5};
Coordinate lineLocate[16];
byte upperData = 0;
byte lowerData = 0;
uint8_t rawdata[16][4] = {0};
bool ifLine = false;
bool BFifLine = false;
void lineLocateCul(){
  for(int i=0; i<16; i++){
    lineLocate[i].R = 1;
    lineLocate[i].T = lineLocate_t[i];
    RTtoXY(&lineLocate[i]);
  }
}; //lineLocateの初期化 setup関数で実行
void getData();
void sendData();

void setup(){
  Serial.begin(9600);
  pinMode(intPin,OUTPUT);
  for(int i=0; i<16; i++){
   pinMode(linePin[i],INPUT_PULLUP);
  }
  //ラインセンサ場所の計算
  lineLocateCul();
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
  Coordinate back = {0,0,1,0}; //線をよけて進む向き
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

  //ベクトルを足して線をよける方向を計算
  for(int i=0; i<16; i++){
    back.X += -(rawData[i] * lineLocate[i].X);
    back.Y += -(rawData[i] * lineLocate[i].Y);
  }
  XYtoRT(&back);

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
