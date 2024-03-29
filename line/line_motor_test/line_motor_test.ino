#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h> //ジャイロセンサ用
#include <ArduinoSort.h> //赤外線ソート用
#include <QuickStats.h>
QuickStats stats; //initialize an instance of this class

//座標
typedef struct {
  float X; //直交座標のⅹ座標 [cm]
  float Y; //直交座標のＹ座標 [cm]
  float R; //　極座標のｒ（中心からの距離）[cm]
  float T; //　極座標のθ（正面0,-180~180）...度
} Coordinate;
void XYtoRT(Coordinate *Data);      //座標の変換
void RTtoXY(Coordinate *Data);      //    〃

//モーター
const uint8_t motorPin[8]         = {6,7,8,9,10,11,4,5};         //モーターの制御ピン
const float   motor_[4]           = {0, 0, 0, 0}; //モーターの中心からの距離[cm]
const float   motor_character[4]  = {1.000, 1.000, 1.000, 1.000}; //モーターの誤差補正
int   motor_PWM         = 255;  //0~255 出力するpwmの最大値
float motor_delay_ratio = 12;   //1cm進むのに待つ時間[ms]
void move_robot(float Theta);   //モータの出力計算(目標の方向)
void move_rotate(float Theta);  //回転する（回転の中心、角度）
void move_stop();               //止まる
void move_off();                //ドライバーへの出力をLOWにする

//赤外線センサ
#define range 20
const uint8_t IRpin[8] = {9,10,11,12,13,14,15,8};
const int IRhigh[8] = {800,800,800,800,800,800,800,800};
const int IRlow[8]  = {0, 0,  0,  0,  1,  7,  4,  6};
float IRlocate_t[8] = {0  ,45 ,90 ,135,180,-135,-90 ,-45 };
Coordinate IRlocate[8];
void IRlocateCul(){
  for(int i=0; i<8; i++){
    IRlocate[i].R = 1;
    IRlocate[i].T = IRlocate_t[i];
    RTtoXY(&IRlocate[i]);
  }
}; //IRlocateの初期化 setup関数で実行
Coordinate ball;
bool noball = false;
void sen_IRball();  //赤外線センサ(ボール位置をballに代入)

//ボール補足センサ
#define capturePin 2
bool ballCapture();

//ラインセンサ
#define LED 12
#define intPin_line 3
float lineLocate_t[16] = {0,  22.5, 45, 67.5, 90, 112.5,  135,  157.5,  180,  -157.5, -135, -112.5, -90,  -67.5,  -45,  -22.5};
Coordinate lineLocate[16];
int rawData[16] = {0};
volatile bool ifLine; //ラインあるなしフラグ
bool ifLine_process; //ライン処理実行中フラグ
void lineLocateCul(){
  for(int i=0; i<16; i++){
    lineLocate[i].R = 1;
    lineLocate[i].T = lineLocate_t[i];
    RTtoXY(&lineLocate[i]);
  }
}; //lineLocateの初期化 setup関数で実行
void sen_line(); //ライン処理
float getData_line(); //ラインデータ読み取り（ナノと通信）
void int_line(); //ラインセンサ割り込み

//超音波センサ
uint8_t echoPin[4][2] = {{47,49},{48,46},{44,42},{43,45}};
const double speed_of_sound = 331.5; // 0℃の気温の速度
const double temp = 25;
double distance[4] = {0};
void echo(); //超音波センサ読み取り

//ジャイロ
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x29);
bool  nogyro = false;
float firstRotation = 0; //はじめの値を記録
float rotate = 0;  //0が前向き
const float rot_ign = 5;  //多少の傾きは無視
void gyro();  //ジャイロセンサ更新(rotateに代入)
void lifted(); //持ち上げ確認


void setup() {
  Serial.begin(9600);

  //モーターのデジタルピン宣言
  for (int i=0; i<8; i++){
    pinMode(motorPin[i], OUTPUT);
  }
  
  //赤外線センサ場所の計算
  IRlocateCul();

  //ラインセンサ通信
  Serial1.begin(9600);
  //ラインセンサ場所の計算、割り込み開始
  lineLocateCul();
  //attachInterrupt(digitalPinToInterrupt(intPin_line),int_line,FALLING);
  //センサのLEDを発光
  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);

  //超音波センサピン宣言
  for(int i=0; i<4; i++){
    pinMode(echoPin[i][1], INPUT);
    pinMode(echoPin[i][0], OUTPUT);
  }

  //ジャイロセンサ開始
  if(!bno.begin()){
    Serial.print("No gyro");
    nogyro = true;
  };
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  firstRotation = euler.x();
}
  
