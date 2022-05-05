/*////////////////////////////
  定数・変数の宣言
  /*////////////////////////////

//モーター
const uint8_t motorPin[8]         = {7,6,9,8,10,11,5,4};         //モーターの制御ピン
const float   motor_[4]           = {0, 0, 0, 0}; //モーターの中心からの距離[cm]
const float   motor_character[4]  = {0.900, 1.000, 1.000, 0.900}; //モーターの誤差補正
//int   motor_PWM         = 255;  //0~255のpwmの基準の値
float motor_delay_ratio = 1;   //1cm進むのに待つ時間[ms]

/*////////////////////////////
  関数のプロトタイプ宣言
  /*////////////////////////////
void  mov_stop();                                         //止まる
void  mov(float V[], float Delay);                        //モーター関数用(動く量、時間(ミリ秒))
/*////////////////////////////
  セットアップ関数
  /*////////////////////////////

void setup() {
  //シリアル通信を9600bpsで始める
  //一応、テストなどの通信用
  Serial.begin(9600);

  //モーターのデジタルピン宣言
  for (int i=0; i<8; i++){
    pinMode(motorPin[i], OUTPUT);
  }
}
/*////////////////////////////
  ループ関数
  /*////////////////////////////
  
void loop() {
  float test0[4] = {225,225,200,200};
  float test[4] = {-225,0,0,-225};
  float test1[4] = {0,-225,-225,0};
  float test2[4] = {225,225,0,0};
  float test3[4] = {0,0,-225,-225};
  float test4[4] = {225,0,-225,0};
  float test5[4] = {0,225,0,-225};
  mov(test0,500);
  mov_stop();
  delay(1000);
  /*mov(test3,200);
  mov_stop();
  delay(1000);//*/
}
/*////////////////////////////
  関数の宣言
  /*////////////////////////////

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
    Serial.println(V[i] /* motor_PWM */ * motor_character[i]);
  }

  //出力
  const unsigned long startTime_us = millis();
  do{
    for(int i=0; i<4; i++){
      int j = i*2;
      analogWrite(motorPin[pin[j]   + j],  V[i] /* motor_PWM*/ * motor_character[i]);   //動かす
      analogWrite(motorPin[pin[j+1] + j],  0);
    }
  } while((millis() - startTime_us) < Delay * motor_delay_ratio);
  
  Serial.print("delay_value ");
  Serial.println(Delay);//*/
}


//止まる
void mov_stop(){
  for(int i=0; i<8; i++){
    analogWrite(motorPin[i],0);
  }
}
