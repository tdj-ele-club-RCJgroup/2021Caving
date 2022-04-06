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
#define Line_interupts() (attachInterrupt(digitalPinToInterrupt(intPin_line),int_line,FALLING))
#define Line_noInterrupts() (detachInterrupt(digitalPinToInterrupt(intPin_line)))
float lineLocate_t[16] = {0,  22.5, 45, 67.5, 90, 112.5,  135,  157.5,  180,  -157.5, -135, -112.5, -90,  -67.5,  -45,  -22.5};
Coordinate lineLocate[16];
int rawData[16] = {0};
void lineLocateCul(){
  for(int i=0; i<16; i++){
    lineLocate[i].R = 1;
    lineLocate[i].T = lineLocate_t[i];
    RTtoXY(&lineLocate[i]);
  }
}; //lineLocateの初期化 setup関数で実行
float getData_line();
void int_line();
volatile bool ifLine; //ラインあるなしフラグ
bool ifLine_process; //ライン処理実行中フラグ

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);
  lineLocateCul();
  Line_interupts();
}

void loop(){
  float back;
  Line_noInterrupts();
  back = getData_line();
  if(digitalRead(intPin_line)){
    ifLine = 0;
    Serial.println("noline");
    Line_interupts();
  }else if(!ifLine){
    Serial.println("line_broken");
  }else{
    digitalWrite(37,HIGH);
    ifLine_process = true;
    Serial.println("line");
    //move_robot(back);
    ifLine_process = false;
    digitalWrite(37,LOW);
    Line_interupts();
  }
}

void int_line(){
  //ラインあるなしフラグ
  ifLine = true;
}

float getData_line(){
  int upperData = 0;
  int lowerData = 0;
  int lineNum = 0;
  Coordinate back = {0,0,1,0}; //線をよけて進む向き
  ifLine = false;

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

  /*Serial.println(lowerData);
  Serial.println(upperData);//*/

  for(int i=0; i<8; i++){
    rawData[i] = lowerData % 2;
    rawData[i+8] = upperData % 2;
    lineNum += rawData[i] + rawData[i+8];
    lowerData = ( lowerData - rawData[i] ) / 2;
    upperData = ( upperData - rawData[i+8] ) / 2;
  }
  if(lineNum) ifLine = true;

  for(int i=0; i<16; i++){
    if(rawData[i])Serial.println((String)i);
  }
  Serial.println();//*/

  //ベクトルを足して線をよける方向を計算
  for(int i=0; i<16; i++){
    back.X += -(rawData[i] * lineLocate[i].X);
    back.Y += -(rawData[i] * lineLocate[i].Y);
  }
  XYtoRT(&back);
  Serial.println((String)"back" + "\t" + back.T);
  return back.T;
}
