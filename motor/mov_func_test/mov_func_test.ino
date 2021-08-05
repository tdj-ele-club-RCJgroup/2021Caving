/*////////////////////////////
  ライブラリなどの初期宣言
  /*////////////////////////////
typedef struct {
  float X; //直交座標のⅹ座標 [cm]
  float Y; //直交座標のＹ座標 [cm]
  float R; //　極座標のｒ（中心からの距離）[cm]
  float T; //　極座標のθ（中心から線を引いたときのｘ軸との角度）...ラジアン/π
  } vector;

/*////////////////////////////
  定数の宣言
  /*////////////////////////////

//モーター
const uint8_t motorPin[8]         = {5,6,7,8,10,11,12,13};         //モーターの制御ピン
const float   motor_[4]           = {0, 0, 0, 0}; //モーターの中心からの距離[cm]
const float   motor_character[4]  = {1.000, 1.000, 1.000, 1.000}; //モーターの誤差補正

/*////////////////////////////
  変数宣言開始
  /*////////////////////////////

//モーター
int   motor_PWM         = 255;  //0~255のpwmの基準の値
float motor_delay_ratio = 12;   //1cm進むのに待つ時間[ms]

/*////////////////////////////
  関数のプロトタイプ宣言開始
  /*////////////////////////////
void  XYtoRT(vector *Data);                               //ベクトルの変換
void  RTtoXY(vector *Data);                               //    〃
void  mov_stop();                                         //止まる
void  mov(float V[], float Delay);                        //モーター関数用(動く量、時間(ミリ秒))
/*////////////////////////////
  セットアップ関数の開始
  /*////////////////////////////

void setup() {
  //シリアル通信を9600bpsで始める
  //一応、テストなどの通信用
  Serial.begin(9600);

  //モーターのデジタルピン宣言
  for (int i=0; i<6; i++){
    pinMode(motorPin[i], OUTPUT);
  }
}
/*////////////////////////////
  ループ関数の開始
  /*////////////////////////////
  
void loop() {
  float test[4] = {0,0,0,1};
  mov(test,1000);
}
/*////////////////////////////
  関数の宣言開始
  /*////////////////////////////


//ベクトルの変換
void XYtoRT(vector *Data){
  Data->R = sqrt(pow(Data->X, 2.0) + pow(Data->Y, 2.0));
  Data->T = atan2(Data->Y, Data->X) / M_PI;
}
void RTtoXY(vector *Data){
  Data->X = Data->R * cos(Data->T * M_PI);
  Data->Y = Data->R * sin(Data->T * M_PI);
}


//モーター関数用
void mov(float V[], float Delay){

  //プラスとマイナスを分ける
  uint8_t pin[8];
  for(int i=0; i<4; i++){
    int j = i*2;
    if (V[i] > 0){pin[j] = 0; pin[j+1] = 1;}
    else {pin[j] = 1; pin[j+1] = 0; V[i] = -V[i];}
    Serial.print(pin[j] + j);
    Serial.print(":");
    Serial.println(V[i] * motor_PWM * motor_character[i]);
  }

  //出力
  const unsigned long startTime_us = millis();
  do{
    for(int i=0; i<4; i++){
      int j = i*2;
      analogWrite(motorPin[pin[j]   + j],  V[i] * motor_PWM * motor_character[i]);   //動かす
      analogWrite(motorPin[pin[j+1] + j],  0);
    }
  } while((millis() - startTime_us) < Delay * motor_delay_ratio);
  
  Serial.print("delay_value ");
  Serial.println(Delay);//*/
}


//止まる
void mov_stop(){
  for(int i=0; i<6; i++){
    analogWrite(motorPin[i], 255);
  }
}
