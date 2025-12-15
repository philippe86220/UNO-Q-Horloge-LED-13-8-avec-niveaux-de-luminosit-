#include <Arduino.h>
#include <Arduino_RouterBridge.h>
#include <Arduino_LED_Matrix.h>

Arduino_LED_Matrix matrix;

const int MATRIX_WIDTH  = 13;
const int MATRIX_HEIGHT = 8;

const uint8_t DIGITS[10][5] = {
  { 0b111, 0b101, 0b101, 0b101, 0b111 }, // 0
  { 0b001, 0b001, 0b001, 0b001, 0b001 }, // 1
  { 0b111, 0b001, 0b111, 0b100, 0b111 }, // 2
  { 0b111, 0b001, 0b111, 0b001, 0b111 }, // 3
  { 0b101, 0b101, 0b111, 0b001, 0b001 }, // 4
  { 0b111, 0b100, 0b111, 0b001, 0b111 }, // 5
  { 0b111, 0b100, 0b111, 0b101, 0b111 }, // 6
  { 0b111, 0b001, 0b001, 0b001, 0b001 }, // 7
  { 0b111, 0b101, 0b111, 0b101, 0b111 }, // 8
  { 0b111, 0b101, 0b111, 0b001, 0b111 }  // 9
};

static inline int idx(int x, int y) {
  return y * MATRIX_WIDTH + x; // 0..103
}

void setPixelIntensity(uint8_t frame[104], int x, int y, uint8_t intensity) {
  if (x < 0 || x >= MATRIX_WIDTH) return;
  if (y < 0 || y >= MATRIX_HEIGHT) return;
  frame[idx(x, y)] = intensity;
}

void drawDigitIntensity(uint8_t frame[104], int digit, int xOffset, uint8_t intensity) {
  if (digit < 0 || digit > 9) return;

  const int yOffset = 1;

  for (int row = 0; row < 5; row++) {
    uint8_t pattern = DIGITS[digit][row];

    for (int col = 0; col < 3; col++) {
      if (pattern & (1u << (2 - col))) {
        setPixelIntensity(frame, xOffset + col, yOffset + row, intensity);
      }
    }
  }
}

void buildClockFrame(uint8_t frame[104], int hour, int minute, bool showColon) {
  for (int i = 0; i < 104; i++) frame[i] = 0;

  int hTens  = hour / 10;
  int hUnits = hour % 10;
  int mTens  = minute / 10;
  int mUnits = minute % 10;

  // Intensités : à ajuster
  // Si vous mettez setGrayscaleBits(3) -> intensités utiles : 0..7
  // Si vous mettez setGrayscaleBits(8) -> intensités utiles : 0..255
  const uint8_t I_TENS  = 255; // dizaines très lumineux
  const uint8_t I_UNITS = 80;  // unités moins lumineux
  const uint8_t I_COLON = 160; // intermédiaire

  // Heures
  if (hour >= 10) drawDigitIntensity(frame, hTens, 0, I_TENS);
  drawDigitIntensity(frame, hUnits, 3, I_UNITS);

  // :
  if (showColon) {
    setPixelIntensity(frame, 6, 2, I_COLON);
    setPixelIntensity(frame, 6, 4, I_COLON);
  }

  // Minutes (vous pouvez choisir : dizaines minutes plus fortes que unités minutes, etc.)
  drawDigitIntensity(frame, mTens, 7, I_TENS);
  drawDigitIntensity(frame, mUnits, 10, I_UNITS);
}

void updateTime(int32_t hour, int32_t minute, int32_t second) {
  if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) return;

  bool showColon = (second % 2 == 0);

  uint8_t frame[104];
  buildClockFrame(frame, (int)hour, (int)minute, showColon);

  matrix.draw(frame);
}

void setup() {
  Serial.begin(115200);

  matrix.begin();

  // IMPORTANT:
  // D’après la doc UNO Q, il y a des niveaux de gris (ex: 3 bits -> 0..7). :contentReference[oaicite:2]{index=2}
  // Certains exemples utilisent 8 bits (0..255). Si 8 ne marche pas visuellement, repassez à 3.
  matrix.setGrayscaleBits(8);

  matrix.clear();

  Bridge.begin();
  Bridge.provide("updateTime", updateTime);
}

void loop() {
  delay(10);
}

