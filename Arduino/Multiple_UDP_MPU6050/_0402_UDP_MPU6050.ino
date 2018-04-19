/*コード概要
 * 安定したUDP通信の実装と加速度センサで値をとってそれをサーバへ送る.
 * Macアドレスを送ることで、サーバ側でアドレスを認識する.
 * Macアドレスによる紐付けでサーバ側と通信することで複数台のセンサデータを高速で受信可能となる
 * MPU6050(加速度センサ)は同じセンサでもスケールが異なることがある。.以下データシート参照
 * 詳細 https://www.invensense.com/products/motion-tracking/6-axis/mpu-6050/
 */

#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

#include "wifiConfig.h"//SSID,PASSの設定を記述するライブラリ
//Wi-Fi設定
static WiFiUDP wifiUdp;
static const char *kRemoteIpadr = "172.20.10.15";
static const int kRmoteUdpPort = 8889;

//MPU-6050の設定
MPU6050 accelGyro;
int16_t ax, ay, az, aveAx, aveAy, aveAz; //Acclel
int16_t gx, gy, gz, aveGx, aveGy, aveGz; //Gyro
double ax_Out, ay_Out, az_Out, gx_Out, gy_Out, gz_Out; //output
String str_UDP_out; //UDPの出力用
byte mac_Addr[6];
char addr[20];

static void WiFi_setup()
{
  static const int kLocalPort = 7000;
  //macアドレスの取得
  WiFi.macAddress(mac_Addr);
  sprintf(addr,"%02x%02x%02x%02x%02x%02x",
  mac_Addr[0],
  mac_Addr[1],
  mac_Addr[2],
  mac_Addr[3],
  mac_Addr[4],
  mac_Addr[5]
  );
  WiFi.begin(kWifiSsid, kWifiPass);
  while ( WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  wifiUdp.begin(kLocalPort);
}

static void Serial_setup()
{
  Serial.begin(115200);
  Serial.println("");
}

static void Mpu6050_setup() {
  //MPU6050の初期設定
  Wire.begin();
  Serial.println("Initializing I2C devices...");
  accelGyro.initialize(); Serial.println("Testing device connections...");
  Serial.println(accelGyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

//生の値から使える値に変換する
static double getRealAccel(double acc) {
  acc = double (acc / 10) / 2048;//MPU6050のデータシートから計算
  return acc;
}
static double getRealGyro(double gyro) {
  gyro = double (gyro / 10) / 16.4;//MPU6050のデータシートから計算
  return gyro;
}

static void resetSum(){
  //加算平均の値の初期化
  aveAx = 0;
  aveAy = 0;
  aveAz = 0;
  aveGx = 0;
  aveGy = 0;
  aveGz = 0;
}

void getSensorAveraging(){
  for (int i = 0; i < 10; i++) {
    delay(6);
    accelGyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    aveAx += ax;
    aveAy += ay;
    aveAz += az;
    aveGx += gx;
    aveGy += gy;
    aveGz += gz;
  }
}
void setup() {
  Serial_setup();
  WiFi_setup();
  Mpu6050_setup();
}

void loop()
{ 
  //センサ関連の処理
  resetSum();
  getSensorAveraging();
  //生データから実値に変換する
  ax_Out = getRealAccel(aveAx);
  ay_Out = getRealAccel(aveAy);
  az_Out = getRealAccel(aveAz);
  gx_Out = getRealGyro(aveGx);
  gy_Out = getRealGyro(aveGy);
  gz_Out = getRealGyro(aveGz);
  
  str_UDP_out = String(addr) +  "," +  String(ax_Out) + "," + String(ay_Out) +  "," + String(az_Out) + "," + String(gx_Out) + "," + String(gy_Out) +  "," + String(gz_Out);
  //str_UDP_outに任意のセンサの値をStringにキャストして代入する

  //UDP通信
  wifiUdp.beginPacket(kRemoteIpadr, kRmoteUdpPort);
  wifiUdp.print(str_UDP_out);
  Serial.println(str_UDP_out);
  wifiUdp.endPacket();
}
