// Không được sử dụng với mục đích kiếm tiền!
// Quay lại tiktok ủng hộ cho mk 1 tim và 1 fl nha.
// CODE này vẫn đang còn trong giai đoạn phát triển thêm. Bạn nào có thêm ý tưởng hay chức năng gì mới thì gửi cho mình với. 

// Các thư viện cần thiết.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DabbleESP32.h>
#include <ESP32Servo.h>

#define CUSTOM_SETTINGS 
#define INCLUDE_GAMEPAD_MODULE
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

// Khai báo các chân. Kết nối với ESP32C3 Supper mini, các chân còn lại nối với GND.
#define LEFT_BUTTON  2  // Nút nhấn bên trái
#define RIGHT_BUTTON 1  // Nút nhấn bên phải
#define UP_BUTTON  4  // Nút nhấn lên
#define DOWN_BUTTON 0  // Nút nhấn xuống
#define ACTION_BUTTON 21  // Nút nhấn nhảy
#define BACK_BUTTON 3 // Nút quay lại
#define BUZZER_PIN 10 // Chân GPIO kết nối với Buzzer

// Servo setup. Khi nào rảnh thì mình thiết kế xe TANK và gắn cái này vào để làm bộ điều khiển các động cơ. 
// Các bạn không sử dụng có thể bỏ qua
Servo servos[4]; // Mảng chứa các đối tượng Servo
#define Servo_Motor1 5 // Có thể là điều khiển bánh xích bên trái
#define Servo_Motor2 6 // Có thể là điều khiển bánh xích bên phải
#define Servo_Motor3 7 // Có thể là điều khiển cho động cơ bắn đạn
#define Servo_Motor4 20 // // Có thể là điều khiển cho động cơ xoay nòng
int servoAngles[4] = {90, 90, 90, 90}; // Góc ban đầu của các servo

bool isBluetoothActive = false; // Trạng thái Bluetooth

#define PADDLE_HEIGHT 16
#define PADDLE_WIDTH 3
#define BALL_SIZE 16
#define CHICKEN_SIZE 16

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int gameState = 0; // 0: Menu, 1: Pong, 2: Chicken, 3: Dino_Run, 4: QRcode, 100: Calculator
int selectedGame = 1; // 1: Pong, 2: Chicken, 3: Dino_Run, 4: QRcode, 100: Calculator
int selectedMainMenu = 0; // 0: Game Menu, 1: QRcode Menu...
int mainMenuState = 0; // 0: Main Menu, 1: Game Menu, 2: QRcode Menu
int qrCodeSelection = 0; // 0: BIDV, 1: VCB, 2: SEPAY
int selectedMusic = 0; // Biến lưu bài nhạc được chọn
bool isPlayingMusic = false; // Trạng thái đang phát nhạc
int selectedServo = 0; // Biến lưu servo được chọn
int selectedMotor_Control = 1;

// Bitmap cho trò "Pong" và "Chicken"
const uint8_t bitmap16x16[] PROGMEM = {
    0b00000000, 0b00000100, 
    0b00000000, 0b00001010,
    0b00000000, 0b00010001,
    0b00000000, 0b00010001,
    0b00011111, 0b11110001,
    0b00101000, 0b00010001,
    0b01001000, 0b00001010,
    0b10001000, 0b00001110,
    0b10001000, 0b00001110,
    0b01001000, 0b00001010,
    0b00101000, 0b00010001,
    0b00011111, 0b11110001,
    0b00000000, 0b00010001,
    0b00000000, 0b00010001,
    0b00000000, 0b00001010,
    0b00000000, 0b00000100
};

// Bitmap cho trò "Dino_Run"
const uint8_t DinoRunbitmap16x16[] PROGMEM = {
    0b00000001, 0b10000000,
    0b00000010, 0b01000000,
    0b00000100, 0b00100000,
    0b00001000, 0b00010000,
    0b00001111, 0b11110000,
    0b00001000, 0b00010000,
    0b00001000, 0b00010000,
    0b00001000, 0b00010000,
    0b00001000, 0b00010000,
    0b00001000, 0b00010000,
    0b00001000, 0b00010000,
    0b00111100, 0b00111100,
    0b01000010, 0b01000010,
    0b10000001, 0b10000001,
    0b01000010, 0b01000010,
    0b00111100, 0b00111100
};

// Bitmap2 cho trò "Dino_Run"
const uint8_t DinoRunbitmap216x16[] PROGMEM = {
    0b00000000,0b01111110,
    0b00000000,0b11011111,
    0b00000000,0b11111111,
    0b01000000,0b11110000,
    0b11000000,0b11111110,
    0b11000001,0b11100000,
    0b11100001,0b11100000,
    0b11110011,0b11111100,
    0b01111111,0b11100100,
    0b01111111,0b11100000,
    0b00111111,0b11000000,
    0b00011111,0b10000000,
    0b00011001,0b00000000,
    0b00010001,0b11000000,
    0b00011000,0b00000000
};

// Bitmap3 cho trò "Dino_Run"
const uint8_t DinoRunbitmap316x16[] PROGMEM = {
    0b00000000,0b01111110,
    0b00000000,0b11011111,
    0b00000000,0b11111111,
    0b01000000,0b11110000,
    0b11000000,0b11111110,
    0b11000001,0b11100000,
    0b11100001,0b11100000,
    0b11110011,0b11111100,
    0b01111111,0b11100100,
    0b01111111,0b11100000,
    0b00111111,0b11000000,
    0b00011111,0b10000000,
    0b00010001,0b10000000,
    0b00011101,0b00000000,
    0b00000001,0b10000000
};

// Bitmap chướng ngại vật cho trò "Dino_Run"
const uint8_t FlyingDinosaur16x16[] PROGMEM = {
    0b00000011, 0b00000000,
    0b00000011, 0b10000000,
    0b00000011, 0b11000000,
    0b00000001, 0b11100000,
    0b00001101, 0b11110000,
    0b00011100, 0b11111000,
    0b00111100, 0b11111100,
    0b01101100, 0b11111100,
    0b11111101, 0b11111111,
    0b11111111, 0b11111111,
    0b00011111, 0b11111100,
    0b00001111, 0b11111100,
    0b00000111, 0b11111111,
    0b00000011, 0b11111111,
    0b00000001, 0b11111000,
    0b00000000, 0b11110000
};

// Bitmap chướng ngại vật cho trò "Dino_Run"
const uint8_t bitmap13x16[] PROGMEM = {
    0b00000001, 0b10000000,
    0b00000011, 0b11000000,
    0b00000011, 0b11000000,
    0b00000011, 0b11000100,
    0b00100011, 0b11001110,
    0b01110011, 0b11001110,
    0b01110011, 0b11001110,
    0b01110011, 0b11001110,
    0b01110011, 0b11001110,
    0b00111011, 0b11011100,
    0b00011111, 0b11111000,
    0b00001111, 0b11110000,
    0b00000011, 0b11000000,
    0b00000011, 0b11000000,
    0b00000011, 0b11000000,
    0b00000011, 0b11000000
};

// Bitmap cho trò "Đào Vàng"
const uint8_t Menbitmap16x16[] PROGMEM = {
  0b00000000,0b01111110,
  0b00000000,0b11011111,
  0b00000000,0b11111111,
  0b01000000,0b11110000,
  0b11000000,0b11111110,
  0b11000001,0b11100000,
  0b11100001,0b11100000,
  0b11110011,0b11111100,
  0b01111111,0b11100100,
  0b01111111,0b11100000,
  0b00111111,0b11000000,
  0b00011111,0b10000000,
  0b00011001,0b10000000,
  0b00010001,0b00000000,
  0b00011001,0b10000000
};

// Bitmap1 cho trò "Flappy_Bird"
const uint8_t Bird1bitmap16x12[] PROGMEM = {
    0b00000011, 0b11100000,
    0b00001100, 0b10010000,
    0b00010001, 0b00001000,
    0b01111001, 0b00010100,
    0b11000101, 0b00010100,
    0b10000010, 0b10000100,
    0b10000010, 0b01111110,
    0b01000100, 0b10000001,
    0b00111001, 0b01111110,
    0b00100000, 0b10000010,
    0b00011000, 0b01111100,
    0b00000111, 0b10000000
};

// Bitmap2 cho trò "Flappy_Bird"
const uint8_t Bird2bitmap16x12[] PROGMEM = {
    0b00100000, 0b00000000,
    0b01010000, 0b00000000,
    0b10001000, 0b00000000,
    0b10001111, 0b11111100,
    0b10001000, 0b00001010,
    0b01110000, 0b00001001,
    0b01110000, 0b00001001,
    0b10001000, 0b00001010,
    0b10001111, 0b11111100,
    0b10001000, 0b00000000,
    0b01010000, 0b00000000,
    0b00100000, 0b00000000
};

// Bitmap vật phẩm 1 cho trò "Crazy_Arcade"
const uint8_t VP1bitmap6x6[] PROGMEM = {
    0b00110000,
    0b01001000,
    0b10000100,
    0b10000100,
    0b01001000,
    0b00110000
};

// Bitmap vật phẩm 2 cho trò "Crazy_Arcade"
const uint8_t VP2bitmap6x6[] PROGMEM = {
    0b00000110,
    0b00111010,
    0b01111000,
    0b01111000,
    0b00110000,
    0b00000000
};

// Bitmap0 là quả bom cho trò "Crazy_Arcade"
const uint8_t boom0bitmap6x6[] PROGMEM = {
    0b00110000,
    0b01001000,
    0b10110100,
    0b10110100,
    0b01001000,
    0b00110000
};

// BitmapX là quả bom của kẻ địch cho trò "Crazy_Arcade"
const uint8_t boomXbitmap6x6[] PROGMEM = {
    0b00110000,
    0b01111000,
    0b11111100,
    0b11111100,
    0b01111000,
    0b00110000
};

// Bitmap1 là nhân vật hoặc kẻ địch cho trò "Crazy_Arcade"
const uint8_t boom1bitmap6x6[] PROGMEM = {
    0b01111000,
    0b11111100,
    0b10110100,
    0b11111100,
    0b01001000,
    0b01111000
};

// Bitmap2 là nhân vật hoặc kẻ địch cho trò "Crazy_Arcade"
const uint8_t boom2bitmap6x6[] PROGMEM = {
    0b11001100,
    0b11111100,
    0b10000100,
    0b11111100,
    0b11001100,
    0b01001000
};

// Bitmap3 là nhân vật hoặc kẻ địch cho trò "Crazy_Arcade"
const uint8_t boom3bitmap6x6[] PROGMEM = {
    0b01001000,
    0b11111100,
    0b10110100,
    0b11111100,
    0b10000100,
    0b01111000
};

// Bitmap4 là nhân vật hoặc kẻ địch cho trò "Crazy_Arcade"
const uint8_t boom4bitmap6x6[] PROGMEM = {
    0b01001000,
    0b11111100,
    0b10110100,
    0b11111100,
    0b01001000,
    0b01111000
};

// Bitmap5 là nhân vật hoặc kẻ địch cho trò "Crazy_Arcade"
const uint8_t boom5bitmap6x6[] PROGMEM = {
    0b01001000,
    0b11111100,
    0b10110100,
    0b11111100,
    0b00000000,
    0b01111000
};

// Bitmap6 là gạch phá được cho trò "Crazy_Arcade"
const uint8_t boom6bitmap6x6[] PROGMEM = {
    0b00000000,
    0b01001000,
    0b00110000,
    0b00110000,
    0b01001000,
    0b00000000
};

// Bitmap7 tường cứng cho trò "Crazy_Arcade"
const uint8_t boom7bitmap6x6[] PROGMEM = {
    0b00000000,
    0b01111000,
    0b01001000,
    0b01001000,
    0b01111000,
    0b00000000
};

// Bitmap QRcode SEPAY_BIDV, 1 là ô đen, 0 là ô trắng
const uint8_t SEPAYQRCODEbitmap40x37[] PROGMEM = {
    0b00111111, 0b11111111, 0b11111111, 0b11111111, 0b11111110,
    0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000010,
    0b00101111, 0b11100010, 0b00100010, 0b10111011, 0b11111010, // Dòng đầu tiên
    0b00101000, 0b00100100, 0b01000101, 0b00100010, 0b00001010,
    0b00101011, 0b10101001, 0b00010110, 0b10101010, 0b11101010,
    0b00101011, 0b10100100, 0b11001110, 0b00011010, 0b11101010,
    0b00101011, 0b10100100, 0b11001110, 0b01001010, 0b11101010,
    0b00101000, 0b00100011, 0b10111100, 0b10001010, 0b00001010,
    0b00101111, 0b11101010, 0b10101010, 0b10101011, 0b11111010,
    0b00100000, 0b00001001, 0b00010001, 0b00110110, 0b10000010,
    0b00101110, 0b11111000, 0b10001000, 0b10000010, 0b01100010,
    0b00100001, 0b01010101, 0b11011010, 0b00110111, 0b00001010,
    0b00101100, 0b11100011, 0b10111010, 0b10011100, 0b11101010,
    0b00100100, 0b11010110, 0b11101000, 0b00110001, 0b01011010,
    0b00101001, 0b00101011, 0b00111110, 0b00100110, 0b11100010,
    0b00101011, 0b10011011, 0b00110011, 0b11101011, 0b01110010,
    0b00100101, 0b11101000, 0b01000000, 0b11010010, 0b10101010,
    0b00101000, 0b00000111, 0b00000001, 0b01110101, 0b01010010,
    0b00100010, 0b10111110, 0b10000000, 0b11100010, 0b11111010,
    0b00101000, 0b01010001, 0b11000000, 0b10001011, 0b01111010,
    0b00101100, 0b10100111, 0b10000000, 0b01101010, 0b10001010,
    0b00100001, 0b00011110, 0b11100111, 0b11010111, 0b00101010,
    0b00101101, 0b01100111, 0b00111010, 0b01000000, 0b11001010,
    0b00100111, 0b01010101, 0b00100101, 0b11111011, 0b11010010,
    0b00101011, 0b11100000, 0b01011011, 0b10011100, 0b10101010,
    0b00100110, 0b01001111, 0b00011011, 0b11100101, 0b00010010,
    0b00101000, 0b01100100, 0b10010101, 0b10101111, 0b11101010,
    0b00100000, 0b00001111, 0b11001100, 0b01011000, 0b11010010,
    0b00101111, 0b11101101, 0b10100101, 0b00101010, 0b11101010,
    0b00101000, 0b00101110, 0b11110101, 0b11011000, 0b11000010,
    0b00101011, 0b10101101, 0b00100010, 0b00101111, 0b10111010,
    0b00101011, 0b10100001, 0b00101001, 0b01110100, 0b01010010,
    0b00101011, 0b10101000, 0b01001011, 0b10110110, 0b10101010,
    0b00101000, 0b00101101, 0b00010001, 0b01011101, 0b10000010,
    0b00101111, 0b11101100, 0b10011011, 0b10111110, 0b00101010, // Dòng cuối cùng
    0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000010,
    0b00111111, 0b11111111, 0b11111111, 0b11111111, 0b11111110
};

// Bitmap QRcode BIDV, tương tự.
const uint8_t BIDVQRCODEbitmap40x37[] PROGMEM = {
    0b00111111, 0b11111111, 0b11111111, 0b11111111, 0b11111110,
    0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000010,
    0b00101111, 0b11100011, 0b01001110, 0b11101011, 0b11111010,
    0b00101000, 0b00101000, 0b10110011, 0b00110010, 0b00001010,
    0b00101011, 0b10100011, 0b11110110, 0b00100010, 0b11101010,
    0b00101011, 0b10100100, 0b11101110, 0b00111010, 0b11101010,
    0b00101011, 0b10101111, 0b10101010, 0b10101010, 0b11101010,
    0b00101000, 0b00100110, 0b11011001, 0b01010010, 0b00001010,
    0b00101111, 0b11101010, 0b10101010, 0b10101011, 0b11111010,
    0b00100000, 0b00000101, 0b01010001, 0b01010000, 0b00000010,
    0b00101010, 0b10100111, 0b00000000, 0b10101000, 0b10010010,
    0b00100101, 0b11010100, 0b01000110, 0b11000100, 0b01010010,
    0b00101001, 0b00101100, 0b10111010, 0b00111010, 0b01010010,
    0b00100111, 0b01010110, 0b11101111, 0b00010100, 0b01011010,
    0b00100001, 0b11110001, 0b00111010, 0b10100010, 0b10000010,
    0b00101110, 0b11000100, 0b10111110, 0b01011101, 0b10110010,
    0b00101101, 0b10111011, 0b01011011, 0b10111010, 0b11101010,
    0b00100010, 0b01011100, 0b00101101, 0b01000100, 0b10010010,
    0b00101001, 0b01110010, 0b00110100, 0b01100111, 0b11101010,
    0b00101011, 0b01000111, 0b00010001, 0b00010001, 0b00000010,
    0b00101110, 0b01110100, 0b01110100, 0b10101100, 0b10111010,
    0b00101001, 0b00000100, 0b01101001, 0b00000101, 0b11000010,
    0b00100111, 0b00110011, 0b01010000, 0b10110001, 0b10100010,
    0b00100001, 0b11011101, 0b00010000, 0b10010001, 0b00010010,
    0b00101011, 0b11100011, 0b00000100, 0b00010011, 0b00001010,
    0b00100111, 0b10001001, 0b11010001, 0b01100001, 0b01000010,
    0b00101001, 0b10110110, 0b11001011, 0b01011111, 0b10111010,
    0b00100000, 0b00001001, 0b11100101, 0b01111000, 0b11100010,
    0b00101111, 0b11100010, 0b11110011, 0b01001010, 0b10010010,
    0b00101000, 0b00100010, 0b01010111, 0b01101000, 0b11000010,
    0b00101011, 0b10101110, 0b01111010, 0b10101111, 0b11001010,
    0b00101011, 0b10100110, 0b10011010, 0b01110100, 0b00010010,
    0b00101011, 0b10101000, 0b10011000, 0b00101011, 0b01101010,
    0b00101000, 0b00100111, 0b01101100, 0b11010011, 0b01000010,
    0b00101111, 0b11101010, 0b10011010, 0b01100111, 0b10001010,
    0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000010,
    0b00111111, 0b11111111, 0b11111111, 0b11111111, 0b11111110
};

