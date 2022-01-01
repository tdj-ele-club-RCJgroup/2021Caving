//座標
typedef struct {
  float X; //直交座標のⅹ座標 [cm]
  float Y; //直交座標のＹ座標 [cm]
  float R; //　極座標のｒ（中心からの距離）[cm]
  float T; //　極座標のθ（正面0,-180~180）...度
} Coordinate;

//ベクトルの変換
void XYtoRT(Coordinate *Data){
  Data->R = sqrt(pow(Data->X, 2.0) + pow(Data->Y, 2.0));
  Data->T = atan2(Data->Y, Data->X) * 180 / M_PI  -  90;
  if(-275 <= Data->T && Data->T <= -180){
    Data->T = Data->T + 360;
  }
};
void RTtoXY(Coordinate *Data){
  Data->Y = Data->R * sin((Data->T + 90) / 180 * M_PI);
  Data->X = Data->R * cos((Data->T + 90) / 180 * M_PI);
  Data->T = fmodf(Data->T + 720 , 360);
  if(Data->T > 180){
    Data->T = Data->T - 360;
  }
};

float lineLocate_t[16] = {0  ,45 ,90 ,135,180,-135,-90 ,-45 };
Coordinate lineLocate[16];
void lineLocateCul(){
  for(int i=0; i<16; i++){
    lineLocate[i].R = 1;
    lineLocate[i].T = lineLocate_t[i];
    RTtoXY(&lineLocate[i]);
  }
}; //lineLocateの初期化 setup関数で実行
void sen_line();

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  lineLocateCul();
  attachInterrupt(digitalPinToInterrupt(2),sen_line,RISING);
}

void loop(){}

void sen_line(){
  byte lowerData = 0;
  byte upperData = 0;
  int rawData[8] = {0};
  if(digitalRead(2)){
    Serial1.write(1);
    while(Serial.available() < 1);
    lowerData = Serial.read();
    Serial1.write(1);
    while(Serial.available() < 1);
    upperData = Serial.read();
  }
  for(int i=0; i<8; i++){
    rawData[i] = lowerData % 2;
    rawData[i+8] = upperData % 2;
    lowerData = ( lowerData - rawData[i] ) / 2;
    upperData = ( upperData - rawData[i+8] ) / 2;
  }
}