//モーター
const uint8_t motorPin[8]         = {6,7,8,9,10,11,4,5};         //モーターの制御ピン


void setup() {
  //シリアル通信を9600bpsで始める
  //一応、テストなどの通信用
  Serial.begin(9600);

  //モーターのデジタルピン宣言
  for (int i=0; i<8; i++){
    pinMode(motorPin[i], OUTPUT);
  }
}

void loop() {
  analogWrite(motorPin[4],0);
  analogWrite(motorPin[5],255);
  
  /*delay(100);
  
  analogWrite(motorPin[0],200);
  analogWrite(motorPin[1],0);
  analogWrite(motorPin[4],0);
  analogWrite(motorPin[5],200);
  analogWrite(motorPin[6],250);
  analogWrite(motorPin[7],0);*/
}
