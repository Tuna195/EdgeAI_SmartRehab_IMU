#include <Arduino.h>
#include <Wire.h>
#include "SparkFun_BMI270_Arduino_Library.h"

// Thư viện tải về trên Edge AI
#include <Smart-Rehab-Tracker_inferencing.h>


BMI270 imu;
unsigned long previousMillis = 0;

// Mảng chứa dữ liệu thô đẩy vào AI (Kích thước tự động tính dựa trên Window Size)
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
int feature_ix = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin(D4, D5);
    delay(100);

    if (imu.beginI2C(0x68) != BMI2_OK) {
        Serial.println("Loi BMI270!");
        while(1);
    }
    Serial.println("He thong AI da san sang. Hay vung tay!");
}

void loop() {
    unsigned long currentMillis = millis();

    // Vòng lặp 50Hz thu thập dữ liệu
    if (currentMillis - previousMillis >= 20) {
        previousMillis = currentMillis;
        imu.getSensorData();

        // Nhồi 6 trục vào mảng features
        features[feature_ix++] = imu.data.accelX;
        features[feature_ix++] = imu.data.accelY;
        features[feature_ix++] = imu.data.accelZ;
        features[feature_ix++] = imu.data.gyroX;
        features[feature_ix++] = imu.data.gyroY;
        features[feature_ix++] = imu.data.gyroZ;

        // Khi mảng đã đầy (thu thập đủ 2 giây dữ liệu) -> Kích hoạt AI dự đoán
        if (feature_ix == EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
            
            // Ép kiểu mảng dữ liệu cho Edge Impulse hiểu
            signal_t signal;
            int err = numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
            
            ei_impulse_result_t result = { 0 };
            // Chạy hàm phân loại (Bộ não hoạt động ở đây)
            err = run_classifier(&signal, &result, false);

            // In kết quả ra màn hình Terminal
            Serial.println("=====================================");
            Serial.print("Dự đoán Bicep Curl: ");
            Serial.print(result.classification[0].value * 100, 2); // Nhân 100 để ra phần trăm
            Serial.println(" %");
            
            Serial.print("Dự đoán Idle: ");
            Serial.print(result.classification[1].value * 100, 2);
            Serial.println(" %");
            Serial.println("=====================================");

            // Reset biến đếm để thu thập lại 2 giây tiếp theo
            feature_ix = 0;
        }
    }
}