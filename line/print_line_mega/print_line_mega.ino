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

#define LED 12
#define intPin_line 3
float lineLocate_t[16] = {0  ,45 ,90 ,135,180,-135,-90 ,-45 };
Coordinate lineLocate[16];
int rawData[8] = {0};
void lineLocateCul(){
  for(int i=0; i<16; i++){
    lineLocate[i].R = 1;
    lineLocate[i].T = lineLocate_t[i];
    RTtoXY(&lineLocate[i]);
  }
}; //lineLocateの初期化 setup関数で実行
void getData_line();
void int_line();
volatile bool ifLine; //ラインあるなしフラグ
bool ifLine_process; //ライン処理実行中フラグ

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);
  lineLocateCul();
  attachInterrupt(digitalPinToInterrupt(intPin_line),int_line,FALLING);
}

void loop(){
  if(ifLine) getData_line(); ifLine = 0;
  //Serial.println(digitalRead(intPin_line));
}

void int_line(){
  noInterrupts();
  //モーターをブレーキ
  
  //ライン処理フラグ
  ifLine = true;
  interrupts();
}

void getData_line(){
  int upperData = 0;
  int lowerData = 0;

  Serial1.write(1);
  Serial1.flush();
  while(!Serial1.available());
  do{
    lowerData = Serial1.read();
  }while(lowerData == -1);
  Serial1.write(1);
  Serial1.flush();
  while(!Serial1.available());
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
    /*Serial.print(rawData[i]);
    Serial.println();//*/
  }
}
