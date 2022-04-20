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
const uint8_t motorPin[8]         = {7,6,9,8,11,10,5,4};         //モーターの制御ピン
const float   motor_[4]           = {0, 0, 0, 0}; //モーターの中心からの距離[cm]
const float   motor_character[4]  = {1.000, 1.000, 1.000, 1.000}; //モーターの誤差補正
int   motor_PWM         = 255;  //0~255 出力するpwmの最大値
int   motor_rotatePWM   = 100;  //回転時の速度
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
bool noball;
void sen_IRball();  //赤外線センサ(ボール位置をballに代入)

//ボール補足センサ
#define capturePin 3
#define capture_rate 300
bool ballCapture();

//ラインセンサ
#define LED 12
#define intPin_line 3
int rawData[16] = {0};
volatile bool ifLine; //ラインあるなしフラグ
bool ifLine_process; //ライン処理実行中フラグ
void sen_line(); //ライン処理
float getData_line(); //ラインデータ読み取り（ナノと通信）

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
  //センサのLEDを発光
  pinMode(LED, OUTPUT);
  digitalWrite(LED,HIGH);

  //ジャイロセンサ開始
  if(!bno.begin()){
    Serial.print("No gyro");
    nogyro = true;
  };
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  firstRotation = euler.x();
}
  
void loop() {
  Serial.println(); 
  Serial.println();

  lifted();//持ち上げ確認
  sen_line();//ライン処理
  gyro();//ジャイロ更新
  sen_line();//ライン処理
  sen_IRball();//赤外線更新

  noball = false;
  ball.T = -60;
  /*if(fabsf(rotate) > 25){
    //回りすぎもどす
    move_rotate(0);
  }//*/
  if(ballCapture()){
    Serial.println("ボール補足");
    move_stop();
  }else if(!noball){
    //digitalWrite(36,HIGH);
    Serial.println((String)"ボール追いかけ" + ball.T * 1.5);
    move_robot(ball.T * 3 / 2);
  }else{
    //ボール失ったら超音波でもとの位置にもどる
  }
  
  digitalWrite(13, HIGH);
  delay(1);
  digitalWrite(13, LOW);
  /*delay(1000);//*/
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
  Serial.println((String)"モータ出力");
  
  //ラインが反応すると動かない
  if(ifLine && !ifLine_process){
    Serial.println((String)"ライン上禁止");
    Serial.println();
    return; 
  }
  //digitalWrite(36,HIGH);

  Coordinate motor_mov; //座標軸を45度回転した後の座標を後で格納
  float V[4]; //モーターごとの動かす量

  //回転を機体からみたものに戻す
  Theta = Theta - rotate;//*/
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
  move_off();
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
  //digitalWrite(36,LOW);
  Serial.println();
}

//回転する（角度）
void move_rotate(float Theta) {
  Serial.println((String)"回転");
  if(fabsf(Theta - rotate) > rot_ign && fabsf(Theta) < 100){
    if (rotate < Theta){  //正転
      //while(rotate < Theta){
        for(int i = 0; i < 4; i++){
          analogWrite(motorPin[2*i], motor_rotatePWM * motor_character[i]);
          analogWrite(motorPin[2*i+1],0);
          Serial.println((String)(2*i) + ":" + (motor_rotatePWM * motor_character[i]));
        }
        gyro();
      //}
    }else{  //逆転
      //while(rotate > Theta){
        for(int i = 0; i < 4; i++){
          analogWrite(motorPin[2*i],  0);
          analogWrite(motorPin[2*i+1], motor_rotatePWM * motor_character[i]);
          Serial.println((String)(2*i+1) + ":" + (motor_rotatePWM * motor_character[i]));
        }
        gyro();
      //}
    }
    //move_stop();
  }
  Serial.println();
}

//止まる
void move_stop(){
  Serial.println((String)"止まる");
  for(int i=0; i<8; i++){
    analogWrite(motorPin[i], 255);
  }
  Serial.println();
}

//ドライバー出力をLOWにする
void move_off(){
  Serial.println((String)"ドライバー出力をLOW");
  for(int i=0; i<8; i++){
    analogWrite(motorPin[i], 0);
  }
  Serial.println();
}

//赤外線センサ(ボール位置をballに代入)
void sen_IRball(){
  Serial.println((String)"赤外線センサ");
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
  Serial.print((String)"変動係数" + stats.CV(IRdata,8));
  Serial.println((String)"  最大値"   + stats.maximum(IRdata,8));
  
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
  //Serial.print((String)"    rotate " + rotate);
  Serial.println((String)"ball.T " + ball.T);
  //Serial.println((String)"    ball.R " + ball.R);//*/

  //だいたいの距離を割り出す(ball.Rに代入)
  //sortArray(IRdata, 8);//IRdataを昇順で並び替え
  Serial.println();
}

//ボール補足センサ
bool ballCapture(){
  Serial.println((String)"ボール補足センサ");
  int cap_rawData;
  cap_rawData = analogRead(capturePin);
  Serial.println((String)"rawData" + cap_rawData);
  if(cap_rawData > capture_rate){
    Serial.println("ball");
    Serial.println();
    return 1;
  }else{
    Serial.println("noball");
    Serial.println();
    return 0;
  }
}

//ライン処理
void sen_line(){
  Serial.println((String)"ライン処理");
  float back = 0;
  back = getData_line();
  if(!ifLine){
    Serial.println("noline");
  }else{
    digitalWrite(37,HIGH);
    ifLine_process = true;
    Serial.println("line");
    move_off();
    move_robot(back);
    ifLine_process = false;
    digitalWrite(37,LOW);
  }
  Serial.println();
}

//ラインデータ読み取り（ナノと通信）
float getData_line(){
  Serial.println((String)"ラインデータ読み取り");
  int lineNum = 0;
  ifLine = false;
  Coordinate back = {0};
  //float back_t = 0;

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
  Serial.println((String)"ifline" + "\t" + ifLine);

  while (Serial1.available()) Serial1.read(); //バッファを消す
  Serial1.write(1);
  Serial1.flush();
  if(ifLine){
    for(int i=0; i<4;){
      if(Serial1.available()){
        *((byte*)&back.T + i++) = Serial1.read();
        //Serial.println("here");
      }
    }
    Serial1.write(1);
    Serial1.flush();//*/
  }
  Serial.println((String)"back" + "\t" + back.T);
  //回転を含める
  back.T = back.T + rotate;
  RTtoXY(&back);
  return back.T;
}

/*/ラインセンサ割り込み
void int_line(){
  //ラインあるなしフラグ
  ifLine = true;
  move_off();
  digitalWrite(36,HIGH);
}//*/

//ジャイロセンサ更新(rotateに代入)
void gyro(){
  Serial.println((String)"ジャイロセンサ更新");
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
  Serial.println();
}

//持ち上げ確認
void lifted(){
  Serial.println((String)"持ち上げ確認");
  if(!nogyro){
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    if(euler.z() < -19){
      //持ち上げてるとき消灯、止まる
      digitalWrite(LED,LOW);
      move_stop();
      //下ろすまで待つ
      while(euler.z() < -19){
        Serial.println("lifted");
        euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
      }
      //再開
      digitalWrite(LED,HIGH);
    }
  }
  Serial.println();
}
