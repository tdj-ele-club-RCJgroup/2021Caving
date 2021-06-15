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
const uint8_t motorPin[6]         = {6,7,4,5,3,2};         //モーターの制御ピン
const float   motor_[3]           = {6.611, 10.28, 10.28}; //モーターの中心からの距離[cm]
const float   motor_character[3]  = {1.000, 1.000, 0.700}; //モーターの誤差補正


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


//回転しながら動く(動く方向, 回転する角度(回転しないのも含む))
void move_robot(vector substantial_mov, float rotate) {
  float centerR;
  int   norotate = 1;
  vector actual_move;  //実際に動こうとする方向
  vector substantial_mov_motor;  //回転を機体からみたものに戻し、原点を機体の中心からモーターの中心に変換したもの
  vector absolute_move; 
  float V[3]; //モーターごとの動かす量
  float delay_value;

  //回転を機体からみたものに戻す
  substantial_mov_motor.R = substantial_mov.R;
  substantial_mov_motor.T = substantial_mov.T - rotation;
  RTtoXY(&substantial_mov_motor);
  /*/原点を機体の中心からモーターの中心に変換
  substantial_mov_motor.X = substantial_mov_motor.X - motor_center.X;
  substantial_mov_motor.Y = substantial_mov_motor.Y - motor_center.Y;
  XYtoRT(&substantial_mov_motor);/*/
  
  centerR = substantial_mov_motor.R / sqrt (2.00  -  2 * cos (rotate*M_PI) );
  
  if (rotate > 0){
    actual_move.T = substantial_mov_motor.T + acos(sqrt(2.00  -  2 * cos(rotate*M_PI) ) / 2.00) /M_PI  - 1;
  }else if (rotate < 0){
    actual_move.T = substantial_mov_motor.T - acos(sqrt(2.00  -  2 * cos(rotate*M_PI) ) / 2.00) /M_PI  + 1;
  }else{
    actual_move.T = substantial_mov_motor.T;
    norotate = 0;
    centerR  = 1;
  }
  
  actual_move.R = 1;
  RTtoXY(&actual_move);
  
  Serial.println("actual_move.R ");
  Serial.println(actual_move.R);
  Serial.println("actual_move.T ");
  Serial.println(actual_move.T);
  Serial.println("centerR ");
  Serial.println(centerR);
  
  V[0] = ((               actual_move.X                                    ) * centerR  + motor_[0] * norotate) / (motor_[0] * norotate  + centerR);
  V[1] = ((-1.00 / 2.00 * actual_move.X + sqrt(3.00) / 2.00 * actual_move.Y) * centerR  + motor_[1] * norotate) / (motor_[1] * norotate  + centerR);
  V[2] = ((-1.00 / 2.00 * actual_move.X - sqrt(3.00) / 2.00 * actual_move.Y) * centerR  + motor_[2] * norotate) / (motor_[2] * norotate  + centerR);
  
  if (rotate == 0){
    delay_value = substantial_mov.R;
  }else{
    delay_value = rotate * M_PI * centerR;
  }

  Serial.println();
  for(int i=0; i<3; i++){
    Serial.print("V");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(V[i]);
  }
  Serial.print("delay_value ");
  Serial.println(delay_value);
  
  mov(V, delay_value);
  
  //記録をとる
  absolute.X += substantial_mov.X;
  absolute.Y += substantial_mov.Y;
  XYtoRT(&absolute);
  rotation += rotate;
}


//回転する（回転の中心、角度）
void move_rotate(vector center, float rotate) {
  vector actual_move; //実際に動こうとする方向
  vector center_motor;  //原点を機体の中心からモーターの中心に変換したもの
  vector absolute_move; //機体の向きを加えたもの
  float V[3]; //モーターごとの動かす量
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
  
  //モーターごとの動かす量を代入
  V[0] = ((               actual_move.X                                    ) * center_motor.R  + motor_[0]) / (motor_[0] + center_motor.R);
  V[1] = ((-1.00 / 2.00 * actual_move.X + sqrt(3.00) / 2.00 * actual_move.Y) * center_motor.R  + motor_[1]) / (motor_[1] + center_motor.R);
  V[2] = ((-1.00 / 2.00 * actual_move.X - sqrt(3.00) / 2.00 * actual_move.Y) * center_motor.R  + motor_[2]) / (motor_[2] + center_motor.R);
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
  if (cos(rotate*M_PI) == 0){
    absolute_move.R = 2 * center.R;
  }else{
    absolute_move.R = sqrt( 2 * pow(center.R, 2.0) * (1 - cos(rotate*M_PI) ));
  }
  absolute_move.T = center.T - acos(sqrt(2  -  2 * cos(rotate*M_PI) ) / 2) / M_PI + rotation;
  RTtoXY(&absolute_move);
  
  absolute.X += absolute_move.X;
  absolute.Y += absolute_move.Y;
  XYtoRT(&absolute);
  rotation += rotate;
}


//モーター関数用
void mov(float V[], float Delay){
  
  uint8_t pin[6];
  for(int i=0; i<3; i++){
    int j = i*2;
    if (V[i] > 0){pin[j] = 0; pin[j+1] = 1;}
    else {pin[j] = 1; pin[j+1] = 0; V[i] = -V[i];}
    Serial.print(pin[j] + j);
    Serial.print(":");
    Serial.println(V[i] * motor_PWM * motor_character[i]);
  }

  const unsigned long startTime_us = millis();
  do{
    for(int i=0; i<3; i++){
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


