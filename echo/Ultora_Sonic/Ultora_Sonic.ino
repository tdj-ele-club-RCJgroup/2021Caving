uint8_t echoPin[4][2] = {{47,49},{48,46},{44,42},{43,45}};
const int echoSensorCharactor[4] = {59,59,58,58}; 
double distance[4] = {0};
double speed_of_sound = 331.5; // 0℃の気温の速度
double temp = 25;

void setup() {
  Serial.begin( 9600 );

  for(int i=0; i<4; i++){
    pinMode(echoPin[i][1], INPUT);
    pinMode(echoPin[i][0], OUTPUT);
  }
}

void loop(){
  echo();
  //delay(300);

  for(int i=0; i<4; i++){
    //0前　1左　2後ろ　3右
    Serial.println((String)i + "\t" + distance[i]);
  }
  Serial.println((String)"横" + "\t" + (distance[1]+distance[3]) );
  Serial.println((String)"縦" + "\t" + (distance[0]+distance[2]) );
  
  Serial.println();
}


void echo() {
  double duration = 0;
  for(int i=0; i<4; i++){
    digitalWrite( echoPin[i][0], LOW ); 
    delayMicroseconds(2); 
    digitalWrite( echoPin[i][0], HIGH );
    delayMicroseconds( 10 ); 
    digitalWrite( echoPin[i][0], LOW );
    duration = pulseIn( echoPin[i][1], HIGH ,12000 ); // 往復にかかった時間が返却される[マイクロ秒]
    //Serial.println(duration);

    if (duration) {
      duration = duration / 2; // 往路にかかった時間
      distance[i] = duration * (speed_of_sound + temp * 3 / 5) * 100 / 1000000;
    }else{
      distance[i] = 0;
    }
  }
}
