#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_BMI270_Arduino_Library.h"

BMI270 imu;

// --- CẤU HÌNH TẦN SỐ LẤY MẪU ---
const unsigned long SAMPLING_PERIOD_MS = 20; // 20ms = 50Hz
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  unsigned long t = millis();
  while (!Serial && (millis() - t < 3000));
  
  Wire.begin(D4, D5);
  delay(100); 

  if (imu.beginI2C(0x68) != BMI2_OK) {
      Serial.println("LOI: Khong the khoi tao BMI270.");
      while(1);
  }
  
  // Dòng Header cực kỳ quan trọng để lưu file CSV sau này
  Serial.println("timestamp,ax,ay,az,gx,gy,gz"); 
}

void loop() {
  unsigned long currentMillis = millis();

  // Chỉ thực thi khi đã trôi qua ĐÚNG 20 mili-giây
  if (currentMillis - previousMillis >= SAMPLING_PERIOD_MS) {
      // Cập nhật lại mốc thời gian ngay lập tức
      previousMillis = currentMillis;

      // 1. Lấy dữ liệu
      imu.getSensorData();

      // 2. In ra Serial (Đã thêm cột timestamp ở đầu để kiểm chứng tần số)
      Serial.print(currentMillis); Serial.print(",");
      
      Serial.print(imu.data.accelX, 3); Serial.print(",");
      Serial.print(imu.data.accelY, 3); Serial.print(",");
      Serial.print(imu.data.accelZ, 3); Serial.print(",");
      
      Serial.print(imu.data.gyroX, 3); Serial.print(",");
      Serial.print(imu.data.gyroY, 3); Serial.print(",");
      Serial.println(imu.data.gyroZ, 3);
  }
  
  // Vi điều khiển ESP32-S3 được rảnh rang ở đây để làm việc khác
}