#define BEATTIME 500 //音を出している時間(msec)
#define SPEAKER 13 //スピーカーの出力ピン番号
#define sounds(hz) (440 * pow(1.0594630943592,hz)) //ドが-9
void setup() {
  /*tone(SPEAKER,sounds(-8),BEATTIME/2) ;
  delay(BEATTIME/2) ;
  tone(SPEAKER,sounds(-10),BEATTIME/2) ;
  delay(BEATTIME/2) ;
  tone(SPEAKER,sounds(-11),BEATTIME/2) ;
  delay(BEATTIME/2) ;
  tone(SPEAKER,sounds(-10),BEATTIME/2) ;
  delay(BEATTIME/2) ;
  tone(SPEAKER,sounds(-8),BEATTIME) ;
  delay(BEATTIME) ;
  tone(SPEAKER,sounds(-3),BEATTIME) ;
  delay(BEATTIME) ;
  tone(SPEAKER,sounds(-4),BEATTIME) ;
  delay(BEATTIME) ;
  tone(SPEAKER,sounds(-3),BEATTIME) ;
  delay(BEATTIME) ;
  tone(SPEAKER,sounds(-1),BEATTIME) ;
  delay(BEATTIME) ;
  tone(SPEAKER,sounds(-8),BEATTIME) ;
  delay(BEATTIME) ;
  tone(SPEAKER,sounds(2),BEATTIME*5/2) ;
  delay(BEATTIME*3/2) ;
  tone(SPEAKER,sounds(1),BEATTIME/2) ;
  delay(BEATTIME/2+1) ;
  tone(SPEAKER,sounds(1),BEATTIME*3) ;
  delay(BEATTIME*3) ;//shine*/
  tone(SPEAKER,sounds(0),BEATTIME) ;
  delay(BEATTIME) ;
  tone(SPEAKER,sounds(9),BEATTIME*2) ;
  delay(BEATTIME*2) ;
  tone(SPEAKER,sounds(7),BEATTIME) ;
  delay(BEATTIME) ;
  tone(SPEAKER,sounds(5),BEATTIME*3/4) ;
  delay(BEATTIME*3/4) ;
  tone(SPEAKER,sounds(9),BEATTIME/4) ;
  delay(BEATTIME/4) ;
  tone(SPEAKER,sounds(12),BEATTIME*2) ;
  delay(BEATTIME*3) ;//校歌*/
}
void loop() {
}
