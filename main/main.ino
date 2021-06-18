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

//メイン
const vector ball_capture_area = {0, 11.405, 11.405, 0}; //ロボットの中心からみた補足エリアの場所
const vector robot_center      = {0, 0, 0, 0};           //ロボットの中心
const vector motor_center      = {0, -2.494,  2.494, 0}; //ロボットの中心からみたモーターの中心の場所

//モーター
const uint8_t motorPin[8]         = {6,7,4,5,3,2,1,0};         //モーターの制御ピン
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
void  move_robot(vector substantial_mov, float rotate);   //回転しながら動く(動く方向, 回転する角度(回転しないのも含む))
void  move_rotate(vector center, float rotate);           //回転する（回転の中心、角度）
void  mov_stop();                                         //止まる
void  mov(float V[], float Delay);                        //モーター関数用
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


//動く(動く方向)
void move_robot(vector substantial_mov) {
  vector motor_mov; //軸を45度回転し、モーターの動かす量を求める
  vector absolute_move;
  float V[4]; //モーターごとの動かす量
  float delay_value;

  //回転を機体からみたものに戻す
  //原点を機体の中心からモーターの中心に変換

  //軸を45度回転し、モーターの動かす量を求める
  motor_mov.R = substantial_mov.R;
  motor_mov.T = substantial_mov.T + 0.125;
  RTtoXY(&motor_mov);

  //モーターごとの動かす量(単位ベクトル)
  V[0] =  motor_mov.Y / motor_mov.R;
  V[1] = -motor_mov.X / motor_mov.R;
  V[2] = -motor_mov.Y / motor_mov.R;
  V[3] =  motor_mov.X / motor_mov.R;

  delay_value = motor_mov.R;

  Serial.println();
  for(int i=0; i<4; i++){
    Serial.print("V");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(V[i]);
  }
  Serial.print("delay_value ");
  Serial.println(delay_value);
  
  mov(V, delay_value);
  
  //記録をとる
}


//回転する（回転の中心、角度）
void move_rotate(vector center, float rotate) {
  vector actual_move; //実際に動こうとする方向
  vector center_motor;  //原点を機体の中心からモーターの中心に変換したもの
  vector absolute_move; //機体の向きを加えたもの
  vector motor_mov; //軸を45度回転し、モーターの動かす量を求める
  float V[4]; //モーターごとの動かす量
  float delay_value; //どれだけ動くか
  
  center_motor.X = center.X - motor_center.X;
  center_motor.Y = center.Y - motor_center.Y;
  XYtoRT(&center_motor);
  Serial.print("center_motor.R ");
  Serial.println(center_motor.R);
  Serial.print("center_motor.T ");
  Serial.println(center_motor.T);

  
  //実際に動こうとする方向を決める
  if (center_motor.R != 0){
    actual_move.R = 1;
  }else{
    actual_move.R  = 0;
    center_motor.R = motor_[0];
  }
  
  if (rotate == 0){
    return NULL;
  }else if (rotate < 0){
    actual_move.T = center_motor.T + 0.5;
  }else if (rotate > 0){
    actual_move.T = center_motor.T - 0.5;
  }
  RTtoXY(&actual_move);

  Serial.print("center_motor.R ");
  Serial.println(center_motor.R);

  
  //軸を45度回転し、モーターの動かす量を求める
  motor_mov.R = actual_move.R;
  motor_mov.T = actual_move.T + 0.125;
  RTtoXY(&motor_mov);
  
  //モーターごとの動かす量(単位ベクトル)
  V[0] = ( motor_mov.Y * center_motor.R  + motor_[0]) / (motor_[0] + center_motor.R);
  V[1] = (-motor_mov.X * center_motor.R  + motor_[1]) / (motor_[1] + center_motor.R);
  V[2] = (-motor_mov.Y * center_motor.R  + motor_[2]) / (motor_[2] + center_motor.R);
  V[3] = ( motor_mov.X * center_motor.R  + motor_[3]) / (motor_[3] + center_motor.R);
  delay_value = center_motor.R * rotate * M_PI;
  
  Serial.println();
  for(int i=0; i<3; i++){
    Serial.print("V");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(V[i]);
  }
  Serial.print("delay_value ");
  Serial.println(delay_value);

  mov(V, delay_value); //動かす

  //記録をとる
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
