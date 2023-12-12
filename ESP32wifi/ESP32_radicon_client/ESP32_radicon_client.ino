#include "WiFi.h"
#include <WiFiUdp.h>

const char * ssid = "ESP32UDP";             // SSID
const char * password = "12345678";         // password

IPAddress ip(192, 168, 4, 2);               // IP Address
IPAddress gateway(192,168, 4, 1);           // Gateway Address
IPAddress subnet(255, 255, 255, 0);         // Subnet Address

IPAddress sv_ip(192, 168, 4, 1);            // Server IP Address

static const int Remote_Port = 9000;        // Destination port
static const int Org_Port = 9000;           // Source port

WiFiUDP UDP;

const int Left_X_PIN = 34;  // ジョイスティック左X軸方向をアナログピンA0に入力
const int Right_X_PIN = 35;  // ジョイスティック右X軸方向をアナログピンA1に入力
const int upPIN = 12;  // ジョイスティック右X軸方向をアナログピンA1に入力
const int downPIN = 14;  // ジョイスティック右X軸方向をアナログピンA1に入力
const int anglePIN = 27;  // ジョイスティック右X軸方向をアナログピンA1に入力
int up = 0;  // ジョイスティック右X軸方向をアナログピンA1に入力
int down = 0;  // ジョイスティック右X軸方向をアナログピンA1に入力
int angle = 7;  // ジョイスティック右X軸方向をアナログピンA1に入力
int Left_X_POS ;            // ジョイスティック左X軸方向の読み取り値の変数を整数型に
int Right_X_POS ;            // ジョイスティック右X軸方向の読み取り値の変数を整数型に
float OFFSET = 0.1;  // センター付近のオフセット値を5％に設定
int DUTY ;             // ディーティ比用変数
const int Dmax = 150;    // 最大のデューティ比

void setup()
{
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);

  // Try forever
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...Connecting to WiFi");
    delay(1000);
  }

  Serial.println("Connected");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  UDP.begin(Org_Port);
  Serial.println("Client OK");

  pinMode(Left_X_PIN, ANALOG);          // A0ピンを入力に(省略可)
  pinMode(Right_X_PIN, ANALOG);          // A1ピンを入力に(省略可)

  pinMode(upPIN, INPUT_PULLUP);
  pinMode(downPIN, INPUT_PULLUP);
  pinMode(anglePIN, INPUT_PULLUP);
}

char a = 'A';
void loop()
{
  Left_X_POS = analogRead(Left_X_PIN)+10;     // X軸方向のアナログ値を読み取る
  Right_X_POS = analogRead(Right_X_PIN)+10;     // Y軸方向のアナログ値を読み取る
  up = digitalRead(upPIN);
  down = digitalRead(downPIN);
  angle = digitalRead(anglePIN);

  Serial.println("Send : " + String(Left_X_POS) + "," + String(Right_X_POS));

 if (up+down+angle == 3){
  UDP.beginPacket(sv_ip, Remote_Port);
  // UDP.write(Left_X_POS);
  UDP.print(String(Left_X_POS) + "," + String(Right_X_POS));
  UDP.endPacket();
 }
  else{
  UDP.beginPacket(sv_ip, Remote_Port);
  // UDP.write(Left_X_POS);
  UDP.print(String(up) + "," + String(down) + "," + String(angle));
  UDP.endPacket();
 }

  a ++;
  if(a > 'F') a = 'A';

  delay(100);
}