void loop() {
  gyro();
  //if(digitalRead(intPin_line))ifLine = 0;
  /*getData_line();
  Serial.println(digitalRead(2));//*/

  for(int i=0; i<8; i=i+2){
    analogWrite(motorPin[i],255);
    analogWrite(motorPin[i+1],0);
  }
  gyro();
  /*getData_line();
  Serial.println(digitalRead(2));//*/
  /*for(int i=0; i<8; i=i+2){
    analogWrite(motorPin[i],255);
    analogWrite(motorPin[i+1],255);
  }
  getData_line();//*/
  delay(1000);//*/
  for(int i=0; i<8; i=i+2){
    analogWrite(motorPin[i],0);
    analogWrite(motorPin[i+1],255);
  }
  //getData_line();
  /*delay(1000);
  for(int i=0; i<8; i=i+2){
    analogWrite(motorPin[i],255);
    analogWrite(motorPin[i+1],255);
  }//*/
  gyro();//*/
  /*getData_line();
  Serial.println(digitalRead(2));//*/
  delay(1000);//*/
}

//ベクトルの変換
void XYtoRT(Coordinate *Data){
  Data->R = sqrt(pow(Data->X, 2.0) + pow(Data->Y, 2.0));
  Data->T = atan2(Data->Y, Data->X) * 180 / M_PI  -  90;
  if(-275 <= Data->T && Data->T <= -180){
    Data->T = Data->T + 360;
  }
}
void RTtoXY(Coordinate *Data){
  Data->Y = Data->R * sin((Data->T + 90) / 180 * M_PI);
  Data->X = Data->R * cos((Data->T + 90) / 180 * M_PI);
  Data->T = fmodf(Data->T + 720 , 360);
  if(Data->T > 180){
    Data->T = Data->T - 360;
  }
}

