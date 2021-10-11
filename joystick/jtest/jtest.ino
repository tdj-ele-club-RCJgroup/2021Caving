typedef struct {
  float X; //直交座標のⅹ座標 [cm]
  float Y; //直交座標のＹ座標 [cm]
  float R; //　極座標のｒ（中心からの距離）[cm]
  float T; //　極座標のθ（正面0,-180~180）...度
} vector;

void XYtoRT(vector *Data);                               //ベクトルの変換
void RTtoXY(vector *Data);                               //    〃

void setup() {
  Serial.begin(9600);

  //ジョイスティックピン宣言
  pinMode(46, INPUT_PULLUP);
}
  
void loop() {
  vector joystick;      //進みたい目的地
  joystick.X = analogRead(6);
  joystick.Y = analogRead(7);
  Serial.println((String)"rowjoystick.X" + joystick.X);
  Serial.println((String)"rowjoystick.Y" + joystick.Y);
  joystick.X = joystick.X - 517;
  joystick.Y = -joystick.Y + 511;
  XYtoRT(&joystick);
  Serial.println((String)"joystick.T" + joystick.T);

  if(/*!digitalRead(46) ||*/ fabsf(joystick.R) < 20){
    Serial.println((String)"stop");
  }else{
    Serial.println((String)"moveT" + joystick.T);
  }

  delay(1000);
}

//ベクトルの変換
void XYtoRT(vector *Data){
  Data->R = sqrt(pow(Data->X, 2.0) + pow(Data->Y, 2.0));
  Data->T = atan2(Data->Y, Data->X) * 180 / M_PI  -  90;
  if(-275 <= Data->T && Data->T <= -180){
    Data->T = Data->T + 360;
  }
}
void RTtoXY(vector *Data){
  Data->Y = Data->R * sin((Data->T + 90) / 180 * M_PI);
  Data->X = Data->R * cos((Data->T + 90) / 180 * M_PI);
  Data->T = fmodf(Data->T + 720 , 360);
  if(Data->T > 180){
    Data->T = Data->T - 360;
  }
}
