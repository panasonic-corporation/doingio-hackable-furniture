#include <IRsend.h>
#include "Config.h"

#if DEVICE == M5STICKCPLUS
    #include <M5StickCPlus.h>
    IRsend irsend(9);
#elif DEVICE == M5STACKCORE2
    #include <M5Core2.h>
    #include <AXP192.h>
    AXP192 power;
    IRsend irsend(32);
#endif


unsigned long tm = 0;
unsigned long last_tm = 0;
unsigned long tm_list[KNOCK_LIMIT - 1];
int knock_count = 0;
float accX, accY, accZ = 0.f;
float acc_thre = 0.f;
float acc_sum = 0.f;

TFT_eSprite Disbuff = TFT_eSprite(&M5.Lcd);

void irCommand(uint8_t pattern) {
    switch (pattern) {
    case 0 : // TV POWER
        irsend.sendPanasonic(tv_addr, data_tv_power);
        break;
    case 1 : // TV CH UP
        irsend.sendPanasonic(tv_addr, data_tv_ch_up);
        break;
    case 2 : // TV CH DOWN
        irsend.sendPanasonic(tv_addr, data_tv_ch_down);
        break;
    case 3 : // TV VOL UP
        irsend.sendPanasonic(tv_addr, data_tv_vol_up);
        break;
    case 4 : // TV VOL DOWN
        irsend.sendPanasonic(tv_addr, data_tv_vol_down);
        break;
    case 5 : // AIRCON COOL
        irsend.sendPanasonicAC(data_ac_cool);
        break;
    case 6 : // AIRCON HEAT
        irsend.sendPanasonicAC(data_ac_heat);
        break;
    case 7 : // AIRCON STOP
        irsend.sendPanasonicAC(data_ac_stop);
        break;
    default:
        break;
    }
}

void caribration() {
    Disbuff.deleteSprite();
    Disbuff.createSprite(240, 135);
    Disbuff.fillSprite(TFT_DARKGREEN);
    Disbuff.setTextColor(TFT_WHITE);
    Disbuff.drawString("Plaese wait...", 40, 50, 4);
    Disbuff.pushSprite(0, 0);
    delay(2000);
    Disbuff.fillSprite(TFT_DARKGREEN);
    Disbuff.drawString("Caribration...", 40, 50, 4);
    Disbuff.pushSprite(0, 0);
    float max = 0.f;
    for (int i = 0; i < 300; i++) {
        M5.IMU.getAccelData(&accX, &accY, &accZ);
        float sum = abs(accX) + abs(accY) + abs(accZ);
        if (sum > max) {
            max = sum;
        }
        delay(10);
    }
    acc_thre = max + ACC_THRE_BIAS;
    Disbuff.deleteSprite();
    drawMain();
}

bool getKnock() {
    M5.IMU.getAccelData(&accX, &accY, &accZ);
    acc_sum = abs(accX) + abs(accY) + abs(accZ);
    drawGauge(); // ゲージ表示

    if (acc_sum > acc_thre) {
        return true;
    }
    return false;
}

void drawGauge() {
    Disbuff.deleteSprite();
    Disbuff.createSprite(40, 135);
    Disbuff.fillRect(0, 0, 40, 135, TFT_WHITE);
    if (acc_sum > acc_thre) {
        Disbuff.fillRect(0, 135 - (acc_sum / (acc_thre * 2) * 135), 40, 135, TFT_RED);
        #if DEVICE == M5STICKCPLUS
        M5.Beep.tone(8300);
        #endif
    } else {
        Disbuff.fillRect(0, 135 - (acc_sum / (acc_thre * 2) * 135), 40, 135, TFT_GREEN);
        #if DEVICE == M5STICKCPLUS
        M5.Beep.mute();
        #endif
    }
    Disbuff.fillRect(0, 67, 40, 1, TFT_BLACK);
    Disbuff.setTextColor(TFT_BLACK);
    Disbuff.drawString(String(acc_sum), 0, 0, 2);
    Disbuff.pushSprite(200, 0);
}

