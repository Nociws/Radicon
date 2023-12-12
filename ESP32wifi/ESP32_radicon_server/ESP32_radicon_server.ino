#include <WiFiUdp.h>
#include "WiFi.h"
// #include <Servo.h>

const char * ssid = "ESP32UDP";             // SSID
const char * password = "12345678";         // password

static const int Remote_Port = 9000;        // Destination port
static const int Org_Port = 9000;           // Source port

WiFiUDP UDP;

// PWM出力関係
const int Left_Xmin = 32;
//const int Left_Xmax = ;
 const int Right_Xmin = 27;
// const int Right_Xmax = 16;
// const int Arm_Angle = 2;
const int dir1 = 33; // 回転方向指定用ピン
const int dir2 = 14; // 回転方向指定用ピン
// Servo myservo; //Servoオブジェクトを作成
// const int upPIN = 32;   // アーム上昇
// const int downPIN = 33;   // アーム下降
const int PWMCH1 = 0;
const int PWMCH2 = 1;
// const int PWMCH3 = 2;
// const int PWMCH4 = 3;
// const int PWMCH5 = 4;
int Left_X_POS =4320;            // ジョイスティック左X軸方向の読み取り値の変数を整数型に
int Right_X_POS =4320;            // ジョイスティック右X軸方向の読み取り値の変数を整数型に
float OFFSET = 0.2;  // センター付近のオフセット値を5％に設定
int DUTY ;             // ディーティ比用変数
const int Dmax = 75;    // 最大のデューティ比（最大値は256だがDmaxを256にすると出力が最大257になってしまう。要改善。）
const int pwmfrec = 100;   // pwm周波数（Hz）
const int stick_center = 2048; // ESP32 = 2048
const int stick_end = 4095; // ESP32 = 4095
int up = 0; // ESP32 = 4095
int down = 0; // ESP32 = 4095
int angle = 0; // ESP32 = 4095

void setup()
{
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  delay(1000);

  Serial.print("AP IP address: ");            //    192.168.4.1
  Serial.println(WiFi.softAPIP());

  UDP.begin(Org_Port);
  Serial.println("Server OK");

  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);

  // PWM出力関係
  ledcSetup(PWMCH1, pwmfrec, 8);
  ledcSetup(PWMCH2, pwmfrec, 8);

  ledcAttachPin(Left_Xmin, PWMCH1);
  ledcAttachPin(Right_Xmin, PWMCH2);
  // ledcAttachPin(Right_Xmin, PWMCH3);
  // ledcAttachPin(Right_Xmax, PWMCH4);
  // ledcAttachPin(Arm_Angle, PWMCH5);
  // myservo.attach(Arm_Angle); //13番ピンにサーボ制御線（オレンジ）を接続
}

void loop()
{
  char rv_data[10] = {0,0,0,0,0,0,0,0,0,0};
  // int test = 0;
  // String rv_data_st = "";

  if (UDP.parsePacket() > 0)
    {
      UDP.read(rv_data , 10);
      // Serial.print("Recieve : ");
      // Serial.println(rv_data);
      UDP.flush();
      // Serial.println(String(rv_data));
      // Serial.println(String(rv_data).toInt());

      int rv_data_list[5];
      stringToIntValues(String(rv_data), rv_data_list, ',');

      if(rv_data_list[0]+rv_data_list[1]>10 ){
        Left_X_POS = rv_data_list[0];            // ジョイスティック左X軸方向の読み取り値の変数を整数型に
        Right_X_POS = rv_data_list[1];            // ジョイスティック右X軸方向の読み取り値の変数を整数型に

        // PWM出力関係
        if((Left_X_POS > stick_center-stick_center*OFFSET) && (Left_X_POS < stick_center+stick_center*OFFSET)){  // X軸方向のセンター(stick_center)の前後OFFSET分は無視
          DUTY = 0;
          ledcWrite(PWMCH1, 0);
          
        }
        if((Right_X_POS > stick_center-stick_center*OFFSET) && (Right_X_POS < stick_center+stick_center*OFFSET)){  // Y軸方向のセンター(stick_center)の前後OFFSET分は無視
          DUTY = 0;
          ledcWrite(PWMCH2, 0);

        }
        if(Left_X_POS <= stick_center-stick_center*OFFSET){                      // スティックを左に倒したときの動作
          DUTY = map(Left_X_POS, stick_center-stick_center*OFFSET, 0, 0, Dmax);       // デューティー比を0~255の範囲に調整
          ledcWrite(PWMCH1, DUTY);
          digitalWrite(dir1, LOW);
        }
        else if(Left_X_POS >= stick_center+stick_center*OFFSET){                 // スティックを下に倒したときの動作
          DUTY = map(Left_X_POS, stick_center+stick_center*OFFSET, stick_end, 0, Dmax);    // デューティー比を0~255の範囲に調整
          ledcWrite(PWMCH1, DUTY);
          digitalWrite(dir1, HIGH);
        }
        if(Right_X_POS <= stick_center-stick_center*OFFSET){                      // スティックを上に倒したときの動作
          DUTY = map(Right_X_POS, stick_center-stick_center*OFFSET, 0, 0, Dmax);       // デューティー比を0~255の範囲に調整
          ledcWrite(PWMCH2, DUTY);
          digitalWrite(dir2, LOW);
        }
        else if(Right_X_POS >= stick_center+stick_center*OFFSET){                 // スティックを下に倒したときの動作
          DUTY = map(Right_X_POS,stick_center+stick_center*OFFSET,stick_end,0,Dmax);    // デューティー比を0~255の範囲に調整
          ledcWrite(PWMCH2, DUTY);
          digitalWrite(dir2, HIGH);
        }

        Serial.println(String(Left_X_POS)+","+String(Right_X_POS)+","+String(DUTY));
        // delay( 50 );

        // ledcWrite(PWMCH1, 1);
        // ledcWrite(PWMCH2, 1);
        // ledcWrite(PWMCH3, 1);
        // ledcWrite(PWMCH4, 1);
        // delay( 10 );
      }

    }
}


// String型のカンマ区切り文字列をint型配列に分解する関数
void stringToIntValues(String str, int value[], char delim) {
  int k = 0;
  int j = 0;
  char text[8];

  for (int i = 0; i <= str.length(); i++) {
    char c = str.charAt(i);
    if ( c == delim || i == str.length() ) {
      text[k] = '\0';
      value[j] = atoi(text);
      j++;
      k = 0;
    } else {
      text[k] = c;
      k++;
    }
  }
}