//モータの出力計算(目標の方向)
void move_robot(float Theta) {
  if(ifLine && !ifLine_process) return; //ラインが反応すると動かない
  //digitalWrite(36,HIGH);

  Coordinate motor_mov; //座標軸を45度回転した後の座標を後で格納
  float V[4]; //モーターごとの動かす量

  //回転を機体からみたものに戻す
  Theta = Theta - rotate;
  //原点を機体の中心からモーターの中心に変換

  //軸を45度回転し、モーターの動かす量を求める
  motor_mov.R = 1;
  Serial.println((String)"motor_mov.R = " + motor_mov.R);
  motor_mov.T = Theta + 45;
  Serial.println((String)"motor_mov.T = " + motor_mov.T);
  RTtoXY(&motor_mov);

  //モーターごとの動かす量(単位ベクトル)
  V[0] = -motor_mov.Y / motor_mov.R;
  V[1] =  motor_mov.X / motor_mov.R;
  V[2] =  motor_mov.Y / motor_mov.R;
  V[3] = -motor_mov.X / motor_mov.R;

  //delay_value = motor_mov.R;  //進む距離

  /* Serial.println();
  for(int i=0; i<4; i++){
    Serial.print("abV");
    Serial.print(i);
    Serial.print(" ");
    Serial.println(V[i]);
  }//*/

  //V[ ]の最大値を1にする
  if(fabsf(V[0]) >= fabsf(V[1])){
    float maximum = fabsf(V[0]);
    for(int i = 0;i < 4; i++){
      V[i] = V[i] / maximum * motor_PWM * motor_character[i];
      /*Serial.print("|V[0]| >= |V[1]|    : ");
      Serial.println((String)"V[" + i + "]= " + V[i]);//*/
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
  }//*/
  
  //出力
  for(int i = 0; i < 4; i++){
    if (V[i] > 0){
      analogWrite(motorPin[2*i], V[i]);
      analogWrite(motorPin[2*i+1],0);
      //Serial.println((String)(2*i) + ":" + V[i]);
    }else{
      analogWrite(motorPin[2*i],  0);
      analogWrite(motorPin[2*i+1],-V[i]);
      //Serial.println((String)(2*i+1) + ":" + -V[i]);
    }
  }

  digitalWrite(36,LOW);
}

//回転する（角度）
void move_rotate(float Theta) {
  float V[4]; //モーターごとの動かす量
  if(fabsf(Theta - rotate) > rot_ign && fabsf(Theta) < 100){
    if (rotate < Theta){  //正転
      while(rotate < Theta){
        for(int i = 0; i < 4; i++){
          analogWrite(motorPin[2*i], motor_PWM * motor_character[i]);
          analogWrite(motorPin[2*i+1],0);
          Serial.println((String)(2*i) + ":" + V[i]);
        }
        gyro();
      }
    }else{  //逆転
      while(rotate > Theta){
        for(int i = 0; i < 4; i++){
          analogWrite(motorPin[2*i],  0);
          analogWrite(motorPin[2*i+1], motor_PWM * motor_character[i]);
          Serial.println((String)(2*i+1) + ":" + V[i]);
        }
        gyro();
      }
    }
    move_stop();
  }
}

//止まる
void move_stop(){
  for(int i=0; i<8; i++){
    analogWrite(motorPin[i], 255);
  }
}

//ドライバー出力をLOWにする
void move_off(){
    for(int i=0; i<8; i++){
    analogWrite(motorPin[i], 0);
  }
}

//赤外線センサ(ボール位置をballに代入)
void sen_IRball(){
  int rawdata[8] = {0};
  float IRdata[8] = {0};
  ball = {0,0,0,0};

  //読み取り(rawdata[]に代入)
  for(int i=0; i<8; i++){
    rawdata[i] = analogRead(IRpin[i]);
  }
  //値の範囲でで分ける(IRdata[]に代入)
  for(int i=0; i<8; i++){
    if(rawdata[i] <= IRlow[i]){
      IRdata[i] = 0;
    }else if(rawdata[i] > IRhigh[i]){
      IRdata[i] = range;
    }else{
      IRdata[i] = (int) (rawdata[i] - IRlow[i]) * (range) / (IRhigh[i] - IRlow[i])  + 1; //小数は切り捨てされる
    }
  }

  /*Serial.print((String)"rawdata   ");
  for(int i=0; i<8; i++){
    Serial.print((String)rawdata[i] + "," + "\t");
  }
  Serial.print("IRdata  ");
  for(int i=0; i<8; i++){
    Serial.print((String)"  " + IRdata[i]);
  }//*/
  //ボールがないときを判定
  if(stats.maximum(IRdata,8) == 0 || stats.CV(IRdata,8) <= 50){
    noball = true;
  }else{
    noball = false;
  }
  if(noball)Serial.print("  noball");
  Serial.print((String)"  変動係数" + stats.CV(IRdata,8));
  Serial.print((String)"  最大値"   + stats.maximum(IRdata,8));
  
  //ベクトルで角度を算出(ball.Tに代入)
  for(int i=0; i<8; i++){
    ball.X += IRdata[i] * IRlocate[i].X;
    ball.Y += IRdata[i] * IRlocate[i].Y;
  }
  //Serial.println((String)"ball.X " + ball.X + "  ball.Y " + ball.Y);
  XYtoRT(&ball);
  //Serial.print((String)"    ball.T " + ball.T);

  //回転を含める
  ball.T = ball.T + rotate;
  RTtoXY(&ball);
  /*Serial.print((String)"    rotate " + rotate);
  Serial.print((String)"    ball.T " + ball.T);
  Serial.println((String)"    ball.R " + ball.R);//*/

  //だいたいの距離を割り出す(ball.Rに代入)
  //sortArray(IRdata, 8);//IRdataを昇順で並び替え
  
}

//ボール補足センサ
bool ballCapture(){
  bool ifBall_capture = false;
  ifBall_capture = digitalRead(capturePin);
  return ifBall_capture;
}


//ラインデータ読み取り（ナノと通信）
float getData_line(){
  int lowerData = 0; //0~7のセンサの値を並べた2進数を10進数になおしたもの
  int upperData = 0; //8~15                  〃
  float back;

  Serial1.write(1); //ナノに送信
  Serial1.flush();
  while(!Serial1.available());
  do{
    lowerData = Serial1.read(); //帰ってきたデータ読み取り
  }while(lowerData == -1);
  Serial1.write(1); //ナノに送信
  Serial1.flush();
  while(!Serial1.available());
  do{
    upperData = Serial1.read(); //帰ってきたデータ読み取り
  }while(upperData == -1);

  Serial.println(lowerData);
  Serial.println(upperData);//*/

  //２進数に変えてデータ読み取り
  for(int i=0; i<8; i++){
    rawData[i] = lowerData % 2;
    rawData[i+8] = upperData % 2;
    lowerData = ( lowerData - rawData[i] ) / 2;
    upperData = ( upperData - rawData[i+8] ) / 2;
  }

  for(int i=0; i<16; i++){
    Serial.print(rawData[i]);
  }
  Serial.println();//*/

}

//ラインセンサ割り込み
void int_line(){
  noInterrupts();
  //モーターをブレーキ
  move_stop();
  //ラインあるなしフラグ
  ifLine = true;
  interrupts();
}

//超音波センサ読み取り
void echo() {
  double duration = 0;
  for(int i=0; i<4; i++){
    digitalWrite( echoPin[i][0], LOW ); 
    delayMicroseconds(2); 
    digitalWrite( echoPin[i][0], HIGH );
    delayMicroseconds( 10 ); 
    digitalWrite( echoPin[i][0], LOW );
    duration = pulseIn( echoPin[i][1], HIGH ); // 往復にかかった時間が返却される[マイクロ秒]
    //Serial.println(duration);

    if (duration) {
      duration = duration / 2; // 往路にかかった時間
      distance[i] = duration * (speed_of_sound + temp * 3 / 5) * 100 / 1000000;
    }else{
      distance[i] = 0;
    }

    Serial.println((String)i + "\t" + distance[i]);
  }
  Serial.println();
}


//ジャイロセンサ更新(rotateに代入)
void gyro(){
  if(!nogyro){
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    rotate = euler.x() - firstRotation; //はじめの値を引いておく
    //0が前、-180~180になるよう調整
    if(rotate >= 180){
      rotate = rotate - 360;
    }
    rotate = -rotate;
  }
  Serial.println((String) "方向" + rotate);
}

//持ち上げ確認
void lifted(){
  if(!nogyro){
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    if(euler.y() < -10){
      //持ち上げてるとき消灯、止まる
      digitalWrite(LED,LOW);
      move_stop();
      //下ろすまで待つ
      while(euler.y() < -10){
        //Serial.println("lifted");
        euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
      }
      //再開
      digitalWrite(LED,HIGH);
    }
  }
}
