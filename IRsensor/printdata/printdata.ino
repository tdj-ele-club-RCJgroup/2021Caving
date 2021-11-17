//座標
typedef struct {
  float X; //直交座標のⅹ座標 [cm]
  float Y; //直交座標のＹ座標 [cm]
  float R; //　極座標のｒ（中心からの距離）[cm]
  float T; //　極座標のθ（正面0,-180~180）...度
} Coordinate;
void XYtoRT(Coordinate *Data);      //座標の変換
void RTtoXY(Coordinate *Data);      //    〃



//赤外線センサ
#define range 20
const uint8_t IRpin[8] = {9,10,11,12,13,14,15,8};
const int IRhigh[8] = {800,800,800,800,800,800,800,800};
const int IRlow[8]  = {0, 0,  0,  0,  1,  7,  4,  6};
float IRlocate_t[8] = {0  ,45 ,90 ,135,180,-135,-90 ,-45 };
Coordinate IRlocate[8];
void IRlocateCul(){
  for(int i=0; i<8; i++){
    IRlocate[i].R = 1;
    IRlocate[i].T = IRlocate_t[i];
    RTtoXY(&IRlocate[i]);
  }
}; //IRlocateの初期化 setup関数で実行
Coordinate ball;
void sen_IRball();  //赤外線センサ(ボール位置をballに代入)



void setup() {
  Serial.begin(9600);
  //赤外線センサ場所の計算
  IRlocateCul();
}

void loop() {
  sen_IRball();
  delay(500);
}

//ベクトルの変換
void XYtoRT(Coordinate *Data){
  Data->R = sqrt(pow(Data->X, 2.0) + pow(Data->Y, 2.0));
  Data->T = atan2(Data->Y, Data->X) * 180 / M_PI  -  90;
  if(-275 <= Data->T && Data->T <= -180){
    Data->T = Data->T + 360;
  }
}
void RTtoXY(Coordinate *Data){
  Data->Y = Data->R * sin((Data->T + 90) / 180 * M_PI);
  Data->X = Data->R * cos((Data->T + 90) / 180 * M_PI);
  Data->T = fmodf(Data->T + 720 , 360);
  if(Data->T > 180){
    Data->T = Data->T - 360;
  }
}

//赤外線センサ(ボール位置をballに代入)
void sen_IRball(){
  int rawdata[8] = {0};
  uint8_t IRdata[8] = {0};
  ball = {0,0,0,0};

  //読み取り(rawdata[]に代入)
  for(int i=0; i<8; i++){
    rawdata[i] = analogRead(IRpin[i]);
  }
  for(int i=0; i<8; i++){
    //Serial.println((String)"rawdata " + i +"  " + rawdata[i]);
  }

  //値の範囲でで分ける(IRdata[]に代入)
  for(int i=0; i<8; i++){
    if(rawdata[i] <= IRlow[i]){
      IRdata[i] = 0;
    }else if(rawdata[i] > IRhigh[i]){
      IRdata[i] = range;
    }else{
      IRdata[i] = (rawdata[i] - IRlow[i]) * (range) / (IRhigh[i] - IRlow[i])  + 1; //小数は切り捨てされる
    }
  }

  Serial.print((String)"rawdata   ");
  for(int i=0; i<8; i++){
    Serial.print((String)rawdata[i] + "," + "\t");
  }
  Serial.print("IRdata  ");
  for(int i=0; i<8; i++){
    Serial.print((String)"  " + IRdata[i]);
  }
  //ベクトルで角度を算出(ball.Tに代入)
  for(int i=0; i<8; i++){
    ball.X += IRdata[i] * IRlocate[i].X;
    ball.Y += IRdata[i] * IRlocate[i].Y;
  }
  //Serial.println((String)"ball.X " + ball.X + "  ball.Y " + ball.Y);
  XYtoRT(&ball);
  Serial.print((String)"    ball.T " + ball.T);
  Serial.println((String)"    ball.R " + ball.R);
  //だいたいの距離を割り出す(ball.Rに代入)
}
