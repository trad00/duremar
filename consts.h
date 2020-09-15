#define VER "v.2020/09/16"
#define LCDDisplay


#define TX 1
#define RX 3

//зумер
#define BUZZER D0

//дисплей
//SCL, SDA - стандартные D1, D2
#define LCD_DSPL_ADDR 0x27
#define OLED_DSPL_ADDR 0x3C

//енкодер
#define ENC_BTN D3
//#define ENC_A D3
//#define ENC_B D3
#define ENC_A TX
#define ENC_B RX

//управление реле
//#define RELAY1 D5
//#define RELAY2 D6
//#define RELAY3 D7
//#define RELAY4 D8 //пока отключено
//#define REL_ON LOW
//#define REL_OFF HIGH

//управление сдвиговым регистром 74HC595 (управление реле)
#define DATA_PIN D5  // к выводу 14 регистра SD
#define CLOCK_PIN D7 // к выводу 11 регистра (SH_CP)
#define LATCH_PIN D6 // к выводу 12 регистра (ST_CP)

//датчики
#define ONE_WIRE_BUS D4
#define BMP_SCL_BUS D1
#define BMP_SDA_BUS D2
#define BMP_PRESSURE0 1013.25
#define DS18_RESOLUTION 11 
//The resolution of the temperature sensor is user-configurable to 9, 10, 11, or 12 bits,
//corresponding to increments of 0.5°C, 0.25°C, 0.125°C, and 0.0625°C, respectively. 

const float emptySignal = -127;

#define APSSID "duremar4"
#define APPSK  "duremar4" //len min 8
#define WWWUSER "duremar4"
#define WWWPWD  "duremar4"