void drawResult(uint8_t pattern) {
    Disbuff.fillSprite(TFT_CYAN);
    Disbuff.setTextColor(TFT_BLACK);
    Disbuff.drawString(device_str[pattern], 30, 50, 2);
    Disbuff.pushSprite(0, 0);
}

void drawMain() {
    Disbuff.createSprite(200, 135);
    Disbuff.fillRect(0, 0, 200, 135, TFT_WHITE);

    Disbuff.setTextColor(TFT_RED);
    for (int i; i < sizeof(device_str) / sizeof(device_str[0]); i++) {
        Disbuff.drawString(device_str[i], 20, 5 + 15 * i, 2);
    }
    for (int i; i < sizeof(pattern_str) / sizeof(pattern_str[0]); i++) {
        Disbuff.drawString(pattern_str[i], 140, 15 * i, 2);
    }

    Disbuff.pushSprite(0, 0);
}

void setup() {
    M5.begin();
    M5.Lcd.setRotation(1);
    M5.IMU.Init();
    irsend.begin();
    caribration();

    tm = millis();
    last_tm = tm;

    Disbuff.deleteSprite();
    Disbuff.createSprite(240, 135);
    Disbuff.fillRect(0, 0, 240, 135, TFT_BLACK);
    Disbuff.pushSprite(0, 0);

    drawMain();
}

void loop() {
#if DEVICE == M5STICKCPLUS
    M5.update();
    if (M5.BtnB.wasPressed()) {
        caribration();
    }
#endif

    tm = millis();
    unsigned long elapsed_tm = tm - last_tm;

    if (getKnock()) {
        Serial.print(". ");
        Disbuff.deleteSprite();
        Disbuff.createSprite(200, 135);
        Disbuff.fillRect(0, 0, 200, 135, TFT_YELLOW);
        Disbuff.pushSprite(0, 0);
        delay(KNOCK_DELAY);
        if (knock_count != 0) {
            tm_list[knock_count - 1] = elapsed_tm;
        } else {
            knock_count++;
            last_tm = tm;
            return;
        }
        knock_count++;
        if (KNOCK_LIMIT <= knock_count) {
            Serial.println("");
            Serial.println("CORRECT");
            for (int i = 0; i < KNOCK_LIMIT - 1; i++) {
                Serial.print(tm_list[i]);
                Serial.print(" ");
            }
            Serial.println("");

            for (int i = 0; i < KNOCK_LIMIT - 1; i++) {
                if (tm_list[i] > LONG_THRE) {
                    Serial.print("-");
                } else {
                    Serial.print(".");
                }
            }

            Serial.println();
            Serial.print("0x");
            uint8_t pattern = 0;
            for (int i = 0; i < KNOCK_LIMIT - 1; i++) {
                if (tm_list[i] > LONG_THRE) {
                    Serial.print("1");
                    pattern++;
                } else {
                    Serial.print("0");
                }
                if (i != KNOCK_LIMIT - 2) {
                    pattern = pattern << 1;
                }
            }
            Serial.println();
            Serial.printf("%d\n", pattern);
            String pattern_str = String(pattern);
            irCommand(pattern);
            drawResult(pattern);

            Serial.println("");
            Serial.println("");
            knock_count = 0;
            delay(800);
            drawMain();
        }
        last_tm = tm;
    }
    if (knock_count > 0 && elapsed_tm > TIME_OUT) {
        Serial.println("TIME OUT");
        Disbuff.deleteSprite();
        Disbuff.createSprite(200, 135);
        Disbuff.fillSprite(TFT_RED);
        Disbuff.setTextColor(TFT_WHITE);
        Disbuff.drawString("TIME OUT", 10, 50, 4);
        Disbuff.pushSprite(0, 0);
        delay(300);
        Disbuff.createSprite(200, 135);
        Disbuff.fillSprite(TFT_BLUE);
        Disbuff.pushSprite(0, 0);
        knock_count = 0;
        drawMain();
    }
}