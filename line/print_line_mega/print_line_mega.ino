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
float lineLocate_t[16] = {0,  22.5, 45, 67.5, 90, 112.5,  135,  157.5,  180,  -157.5, -135, -112.5, -90,  -67.5,  -45,  -22.5};
Coordinate lineLocate[16];
Coordinate n;
int rawData[16] = {0};
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
  if(ifLine)getData_line();
  if(digitalRead(intPin_line))ifLine = 0;
  Serial.println(digitalRead(intPin_line));
}

void int_line(){
  //モーターをブレーキ
  
  //ライン処理フラグ
  ifLine = true;
}

void getData_line(){
  int upperData = 0;
  int lowerData = 0;
  int lineNum = 0;

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
    lineNum += rawData[i] + rawData[i+8];
    lowerData = ( lowerData - rawData[i] ) / 2;
    upperData = ( upperData - rawData[i+8] ) / 2;
  }

  for(int i=0; i<16; i++){
    Serial.print((String)rawData[i]);
  }
  Serial.println();//*/

  //一つだけ反応してたら接線
  //二つ以上反応してたら最小二乗法で直線
  //線に垂直な向きに動く
  //反応した点の個数
  /*if(lineNum == 1){
    for(int i=0; i<16; i++){
      n.T = rawData[i] * -lineLocate.T[i];
    }
    n.R = 1;
    RTtoXY(&n);
  }else{
    double Sx,Sy,Sxx,Sxy,a,b;

    for(i=0;i<16;i++){
      if(rawData[i]){
        Sx += lineLocate.X[i];
        Sy += lineLocate.Y[i];
        Sxx += lineLocate.X[i]*lineLocate.X[i];　//2乗和
        Sxy += lineLocate.X[i]*lineLocate.Y[i];
      }
    }

    a = (Sx*Sy – lineNum*Sxy);
    b = -(Sx*Sx – lineNum*Sxx);
    c = (Sx*Sxy – Sxx*Sy);

  }//*/
}