// Bitmap chướng ngại vật 1 cho trò "Chicken"
const uint8_t chicken1Bitmap4x4[] PROGMEM = {
    0b1001,
    0b0110,
    0b0110,
    0b1001
};

// Bitmap chướng ngại vật 2 cho trò "Chicken"
const uint8_t chickenBit2map4x4[] PROGMEM = {
    0b1111,
    0b1001,
    0b1001,
    0b1111
};

// Bitmap số 1
const uint8_t Number1Bitmap7x9[] PROGMEM = {
    0b0000000,
    0b0001000,
    0b0011000,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0001000,
    0b0011100,
    0b0000000
};

// Bitmap số 2
const uint8_t Number2Bitmap7x9[] PROGMEM = {
    0b0000000,
    0b0011100,
    0b0100010,
    0b0000010,
    0b0011100,
    0b0100000,
    0b0100000,
    0b0111110,
    0b0000000
};

// Bitmap số 3
const uint8_t Number3Bitmap7x9[] PROGMEM = {
    0b0000000,
    0b0011100,
    0b0100010,
    0b0000010,
    0b0011100,
    0b0000010,
    0b0100010,
    0b0011100,
    0b0000000
};

// Bitmap số 4
const uint8_t Number4Bitmap7x9[] PROGMEM = {
    0b0000000,
    0b0000100,
    0b0001100,
    0b0010100,
    0b0100100,
    0b0111110,
    0b0000100,
    0b0000100,
    0b0000000
};

// Bitmap số 5
const uint8_t Number5Bitmap7x9[] PROGMEM = {
    0b0000000,
    0b0111110,
    0b0100000,
    0b0111100,
    0b0000010,
    0b0000010,
    0b0100010,
    0b0011100,
    0b0000000
};

// Bitmap số 6
const uint8_t Number6Bitmap7x9[] PROGMEM = {
    0b0000000,
    0b0001110,
    0b0010000,
    0b0100000,
    0b0111100,
    0b0100010,
    0b0100010,
    0b0011100,
    0b0000000
};

// Bitmap số 7
const uint8_t Number7Bitmap7x9[] PROGMEM = {
    0b0000000,
    0b0111110,
    0b0000010,
    0b0000010,
    0b0000100,
    0b0001000,
    0b0010000,
    0b0100000,
    0b0000000
};

// Bitmap số 8
const uint8_t Number8Bitmap7x9[] PROGMEM = {
    0b0000000,
    0b0011100,
    0b0100010,
    0b0100010,
    0b0011100,
    0b0100010,
    0b0100010,
    0b0011100,
    0b0000000
};

// Bitmap số 9
const uint8_t Number9Bitmap7x9[] PROGMEM = {
    0b0000000,
    0b0011100,
    0b0100010,
    0b0100010,
    0b0011110,
    0b0000010,
    0b0000100,
    0b0111000,
    0b0000000
};

// Bitmap số 0
const uint8_t Number0Bitmap7x9[] PROGMEM = {
    0b0000000,
    0b0011100,
    0b0100010,
    0b0100110,
    0b0101010,
    0b0110010,
    0b0100010,
    0b0011100,
    0b0000000
};

// Bitmap phép "+"
const uint8_t CongBitmap7x9[] PROGMEM = {
    0b0000000,
    0b0000000,
    0b0001000,
    0b0001000,
    0b0111110,
    0b0001000,
    0b0001000,
    0b0000000,
    0b0000000
};

// Bitmap phép "-"
const uint8_t TruBitmap7x9[] PROGMEM = {
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
    0b0111110,
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000
};

// Bitmap phép "*"
const uint8_t NhanBitmap7x9[] PROGMEM = {
    0b0000000,
    0b0000000,
    0b0100010,
    0b0010100,
    0b0001000,
    0b0010100,
    0b0100010,
    0b0000000,
    0b0000000
};

// Bitmap phép "/"
const uint8_t ChiaBitmap7x9[] PROGMEM = {
    0b0000000,
    0b0000000,
    0b0001000,
    0b0000000,
    0b0111110,
    0b0000000,
    0b0001000,
    0b0000000,
    0b0000000
};

// Bitmap dấu "="
const uint8_t BangBitmap7x9[] PROGMEM = {
    0b0000000,
    0b0000000,
    0b0000000,
    0b0111110,
    0b0000000,
    0b0111110,
    0b0000000,
    0b0000000,
    0b0000000
};

// Bitmap dấu "." (dấu phẩy)
const uint8_t ChamBitmap7x9[] PROGMEM = {
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
    0b0000000,
    0b0110000,
    0b0110000,
    0b0000000
};

// Bitmap phép xóa 1 ký tự khi chọn sai
const uint8_t XoaBitmap7x9[] PROGMEM = {
    0b0000111,
    0b0001011,
    0b0010011,
    0b0110111,
    0b1001011,
    0b0110111,
    0b0010011,
    0b0001011,
    0b0000111
};

void setup() {
  Wire.begin();
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(ACTION_BUTTON, INPUT_PULLUP);
  pinMode(BACK_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT); // Cấu hình chân Buzzer là OUTPUT
  digitalWrite(BUZZER_PIN, HIGH); // Đảm bảo Buzzer tắt ban đầu

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }
  display.clearDisplay();
  display.display();
}

void loop() {
    if (gameState == 0) {
        // Hiển thị menu
        drawMenu();

        // Nếu nhấn nút ACTION
        if (digitalRead(ACTION_BUTTON) == LOW) {
            if (mainMenuState == 0) {
                // Menu chính
                if (selectedMainMenu == 0) {
                    mainMenuState = 1; // Sang menu Game
                    selectedMainMenu = 0;  // Reset selectedGame
                } else if (selectedMainMenu == 1) {
                    mainMenuState = 2; // Sang menu QRcode
                    qrCodeSelection = 0; // Reset QR selection
                } else if (selectedMainMenu == 2) {
                    gameState = 100; // Chuyển sang Máy tính (gameState = 100)
                } else if (selectedMainMenu == 3) {
                    mainMenuState = 4; // Sang menu Music
                    selectedMusic = 0; // Reset bài nhạc được chọn
                } else if (selectedMainMenu == 4) { // Motor Control
                    mainMenuState = 5;
                    selectedMotor_Control = 0; // Reset chế độ Motor Control
                } else if (selectedMainMenu == 5) {
                    ESP.restart(); // Reset chương trình
                }
            } 
            else if (mainMenuState == 1) {
                // Chọn game để chơi
                gameState = selectedGame;
            } else if (mainMenuState == 2) {
                // Chọn QRcode để hiển thị
                displayQRCode(qrCodeSelection);
            } else if (mainMenuState == 4) {
                // Chọn bài nhạc để phát
                playMusic(selectedMusic); // Phát nhạc
            } else if (mainMenuState == 5) { // Menu Motor Control
                if (digitalRead(ACTION_BUTTON) == LOW) {
                    if (selectedMotor_Control == 0) { // Bluetooth
                        isBluetoothActive = true;
                        Dabble.begin("RC_Tank"); // Bật Bluetooth, làm bộ điều khiển từ xa kết nối với điện thoại
                        servos[0].attach(Servo_Motor1);
                        servos[1].attach(Servo_Motor2);
                        servos[2].attach(Servo_Motor3);
                        servos[3].attach(Servo_Motor4);
                    } else if (selectedMotor_Control == 1) { // BUTTON
                        isBluetoothActive = false; // Đảm bảo Bluetooth không hoạt động
                        servos[0].attach(Servo_Motor1);
                        servos[1].attach(Servo_Motor2);
                        servos[2].attach(Servo_Motor3);
                        servos[3].attach(Servo_Motor4);
                        mainMenuState = 6; // Sang menu BUTTON Control
                        selectedServo = 0; // Reset servo được chọn
                    }
                    delay(300); // Chống dội phím
                }
            }
            delay(300); // Chống dội phím
        }

        if (mainMenuState == 4) { // Menu Music
            if (digitalRead(ACTION_BUTTON) == LOW) {
                playMusic(selectedMusic); // Phát nhạc
                delay(300); // Chống dội phím
            }
            if (digitalRead(BACK_BUTTON) == LOW) {
                mainMenuState = 0; // Quay lại menu chính
                delay(300); // Chống dội phím
            }
        }
        if (mainMenuState == 6) { // Menu BUTTON Control
            if (digitalRead(ACTION_BUTTON) == LOW) {
                controlServo(selectedServo); // Điều khiển servo được chọn
                delay(300); // Chống dội phím
            }
            if (digitalRead(BACK_BUTTON) == LOW) {
                mainMenuState = 0; // Quay lại menu trước đó
                delay(300); // Chống dội phím
            }
        }

        if (isBluetoothActive) {
            Dabble.processInput(); // Xử lý dữ liệu từ Dabble

            // Điều khiển servo1 qua GAMEPAD
            if (GamePad.isUpPressed()) {
                servos[0].write(180); // Quay servo1 đến góc 180 độ khi nhấn giữ nút Up
            } else if (GamePad.isDownPressed()) {
                servos[0].write(0); // Quay servo1 đến góc 0 độ khi nhấn giữ nút Down
            } else {
                servos[0].write(90); // Quay servo1 về góc ban đầu (90 độ) khi nhả nút
            }

            if (GamePad.isStartPressed()) {
                servos[3].write(180); // Quay servo3 đến góc 180 độ khi nhấn giữ nút Start
            } else if (GamePad.isSelectPressed()) {
                servos[3].write(0); // Quay servo3 đến góc 0 độ khi nhấn giữ nút Select
            } else {
                servos[3].write(90); // Quay servo3 về góc ban đầu (90 độ) khi nhả nút
            }
            
            if (GamePad.isTrianglePressed()) {
                servos[1].write(servoAngles[1] += 2); // Quay servo2 tăng góc
            } else if (GamePad.isCrossPressed()) {
                servos[1].write(servoAngles[1] -= 2); // Quay servo2 giảm góc
            }
            
            if (GamePad.isLeftPressed()) {
                servos[2].write(servoAngles[2] += 2); // Quay servo3 tăng góc
            } else if (GamePad.isRightPressed()) {
                servos[2].write(servoAngles[2] -= 2); // Quay servo3 giảm góc
            }

            if (digitalRead(BACK_BUTTON) == LOW) {
                isBluetoothActive = false;
                mainMenuState = 5; // Quay lại menu chính
                delay(300); // Chống dội phím
            }
        }
    } 
    else if (gameState == 100) {
        // Máy tính đang chạy
        playCalculator();
        if (digitalRead(BACK_BUTTON) == LOW) {
            gameState = 0;
            mainMenuState = 0;
            delay(300);
        }
    }

    else {
        // Đang ở trong trò chơi
        if (digitalRead(BACK_BUTTON) == LOW) {
            gameState = 0; // Quay về màn hình chính
            mainMenuState = 1; // Quay lại menu Game
            delay(300);
            return;
        }

        // Chạy trò chơi tương ứng
        if (gameState == 1) {
            playPong();
        } 
        else if (gameState == 2) {
            playChicken();
        }
        else if (gameState == 3) {
            playDinoRun();
        }
        else if (gameState == 4) {
            playFlappy_Bird();
        }
        else if (gameState == 5) {
            playCrazy_Arcade();
        }
        else if (gameState == 6) {
            playSnake();
        } 
        else if (gameState == 7) {
            playDaoVang();
        }
        else if (gameState == 8) {
            playTetris();
        } 
    }
}

void drawMenu() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    if (mainMenuState == 0) { // Menu chính
        display.setCursor(0, 0);
        display.println(selectedMainMenu == 0 ? ">Game" : " Game");
        display.setCursor(0, 8);
        display.println(selectedMainMenu == 1 ? ">QRcode" : " QRcode");
        display.setCursor(0, 16);
        display.println(selectedMainMenu == 2 ? ">May tinh" : " May tinh");
        display.setCursor(0, 24);
        display.println(selectedMainMenu == 3 ? ">Music" : " Music");
        display.setCursor(0, 32);
        display.println(selectedMainMenu == 4 ? ">Motor Control" : " Motor Control");
        display.setCursor(0, 40);
        display.println(selectedMainMenu == 5 ? ">Reset" : " Reset");
    } else if (mainMenuState == 1) { // Menu Game
        display.setCursor(0, 0);
        display.println(selectedGame == 1 ? ">Pong" : " Pong");
        display.setCursor(0, 8);
        display.println(selectedGame == 2 ? ">Chicken" : " Chicken");
        display.setCursor(0, 16);
        display.println(selectedGame == 3 ? ">DinoRun" : " DinoRun");
        display.setCursor(0, 24);
        display.println(selectedGame == 4 ? ">Flappy_Bird" : " Flappy_Bird");
        display.setCursor(0, 32);
        display.println(selectedGame == 5 ? ">Boom" : " Boom");
        display.setCursor(0, 40);
        display.println(selectedGame == 6 ? ">Snake" : " Snake");
        display.setCursor(0, 48);
        display.println(selectedGame == 7 ? ">Dao Vang" : " Dao Vang");
        display.setCursor(0, 56);
        display.println(selectedGame == 8 ? ">Tetris" : " Tetris");
    } else if (mainMenuState == 2) { // Menu QRcode
        display.setCursor(0, 0);
        display.println(qrCodeSelection == 0 ? ">SEPAY" : " SEPAY");
        display.setCursor(0, 8);
        display.println(qrCodeSelection == 1 ? ">BIDV" : " BIDV");
    } else if (mainMenuState == 3) {
        display.setCursor(0, 0);
        display.println(">May tinh");
    } else if (mainMenuState == 4) { // Menu Music
        display.setCursor(0, 0);
        display.println(selectedMusic == 0 ? ">Happy Birthday" : " Happy Birthday");
        display.setCursor(0, 8);
        display.println(selectedMusic == 1 ? ">Butterfly" : " Butterfly");
        display.setCursor(0, 16);
        display.println(selectedMusic == 2 ? ">Twinkle Twinkle" : " Twinkle Twinkle");
        display.setCursor(0, 24);
        display.println(selectedMusic == 3 ? ">Jingle Bells" : " Jingle Bells");
        display.setCursor(0, 32);
        display.println(selectedMusic == 4 ? ">Mary Had" : " Mary Had");
        display.setCursor(0, 40);
        display.println(selectedMusic == 5 ? ">Super Mario" : " Super Mario");
        display.setCursor(0, 48);
        display.println(selectedMusic == 6 ? ">Canon in D" : " Canon in D");
        display.setCursor(0, 56);
        display.println(selectedMusic == 7 ? ">Doraemon" : " Doraemon");
    } else if (mainMenuState == 5) { // Menu Motor Control
        display.setCursor(0, 0);
        display.println(selectedMotor_Control == 0 ? ">Bluetooth" : " Bluetooth");
        display.setCursor(0, 8);
        display.println(selectedMotor_Control == 1 ? ">BUTTON" : " BUTTON");
    } else if (mainMenuState == 6) { // Menu BUTTON Control
        display.setCursor(0, 0);
        display.println(selectedServo == 0 ? ">Servo1" : " Servo1");
        display.setCursor(0, 8);
        display.println(selectedServo == 1 ? ">Servo2" : " Servo2");
        display.setCursor(0, 16);
        display.println(selectedServo == 2 ? ">Servo3" : " Servo3");
        display.setCursor(0, 24);
        display.println(selectedServo == 3 ? ">Servo4" : " Servo4");
    }
    display.display();

    // Điều hướng menu
    if (digitalRead(UP_BUTTON) == LOW) {
        if (mainMenuState == 0) {
            selectedMainMenu = (selectedMainMenu == 0) ? 5 : selectedMainMenu - 1; // Menu chính
        } else if (mainMenuState == 1) {
          selectedGame = (selectedGame == 1) ? 8 : selectedGame - 1;  // Menu Game
        } else if (mainMenuState == 2) {
            qrCodeSelection = (qrCodeSelection == 0) ? 1 : 0; // Chuyển giữa BIDV và SEPAY
        } else if (mainMenuState == 4) { // Điều hướng trong Menu Music
            selectedMusic = (selectedMusic == 0) ? 7 : selectedMusic - 1; // Chuyển sang bài nhạc trước
        } else if (mainMenuState == 5) { // Điều hướng trong Menu Motor Control
            selectedMotor_Control = (selectedMotor_Control == 0) ? 1 : selectedMotor_Control - 1; // Chuyển giữa Bluetooth và BUTTON
        } else if (mainMenuState == 6) { // Điều hướng trong Menu BUTTON Control
            selectedServo = (selectedServo == 0) ? 3 : selectedServo - 1; // Chọn servo trước đó
        }
        delay(200);
    }
    if (digitalRead(DOWN_BUTTON) == LOW) {
        if (mainMenuState == 0) {
            selectedMainMenu = (selectedMainMenu == 5) ? 0 : selectedMainMenu + 1; // Menu chính
        } else if (mainMenuState == 1) {
          selectedGame = (selectedGame == 8) ? 1 : selectedGame + 1; // Menu Game
        } else if (mainMenuState == 2) {
            qrCodeSelection = (qrCodeSelection == 1) ? 0 : 1; // Chuyển giữa BIDV và SEPAY
        } else if (mainMenuState == 4) { // Điều hướng trong Menu Music
            selectedMusic = (selectedMusic == 7) ? 0 : selectedMusic + 1; // Chuyển sang bài nhạc tiếp theo
        } else if (mainMenuState == 5) { // Điều hướng trong Menu Motor Control
            selectedMotor_Control = (selectedMotor_Control == 1) ? 0 : selectedMotor_Control + 1; // Chuyển giữa Bluetooth và BUTTON
        } else if (mainMenuState == 6) { // Điều hướng trong Menu BUTTON Control
            selectedServo = (selectedServo == 3) ? 0 : selectedServo + 1; // Chọn servo tiếp theo
        }
        delay(200);
    }

    // Quay lại menu chính
    if (digitalRead(BACK_BUTTON) == LOW) {
      if (mainMenuState == 6) {
        mainMenuState = 5; // Quay lại menu Motor Control
      } else if (mainMenuState != 0) {
            mainMenuState = 0; // Quay lại menu chính
      }
            delay(200);
    }
}

