//モーター
const uint8_t motorPin[8]         = {5,6,7,8,10,11,12,13};         //モーターの制御ピン


void setup() {
  //シリアル通信を9600bpsで始める
  //一応、テストなどの通信用
  Serial.begin(9600);

  //モーターのデジタルピン宣言
  for (int i=0; i<6; i++){
    pinMode(motorPin[i], OUTPUT);
  }
}

void loop() {
    for(int i=0; i<8; i=i+2){
        analogWrite(motorPin[i],0);
        analogWrite(motorPin[i+1],255);
        
    }
}
