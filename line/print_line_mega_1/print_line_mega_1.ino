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
float getData_line();
volatile bool ifLine; //ラインあるなしフラグ
bool ifLine_process; //ライン処理実行中フラグ

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);
}

void loop(){
  float back;
  back = getData_line();
  if(!ifLine){
    Serial.println("noline");
  }else{
    digitalWrite(37,HIGH);
    ifLine_process = true;
    Serial.println("line");
    //move_robot(back);
    ifLine_process = false;
    digitalWrite(37,LOW);
  }
}

float getData_line(){
  ifLine = false;
  int incomingByte = 0;
  float back_t = 0;

  while (Serial1.available()) Serial1.read(); //バッファを消す
  Serial1.write(1);
  Serial1.flush();
  for(int i=0; i<1;){
    if(Serial1.available()){
      ifLine = Serial1.read();
      i++;
    }/*else{
      Serial1.write(1);
      Serial1.flush();
    }//*/
  }
/*  while(!Serial1.available());
  do{
    incomingByte = Serial1.read();
    ifLine = incomingByte;
  }while(incomingByte == -1);*/
  Serial.println((String)"ifline" + "\t" + ifLine);
  while (Serial1.available()) Serial1.read(); //バッファを消す
  Serial1.write(1);
  Serial1.flush();
  //while(!Serial1.available());
  if(ifLine){
    for(int i=0; i<4;){
      if(Serial1.available()){
        *((byte*)&back_t + i++) = Serial1.read();
        //Serial.println("here");
      }
    }
    Serial1.write(1);
    Serial1.flush();//*/
  }

  Serial.println((String)"back" + "\t" + back_t);
  return back_t;
}