void controlServo(int servoIndex) {
    int angle = servoAngles[servoIndex];
    while (true) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.print("Servo");
        display.print(servoIndex + 1);
        display.print(": ");
        display.print(angle);
        display.display();

        if (digitalRead(UP_BUTTON) == LOW) {
            angle = 180; // Quay đến 180 độ
            delay(200);
        } else if (digitalRead(DOWN_BUTTON) == LOW) {
            angle = 0; // Quay đến 0 độ
            delay(200);
        } else if (digitalRead(RIGHT_BUTTON) == LOW) {
            angle = constrain(angle + 2, 0, 180); // Tăng góc từ từ
        } else if (digitalRead(LEFT_BUTTON) == LOW) {
            angle = constrain(angle - 2, 0, 180); // Giảm góc từ từ
        } else if (digitalRead(BACK_BUTTON) == LOW) {
            break; // Quay lại menu BUTTON Control
        }

        servos[servoIndex].write(angle); // Cập nhật góc servo
        servoAngles[servoIndex] = angle; // Lưu góc hiện tại
    }
}

void displayQRCode(int qrCodeType) {
    display.clearDisplay();
    if (qrCodeType == 0) { // BIDV
        display.drawBitmap(43, 16, SEPAYQRCODEbitmap40x37, 40, 37, SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(30, 0);
        display.println("PHAN VAN KHAI");
        display.setCursor(23, 8);
        display.println("BIDV-CN HAI VAN");
        display.setCursor(35, 56);
        display.println("$$$$$$$$$$");
    } else if (qrCodeType == 1) { // SEPAY
        display.drawBitmap(43, 16, BIDVQRCODEbitmap40x37, 40, 37, SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(30, 0);
        display.println("PHAN VAN KHAI");
        display.setCursor(23, 8);
        display.println("BIDV-CN HAI VAN");
        display.setCursor(35, 56);
        display.println("***********");
    }
    display.display();

    // Chờ nhấn nút quay lại
    while (digitalRead(BACK_BUTTON) != LOW) {
        delay(300);
    }
}

void playPong() {
    // Pong variables
    static int paddleLeftY = (SCREEN_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
    static int paddleRightY = (SCREEN_HEIGHT / 2) - (PADDLE_HEIGHT / 2);
    static int ballX = SCREEN_WIDTH / 2;
    static int ballY = SCREEN_HEIGHT / 2;
    static int ballSpeedX = 1;
    static int ballSpeedY = 1;

    // Biến lưu bitmap hiện tại của bóng
    static const uint8_t* currentBallBitmap = bitmap16x16;

    // Điều khiển paddle
    if (digitalRead(LEFT_BUTTON) == LOW && paddleLeftY > 0) paddleLeftY -= 2;
    if (digitalRead(LEFT_BUTTON) == HIGH && paddleLeftY < SCREEN_HEIGHT - PADDLE_HEIGHT) paddleLeftY += 2;
    if (digitalRead(RIGHT_BUTTON) == LOW && paddleRightY > 0) paddleRightY -= 2;
    if (digitalRead(RIGHT_BUTTON) == HIGH && paddleRightY < SCREEN_HEIGHT - PADDLE_HEIGHT) paddleRightY += 2;

    // Cập nhật bóng
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    if (ballY <= 0 || ballY >= SCREEN_HEIGHT - BALL_SIZE) ballSpeedY = -ballSpeedY;

    if ((ballX <= PADDLE_WIDTH && ballY + BALL_SIZE >= paddleLeftY && ballY <= paddleLeftY + PADDLE_HEIGHT) ||
        (ballX >= SCREEN_WIDTH - PADDLE_WIDTH - BALL_SIZE && ballY + BALL_SIZE >= paddleRightY && ballY <= paddleRightY + PADDLE_HEIGHT)) {
        ballSpeedX = -ballSpeedX;

        // Thay đổi bóng thành bitmap ngẫu nhiên
        int randomBitmap = random(0, 4);
        switch (randomBitmap) {
            case 0:
                currentBallBitmap = bitmap16x16;
                break;
            case 1:
                currentBallBitmap = DinoRunbitmap16x16;
                break;
            case 2:
                currentBallBitmap = FlyingDinosaur16x16;
                break;
            case 3:
                currentBallBitmap = bitmap13x16;
                break;
        }
    }

    if (ballX < 0 || ballX > SCREEN_WIDTH) {
        display.clearDisplay();
        display.setCursor(10, 25);
        display.setTextSize(2);
        display.println("GAME OVER");
        display.display();
        delay(1000);
        ballX = SCREEN_WIDTH / 2;
        ballY = SCREEN_HEIGHT / 2;
        currentBallBitmap = bitmap16x16; // Đặt lại bóng về bitmap mặc định
    }

    display.clearDisplay();
    display.fillRect(0, paddleLeftY, PADDLE_WIDTH, PADDLE_HEIGHT, SSD1306_WHITE);
    display.fillRect(SCREEN_WIDTH - PADDLE_WIDTH, paddleRightY, PADDLE_WIDTH, PADDLE_HEIGHT, SSD1306_WHITE);
    display.drawBitmap(ballX, ballY, currentBallBitmap, BALL_SIZE, BALL_SIZE, SSD1306_WHITE);
    display.display();
    delay(10);
}

void playChicken() {
    // Chicken variables
    static int planeY = SCREEN_HEIGHT / 2;
    static int bulletX[100], bulletY[100];
    static bool bulletActive[100] = {false};
    static int bulletIndex = 0;
    static int chickensShot = 0; // Số lượng gà đã bắn trúng
    static int currentLevel = 1; // Cấp độ hiện tại
    const int maxLevel = 10; // Số cấp độ tối đa

    const int maxChickens = 5;
    static int chickenX[maxChickens], chickenY[maxChickens];
    static bool chickenActive[maxChickens] = {false};
    static int chickenDirection[maxChickens] = {1}; // Hướng di chuyển theo chiều dọc (1: xuống, -1: lên)
    
    // Reset game logic (nếu cần)
    auto resetChickenGame = [&]() {
        planeY = SCREEN_HEIGHT / 2;
        for (int i = 0; i < 100; i++) bulletActive[i] = false;
        for (int i = 0; i < maxChickens; i++) {
            chickenX[i] = 0;
            chickenY[i] = random(0, SCREEN_HEIGHT - 3);
            chickenActive[i] = true;
        }
        chickensShot = 0;
        currentLevel = 1;
    };
    
    // Điều khiển máy bay
    if (digitalRead(UP_BUTTON) == LOW && planeY > -5) planeY -= 3; // Cho phép máy bay đi lên mép trên hết một nửa chiều cao
    if (digitalRead(DOWN_BUTTON) == LOW && planeY < SCREEN_HEIGHT - 8) planeY += 3; // Cho phép máy bay đi xuống mép dưới hết một nửa chiều cao

    // Quản lý danh sách đạn
    static unsigned long lastBulletTime = 0;
    if (millis() - lastBulletTime > 50) { // Tạo đạn mới mỗi 50ms
        bulletX[bulletIndex] = SCREEN_WIDTH - 16; // Đạn xuất phát từ đầu máy bay
        bulletY[bulletIndex] = planeY + 7;  
        bulletActive[bulletIndex] = true;       // Kích hoạt đạn
        bulletIndex = (bulletIndex + 1) % 100;  // Chuyển sang viên đạn tiếp theo (vòng lặp)
        lastBulletTime = millis();
    }

    // Cập nhật vị trí đạn
    for (int i = 0; i < 100; i++) {
        if (bulletActive[i]) {
            bulletX[i] -= 2; // Đạn di chuyển sang trái
            if (bulletX[i] < 0) bulletActive[i] = false; // Đạn ra khỏi màn hình
        }
    }

    // Xác định số lượng gà hoạt động
    int activeChickens = (currentLevel <= 5) ? currentLevel : (currentLevel - 5);

    // Cập nhật vị trí gà
    for (int i = 0; i < activeChickens; i++) { // Số lượng gà dựa trên cấp độ
        if (chickenActive[i]) {
            chickenX[i] += 1; // Gà luôn di chuyển sang phải

            // Nếu ở Level 6 trở lên, gà di chuyển ngẫu nhiên theo chiều dọc
            if (currentLevel >= 6) {
                chickenY[i] += chickenDirection[i]; // Di chuyển theo hướng hiện tại

                // Đổi hướng nếu gà chạm mép trên hoặc dưới
                if (chickenY[i] <= 0 || chickenY[i] >= SCREEN_HEIGHT - 4) {
                    chickenDirection[i] *= -1; // Đổi hướng
                }
            }

            // Kiểm tra nếu gà ra khỏi màn hình
            if (chickenX[i] > SCREEN_WIDTH) {
                // Khi gà ra khỏi màn hình, trò chơi kết thúc
                display.clearDisplay();
                display.setCursor(10, 25);
                display.setTextSize(2);
                display.println("GAME OVER");
                display.display();
                delay(1000);

                // Reset trò chơi
                resetChickenGame();
                chickensShot = 0; // Đặt lại số lượng gà bị bắn trúng
                currentLevel = 1; // Đặt lại cấp độ
                return;
            }
        }
    }

    // Khởi tạo lại gà nếu cần
    static const uint8_t* chickenBitmaps[maxChickens]; // Lưu bitmap của từng con gà
    for (int i = 0; i < activeChickens; i++) {
        if (!chickenActive[i]) {
            chickenX[i] = 0; // Gà xuất hiện ngay tại mép trái màn hình
            chickenY[i] = random(0, SCREEN_HEIGHT - 4); // Gà xuất hiện ngẫu nhiên theo chiều dọc
            chickenDirection[i] = random(0, 2) == 0 ? 1 : -1; // Hướng di chuyển ngẫu nhiên (1: xuống, -1: lên)
            chickenBitmaps[i] = (random(0, 2) == 0) ? chicken1Bitmap4x4 : chickenBit2map4x4; // Chọn ngẫu nhiên bitmap
            chickenActive[i] = true; // Kích hoạt gà
        }
    }

    // Kiểm tra va chạm giữa đạn và gà
    for (int i = 0; i < 100; i++) {
        if (bulletActive[i]) {
            for (int j = 0; j < activeChickens; j++) { // Số lượng gà dựa trên cấp độ
                if (chickenActive[j] &&
                    bulletX[i] >= chickenX[j] && bulletX[i] < chickenX[j] + 4 &&
                    bulletY[i] >= chickenY[j] && bulletY[i] < chickenY[j] + 4) {
                    bulletActive[i] = false; // Đạn biến mất
                    chickenActive[j] = false; // Gà biến mất
                    chickensShot++; // Tăng số lượng gà bị bắn trúng

                    // Tăng cấp độ nếu đạt đủ số lượng gà bắn trúng
                    if ((chickensShot == 10 && currentLevel == 1) ||
                        (chickensShot == 30 && currentLevel == 2) ||
                        (chickensShot == 60 && currentLevel == 3) ||
                        (chickensShot == 120 && currentLevel == 4) ||
                        (chickensShot == 240 && currentLevel == 5) ||
                        (chickensShot == 250 && currentLevel == 6) ||
                        (chickensShot == 270 && currentLevel == 7) ||
                        (chickensShot == 310 && currentLevel == 8) ||
                        (chickensShot == 390 && currentLevel == 9)) {
                        currentLevel++; // Tăng cấp độ
                    }
                }
            }
        }
    }

    // Vẽ màn hình
    display.clearDisplay();

    // Hiển thị số lượng gà bị bắn trúng và cấp độ
    display.setTextSize(1);
    display.setCursor(100, 0);
    display.print("");
    display.print(chickensShot);
    display.setCursor(0, 0);
    display.print("Level:");
    display.print(currentLevel);

    // Vẽ máy bay
    display.drawBitmap(SCREEN_WIDTH - 16, planeY, bitmap16x16, 16, 16, SSD1306_WHITE);

    // Vẽ đạn
    for (int i = 0; i < 100; i++) {
        if (bulletActive[i]) {
            display.fillRect(bulletX[i], bulletY[i], 2, 2, SSD1306_WHITE); // Vẽ đạn kích thước 2x2
        }
    }

    // Vẽ gà
    for (int i = 0; i < activeChickens; i++) { // Số lượng gà dựa trên cấp độ
        if (chickenActive[i]) {
            for (int j = 0; j < 4; j++) { // Vẽ từng dòng của bitmap
                for (int k = 0; k < 4; k++) { // Vẽ từng cột của bitmap
                    if (chickenBitmaps[i][j] & (1 << k)) {
                        display.drawPixel(chickenX[i] + k, chickenY[i] + j, SSD1306_WHITE);
                    }
                }
            }
        }
    }

    display.display();
    delay(10);
}

void playDinoRun() {
    static int dinoY = SCREEN_HEIGHT - 16; 
    static int dinoJump = 0; 
    static int obstacleX = SCREEN_WIDTH; 
    static int dinoY2 = SCREEN_HEIGHT / 2 - 16; 
    static int dinoJump2 = 0; 
    static int obstacleX2 = SCREEN_WIDTH + 64; 
    static bool gameOver = false; 
    static int obstaclesCleared = 0; 
    static int dinoLevel = 1; 
    static bool inverted = false; 
    static bool invertChanged = false;

    // Biến để lưu loại bitmap của khủng long
    static const uint8_t* currentDinoBitmap = DinoRunbitmap216x16;
    static bool isSwitchingBitmap = true; // Trạng thái chuyển đổi bitmap
    static int frameCounter = 0; // Bộ đếm để giảm tốc độ chuyển đổi bitmap
    static int frameThreshold = 10; // Ngưỡng để chuyển đổi bitmap

    // Biến để lưu loại bitmap của chướng ngại vật
    static const uint8_t* obstacleBitmap1 = bitmap13x16;
    static const uint8_t* obstacleBitmap2 = bitmap13x16;

    // Mảng lưu mức vượt qua chướng ngại vật cần thiết tại từng cấp độ
    const int obstacleThresholds[] = {2, 4, 8, 12, 16, 20, 24, 28, 32, 36};
    const int maxLevel = sizeof(obstacleThresholds) / sizeof(obstacleThresholds[0]);

    if (digitalRead(ACTION_BUTTON) == LOW && dinoY == SCREEN_HEIGHT - 16) {
        int jumpValue = (dinoLevel <= 5) ? (32 - (dinoLevel - 1) * 4) : (32 - (dinoLevel - 6) * 4);
        dinoJump = jumpValue;
        isSwitchingBitmap = false; // Dừng chuyển đổi bitmap khi nhảy
    }

    if (dinoLevel >= 6 && digitalRead(DOWN_BUTTON) == LOW && dinoY2 == SCREEN_HEIGHT / 2 - 16) {
        int jumpValue2 = (32 - (dinoLevel - 6) * 4);
        dinoJump2 = jumpValue2;
    }

    if (dinoJump > 0) {
        if (dinoY > SCREEN_HEIGHT - 32) {
            dinoY -= 2; 
        }
        dinoJump--;
    } else if (dinoY < SCREEN_HEIGHT - 16) {
        dinoY += 2; 
    } else {
        isSwitchingBitmap = true; // Tiếp tục chuyển đổi bitmap khi chạm đất
    }

    if (dinoLevel >= 6 && dinoJump2 > 0) {
        if (dinoY2 > SCREEN_HEIGHT / 2 - 32) {
            dinoY2 -= 2; 
        }
        dinoJump2--;
    } else if (dinoLevel >= 6 && dinoY2 < SCREEN_HEIGHT / 2 - 16) {
        dinoY2 += 2; 
    }

    int obstacleSpeed = (dinoLevel - 1) % 5 + 1;
    obstacleX -= obstacleSpeed;
    if (obstacleX < -16) {
        obstacleX = SCREEN_WIDTH;
        obstaclesCleared++; 

        // Chọn ngẫu nhiên bitmap cho chướng ngại vật 1
        obstacleBitmap1 = (random(0, 2) == 0) ? bitmap13x16 : FlyingDinosaur16x16;
    }

    if (dinoLevel >= 6) {
        obstacleX2 -= obstacleSpeed;
        if (obstacleX2 < -16) {
            obstacleX2 = SCREEN_WIDTH + 64;
            obstaclesCleared++; 

            // Chọn ngẫu nhiên bitmap cho chướng ngại vật 2
            obstacleBitmap2 = (random(0, 2) == 0) ? bitmap13x16 : FlyingDinosaur16x16;
        }
    }

    if (obstaclesCleared % 5 == 0 && obstaclesCleared > 0 && !invertChanged) {
        inverted = !inverted;
        display.invertDisplay(inverted);
        invertChanged = true; 
    } else if (obstaclesCleared % 5 != 0) {
        invertChanged = false; 
    }

    // Tăng cấp độ nếu đạt số lượng chướng ngại vật yêu cầu
    if (dinoLevel < maxLevel && obstaclesCleared >= obstacleThresholds[dinoLevel - 1]) {
        dinoLevel++; 
        obstacleSpeed++;

        // Điều chỉnh tốc độ chuyển đổi bitmap
        if (dinoLevel <= 5) {
            frameThreshold = max(2, 10 - dinoLevel * 2); // Giảm ngưỡng để tăng tốc độ chuyển đổi
        } else if (dinoLevel <= 10) {
            frameThreshold = max(2, 10 - (dinoLevel - 5) * 2); // Đặt lại ngưỡng khi đến level 6
        } else {
            frameThreshold = 2;
        }
    }

    if (obstacleX < 16 && obstacleX > 0 && dinoY > SCREEN_HEIGHT - 32) {
        gameOver = true; 
    }

    if (dinoLevel >= 6 && obstacleX2 < 16 && obstacleX2 > 0 && dinoY2 > SCREEN_HEIGHT / 2 - 32) {
        gameOver = true; 
    }

    display.clearDisplay();

    if (gameOver) {
        display.setCursor(10, 17);
        display.setTextSize(2);
        display.println("GAME OVER");

        // Hiển thị số ống đã vượt qua
        display.setTextSize(2);
        display.setCursor(45, 35);
        display.print("$:");
        display.print(obstaclesCleared);

        display.display();
        delay(3000);
    
        // Reset game
        dinoY = SCREEN_HEIGHT - 16;
        dinoJump = 0;
        obstacleX = SCREEN_WIDTH;
        dinoY2 = SCREEN_HEIGHT / 2 - 16;
        dinoJump2 = 0;
        obstacleX2 = SCREEN_WIDTH + 64;
        obstaclesCleared = 0;
        dinoLevel = 1;
        gameOver = false;
        frameThreshold = 10; // Đặt lại tốc độ chuyển đổi bitmap
        if (inverted) {
            display.invertDisplay(false);
            inverted = false;
        }
        return;
    }

    // Chuyển đổi bitmap khi khủng long ở dưới đất
    if (isSwitchingBitmap) {
        frameCounter++;
        if (frameCounter >= frameThreshold) { // Chuyển đổi bitmap dựa trên ngưỡng hiện tại
            currentDinoBitmap = (currentDinoBitmap == DinoRunbitmap216x16) ? DinoRunbitmap316x16 : DinoRunbitmap216x16;
            frameCounter = 0; // Reset bộ đếm
        }
    }

    display.drawBitmap(0, dinoY, currentDinoBitmap, 16, 16, SSD1306_WHITE);
    display.drawBitmap(obstacleX, SCREEN_HEIGHT - 16, obstacleBitmap1, 16, 16, SSD1306_WHITE);

    if (dinoLevel >= 6) {
        display.drawBitmap(0, dinoY2, DinoRunbitmap16x16, 16, 16, SSD1306_WHITE);
        display.drawBitmap(obstacleX2, SCREEN_HEIGHT / 2 - 16, obstacleBitmap2, 16, 16, SSD1306_WHITE);
    }

    display.setTextSize(1);
    display.setCursor(16, 0);
    display.print("Level:");
    display.print(dinoLevel);
    display.setCursor(110, 0);
    display.print("");
    display.print(obstaclesCleared);

    display.display();
    delay(10);
}

void playFlappy_Bird() {
    static int birdY = SCREEN_HEIGHT / 2; // Vị trí Y của chim
    static float birdVelocity = 0; // Tốc độ rơi của chim (sử dụng float để tăng độ mượt)
    static int pipeX[5] = {SCREEN_WIDTH, SCREEN_WIDTH + SCREEN_WIDTH / 5, SCREEN_WIDTH + 2 * SCREEN_WIDTH / 5, SCREEN_WIDTH + 3 * SCREEN_WIDTH / 5, SCREEN_WIDTH + 4 * SCREEN_WIDTH / 5};
    static int pipeGapY[5] = {
        random(10, SCREEN_HEIGHT - 40),
        random(10, SCREEN_HEIGHT - 40),
        random(10, SCREEN_HEIGHT - 40),
        random(10, SCREEN_HEIGHT - 40),
        random(10, SCREEN_HEIGHT - 40)
    };

    static int score = 0; // Điểm số
    static int level = 1; // Cấp độ hiện tại
    static bool gameOver = false; // Trạng thái trò chơi
    const int pipeWidth = 7; // Chiều rộng của ống (thu nhỏ lại)
    const int pipeGapHeight = 30; // Chiều cao khoảng trống giữa ống trên và ống dưới (tăng lên)

    // Biến để lưu bitmap hiện tại của chim
    static const uint8_t* currentBirdBitmap = Bird1bitmap16x12;

    // Biến để theo dõi thời gian bắt đầu trò chơi
    static unsigned long startTime = millis();

    // Tính thời gian đã trôi qua
    unsigned long elapsedTime = millis() - startTime;

    // Vẽ màn hình
    display.clearDisplay();

    // Vẽ chim
    display.drawBitmap(10, birdY, currentBirdBitmap, 16, 12, SSD1306_WHITE);

    // Vẽ ống
    for (int i = 0; i < level; i++) {
        display.fillRect(pipeX[i], 0, pipeWidth, pipeGapY[i], SSD1306_WHITE); // Ống trên (thu nhỏ chiều rộng)
        display.fillRect(pipeX[i], pipeGapY[i] + pipeGapHeight, pipeWidth, SCREEN_HEIGHT - pipeGapY[i] - pipeGapHeight, SSD1306_WHITE); // Ống dưới
    }

    // Hiển thị "Ready" và "Go!" trong 2 giây đầu tiên
    if (elapsedTime <= 2000) {
        display.setTextSize(2);
        display.setCursor(30, 25);
        if (elapsedTime <= 1000) {
            display.println("Ready");
        } else {
            display.setCursor(50, 25);
            display.println("Go!");
        }
        display.display();
        return; // Không thực hiện các logic khác trong 2 giây đầu tiên
    }

    // Điều khiển chim bay lên khi nhấn nút (chỉ sau 2 giây)
    if (digitalRead(ACTION_BUTTON) == LOW) {
        birdVelocity = -2.5; // Chim bay lên (giảm tốc độ bay lên)
    }

    // Cập nhật vị trí chim
    birdVelocity += 0.5; // Trọng lực (giảm tốc độ rơi)
    birdY += birdVelocity;

    // Tăng cấp độ dựa trên điểm số
    if (score >= 20) {
        level = 5; // Level 5: 5 ống
    } else if (score >= 12) {
        level = 4; // Level 4: 4 ống
    } else if (score >= 8) {
        level = 3; // Level 3: 3 ống
    } else if (score >= 4) {
        level = 2; // Level 2: 2 ống
    } else {
        level = 1; // Level 1: 1 ống
    }

    // Đảm bảo các ống được phân bố đều trên màn hình
    int spacing = SCREEN_WIDTH / level; // Khoảng cách giữa các ống
    for (int i = 0; i < level; i++) {
        if (pipeX[i] < -pipeWidth) { // Khi ống ra khỏi màn hình
           // Đặt lại vị trí ống cách đều nhau dựa trên ống xa nhất
           int maxPipeX = 0;
           for (int j = 0; j < level; j++) {
               if (pipeX[j] > maxPipeX) {
                  maxPipeX = pipeX[j];
               }
           }
            pipeX[i] = maxPipeX + spacing; // Đặt vị trí mới cho ống
            pipeGapY[i] = random(10, SCREEN_HEIGHT - pipeGapHeight - 10); // Tạo khoảng trống mới
            score++; // Tăng điểm mỗi khi chim bay qua bất kỳ ống nào

            // Thay đổi chim thành bitmap ngẫu nhiên 
            currentBirdBitmap = (random(0, 2) == 0) ? Bird1bitmap16x12 : Bird2bitmap16x12;
        }
    }

    // Cập nhật vị trí ống
    for (int i = 0; i < level; i++) {
        pipeX[i] -= 1; // Ống di chuyển chậm hơn
    }

    // Kiểm tra va chạm
    for (int i = 0; i < level; i++) {
        if (birdY > SCREEN_HEIGHT - 12 || // Chim chạm mép dưới
            (pipeX[i] < 16 && pipeX[i] > 0 && // Chim va chạm với ống
             (birdY < pipeGapY[i] || birdY > pipeGapY[i] + pipeGapHeight))) {
            gameOver = true;
        }
    }

    // Xử lý khi trò chơi kết thúc
    if (gameOver) {
        display.clearDisplay();
        display.setCursor(10, 17);
        display.setTextSize(2);
        display.println("GAME OVER");

     // Hiển thị số ống đã vượt qua
        display.setTextSize(2);
        display.setCursor(45, 35);
        display.print("$:");
        display.print(score);

        display.display();
        delay(3000);
    
      // Reset game
        birdY = SCREEN_HEIGHT / 2;
        birdVelocity = 0;
    for (int i = 0; i < 3; i++) {
        pipeX[i] = SCREEN_WIDTH + i * (SCREEN_WIDTH / 3);
        pipeGapY[i] = random(10, SCREEN_HEIGHT - 40);
    }
        score = 0;
        level = 1;
        gameOver = false;
        startTime = millis();
        return;
    }

    // Hiển thị điểm và cấp độ
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Level:");
    display.print(level);
    display.setCursor(100, 0);
    display.print("");
    display.print(score);

    display.display();
    delay(10);
}

void playCalculator() {
    static int selectedIndex = 0; // Chỉ số của bitmap được chọn
    static String operand1 = ""; // Toán hạng 1
    static String operand2 = ""; // Toán hạng 2
    static char operation = '\0'; // Phép toán (+, -, *, /)
    static String result = ""; // Kết quả
    static bool resultDisplayed = false; // Trạng thái hiển thị kết quả

    // Danh sách các bitmap
    const uint8_t* bitmaps[] = {
        Number7Bitmap7x9, Number8Bitmap7x9, Number9Bitmap7x9, ChiaBitmap7x9, XoaBitmap7x9,
        Number4Bitmap7x9, Number5Bitmap7x9, Number6Bitmap7x9, NhanBitmap7x9, nullptr,
        Number1Bitmap7x9, Number2Bitmap7x9, Number3Bitmap7x9, TruBitmap7x9, nullptr,
        ChamBitmap7x9, Number0Bitmap7x9, nullptr, CongBitmap7x9, BangBitmap7x9
    };

    // Quay lại menu chính khi nhấn nút BACK_BUTTON
    if (digitalRead(BACK_BUTTON) == LOW) {
        mainMenuState = 0; // Quay lại Main Menu
        delay(300); // Chống dội phím
        return;
    }

    // Điều khiển chuyển đổi giữa các bitmap
    if (digitalRead(RIGHT_BUTTON) == LOW) {
        do {
            selectedIndex = (selectedIndex + 1) % 20; // Chuyển sang bitmap tiếp theo
        } while (bitmaps[selectedIndex] == nullptr); // Bỏ qua các ô trống
        delay(200); // Chống dội phím
    } else if (digitalRead(LEFT_BUTTON) == LOW) {
        do {
            selectedIndex = (selectedIndex - 1 + 20) % 20; // Trở về bitmap trước đó
        } while (bitmaps[selectedIndex] == nullptr); // Bỏ qua các ô trống
        delay(200); // Chống dội phím
    } else if (digitalRead(UP_BUTTON) == LOW) {
        do {
            selectedIndex = (selectedIndex - 5 + 20) % 20; // Di chuyển lên trên (giảm 5 hàng)
        } while (bitmaps[selectedIndex] == nullptr); // Bỏ qua các ô trống
        delay(200); // Chống dội phím
    } else if (digitalRead(DOWN_BUTTON) == LOW) {
        do {
            selectedIndex = (selectedIndex + 5) % 20; // Di chuyển xuống dưới (tăng 5 hàng)
        } while (bitmaps[selectedIndex] == nullptr); // Bỏ qua các ô trống
        delay(200); // Chống dội phím
    }

    // Xử lý khi nhấn nút chọn
    if (digitalRead(ACTION_BUTTON) == LOW) {
        if (selectedIndex < 3) { // Chọn số 7, 8, 9
            if (!resultDisplayed) {
                if (operation == '\0') {
                    if (operand1.length() < 6) {
                        operand1 += String(selectedIndex + 7); // Thêm số 7, 8, 9
                    }
                } else {
                    if (operand2.length() < 6) {
                        operand2 += String(selectedIndex + 7); // Thêm số 7, 8, 9
                    }
                }
            }
        } else if (selectedIndex == 3) { // Chọn phép chia
            if (!resultDisplayed && !operand1.isEmpty() && operand2.isEmpty()) {
                operation = '/';
            }
        } else if (selectedIndex == 4) { // Chọn "Xóa"
            if (resultDisplayed) {
                operand1 = "";
                operand2 = "";
                operation = '\0';
                result = "";
                resultDisplayed = false;
            } else if (!operand2.isEmpty()) {
                operand2.remove(operand2.length() - 1); // Xóa ký tự cuối của operand2
            } else if (operation != '\0') {
                operation = '\0'; // Xóa phép toán
            } else if (!operand1.isEmpty()) {
                operand1.remove(operand1.length() - 1); // Xóa ký tự cuối của operand1
            }
        } else if (selectedIndex >= 5 && selectedIndex <= 7) { // Chọn số 4, 5, 6
            if (!resultDisplayed) {
                if (operation == '\0') {
                    if (operand1.length() < 6) {
                        operand1 += String(selectedIndex - 1); // Thêm số 4, 5, 6
                    }
                } else {
                    if (operand2.length() < 6) {
                        operand2 += String(selectedIndex - 1); // Thêm số 4, 5, 6
                    }
                }
            }
        } else if (selectedIndex == 8) { // Chọn phép nhân
            if (!resultDisplayed && !operand1.isEmpty() && operand2.isEmpty()) {
                operation = '*';
            }
        } else if (selectedIndex >= 10 && selectedIndex <= 12) { // Chọn số 1, 2, 3
            if (!resultDisplayed) {
                if (operation == '\0') {
                    if (operand1.length() < 6) {
                        operand1 += String(selectedIndex - 9); // Thêm số 1, 2, 3
                    }
                } else {
                    if (operand2.length() < 6) {
                        operand2 += String(selectedIndex - 9); // Thêm số 1, 2, 3
                    }
                }
            }
        } else if (selectedIndex == 13) { // Chọn phép trừ
            if (!resultDisplayed && !operand1.isEmpty() && operand2.isEmpty()) {
                operation = '-';
            }
        } else if (selectedIndex == 15) { // Chọn dấu phẩy
            if (!resultDisplayed) {
                if (operation == '\0') {
                    if (operand1.indexOf('.') == -1 && operand1.length() < 4) {
                        operand1 += "."; // Thêm dấu phẩy vào operand1
                    }
                } else {
                    if (operand2.indexOf('.') == -1 && operand2.length() < 4) {
                        operand2 += "."; // Thêm dấu phẩy vào operand2
                    }
                }
            }
        } else if (selectedIndex == 16) { // Chọn số 0
            if (!resultDisplayed) {
                if (operation == '\0') {
                    if (operand1.length() < 6) {
                        operand1 += "0"; // Thêm số 0
                    }
                } else {
                    if (operand2.length() < 6) {
                        operand2 += "0"; // Thêm số 0
                    }
                }
            }
        } else if (selectedIndex == 18) { // Chọn phép cộng
            if (!resultDisplayed && !operand1.isEmpty() && operand2.isEmpty()) {
                operation = '+';
            }
        } else if (selectedIndex == 19) { // Chọn "=" để tính toán
            if (!operand1.isEmpty() && !operand2.isEmpty() && operation != '\0') {
                float res = 0;
                if (operation == '/' && operand2.toFloat() == 0) {
                    result = "Error"; // Xử lý chia cho 0
                } else {
                    switch (operation) {
                        case '+': res = operand1.toFloat() + operand2.toFloat(); break;
                        case '-': res = operand1.toFloat() - operand2.toFloat(); break;
                        case '*': res = operand1.toFloat() * operand2.toFloat(); break;
                        case '/': res = operand1.toFloat() / operand2.toFloat(); break;
                    }

                    // Làm tròn đến phần chục nghìn
                    res = round(res * 10000.0) / 10000.0;

                    result = String(res, 4); // Hiển thị kết quả với 4 chữ số thập phân
                    if (result.indexOf(".0000") != -1) { // Xóa ".0000" nếu không có phần thập phân
                        result = result.substring(0, result.indexOf("."));
                    }
                }
                resultDisplayed = true;
            }
        }
        delay(200); // Chống dội phím
    }

    // Vẽ màn hình
    display.clearDisplay();

    // Hiển thị biểu thức
    display.setCursor(5, 0);
    display.setTextSize(1);
    display.print(operand1);
    if (operation != '\0') {
        display.print(" ");
        display.print(operation);
        display.print(" ");
    }
    display.print(operand2);

    // Hiển thị kết quả
    if (resultDisplayed) {
        display.setCursor(5, 8);
        display.print("= ");
        display.print(result);
    }

    // Hiển thị danh sách các bitmap theo dạng lưới (5 cột x 4 hàng)
    for (int i = 0; i < 20; i++) {
        if (bitmaps[i] != nullptr) {
            int x = 2 + (i % 5) * 9;       // Bắt đầu từ x = 20, mỗi cột cách nhau 9 pixel
            int y = 17 + (i / 5) * 12;      // Bắt đầu từ y = 16, mỗi hàng cách nhau 12 pixel

            if (i == selectedIndex) {
               // Đảo ngược bitmap được chọn
               display.fillRect(x , y - 1, 9, 11, SSD1306_WHITE);  // Vẽ khung trắng 9x11
               display.drawBitmap(x, y, bitmaps[i], 7, 9, SSD1306_BLACK); // Bitmap màu đen trong khung
            } else {
               display.drawBitmap(x, y, bitmaps[i], 7, 9, SSD1306_WHITE); // Bitmap bình thường
            }
        }
    }

    display.display();
}

void playCrazy_Arcade() {
    const int MAP_WIDTH = 21;
    const int MAP_HEIGHT = 10;
    const int MAX_ENEMIES = 11;
    const int BOMB_DELAY = 50;
    const int EXPLOSION_RANGE = 3;
    const int EXPLOSION_DURATION = 10;
    const int MAX_LEVEL = 20;

    static int level = 1; // Level bắt đầu từ 1
    static int playerX = 1, playerY = 1; // Vị trí ban đầu của nhân vật
    static int enemyX[MAX_ENEMIES] = {3, 5, 7, 9, 11};
    static int enemyY[MAX_ENEMIES] = {3, 5, 7, 9, 11};
    static bool enemyAlive[MAX_ENEMIES] = {false};
    static int totalEnemies = 1;
    static int enemiesKilled = 0; // Đếm số lượng kẻ địch đã tiêu diệt

    // Thêm mảng để quản lý nhiều quả bom của người chơi
    static int playerBombX[10] = {-1}; // Tối đa 10 quả bom
    static int playerBombY[10] = {-1};
    static int playerBombTimer[10] = {0}; // Bộ đếm thời gian cho từng quả bom

    static char explosionMap[MAP_HEIGHT][MAP_WIDTH] = {0};
    static int explosionTimer[MAP_HEIGHT][MAP_WIDTH] = {0};
    // Thêm biến để quản lý bom của kẻ địch
    static int enemyBombX[MAX_ENEMIES] = {-1}; // Vị trí X của bom kẻ địch
    static int enemyBombY[MAX_ENEMIES] = {-1}; // Vị trí Y của bom kẻ địch
    static int enemyBombTimer[MAX_ENEMIES] = {0}; // Bộ đếm thời gian bom kẻ địch

    // Thêm biến để quản lý số lượng bom có thể đặt
    static int playerMaxBombs = 1; // Số lượng bom tối đa mà nhân vật có thể đặt
    static int playerCurrentBombs = 0; // Số lượng bom hiện tại đã đặt
    static int enemyMaxBombs[MAX_ENEMIES] = {1}; // Số lượng bom tối đa cho từng kẻ địch
    static int enemyCurrentBombs[MAX_ENEMIES] = {0}; // Số lượng bom hiện tại đã đặt cho từng kẻ địch

    // Thêm biến để quản lý loại vật phẩm
    static bool itemType[MAP_HEIGHT][MAP_WIDTH] = {false}; // `false` là VP1, `true` là VP2
    
    // Thêm biến để quản lý vật phẩm
    static bool itemActive[MAP_HEIGHT][MAP_WIDTH] = {false}; // Trạng thái vật phẩm (có hoặc không)
    static int explosionRangeBonus = 0; // Phạm vi vụ nổ được cộng thêm

    // Thêm mảng để quản lý phạm vi vụ nổ riêng cho từng kẻ địch
    static int enemyExplosionRangeBonus[MAX_ENEMIES] = {0}; // Phạm vi vụ nổ riêng của từng kẻ địch

    // Mỗi kẻ địch có một bitmap ngẫu nhiên
    static const uint8_t* enemyBitmaps[MAX_ENEMIES] = {nullptr};

        // Bản đồ gốc 0
    const uint8_t originalMap0[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1},
        {1,0,1,0,1,1,1,0,2,0,0,1,0,0,1,1,0,1,0,0,1},
        {1,0,2,0,0,2,2,0,1,1,0,2,1,0,0,0,0,1,1,0,1},
        {1,0,1,1,1,0,0,2,2,1,1,0,0,0,0,1,0,0,0,0,1},
        {1,2,0,0,0,2,1,0,1,0,0,0,1,0,0,2,0,0,1,0,1},
        {1,0,1,0,1,0,1,0,2,2,0,0,0,1,0,1,0,0,1,0,1},
        {1,0,0,1,0,0,1,0,2,0,1,1,0,1,0,0,1,0,1,0,1},
        {1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Bản đồ gốc 1
    const uint8_t originalMap1[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

        // Bản đồ gốc 2
    const uint8_t originalMap2[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1},
        {1,0,1,0,1,1,1,0,2,0,0,1,0,0,1,1,0,1,0,0,1},
        {1,0,2,0,0,2,2,0,1,1,0,2,1,0,0,0,0,1,1,0,1},
        {1,0,1,1,1,0,0,2,2,1,1,0,0,0,0,1,0,0,0,0,1},
        {1,2,0,0,0,2,1,0,1,0,0,0,1,0,0,2,0,0,1,0,1},
        {1,0,1,0,1,0,1,0,2,2,0,0,0,1,0,1,0,0,1,0,1},
        {1,0,0,1,0,0,1,0,2,0,1,1,0,1,0,0,1,0,1,0,1},
        {1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Bản đồ gốc 3
    const uint8_t originalMap3[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,2,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,2,1},
        {1,2,1,0,0,1,0,1,0,1,0,0,2,0,0,0,0,2,1,0,1},
        {1,0,0,0,0,0,0,1,0,0,1,1,2,1,1,0,1,0,0,2,1},
        {1,2,2,1,1,1,0,1,1,0,0,0,0,0,2,0,0,0,1,0,1},
        {1,0,0,0,0,2,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1},
        {1,0,1,1,0,1,0,0,1,0,0,1,0,0,2,0,1,1,0,0,1},
        {1,0,0,0,0,0,1,0,1,1,1,2,0,2,0,1,0,0,2,0,1},
        {1,0,0,0,0,0,1,2,2,0,0,2,0,2,0,2,0,0,1,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Bản đồ gốc 4
    const uint8_t originalMap4[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,0,0,2,1},
        {1,2,1,0,0,1,0,1,0,1,0,0,2,2,0,2,2,2,1,0,1},
        {1,2,2,1,1,1,0,1,1,0,2,2,1,2,2,2,1,0,0,2,1},
        {1,0,1,0,2,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1},
        {1,0,2,0,2,2,1,0,0,0,0,1,0,0,1,0,1,1,0,0,1},
        {1,0,2,1,1,1,1,0,1,1,1,1,0,1,0,1,0,0,1,0,1},
        {1,0,0,0,2,2,1,0,2,0,0,2,0,2,0,1,0,0,2,0,1},
        {1,0,0,0,0,0,1,2,2,0,0,1,0,1,0,2,0,0,1,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Bản đồ gốc 5
    const uint8_t originalMap5[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,0,0,2,1},
        {1,2,1,0,0,1,0,1,0,1,0,0,2,0,0,2,2,2,1,0,1},
        {1,2,2,1,1,1,0,1,1,0,2,2,2,2,2,2,1,0,0,2,1},
        {1,0,1,0,2,0,0,0,0,0,0,1,1,0,0,1,0,0,1,0,1},
        {1,0,2,0,2,2,1,0,0,0,0,2,0,0,1,0,2,1,0,0,1},
        {1,0,2,1,1,1,0,0,1,1,1,2,1,2,0,2,0,0,2,1,1},
        {1,0,0,2,0,0,0,0,2,0,0,2,0,2,1,1,2,2,2,0,1},
        {1,0,0,1,0,0,1,2,2,0,0,1,0,1,0,2,0,0,1,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Bản đồ gốc 6
    const uint8_t originalMap6[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,2,0,2,0,2,0,2,0,2,0,0,0,0,0,0,0,0,0,1},
        {1,2,2,0,2,0,2,0,2,0,2,0,0,0,2,0,0,2,0,0,1},
        {1,0,0,0,2,0,2,0,2,0,2,0,0,2,2,2,2,2,2,0,1},
        {1,2,2,2,2,0,2,0,2,0,2,0,0,2,0,2,2,0,2,0,1},
        {1,0,0,0,0,0,2,0,2,0,2,0,0,2,2,2,2,2,2,0,1},
        {1,2,2,2,2,2,2,0,2,0,2,0,0,2,2,0,0,2,2,0,1},
        {1,0,0,0,0,0,0,0,2,0,2,0,0,0,2,2,2,2,0,0,1},
        {1,0,0,0,0,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Bản đồ gốc 7
    const uint8_t originalMap7[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,2,0,2,0,2,0,2,0,2,0,0,0,0,0,0,0,0,0,1},
        {1,2,2,0,2,0,2,0,2,0,2,0,0,2,2,0,0,2,2,0,1},
        {1,0,0,0,2,0,2,0,2,0,2,0,0,2,2,2,2,2,2,0,1},
        {1,2,2,2,2,0,2,0,2,0,2,0,0,2,0,0,0,0,2,0,1},
        {1,0,0,0,0,0,2,0,2,0,2,0,0,2,2,2,2,2,2,0,1},
        {1,2,2,2,2,2,2,0,2,0,2,0,0,2,2,0,0,2,2,0,1},
        {1,0,0,0,0,0,0,0,2,0,2,0,0,0,2,0,0,2,0,0,1},
        {1,0,0,0,0,0,0,0,2,0,2,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Bản đồ gốc 8
    const uint8_t originalMap8[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,2,0,2,0,2,0,2,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,2,2,0,2,0,2,0,2,0,0,2,2,0,0,0,2,2,0,0,1},
        {1,0,0,0,2,0,2,0,2,0,2,0,0,2,0,2,0,0,2,0,1},
        {1,2,2,2,2,0,2,0,2,0,2,0,0,0,2,0,0,0,2,0,1},
        {1,0,0,0,0,0,2,0,2,0,0,2,0,0,0,0,0,2,0,0,1},
        {1,2,2,2,2,2,2,0,2,0,0,0,2,0,0,0,2,0,0,0,1},
        {1,0,0,0,0,0,0,0,2,2,2,0,0,2,0,2,0,0,2,2,1},
        {1,0,0,0,0,0,0,0,2,0,2,0,0,0,2,0,0,0,2,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Bản đồ gốc 9
    const uint8_t originalMap9[MAP_HEIGHT][MAP_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,2,0,2,0,0,0,0,0,0,0,0,0,1,1,0,2,0,0,1},
        {1,2,2,0,2,0,0,0,2,2,0,0,0,2,2,1,0,2,0,0,1},
        {1,0,0,0,2,0,0,2,0,0,2,0,2,1,0,2,0,2,0,0,1},
        {1,2,2,2,2,0,0,2,0,0,0,2,1,0,0,2,0,2,0,0,1},
        {1,0,0,0,0,0,0,0,2,0,0,1,0,0,2,0,2,2,0,0,1},
        {1,2,2,2,2,2,2,2,0,2,1,0,0,2,0,2,2,2,0,0,1},
        {1,0,0,0,0,0,0,0,1,1,2,0,2,0,2,2,2,2,0,0,1},
        {1,0,0,0,0,0,0,0,0,1,0,2,0,2,2,2,2,2,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // Bản đồ hiện tại
    static uint8_t map[MAP_HEIGHT][MAP_WIDTH];

    // Khởi tạo bản đồ lần đầu
    static bool initialized = false;
    if (!initialized) {
        memcpy(map, originalMap1, sizeof(originalMap1));
        initialized = true;

        // Gán bitmap ngẫu nhiên cho mỗi kẻ địch
        for (int i = 0; i < MAX_ENEMIES; i++) {
            int randomBitmap = random(1, 6); // Chọn ngẫu nhiên từ boom1 đến boom5
            switch (randomBitmap) {
                case 1: enemyBitmaps[i] = boom1bitmap6x6; break;
                case 2: enemyBitmaps[i] = boom2bitmap6x6; break;
                case 3: enemyBitmaps[i] = boom3bitmap6x6; break;
                case 4: enemyBitmaps[i] = boom4bitmap6x6; break;
                case 5: enemyBitmaps[i] = boom5bitmap6x6; break;
            }
        }

        // Kích hoạt kẻ địch đầu tiên
        enemyAlive[0] = true;
    }

    // Điều khiển nhân vật với tốc độ chậm hơn
    static int moveDelay = 0; // Bộ đếm để giảm tốc độ di chuyển
    moveDelay++;
    if (moveDelay >= 5) { // Chỉ cho phép di chuyển mỗi 5 lần lặp
        bool isBlocked = false;

        // Kiểm tra nếu vị trí mới trùng với bất kỳ quả bom nào
        auto isPositionBlockedByBomb = [&](int newY, int newX) {
            for (int i = 0; i < 10; i++) {
                if (playerBombTimer[i] > 0 && playerBombY[i] == newY && playerBombX[i] == newX) {
                    return true;
                }
            }
            return false;
        };

        if (digitalRead(UP_BUTTON) == LOW && playerY > 1 && map[playerY - 1][playerX] == 0 && !isPositionBlockedByBomb(playerY - 1, playerX)) {
            playerY--;
        }
        if (digitalRead(DOWN_BUTTON) == LOW && playerY < MAP_HEIGHT - 2 && map[playerY + 1][playerX] == 0 && !isPositionBlockedByBomb(playerY + 1, playerX)) {
            playerY++;
        }
         if (digitalRead(LEFT_BUTTON) == LOW && playerX > 1 && map[playerY][playerX - 1] == 0 && !isPositionBlockedByBomb(playerY, playerX - 1)) {
            playerX--;
        }
        if (digitalRead(RIGHT_BUTTON) == LOW && playerX < MAP_WIDTH - 2 && map[playerY][playerX + 1] == 0 && !isPositionBlockedByBomb(playerY, playerX + 1)) {
            playerX++;
        }
        moveDelay = 0; // Reset bộ đếm
     }

    // Đặt bom người chơi
    if (digitalRead(ACTION_BUTTON) == LOW && playerCurrentBombs < playerMaxBombs) {
        for (int i = 0; i < 10; i++) { // Tìm vị trí trống trong mảng bom
            if (playerBombTimer[i] == 0) { // Nếu vị trí này chưa có bom
                if (map[playerY][playerX] == 0) { // Không cho phép đặt bom chồng lên nhau
                    playerBombX[i] = playerX;
                    playerBombY[i] = playerY;
                    playerBombTimer[i] = BOMB_DELAY;

                    // Hiển thị bom ngay lập tức
                    map[playerBombY[i]][playerBombX[i]] = 3; // 3 đại diện cho bom
                    playerCurrentBombs++; // Tăng số lượng bom hiện tại
                }
                break; // Thoát khỏi vòng lặp sau khi đặt bom
            }
        }
    }

    // Xử lý bom người chơi
    for (int i = 0; i < 10; i++) {
        if (playerBombTimer[i] > 0) {
            playerBombTimer[i]--;
            if (playerBombTimer[i] == 0) {
                // Gây nổ tại tâm
                explosionMap[playerBombY[i]][playerBombX[i]] = 'x';
                explosionTimer[playerBombY[i]][playerBombX[i]] = EXPLOSION_DURATION;

                int dx[] = {0, 0, -1, 1};
                int dy[] = {-1, 1, 0, 0};
                for (int dir = 0; dir < 4; dir++) {
                    for (int r = 1; r <= EXPLOSION_RANGE + explosionRangeBonus - 2; r++) { // Tăng phạm vi vụ nổ
                        int tx = playerBombX[i] + dx[dir] * r;
                        int ty = playerBombY[i] + dy[dir] * r;
                        if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT) break;
                        if (map[ty][tx] == 1) break;
                        explosionMap[ty][tx] = 'x';
                        explosionTimer[ty][tx] = EXPLOSION_DURATION;

                        if (map[ty][tx] == 2) { // Phá gạch
                            map[ty][tx] = 0;

                            // Ngẫu nhiên xuất hiện vật phẩm VP1 hoặc VP2
                            if (random(0, 100) < 40) { // 40% cơ hội xuất hiện vật phẩm
                                itemActive[ty][tx] = true;
                                itemType[ty][tx] = (random(0, 2) == 0); // `false` là VP1, `true` là VP2
                            }
                            break;
                        }
  
                        // Giết kẻ địch
                        for (int j = 0; j < totalEnemies; j++) {
                            if (enemyAlive[j] && enemyX[j] == tx && enemyY[j] == ty) {
                                enemyAlive[j] = false;
                                enemiesKilled++; // Tăng số lượng kẻ địch đã tiêu diệt
                             }
                        }
                    }
                }
                map[playerBombY[i]][playerBombX[i]] = 0; // Xóa bom sau khi nổ
                playerBombX[i] = -1;
                playerBombY[i] = -1;
                playerCurrentBombs--; // Giảm số lượng bom hiện tại
            }
        }
    }

    // Di chuyển kẻ địch ngẫu nhiên với tốc độ chậm hơn
    static int enemyMoveDelay = 0; // Bộ đếm để giảm tốc độ di chuyển của kẻ địch
    enemyMoveDelay++;
    if (enemyMoveDelay >= 10) { // Chỉ cho phép di chuyển mỗi 10 lần lặp
        for (int i = 0; i < totalEnemies; i++) {
            if (enemyAlive[i]) {
                int direction = random(0, 4); // 0: lên, 1: xuống, 2: trái, 3: phải
                int newX = enemyX[i];
                int newY = enemyY[i];

                if (direction == 0 && newY > 1 && map[newY - 1][newX] == 0) newY--;
                else if (direction == 1 && newY < MAP_HEIGHT - 2 && map[newY + 1][newX] == 0) newY++;
                else if (direction == 2 && newX > 1 && map[newY][newX - 1] == 0) newX--;
                else if (direction == 3 && newX < MAP_WIDTH - 2 && map[newY][newX + 1] == 0) newX++;

                // Cập nhật vị trí nếu hợp lệ
                if (map[newY][newX] == 0) {
                    enemyX[i] = newX;
                    enemyY[i] = newY;
                }

                // Ngẫu nhiên đặt bom
                if (random(0, 100) < 10 && enemyBombX[i] == -1) { // 10% cơ hội đặt bom
                    enemyBombX[i] = enemyX[i];
                    enemyBombY[i] = enemyY[i];
                    enemyBombTimer[i] = BOMB_DELAY;
                    map[enemyBombY[i]][enemyBombX[i]] = 4; // 4 đại diện cho bom kẻ địch
                 }
            }
        }
        enemyMoveDelay = 0; // Reset bộ đếm
     }

    // Xử lý bom của kẻ địch
    for (int i = 0; i < totalEnemies; i++) {
        if (enemyBombTimer[i] > 0) {
            enemyBombTimer[i]--;
            if (enemyBombTimer[i] == 0) {
                // Gây nổ tại tâm
                explosionMap[enemyBombY[i]][enemyBombX[i]] = 'e';
                explosionTimer[enemyBombY[i]][enemyBombX[i]] = EXPLOSION_DURATION;

                int dx[] = {0, 0, -1, 1};
                int dy[] = {-1, 1, 0, 0};
                for (int dir = 0; dir < 4; dir++) {
                    for (int r = 1; r <= EXPLOSION_RANGE + enemyExplosionRangeBonus[i] - 2; r++) { // Tăng phạm vi vụ nổ riêng cho kẻ địch
                        int tx = enemyBombX[i] + dx[dir] * r;
                        int ty = enemyBombY[i] + dy[dir] * r;
                        if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT) break;
                        if (map[ty][tx] == 1) break;
                        explosionMap[ty][tx] = 'e';
                        explosionTimer[ty][tx] = EXPLOSION_DURATION;

                        if (map[ty][tx] == 2) { // Phá gạch
                            map[ty][tx] = 0;

                            // Ngẫu nhiên xuất hiện vật phẩm
                            if (random(0, 100) < 40) { // 40% cơ hội xuất hiện vật phẩm
                                itemActive[ty][tx] = true;
                            }
                            break;
                        }
  
                         // Kiểm tra nếu nhân vật trúng bom
                        if (playerX == tx && playerY == ty) {
                            // GAME OVER
                            display.clearDisplay();
                            display.setCursor(10, 25);
                            display.setTextSize(2);
                            display.println("GAME OVER");
                            display.display();
                            delay(2000);

                            // Reset game
                            level = 1;
                            totalEnemies = 1;
                            enemiesKilled = 0;
                            playerX = 1;
                            playerY = 1;
                            memcpy(map, originalMap9, sizeof(originalMap9));
                            memset(enemyBombX, -1, sizeof(enemyBombX));
                            memset(enemyBombY, -1, sizeof(enemyBombY));
                            memset(enemyBombTimer, 0, sizeof(enemyBombTimer));
                            memset(itemActive, false, sizeof(itemActive)); // Xóa tất cả vật phẩm
                            explosionRangeBonus = 0; // Reset phạm vi vụ nổ của nhân vật
                            memset(enemyExplosionRangeBonus, 0, sizeof(enemyExplosionRangeBonus)); // Reset phạm vi vụ nổ của kẻ địch
                            return;
                        }
                    }
                }
                map[enemyBombY[i]][enemyBombX[i]] = 0; // Xóa bom sau khi nổ
                enemyBombX[i] = -1;
                enemyBombY[i] = -1;
            }
        }
    }

    // Xử lý khi nhân vật hoặc kẻ địch ăn vật phẩm
    if (itemActive[playerY][playerX]) {
        if (itemType[playerY][playerX] == false) { // VP1: Tăng phạm vi vụ nổ
            explosionRangeBonus++;
        } else { // VP2: Tăng số lượng bom
            playerMaxBombs++;
        }
        itemActive[playerY][playerX] = false; // Vật phẩm biến mất
    }
    for (int i = 0; i < totalEnemies; i++) {
        if (enemyAlive[i] && itemActive[enemyY[i]][enemyX[i]]) {
            if (itemType[enemyY[i]][enemyX[i]] == false) { // VP1: Tăng phạm vi vụ nổ
                enemyExplosionRangeBonus[i]++;
            } else { // VP2: Tăng số lượng bom
                 enemyMaxBombs[i]++;
            }
            itemActive[enemyY[i]][enemyX[i]] = false; // Vật phẩm biến mất
        }
    }

    // Cập nhật hiệu ứng cháy nổ
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (explosionTimer[y][x] > 0) {
                explosionTimer[y][x]--;
                if (explosionTimer[y][x] == 0) {
                    explosionMap[y][x] = 0;
                }
            }
        }
    }

    // Qua màn nếu tiêu diệt hết địch
    bool allDead = true;
    for (int i = 0; i < totalEnemies; i++) {
        if (enemyAlive[i]) allDead = false;
    }
    if (allDead) {
        if (level < MAX_LEVEL) level++;
        totalEnemies = level;

        // Reset vị trí nhân vật
        playerX = 1;
        playerY = 1;

        // Reset phạm vi vụ nổ
        explosionRangeBonus = 0;

        // Reset số lượng bom
        playerMaxBombs = 1;
        playerCurrentBombs = 0;

        // Reset trạng thái bom của người chơi
        memset(playerBombX, -1, sizeof(playerBombX));
        memset(playerBombY, -1, sizeof(playerBombY));
        memset(playerBombTimer, 0, sizeof(playerBombTimer));

        // Reset trạng thái bom của kẻ địch
        memset(enemyBombX, -1, sizeof(enemyBombX));
        memset(enemyBombY, -1, sizeof(enemyBombY));
        memset(enemyBombTimer, 0, sizeof(enemyBombTimer));
        memset(enemyExplosionRangeBonus, 0, sizeof(enemyExplosionRangeBonus));

        // Reset trạng thái vật phẩm
        memset(itemActive, false, sizeof(itemActive));
        memset(itemType, false, sizeof(itemType));

        // Reset trạng thái kẻ địch
        for (int i = 0; i < MAX_ENEMIES; i++) {
            do {
                enemyX[i] = random(1, MAP_WIDTH - 1);
                enemyY[i] = random(2, MAP_HEIGHT - 1);
             } while (map[enemyY[i]][enemyX[i]] != 0 || (enemyX[i] == playerX && enemyY[i] == playerY));
            enemyAlive[i] = true;
  
            // Gán bitmap ngẫu nhiên cho kẻ địch mới
            int randomBitmap = random(1, 6);
            switch (randomBitmap) {
                case 1: enemyBitmaps[i] = boom1bitmap6x6; break;
                case 2: enemyBitmaps[i] = boom2bitmap6x6; break;
                case 3: enemyBitmaps[i] = boom3bitmap6x6; break;
                case 4: enemyBitmaps[i] = boom4bitmap6x6; break;
                case 5: enemyBitmaps[i] = boom5bitmap6x6; break;
             }
        }

        // Chọn bản đồ ngẫu nhiên
        int randomMap = random(1, 11); // Chọn ngẫu nhiên từ 0 đến 9
        switch (randomMap) {
            case 1: memcpy(map, originalMap0, sizeof(originalMap0)); break;
            case 2: memcpy(map, originalMap1, sizeof(originalMap1)); break;
            case 3: memcpy(map, originalMap2, sizeof(originalMap2)); break;
            case 4: memcpy(map, originalMap3, sizeof(originalMap3)); break;
            case 5: memcpy(map, originalMap4, sizeof(originalMap4)); break;
            case 6: memcpy(map, originalMap5, sizeof(originalMap5)); break;
            case 7: memcpy(map, originalMap6, sizeof(originalMap6)); break;
            case 8: memcpy(map, originalMap7, sizeof(originalMap7)); break;
            case 9: memcpy(map, originalMap8, sizeof(originalMap8)); break;
            case 10: memcpy(map, originalMap9, sizeof(originalMap9)); break;
        }
    }

    // Vẽ bản đồ
    display.clearDisplay();

    // Hiển thị Level và số lượng kẻ địch đã tiêu diệt
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Level: ");
    display.print(level);
    display.setCursor(70, 0);
    display.print("Kills: ");
    display.print(enemiesKilled);

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int drawY = y * 6 + 8; // Lùi xuống 8 pixel
            int drawX = x * 6;

            if (map[y][x] == 1) {
                display.drawBitmap(drawX, drawY, boom7bitmap6x6, 6, 6, SSD1306_WHITE);
            } else if (map[y][x] == 2) {
                display.drawBitmap(drawX, drawY, boom6bitmap6x6, 6, 6, SSD1306_WHITE);
            } else if (map[y][x] == 3) { // Hiển thị bom
                display.drawBitmap(drawX, drawY, boom0bitmap6x6, 6, 6, SSD1306_WHITE);
            } else if (explosionMap[y][x] == 'x') {
                display.drawBitmap(drawX, drawY, boom0bitmap6x6, 6, 6, SSD1306_WHITE);
            } else if (explosionMap[y][x] == 'e') { // Hiệu ứng cháy nổ của kẻ địch
            display.drawBitmap(drawX, drawY, boomXbitmap6x6, 6, 6, SSD1306_WHITE);
            }

             // Vẽ vật phẩm
            if (itemActive[y][x]) {
                if (itemType[y][x] == false) {
                    display.drawBitmap(drawX, drawY, VP1bitmap6x6, 6, 6, SSD1306_WHITE); // VP1
                } else {
                    display.drawBitmap(drawX, drawY, VP2bitmap6x6, 6, 6, SSD1306_WHITE); // VP2
                }
             }
        }
    }

    // Vẽ người chơi
    display.drawBitmap(playerX * 6, playerY * 6 + 8, boom2bitmap6x6, 6, 6, SSD1306_WHITE);

    // Vẽ kẻ địch
    for (int i = 0; i < totalEnemies; i++) {
        if (enemyAlive[i]) {
            display.drawBitmap(enemyX[i] * 6, enemyY[i] * 6 + 8, enemyBitmaps[i], 6, 6, SSD1306_WHITE);
        }
    }

    // Vẽ bom của kẻ địch
    for (int i = 0; i < totalEnemies; i++) {
        if (enemyBombX[i] != -1) {
            display.drawBitmap(enemyBombX[i] * 6, enemyBombY[i] * 6 + 8, boomXbitmap6x6, 6, 6, SSD1306_WHITE);
        }
    }

    display.display();
}

void playSnake() {
    const int GRID_SIZE = 4; // Kích thước mỗi ô lưới
    const int GRID_WIDTH = SCREEN_WIDTH / GRID_SIZE;
    const int GRID_HEIGHT = SCREEN_HEIGHT / GRID_SIZE;

    static int snakeX[100] = {GRID_WIDTH / 2}; // Vị trí X của từng đoạn rắn
    static int snakeY[100] = {GRID_HEIGHT / 2}; // Vị trí Y của từng đoạn rắn
    static int snakeLength = 3; // Chiều dài ban đầu của rắn
    static int foodX = random(0, GRID_WIDTH); // Vị trí X của thức ăn
    static int foodY = random(0, GRID_HEIGHT); // Vị trí Y của thức ăn
    static int direction = 0; // Hướng di chuyển: 0 = lên, 1 = phải, 2 = xuống, 3 = trái
    static bool gameOver = false;

    // Điều khiển hướng di chuyển
    if (digitalRead(UP_BUTTON) == LOW && direction != 2) direction = 0;
    if (digitalRead(RIGHT_BUTTON) == LOW && direction != 3) direction = 1;
    if (digitalRead(DOWN_BUTTON) == LOW && direction != 0) direction = 2;
    if (digitalRead(LEFT_BUTTON) == LOW && direction != 1) direction = 3;

    // Di chuyển rắn
    for (int i = snakeLength - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }
    if (direction == 0) snakeY[0]--; // Lên
    if (direction == 1) snakeX[0]++; // Phải
    if (direction == 2) snakeY[0]++; // Xuống
    if (direction == 3) snakeX[0]--; // Trái

    // Kiểm tra va chạm với tường
    if (snakeX[0] < 0 || snakeX[0] >= GRID_WIDTH || snakeY[0] < 0 || snakeY[0] >= GRID_HEIGHT) {
        gameOver = true;
    }

    // Kiểm tra va chạm với chính rắn
    for (int i = 1; i < snakeLength; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            gameOver = true;
        }
    }

    // Kiểm tra nếu rắn ăn thức ăn
    if (snakeX[0] == foodX && snakeY[0] == foodY) {
        snakeLength++;
        foodX = random(0, GRID_WIDTH);
        foodY = random(0, GRID_HEIGHT);
    }

    // Xử lý khi trò chơi kết thúc
    if (gameOver) {
        display.clearDisplay();
        display.setCursor(10, 25);
        display.setTextSize(2);
        display.println("GAME OVER");
        display.display();
        delay(2000);

        // Reset trò chơi
        snakeLength = 3;
        snakeX[0] = GRID_WIDTH / 2;
        snakeY[0] = GRID_HEIGHT / 2;
        direction = 0;
        foodX = random(0, GRID_WIDTH);
        foodY = random(0, GRID_HEIGHT);
        gameOver = false;
        return;
    }

    // Vẽ màn hình
    display.clearDisplay();

    // Vẽ rắn
    for (int i = 0; i < snakeLength; i++) {
        display.fillRect(snakeX[i] * GRID_SIZE, snakeY[i] * GRID_SIZE, GRID_SIZE, GRID_SIZE, SSD1306_WHITE);
    }

    // Vẽ thức ăn
    display.fillRect(foodX * GRID_SIZE, foodY * GRID_SIZE, GRID_SIZE, GRID_SIZE, SSD1306_WHITE);

    display.display();
    delay(100); // Điều chỉnh tốc độ rắn
}

void playDaoVang() {
  const int HOOK_SPEED = 2; // Tốc độ di chuyển của móc câu
  const int MAX_HOOK_LENGTH = 80; // Chiều dài tối đa của móc câu
  const int MIN_HOOK_LENGTH = 10; // Chiều dài tối thiểu của móc câu
  const int GOLD_RADIUS = 4; // Bán kính của vàng
  const int SMALL_GOLD_RADIUS = 2; // Bán kính của vàng mới
  const int DIAMOND_SIZE = 2; // Kích thước của kim cương
  const int LEVEL_COUNT = 17; // Số lượng cấp độ

  // Cấu hình cho từng cấp độ
  const int levelTime[LEVEL_COUNT] = {60, 65, 70, 75, 80, 60, 65, 70, 75, 80, 81, 82, 84, 87, 91, 96, 102}; // Thời gian tối đa cho từng cấp độ (giây)
  const int levelScore[LEVEL_COUNT] = {70, 80, 90, 100, 110, 80, 100, 120, 140, 160, 170, 180, 190, 200, 210, 220, 230}; // Điểm cần đạt để qua cấp độ
  const int levelGoldCount[LEVEL_COUNT] = {8, 9, 10, 11, 12, 8, 9, 10, 11, 12, 12, 12, 12, 12, 12, 12, 12}; // Số lượng vàng cho từng cấp độ

  static int hookX = SCREEN_WIDTH / 2; // Vị trí X của móc câu
  static int hookY = 9; // Vị trí Y của móc câu
  static int hookLength = MIN_HOOK_LENGTH; // Chiều dài hiện tại của móc câu
  static int hookAngle = 90; // Góc của móc câu (0-180 độ)
  static bool isHookExtending = false; // Trạng thái móc câu đang kéo dài
  static bool isHookRetracting = false; // Trạng thái móc câu đang thu lại
  static int score = 0; // Điểm số của người chơi
  static int level = 1; // Cấp độ hiện tại
  static bool gameOver = false;

  // Vị trí của vàng
  static int goldX[12]; // Tối đa 12 vàng (Level 5)
  static int goldY[12];
  static bool goldCollected[12] = {false};
  static bool goldMoving[12] = {false}; // Trạng thái vàng di chuyển
  static int goldDirection[12] = {1}; // Hướng di chuyển của vàng (1: phải, -1: trái)

  // Vị trí của vàng mới
  static int smallGoldX;
  static int smallGoldY;
  static bool smallGoldCollected = false;
  static int smallGoldDirection = 1; // Hướng di chuyển của vàng mới (1: phải, -1: trái)

  // Vị trí của kim cương
  static int diamondX;
  static int diamondY;
  static bool diamondCollected = false;

  static int attachedGoldIndex = -1; // Chỉ số của vàng đang được kéo (nếu có)
  static bool attachedSmallGold = false; // Trạng thái kéo vàng mới
  static bool attachedDiamond = false; // Trạng thái kéo kim cương

  // Thời gian
  static unsigned long startTime = millis(); // Thời gian bắt đầu cấp độ

  // Khởi tạo vị trí vàng, vàng mới và kim cương
  static bool initialized = false;
  if (!initialized) {
      for (int i = 0; i < levelGoldCount[level - 1]; i++) {
          goldX[i] = random(2, SCREEN_WIDTH - 2);
          goldY[i] = random(23, SCREEN_HEIGHT - 2);
          goldCollected[i] = false; // Đặt lại trạng thái vàng chưa được thu thập
          goldMoving[i] = false; // Mặc định vàng không di chuyển
      }

      // Từ Level 6 trở đi, chọn ngẫu nhiên vàng để di chuyển
      if (level >= 6) {
          int movingGoldCount = level - 5; // Số lượng vàng di chuyển
          for (int i = 0; i < movingGoldCount; i++) {
              int randomIndex;
              do {
                  randomIndex = random(0, levelGoldCount[level - 1]);
              } while (goldMoving[randomIndex]); // Đảm bảo không chọn trùng
              goldMoving[randomIndex] = true; // Đánh dấu vàng này sẽ di chuyển
              goldDirection[randomIndex] = (random(0, 2) == 0) ? 1 : -1; // Hướng ngẫu nhiên
          }
      }

      // Khởi tạo vị trí vàng mới
      smallGoldX = random(2, SCREEN_WIDTH - 2);
      smallGoldY = random(23, SCREEN_HEIGHT - 2);
      smallGoldCollected = false;

      // Khởi tạo vị trí kim cương
      diamondX = random(2, SCREEN_WIDTH - 2);
      diamondY = random(23, SCREEN_HEIGHT - 2);
      diamondCollected = false;

      initialized = true;
  }

  // Tự động điều chỉnh góc của móc câu
  if (!isHookExtending && !isHookRetracting) {
      static bool angleIncreasing = true; // Góc đang tăng hay giảm
      if (angleIncreasing) {
          hookAngle += 2;
          if (hookAngle >= 175) angleIncreasing = false;
      } else {
          hookAngle -= 2;
          if (hookAngle <= 5) angleIncreasing = true;
      }
  }

  // Kéo dài móc câu khi nhấn nút ACTION
  if (digitalRead(ACTION_BUTTON) == LOW && !isHookRetracting) {
      isHookExtending = true;
  }

  // Cập nhật trạng thái móc câu
  if (isHookExtending) {
      hookLength += HOOK_SPEED;
      if (hookLength >= MAX_HOOK_LENGTH) {
          isHookExtending = false;
          isHookRetracting = true;
      }
  } else if (isHookRetracting) {
      hookLength -= HOOK_SPEED / 2;

      // Nếu có vàng được kéo, cập nhật vị trí vàng theo móc câu
      if (isHookRetracting) {
          hookLength -= HOOK_SPEED;

          if (attachedGoldIndex != -1) {
              goldX[attachedGoldIndex] = hookX;
              goldY[attachedGoldIndex] = hookY;
          } else if (attachedSmallGold) {
              smallGoldX = hookX;
              smallGoldY = hookY;
          } else if (attachedDiamond) {
              diamondX = hookX;
              diamondY = hookY;
          }

              if (hookLength <= MIN_HOOK_LENGTH) {
        isHookRetracting = false;
    }
}

      // Khi móc câu thu về hết cỡ, xử lý vàng
      if (hookLength <= MIN_HOOK_LENGTH) {
          if (attachedGoldIndex != -1) {
              goldCollected[attachedGoldIndex] = true; // Đánh dấu vàng đã được thu thập
              score += goldMoving[attachedGoldIndex] ? 20 : 10; // Vàng di chuyển có giá trị 20 điểm, vàng thường 10 điểm
              attachedGoldIndex = -1; // Giải phóng vàng
          } else if (attachedSmallGold) {
              smallGoldCollected = true; // Đánh dấu vàng mới đã được thu thập
              score += 40; // Vàng mới 40 điểm
              attachedSmallGold = false; // Giải phóng vàng mới
          } else if (attachedDiamond) {
              diamondCollected = true; // Đánh dấu kim cương đã được thu thập
              score += 50; // Kim cương 50 điểm
              attachedDiamond = false; // Giải phóng kim cương
          }
          isHookRetracting = false;
      }
  }

  // Tính toán vị trí của móc câu
  hookX = (SCREEN_WIDTH / 2) + hookLength * cos(radians(hookAngle));
  hookY = 9 + hookLength * sin(radians(hookAngle));

  // Cập nhật vị trí vàng mới di chuyển
  if (!smallGoldCollected) {
      smallGoldX += smallGoldDirection * 2; // Vàng mới di chuyển nhanh hơn
      if (smallGoldX <= 10 || smallGoldX >= SCREEN_WIDTH - 10) {
          smallGoldDirection *= -1; // Đổi hướng khi chạm mép
      }
  }

  // Cập nhật vị trí vàng di chuyển
  for (int i = 0; i < levelGoldCount[level - 1]; i++) {
    if (goldMoving[i]) {
        goldX[i] += goldDirection[i];
        if (goldX[i] <= 10 || goldX[i] >= SCREEN_WIDTH - 10) {
            goldDirection[i] *= -1; // Đổi hướng khi chạm mép
        }
    }
  }

  // Kiểm tra va chạm giữa móc câu và vàng
  if (isHookExtending && attachedGoldIndex == -1 && !attachedSmallGold && !attachedDiamond) {
      for (int i = 0; i < levelGoldCount[level - 1]; i++) {
          if (!goldCollected[i] &&
              abs(hookX - goldX[i]) < GOLD_RADIUS &&
              abs(hookY - goldY[i]) < GOLD_RADIUS) {
              attachedGoldIndex = i; // Gắn vàng vào móc câu
              isHookExtending = false; // Dừng kéo dài móc câu
              isHookRetracting = true; // Bắt đầu thu móc câu
              break;
          }
      }

      // Kiểm tra va chạm với vàng mới
      if (!smallGoldCollected &&
          abs(hookX - smallGoldX) < SMALL_GOLD_RADIUS &&
          abs(hookY - smallGoldY) < SMALL_GOLD_RADIUS) {
          attachedSmallGold = true; // Gắn vàng mới vào móc câu
          isHookExtending = false;
          isHookRetracting = true;
      }

      // Kiểm tra va chạm với kim cương
      if (!diamondCollected &&
          abs(hookX - diamondX) < DIAMOND_SIZE &&
          abs(hookY - diamondY) < DIAMOND_SIZE) {
          attachedDiamond = true; // Gắn kim cương vào móc câu
          isHookExtending = false;
          isHookRetracting = true;
      }
  }

  // Kiểm tra nếu đạt đủ điểm để qua cấp độ
  if (score >= levelScore[level - 1]) {
      level++;
      if (level > LEVEL_COUNT) {
          // Người chơi thắng trò chơi
          display.clearDisplay();
          display.setCursor(10, 25);
          display.setTextSize(2);
          display.println("YOU WIN!");
          display.display();
          delay(3000);

          // Reset trò chơi
          level = 1;
          score = 0;
          initialized = false;
          hookAngle = 90; // Reset góc của móc câu
          startTime = millis();
          return;
      }

      // Qua cấp độ mới
      score = 0; // Reset điểm số
      initialized = false; // Reset trạng thái khởi tạo vàng
      hookAngle = 90;
      startTime = millis();
      return;
  }

  // Kiểm tra thời gian
  unsigned long elapsedTime = (millis() - startTime) / 1000; // Thời gian đã trôi qua (giây)
  if (elapsedTime >= levelTime[level - 1]) {
      // Hết thời gian, trò chơi kết thúc
      gameOver = true;
  }

  // Xử lý khi trò chơi kết thúc
  if (gameOver) {
      display.clearDisplay();
      display.setCursor(10, 25);
      display.setTextSize(2);
      display.println("GAME OVER");
      display.display();
      delay(3000);

      // Reset trò chơi
      level = 1;
      score = 0;
      initialized = false;
      hookAngle = 90;
      startTime = millis();
      gameOver = false;
      return;
  }

  // Vẽ màn hình
  display.clearDisplay();

  // Vẽ người đàn ông
  display.drawBitmap(51, 0, Menbitmap16x16, 16, 16, SSD1306_WHITE);

  // Vẽ vàng
  for (int i = 0; i < levelGoldCount[level - 1]; i++) {
      if (!goldCollected[i]) {
          display.fillCircle(goldX[i], goldY[i], GOLD_RADIUS, SSD1306_WHITE);
      }
  }

  // Vẽ vàng mới
  if (!smallGoldCollected) {
      display.fillCircle(smallGoldX, smallGoldY, SMALL_GOLD_RADIUS, SSD1306_WHITE);
  }

  // Vẽ kim cương
  if (!diamondCollected) {
      display.fillRect(diamondX, diamondY, DIAMOND_SIZE, DIAMOND_SIZE, SSD1306_WHITE);
  }

  // Vẽ móc câu
  display.drawLine(SCREEN_WIDTH / 2, 9, hookX, hookY, SSD1306_WHITE);
  display.fillCircle(hookX, hookY, 2, SSD1306_WHITE);

  // Hiển thị điểm số
  display.setCursor(88, 0);
  display.setTextSize(1);
  display.print("");
  display.print(score);

  // Hiển thị thời gian còn lại
  display.setCursor(110, 0);
  display.print("");
  display.print(levelTime[level - 1] - elapsedTime);

  // Hiển thị cấp độ
  display.setCursor(0, 0);
  display.print("Level:");
  display.print(level);

  display.display();
  delay(20);
}

void playTetris() {
  const int GRID_WIDTH = 24;
  const int GRID_HEIGHT = 10; // Đổi GRID_HEIGHT thành 10 (trục X cũ)
  const int BLOCK_SIZE = 5;
  const int SCREEN_OFFSET_X = 5;
  const int SCREEN_OFFSET_Y = 5;

  static int grid[GRID_WIDTH][GRID_HEIGHT] = {0}; // Đổi chiều mảng grid

  const int tetrominoes[5][4][4][4] = {
      // I
      {{{0, 0, 0, 0}, 
        {1, 1, 1, 1}, 
        {0, 0, 0, 0}, 
        {0, 0, 0, 0}},

       {{0, 1, 0, 0}, 
        {0, 1, 0, 0}, 
        {0, 1, 0, 0}, 
        {0, 1, 0, 0}},

       {{0, 0, 0, 0}, 
        {1, 1, 1, 1}, 
        {0, 0, 0, 0}, 
        {0, 0, 0, 0}},
       
       {{0, 1, 0, 0}, 
        {0, 1, 0, 0}, 
        {0, 1, 0, 0}, 
        {0, 1, 0, 0}}},

      // T
      {{{0, 1, 0, 0}, 
        {1, 1, 1, 0}, 
        {0, 0, 0, 0}, 
        {0, 0, 0, 0}},
       
       {{0, 1, 0, 0}, 
        {1, 1, 0, 0}, 
        {0, 1, 0, 0}, 
        {0, 0, 0, 0}},
       
       {{1, 1, 1, 0}, 
        {0, 1, 0, 0}, 
        {0, 0, 0, 0}, 
        {0, 0, 0, 0}},
       
       {{0, 1, 0, 0}, 
        {0, 1, 1, 0}, 
        {0, 1, 0, 0}, 
        {0, 0, 0, 0}}},
      // O
      {{{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
       {{1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
     
       // L
      {{{1, 0, 0, 0}, 
        {1, 0, 0, 0}, 
        {1, 1, 0, 0}, 
        {0, 0, 0, 0}},
       
       {{1, 1, 1, 0}, 
        {1, 0, 0, 0}, 
        {0, 0, 0, 0}, 
        {0, 0, 0, 0}},
       
       {{1, 1, 0, 0}, 
        {0, 1, 0, 0}, 
        {0, 1, 0, 0}, 
        {0, 0, 0, 0}},
       
       {{0, 0, 1, 0}, 
        {1, 1, 1, 0}, 
        {0, 0, 0, 0}, 
        {0, 0, 0, 0}}},
      
      // Z
      {{{1, 1, 0, 0}, 
        {0, 1, 1, 0}, 
        {0, 0, 0, 0}, 
        {0, 0, 0, 0}},
       
       {{0, 1, 0, 0}, 
        {1, 1, 0, 0}, 
        {1, 0, 0, 0}, 
        {0, 0, 0, 0}},
       
       {{1, 1, 0, 0}, 
        {0, 1, 1, 0}, 
        {0, 0, 0, 0}, 
        {0, 0, 0, 0}},
       
       {{0, 1, 0, 0}, 
        {1, 1, 0, 0}, 
        {1, 0, 0, 0}, 
        {0, 0, 0, 0}}}
  };

  static int currentTetromino = 0;
  static int rotation = 0;
  static int posX = GRID_HEIGHT - 4; // Bắt đầu từ bên phải
  static int posY = GRID_WIDTH - 1; // Bắt đầu từ dưới cùng
  static bool gameOver = false;

  auto checkCollision = [&](int x, int y, int rot) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (tetrominoes[currentTetromino][rot][i][j]) {
          int newX = x - j; // Đảo ngược trục X
          int newY = y - i; // Đảo ngược trục Y
          if (newX < 0 || newX >= GRID_HEIGHT || newY < 0 || newY >= GRID_WIDTH || grid[newY][newX]) {
            return true;
          }
        }
      }
    }
    return false;
  };

  auto lockTetromino = [&]() {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (tetrominoes[currentTetromino][rotation][i][j]) {
          grid[posY - i][posX - j] = 1; // Đảo ngược trục X và Y
        }
      }
    }
  };

  auto clearLines = [&]() {
    for (int i = 0; i < GRID_WIDTH; i++) { // Duyệt từ dòng trên cùng xuống
        bool full = true;
        for (int j = 0; j < GRID_HEIGHT; j++) {
            if (!grid[i][j]) {
                full = false;
                break;
            }
        }
        if (full) {
            // Xóa dòng đầy
            for (int k = i; k < GRID_WIDTH - 1; k++) { // Dịch các dòng phía dưới lên
                for (int j = 0; j < GRID_HEIGHT; j++) {
                    grid[k][j] = grid[k + 1][j];
                }
            }
            // Xóa dòng cuối cùng (sau khi dịch lên)
            for (int j = 0; j < GRID_HEIGHT; j++) {
                grid[GRID_WIDTH - 1][j] = 0;
            }
            i--; // Kiểm tra lại dòng hiện tại sau khi dịch lên
        }
    }
};

  // Đọc nút điều khiển
  if (digitalRead(UP_BUTTON) == LOW && !checkCollision(posX - 1, posY, rotation)) {
    posX--;
    delay(200);
  }
  if (digitalRead(DOWN_BUTTON) == LOW && !checkCollision(posX + 1, posY, rotation)) {
    posX++;
    delay(200);
  }
  if (digitalRead(LEFT_BUTTON) == LOW && !checkCollision(posX, posY - 1, rotation)) {
    posY--;
    delay(200);
  }
  if (digitalRead(ACTION_BUTTON) == LOW) {
    int newRotation = (rotation + 1) % 4;
    if (!checkCollision(posX, posY, newRotation)) {
      rotation = newRotation;
    }
    delay(200);
  }

  // Rơi tự động
  static unsigned long lastDropTime = 0;
  if (millis() - lastDropTime > 500) {
    if (!checkCollision(posX, posY - 1, rotation)) {
      posY--;
    } else {
      lockTetromino();
      clearLines();
      currentTetromino = random(0, 5);
      rotation = 0;
      posX = GRID_HEIGHT - 4; // Bắt đầu từ bên phải
      posY = GRID_WIDTH - 1;  // Bắt đầu từ dưới cùng
      if (checkCollision(posX, posY, rotation)) {
        gameOver = true;
      }
    }
    lastDropTime = millis();
  }

  // Vẽ màn hình
  display.clearDisplay();

  // Vẽ ô bao trọn lưới
  display.drawRect(SCREEN_OFFSET_X - 2, SCREEN_OFFSET_Y - 2, GRID_WIDTH * BLOCK_SIZE + 2, GRID_HEIGHT * BLOCK_SIZE + 3, SSD1306_WHITE);

  // Vẽ lưới đã cố định
  for (int y = 0; y < GRID_WIDTH; y++) {
    for (int x = 0; x < GRID_HEIGHT; x++) {
      if (grid[y][x]) {
        display.fillRect(SCREEN_OFFSET_X + y * BLOCK_SIZE, SCREEN_OFFSET_Y + x * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1, WHITE);
      }
    }
  }

  // Vẽ khối hiện tại
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (tetrominoes[currentTetromino][rotation][i][j]) {
        int x = posX - j; // Đảo ngược trục X
        int y = posY - i; // Đảo ngược trục Y
        display.fillRect(SCREEN_OFFSET_X + y * BLOCK_SIZE, SCREEN_OFFSET_Y + x * BLOCK_SIZE, BLOCK_SIZE - 1, BLOCK_SIZE - 1, WHITE);
      }
    }
  }

  if (gameOver) {
    display.setTextSize(1);
    display.setCursor(10, 30);
    display.println("GAME OVER");
    display.display();
    delay(2000); // Hiển thị "GAME OVER" trong 2 giây

    // Reset trò chơi
    memset(grid, 0, sizeof(grid));
    currentTetromino = random(0, 5);
    rotation = 0;
    posX = GRID_HEIGHT - 1; // Bắt đầu từ bên phải
    posY = GRID_WIDTH - 1;  // Bắt đầu từ dưới cùng
    gameOver = false;
  }

  display.display();
}

void playNote(int freq, int duration) {
    if (freq == 0) {
        noTone(BUZZER_PIN);
    } else {
        tone(BUZZER_PIN, freq);
    }

    unsigned long startTime = millis();
    while (millis() - startTime < duration) {
        // Kiểm tra nút BACK trong khi phát nốt
        if (digitalRead(BACK_BUTTON) == LOW) {
            isPlayingMusic = false;
            gameState = 0;
            mainMenuState = 4;
            noTone(BUZZER_PIN);
            delay(300); // Chống dội phím
            return;
        }
    }

    noTone(BUZZER_PIN);
    delay(20); // Nghỉ giữa các nốt
}

void playMusic(int music) {
    isPlayingMusic = true;

    while (isPlayingMusic) {

      if (digitalRead(BACK_BUTTON) == LOW) {
            isPlayingMusic = false;
            gameState = 0;
            mainMenuState = 4;
            delay(300);
            break;
        }

        if (music == 0) { // Happy Birthday
            playNote(264, 250); if (!isPlayingMusic) return; displayBass(1);
            playNote(264, 250); if (!isPlayingMusic) return; displayBass(1);
            playNote(297, 500); if (!isPlayingMusic) return; displayBass(2);
            playNote(264, 500); if (!isPlayingMusic) return; displayBass(2);
            playNote(352, 500); if (!isPlayingMusic) return; displayBass(3);
            playNote(330, 1000); if (!isPlayingMusic) return; displayBass(3);

            playNote(264, 250); if (!isPlayingMusic) return; displayBass(1);
            playNote(264, 250); if (!isPlayingMusic) return; displayBass(1);
            playNote(297, 500); if (!isPlayingMusic) return; displayBass(2);
            playNote(264, 500); if (!isPlayingMusic) return; displayBass(2);
            playNote(396, 500); if (!isPlayingMusic) return; displayBass(4);
            playNote(352, 1000); if (!isPlayingMusic) return; displayBass(3);

            playNote(264, 250); if (!isPlayingMusic) return; displayBass(1);
            playNote(264, 250); if (!isPlayingMusic) return; displayBass(1);
            playNote(523, 500); if (!isPlayingMusic) return; displayBass(5);
            playNote(440, 500); if (!isPlayingMusic) return; displayBass(4);
            playNote(352, 500); if (!isPlayingMusic) return; displayBass(3);
            playNote(330, 500); if (!isPlayingMusic) return; displayBass(3);
            playNote(297, 1000); if (!isPlayingMusic) return; displayBass(2);

            playNote(466, 250); if (!isPlayingMusic) return; displayBass(4);
            playNote(466, 250); if (!isPlayingMusic) return; displayBass(4);
            playNote(440, 500); if (!isPlayingMusic) return; displayBass(4);
            playNote(352, 500); if (!isPlayingMusic) return; displayBass(3);
            playNote(396, 500); if (!isPlayingMusic) return; displayBass(4);
            playNote(352, 1000); if (!isPlayingMusic) return; displayBass(3);
        } 
        else if (music == 1) {
            // "Ay, iyaiyai"
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(2); // Ay
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(2); // iyaiyai
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(2); // Ay
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(2); // iyaiyai
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(2); // Ay
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(2); // iyaiyai

            // "Where's my samurai"
            playNote(392, 250); if (!isPlayingMusic) return; displayBass(2); // Where's
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(3); // my
            playNote(392, 250); if (!isPlayingMusic) return; displayBass(3); // samurai
            playNote(330, 500); if (!isPlayingMusic) return; displayBass(4); // ?

            // "I've been searching for a man"
            playNote(294, 250); if (!isPlayingMusic) return; displayBass(3); // I've
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(4); // been
            playNote(349, 250); if (!isPlayingMusic) return; displayBass(4); // searching
            playNote(392, 500); if (!isPlayingMusic) return; displayBass(5); // for

            // "All across Japan"
            playNote(294, 250); if (!isPlayingMusic) return; displayBass(3); // All
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(4); // across
            playNote(349, 250); if (!isPlayingMusic) return; displayBass(4); // Japan
            playNote(392, 500); if (!isPlayingMusic) return; displayBass(5); // ?

            // "Just to find, to find my samurai"
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(4); // Just
            playNote(392, 250); if (!isPlayingMusic) return; displayBass(5); // to
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(4); // find
            playNote(294, 500); if (!isPlayingMusic) return; displayBass(3); // my

            // "Someone who is strong"
            playNote(392, 250); if (!isPlayingMusic) return; displayBass(5); // Someone
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(6); // who
            playNote(392, 250); if (!isPlayingMusic) return; displayBass(5); // is
            playNote(330, 500); if (!isPlayingMusic) return; displayBass(4); // strong

            // "But still a little shy"
            playNote(294, 250); if (!isPlayingMusic) return; displayBass(3); // But
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(4); // still
            playNote(349, 250); if (!isPlayingMusic) return; displayBass(4); // a
            playNote(392, 500); if (!isPlayingMusic) return; displayBass(5); // little

            // "Yes I need, I need my samurai"
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(4); // Yes
            playNote(392, 250); if (!isPlayingMusic) return; displayBass(5); // I
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(4); // need
            playNote(294, 500); if (!isPlayingMusic) return; displayBass(3); // my

            // Phần điệp khúc
            playNote(392, 250); if (!isPlayingMusic) return; displayBass(2); // I'm
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(3); // your
            playNote(392, 250); if (!isPlayingMusic) return; displayBass(3); // little
            playNote(330, 500); if (!isPlayingMusic) return; displayBass(4); // butterfly

            playNote(294, 250); if (!isPlayingMusic) return; displayBass(3); // Green
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(4); // black
            playNote(349, 250); if (!isPlayingMusic) return; displayBass(4); // and
            playNote(392, 500); if (!isPlayingMusic) return; displayBass(5); // blue

            playNote(440, 250); if (!isPlayingMusic) return; displayBass(4); // Make
            playNote(392, 250); if (!isPlayingMusic) return; displayBass(5); // the
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(4); // colors
            playNote(294, 500); if (!isPlayingMusic) return; displayBass(3); // in

            playNote(392, 250); if (!isPlayingMusic) return; displayBass(2); // the
            playNote(440, 250); if (!isPlayingMusic) return; displayBass(3); // sky
        }

        else if (music == 2) { // Twinkle Twinkle Little Star
            playNote(264, 500); if (!isPlayingMusic) return; displayBass(1);  // C
            playNote(264, 500); if (!isPlayingMusic) return; displayBass(1);  // C
            playNote(396, 500); if (!isPlayingMusic) return; displayBass(4);  // G
            playNote(396, 500); if (!isPlayingMusic) return; displayBass(4);  // G
            playNote(440, 500); if (!isPlayingMusic) return; displayBass(5);  // A
            playNote(440, 500); if (!isPlayingMusic) return; displayBass(5);  // A
            playNote(396, 1000); if (!isPlayingMusic) return; displayBass(4); // G

            playNote(352, 500); if (!isPlayingMusic) return; displayBass(3);  // F
            playNote(352, 500); if (!isPlayingMusic) return; displayBass(3);  // F
            playNote(330, 500); if (!isPlayingMusic) return; displayBass(2);  // E
            playNote(330, 500); if (!isPlayingMusic) return; displayBass(2);  // E
            playNote(297, 500); if (!isPlayingMusic) return; displayBass(2);  // D
            playNote(297, 500); if (!isPlayingMusic) return; displayBass(2);  // D
            playNote(264, 1000); if (!isPlayingMusic) return; displayBass(1); // C
        }

        else if (music == 3) { // Jingle Bells (short)
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(3);
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(3);
            playNote(330, 500); if (!isPlayingMusic) return; displayBass(3);
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(3);
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(3);
            playNote(330, 500); if (!isPlayingMusic) return; displayBass(3);

            playNote(330, 250); if (!isPlayingMusic) return; displayBass(3);
            playNote(396, 250); if (!isPlayingMusic) return; displayBass(4);
            playNote(264, 250); if (!isPlayingMusic) return; displayBass(1);
            playNote(297, 250); if (!isPlayingMusic) return; displayBass(2);
            playNote(330, 1000); if (!isPlayingMusic) return; displayBass(3);
        }

        else if (music == 4) {
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(3); // E
            playNote(297, 250); if (!isPlayingMusic) return; displayBass(2); // D
            playNote(264, 250); if (!isPlayingMusic) return; displayBass(1); // C
            playNote(297, 250); if (!isPlayingMusic) return; displayBass(2); // D
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(3); // E
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(3); // E
            playNote(330, 500); if (!isPlayingMusic) return; displayBass(3); // E

            playNote(297, 250); if (!isPlayingMusic) return; displayBass(2);
            playNote(297, 250); if (!isPlayingMusic) return; displayBass(2);
            playNote(297, 500); if (!isPlayingMusic) return; displayBass(2);
            playNote(330, 250); if (!isPlayingMusic) return; displayBass(3);
            playNote(396, 250); if (!isPlayingMusic) return; displayBass(4);
            playNote(396, 500); if (!isPlayingMusic) return; displayBass(4);
        }

        else if (music == 5) {
            playNote(660, 100); if (!isPlayingMusic) return; displayBass(1);
            playNote(660, 100); if (!isPlayingMusic) return; displayBass(1);
            delay(100);
            playNote(660, 100); if (!isPlayingMusic) return; displayBass(1);
            delay(150);
            playNote(510, 100); if (!isPlayingMusic) return; displayBass(2);
            playNote(660, 100); if (!isPlayingMusic) return; displayBass(1);
            delay(150);
            playNote(770, 100); if (!isPlayingMusic) return; displayBass(3);
            delay(275);
            playNote(380, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(400);

            playNote(510, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(100);
            playNote(380, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(100);
            playNote(320, 100); if (!isPlayingMusic) return; displayBass(1);
            delay(200);
            playNote(440, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(100);
            playNote(480, 80);  if (!isPlayingMusic) return; displayBass(2);
            playNote(450, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(100);
            playNote(430, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(100);
            playNote(380, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(150);
            playNote(660, 80);  if (!isPlayingMusic) return; displayBass(1);
            delay(100);
            playNote(760, 50);  if (!isPlayingMusic) return; displayBass(3);
            delay(100);
            playNote(860, 100); if (!isPlayingMusic) return; displayBass(3);
            delay(100);
            playNote(700, 80);  if (!isPlayingMusic) return; displayBass(3);
            delay(100);
            playNote(760, 50);  if (!isPlayingMusic) return; displayBass(3);
            delay(100);
            playNote(660, 100); if (!isPlayingMusic) return; displayBass(1);
            delay(150);
            playNote(520, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(100);
            playNote(580, 80);  if (!isPlayingMusic) return; displayBass(2);
            playNote(480, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(100);
            playNote(510, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(300);
            playNote(380, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(300);
            playNote(320, 100); if (!isPlayingMusic) return; displayBass(1);
            delay(300);
            playNote(440, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(100);
            playNote(480, 80);  if (!isPlayingMusic) return; displayBass(2);
            playNote(450, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(100);
            playNote(430, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(100);
            playNote(380, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(150);
            playNote(660, 80);  if (!isPlayingMusic) return; displayBass(1);
            delay(100);
            playNote(760, 50);  if (!isPlayingMusic) return; displayBass(3);
            delay(100);
            playNote(860, 100); if (!isPlayingMusic) return; displayBass(3);
            delay(100);
            playNote(700, 80);  if (!isPlayingMusic) return; displayBass(3);
            delay(100);
            playNote(760, 50);  if (!isPlayingMusic) return; displayBass(3);
            delay(100);
            playNote(660, 100); if (!isPlayingMusic) return; displayBass(1);
            delay(150);
            playNote(520, 100); if (!isPlayingMusic) return; displayBass(2);
            delay(100);
            playNote(580, 80);  if (!isPlayingMusic) return; displayBass(2);
            playNote(480, 100); if (!isPlayingMusic) return; displayBass(2);
        }

        else if (music == 6) {
            playNote(330, 300); if (!isPlayingMusic) return; displayBass(3);
            playNote(396, 300); if (!isPlayingMusic) return; displayBass(4);
            playNote(440, 300); if (!isPlayingMusic) return; displayBass(5);
            playNote(523, 300); if (!isPlayingMusic) return; displayBass(5);
            playNote(493, 300); if (!isPlayingMusic) return; displayBass(5);
            playNote(440, 300); if (!isPlayingMusic) return; displayBass(5);
            playNote(396, 300); if (!isPlayingMusic) return; displayBass(4);
            playNote(330, 300); if (!isPlayingMusic) return; displayBass(3);
            playNote(330, 500); if (!isPlayingMusic) return; displayBass(3);
            playNote(349, 500); if (!isPlayingMusic) return; displayBass(3);
            playNote(392, 1000); if (!isPlayingMusic) return; displayBass(4);
            playNote(330, 500); if (!isPlayingMusic) return; displayBass(3);
            playNote(349, 500); if (!isPlayingMusic) return; displayBass(3);
            playNote(392, 1000); if (!isPlayingMusic) return; displayBass(4);
        }

        else if (music == 7) { // Doraemon Opening Theme
            playNote(392, 300); if (!isPlayingMusic) return; displayBass(1); // G
            playNote(392, 300); if (!isPlayingMusic) return; displayBass(1);
            playNote(392, 300); if (!isPlayingMusic) return; displayBass(1);
            delay(200);

            playNote(392, 150); if (!isPlayingMusic) return; displayBass(1);
            playNote(440, 150); if (!isPlayingMusic) return; displayBass(2);
            playNote(392, 150); if (!isPlayingMusic) return; displayBass(1);
            playNote(349, 300); if (!isPlayingMusic) return; displayBass(1);
            delay(200);

            playNote(392, 150); if (!isPlayingMusic) return; displayBass(1);
            playNote(440, 150); if (!isPlayingMusic) return; displayBass(2);
            playNote(392, 150); if (!isPlayingMusic) return; displayBass(1);
            playNote(349, 300); if (!isPlayingMusic) return; displayBass(1);
            delay(200);

            playNote(330, 150); if (!isPlayingMusic) return; displayBass(1);
            playNote(330, 150); if (!isPlayingMusic) return; displayBass(1);
            playNote(330, 150); if (!isPlayingMusic) return; displayBass(1);
            playNote(330, 150); if (!isPlayingMusic) return; displayBass(1);
            playNote(330, 300); if (!isPlayingMusic) return; displayBass(1);
          }
    }
}

void displayBass(int intensity) {
    display.clearDisplay();

    int barWidth = 10;
    int spacing = 4;
    int baseX = 3;
    int maxHeight = 48;
    int segmentHeight = 4;
    int segmentSpacing = 1;

    // Vẽ 10 thanh bass động, chia thành các đốt nhỏ
    for (int i = 0; i < 9; i++) {
        int variation = random(-5, 10); // Hiệu ứng nhiễu nhẹ
        int height = constrain((intensity * 8) + variation, 4, maxHeight);
        int numSegments = height / (segmentHeight + segmentSpacing);
        
        int x = baseX + i * (barWidth + spacing);
        int y = 64 - segmentHeight;

        for (int j = 0; j < numSegments; j++) {
            display.fillRect(x, y - j * (segmentHeight + segmentSpacing), barWidth, segmentHeight, SSD1306_WHITE);
        }
    }

    display.display();
}
