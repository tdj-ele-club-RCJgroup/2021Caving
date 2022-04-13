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
}; //座標の変換
void RTtoXY(Coordinate *Data){
  Data->Y = Data->R * sin((Data->T + 90) / 180 * M_PI);
  Data->X = Data->R * cos((Data->T + 90) / 180 * M_PI);
  Data->T = fmodf(Data->T + 720 , 360);
  if(Data->T > 180){
    Data->T = Data->T - 360;
  }
}; //座標の変換
float innerP(Coordinate a,Coordinate b){
  return a.X * b.X + a.Y * b.Y;
}; //内積


//ライン制御
#define intPin 2
const uint8_t linePin[16] = {11,18,19,16,17,14,15,4,3,6,5,8,7,10,9,12};
const float lineLocate_t[16] = {0,  22.5, 45, 67.5, 90, 112.5,  135,  157.5,  180,  -157.5, -135, -112.5, -90,  -67.5,  -45,  -22.5};
Coordinate lineLocate[16];
//byte upperData = 0;
//byte lowerData = 0;
uint8_t rawdata[16][4] = {0};
Coordinate back = {0,0,1,0}; //線をよけて進む向き
Coordinate back_first;
bool ifLine_raw[4] = {false};
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
    //digitalWrite(intPin,LOW);
    back_first = back;
    //Serial.println((String)"line founded " + back.T);
  }
  if(BFifLine && !ifLine){
    //digitalWrite(intPin,HIGH);
    //Serial.println((String)"lineEnded");
  }
  if(BFifLine && ifLine){
    float chang = innerP(back_first, back) / (back_first.R * back.R);
    if(chang < -0.5){
      back.T = back.T + 180;
      RTtoXY(&back);
      //Serial.println((String)"over");
    }else if(-0.5 < chang || chang < 0.5){
      back = back_first;
    }
    //Serial.println((String)"line" + back.T);
  }
  //if(Serial.available()) sendData();
}

void serialEvent(){
  sendData();
}

 //読み取り
void getData(){
  uint8_t culdata[16] = {0};
//  upperData = 0;
//  lowerData = 0;
  ifLine = false;
  back.X = 0;
  back.Y = 0;
  for(int i=0; i<4; i++){
    ifLine_raw[i] = ifLine_raw[i+1];
    for(int j=0; j<16; j++){
      rawdata[j][i] = rawdata[j][i+1];
    }
  }//配列更新

  ifLine_raw[3] = false;
  for(int i=0; i<16; i++){
    rawdata[i][3] = !digitalRead(linePin[i]);
    if((rawdata[i][0] + rawdata[i][1] + rawdata[i][2] + rawdata[i][3]) >= 3){
      culdata[i] = 1;
    }
    if(culdata[i]) ifLine_raw[3] = true;
    //if(culdata[i]) Serial.println((String)i + "\t" + culdata[i]);
  }

  if((ifLine_raw[0] + ifLine_raw[1] + ifLine_raw[2] + ifLine_raw[3]) >=1){
    ifLine = true;
    //Serial.println((String)"iflineTrue");
  }

  //ベクトルを足して線をよける方向を計算
  for(int i=0; i<16; i++){
    back.X += -(culdata[i] * lineLocate[i].X);
    back.Y += -(culdata[i] * lineLocate[i].Y);
  }
  XYtoRT(&back);
  //if(ifLine)Serial.println((String)"back.T " + back.T);
  
  /*for(int i=0; i<8; i++){
    lowerData += culdata[i] * (1<<i);
    upperData += culdata[i+8] * (1<<i);
  }
  Serial.println((String)"lowerData" + lowerData);
  Serial.println((String)"upperData" + upperData);//*/
}

void sendData(){
  while (Serial.available()) Serial.read(); //バッファを消す
  if(ifLine){
    Serial.write(1);
    while (!Serial.available()); //arduinoから送られてくるまで待つ
    Serial.read(); //バッファを消す
  }else{
    Serial.write(0);
    while (!Serial.available()); //arduinoから送られてくるまで待つ
    Serial.read(); //バッファを消す
    return;
  }
  Serial.write((byte*)&back.T,4);
  while (!Serial.available()); //arduinoから送られてくるまで待つ
  Serial.read(); //バッファを消す
}
