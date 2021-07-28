/* ノックに関する設定 */
#define KNOCK_LIMIT 4 // ノックの回数 間の秒数配列は-1
#define TIME_OUT 2000 // タイムアウト時間 ms
#define LONG_THRE 300 // 長いノックを判定するしきい値 ms
#define KNOCK_DELAY 140 // チャタリング防止Delay ms
#define ACC_THRE_BIAS 0.05 // ノック感度の調整

/* 使用するデバイス設定 */
#define M5STICKCPLUS 0 // M5Stick C Plus
#define M5STACKCORE2 1 // M5Stack Core2
#define DEVICE       0 // 使用するデバイスの数字を上から選んで記入してください

/* 表示設定 */
const String device_str[8] = {
    "TV (POWER)",
    "TV (CH UP)",
    "TV (CH DOWN)",
    "TV (VOL UP)",
    "TV (VOL DOWN)",
    "AIRCON (COOL)",
    "AIRCON (HOT)",
    "AIRCON (STOP)"
};

const String pattern_str[8] = {
    ". . . .",
    ". . _ .",
    ". _ . .",
    ". _ _ .",
    "_ . . .",
    "_ . _ .",
    "_ _ . .",
    "_ _ _ ."
};

/* リモコン信号 */
unsigned int tv_addr = 0x4004;
unsigned long data_tv_power = 0x100BCBD;
unsigned long data_tv_ch_up = 0x1002C2D;
unsigned long data_tv_ch_down = 0x100ACAD;
unsigned long data_tv_vol_up = 0x1000405;
unsigned long data_tv_vol_down = 0x1008485;

uint8_t data_ac_cool[27] = { // AC冷房24.5度
    0x02,
    0x20,
    0xE0,
    0x04,
    0x00,
    0x00,
    0x00,
    0x06,
    0x02,
    0x20,
    0xE0,
    0x04,
    0x00,
    0x39,
    0x30,
    0x80,
    0xA1,
    0x09,
    0x00,
    0x0E,
    0xE0,
    0x00,
    0x80,
    0x8D,
    0x74,
    0x04,
    0x0C
};

uint8_t data_ac_heat[27] = { // AC暖房26.0度
    0x02,
    0x20,
    0xE0,
    0x04,
    0x00,
    0x00,
    0x00,
    0x06,
    0x02,
    0x20,
    0xE0,
    0x04,
    0x00,
    0x49,
    0x34,
    0x80,
    0xA1,
    0x09,
    0x00,
    0x0E,
    0xE0,
    0x00,
    0x80,
    0x8D,
    0x74,
    0x04,
    0x20
};

uint8_t data_ac_stop[27] = { // AC停止
    0x02,
    0x20,
    0xE0,
    0x04,
    0x00,
    0x00,
    0x00,
    0x06,
    0x02,
    0x20,
    0xE0,
    0x04,
    0x00,
    0x38,
    0x30,
    0x80,
    0xA1,
    0x09,
    0x00,
    0x0E,
    0xE0,
    0x00,
    0x80,
    0x8D,
    0x74,
    0x04,
    0x0B
};