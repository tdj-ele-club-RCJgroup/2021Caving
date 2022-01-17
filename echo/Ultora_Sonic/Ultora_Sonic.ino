uint8_t echoPin[4][2] = {{47,49},{48,46},{44,42},{43,45}};
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
  delay(300);
}


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