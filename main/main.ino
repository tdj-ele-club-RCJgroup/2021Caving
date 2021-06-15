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

/*////////////////////////////
  ループ関数の開始
  /*////////////////////////////
  

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


