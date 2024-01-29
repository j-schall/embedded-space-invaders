#include <U8glib.h>
#define RIGHT_BUTTON 5
#define LEFT_BUTTON 6
#define STATUS_LED 2
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);

bool rightPressed = false;
bool leftPressed = false;
bool enemyVisible = true;
int right = 0;

const int RECT_WIDTH = 20;
const int RECT_HEIGHT = 3;
int rectX = (u8g.getWidth() - RECT_WIDTH) / 2;
int rectY = u8g.getHeight() - RECT_HEIGHT;

int ammoPosition = 0;
unsigned long lastShotTime = 0;
const unsigned long shotInterval = 1900;

int enemyPosX = 10;
int enemyPosY = 10;

const int GRID_COL = 12;
const int GRID_ROW = 4;
const int ENEMY_SIZE = 3;
bool enemyStatus[GRID_COL][GRID_ROW];

int score = 0;

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

  clearScreen();
}

void spawnEnemy() {
  for (int i = 2; i <= GRID_COL; i++) {
    for (int j = 2; j <= GRID_ROW; j++) {
      int enemyX = i * 12;
      int enemyY = j * 12;
      if (!enemyStatus[i][j]) {  // Only spawn if enemy isn't struck
        u8g.drawFilledEllipse(enemyX, enemyY, ENEMY_SIZE, ENEMY_SIZE);
      }
    }
  }
}

bool hasWon() {
  for (int i = 2; i <= GRID_COL; i++) {
    for (int j = 2; j <= GRID_ROW; j++) {
      if (enemyStatus[i][j]) {
        return false;
      }
    }
  }
  return true;
}

void clearScreen() {
  u8g.firstPage();
  do {
  } while (u8g.nextPage());
}

void checkCollision() {
  for (int i = 2; i <= GRID_COL; i++) {
    for (int j = 2; j <= GRID_ROW; j++) {
      int enemyX = i * 12;
      int enemyY = j * 12;

      // If the bullet position is the same as the enemy position, the enemy will be removed from the display
      if (!enemyStatus[i][j] && rectX + right < enemyX + ENEMY_SIZE && rectX + right + RECT_WIDTH > enemyX && rectY - ammoPosition < enemyY + ENEMY_SIZE && rectY > enemyY) {
        enemyStatus[i][j] = true;
        score += 10;
      }
    }
  }
}

void showScore(int newScore) {
  u8g.setFont(u8g_font_6x12r);  // Wähle eine geeignete Schriftart
  u8g.drawStr(0, 10, "Score: ");
  u8g.drawStr(40, 10, String(newScore).c_str());
}

void loop() {
  u8g.setFont(u8g_font_6x12r);
  if (hasWon()) {
    Serial.println("YOU WON");
  }
  spawnEnemy();
  rightPressed = digitalRead(RIGHT_BUTTON);
  leftPressed = digitalRead(LEFT_BUTTON);
  unsigned long currentTime = millis();

  if (currentTime - lastShotTime >= shotInterval) {
    // Get new shot-time
    lastShotTime = currentTime;
    // Reset ammo position
    ammoPosition = 0;
  }

  // As long as the player is in the shown display area, he can go right or left
  if (rectX + right > 0) {
    if (rectX + right + RECT_WIDTH < u8g.getWidth()) {
      ammoPosition += 3;
      if (rightPressed) right += 2;
      if (leftPressed) right -= 2;
      u8g.drawVLine(rectX + right + RECT_WIDTH / 2, rectY - ammoPosition, 5);
    } else {
      right -= 2;
    }
  } else {
    right += 2;
  }
  u8g.firstPage();
  do {
    u8g.drawBox(rectX + right, rectY, RECT_WIDTH, RECT_HEIGHT);
    u8g.drawVLine(rectX + right + RECT_WIDTH / 2, rectY - ammoPosition, 5);
    spawnEnemy();
    checkCollision();
    showScore(score);
  } while (u8g.nextPage());
}
