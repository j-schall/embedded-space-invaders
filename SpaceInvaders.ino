#include <U8glib.h>
#define RIGHT_BUTTON 5
#define LEFT_BUTTON 6
#define STATUS_LED 2

U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);
bool rightPressed = false;
bool leftPressed = false;
int right = 0;
const int rectWidth = 30;
const int rectHeight = 5;
int rectX = (u8g.getWidth() - rectWidth) / 2;
int rectY = u8g.getHeight() - rectHeight;
int ammoPosition = 0;
unsigned long lastShotTime = 0;
const unsigned long shotInterval = 1500;

void setup() {
  pinMode(RIGHT_BUTTON, INPUT);
  pinMode(LEFT_BUTTON, INPUT);
  pinMode(STATUS_LED, OUTPUT);

  digitalWrite(STATUS_LED, HIGH);

  if (!u8g.begin())
    digitalWrite(STATUS_LED, LOW);

  Serial.begin(9600);

  if (u8g.getMode() == U8G_MODE_R3G3B2) {
    u8g.setColorIndex(255);  // white
  } else if (u8g.getMode() == U8G_MODE_GRAY2BIT) {
    u8g.setColorIndex(3);  // max intensity
  } else if (u8g.getMode() == U8G_MODE_BW) {
    u8g.setColorIndex(1);  // pixel on
  } else if (u8g.getMode() == U8G_MODE_HICOLOR) {
    u8g.setHiColorByRGB(255, 255, 255);
  }
}

void loop() {
  rightPressed = digitalRead(RIGHT_BUTTON);
  leftPressed = digitalRead(LEFT_BUTTON);
  unsigned long currentTime = millis();
  if (currentTime - lastShotTime >= shotInterval) {
    // Neue Schusszeit festlegen
    lastShotTime = currentTime;
    // Ammo-Anzeige zurücksetzen
    ammoPosition = 0;
  }

  // As long as the player is in the shown display area, he can go right or left
  if (rectX + right > 0) {
    if (rectX + right + rectWidth < u8g.getWidth()) {
      ammoPosition += 3;
      if (rightPressed) {
        right += 2;
      }
      if (leftPressed) {
        right -= 2;
      }
      u8g.drawVLine(rectX + right + rectWidth/2, rectY - ammoPosition, 5);
    } else {
      right -= 2;
    }
  } else {
    right += 2;
  }
  u8g.firstPage();
  do {
    //u8g.drawCircle(u8g.getWidth()/2+right, u8g.getHeight()/2, 15);
    u8g.drawBox(rectX + right, rectY, rectWidth, rectHeight);
    u8g.drawVLine(rectX + right + rectWidth/2, rectY - ammoPosition, 5);
  } while (u8g.nextPage());
}
