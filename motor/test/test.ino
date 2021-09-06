typedef struct {
  float X; //直交座標のⅹ座標 [cm]
  float Y; //直交座標のＹ座標 [cm]
  float R; //　極座標のｒ（中心からの距離）[cm]
  float T; //　極座標のθ（中心から線を引いたときのｘ軸との角度）...ラジアン/π
} Coordinate;

//モーター
const uint8_t motorPin[8]         = {7,8,10,11,12,13,5,6};         //モーターの制御ピン
const float   motor_[4]           = {0, 0, 0, 0}; //モーターの中心からの距離[cm]
const float   motor_character[4]  = {1.000, 1.000, 1.000, 1.000}; //モーターの誤差補正

//モーター
int   motor_PWM         = 255;  //0~255のpwmの基準の値
float motor_delay_ratio = 12;   //1cm進むのに待つ時間[ms]

void  XYtoRT(Coordinate *Data);                               //ベクトルの変換
void  RTtoXY(Coordinate *Data);                               //    〃
void  move_robot(float Theta);                            //モータの出力計算(目標の方向)
void  mov_stop();                                         //止まる
void  mov(float V[], float Delay);                        //モーター関数用

void setup() {
  Serial.begin(9600);

  //モーターのデジタルピン宣言
  for (int i=0; i<6; i++){
    pinMode(motorPin[i], OUTPUT);
  }
}
  
void loop() {
  Coordinate aim;      //進みたい目的地
  aim.R = 1;
  aim.T = 0.5;       //前方向
  move_robot(aim.T);
}

//ベクトルの変換
void XYtoRT(Coordinate *Data){
  Data->R = sqrt(pow(Data->X, 2.0) + pow(Data->Y, 2.0));
  Data->T = atan2(Data->Y, Data->X) / M_PI;
}
void RTtoXY(Coordinate *Data){
  Data->X = Data->R * cos(Data->T * M_PI);
  Data->Y = Data->R * sin(Data->T * M_PI);
}

//モータの出力計算(目標の方向)
void move_robot(float Theta) {
  Coordinate motor_mov; //座標軸を45度回転した後の座標を後で格納
  float V[4]; //モーターごとの動かす量

  //回転を機体からみたものに戻す
  //原点を機体の中心からモーターの中心に変換

  //軸を45度回転し、モーターの動かす量を求める
  motor_mov.R = 1;
  Serial.println((String)"motor_mov.R = " + motor_mov.R);
  motor_mov.T = Theta + 0.25;
  Serial.println((String)"motor_mov.T = " + motor_mov.T);
  RTtoXY(&motor_mov);

  //モーターごとの動かす量(単位ベクトル)
  V[0] = -motor_mov.Y / motor_mov.R;
  V[1] =  motor_mov.X / motor_mov.R;
  V[2] =  motor_mov.Y / motor_mov.R;
  V[3] = -motor_mov.X / motor_mov.R;

  //delay_value = motor_mov.R;  //進む距離

   Serial.println();
  for(int i=0; i<4; i++){
    Serial.print("abV");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(V[i]);
  }

  //V[ ]の最大値を1にする
  if(fabsf(V[0]) >= fabsf(V[1])){
    float maximum = fabsf(V[0]);
    for(int i = 0;i < 4; i++){
      V[i] = V[i] / maximum * motor_PWM * motor_character[i];
      Serial.print("|V[0]| >= |V[1]|    : ");
      Serial.println((String)"V[" + i + "]= " + V[i]);
    }
  }else{
    float maximum = fabsf(V[1]);
    for(int i = 0;i < 4; i++){
      V[i] = V[i] / maximum * motor_PWM * motor_character[i];
    }
  }
  
  Serial.println();
  for(int i=0; i<4; i++){
    Serial.print("V");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(V[i]);
  }
  
  //出力
  for(int i = 0; i < 4; i++){
    if (V[i] > 0){
      analogWrite(motorPin[2*i], V[i]);
      analogWrite(motorPin[2*i+1],0);
      Serial.println((String)motorPin[2*i] + ":" + V[i]);
    }else{
      analogWrite(motorPin[2*i],  0);
      analogWrite(motorPin[2*i+1],-V[i]);
      Serial.println((String)motorPin[2*i+1] + ":" + -V[i]);
    }
  }
}

//止まる
void mov_stop(){
  for(int i=0; i<6; i++){
    analogWrite(motorPin[i], 255);
  }
}
