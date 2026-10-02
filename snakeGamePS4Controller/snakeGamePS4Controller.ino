#include <Bluepad32.h>

//PS4 controller
ControllerPtr myController = nullptr;

//game settings
const int WIDTH = 20;
const int HEIGHT = 12;
const int MAX_SNAKE_LENGTH = WIDTH * HEIGHT;

//speed 
const unsigned long NORMAL_MOVE_INTERVAL = 300;
const unsigned long FAST_MOVE_INTERVAL = 100;

struct Point {
  int x;
  int y;
};

//snake
Point snake[MAX_SNAKE_LENGTH];
int snakeLength = 3;

Point food;

enum Direction {
  UP,
  DOWN,
  LEFT,
  RIGHT
};

Direction direction = RIGHT;

//game state
bool gameOver = false;
bool gamePaused = false;
int score = 0;

//timing
unsigned long lastMoveTime = 0;

//prev button states
bool previousA = false;
bool previousB = false;
bool previousX = false;
bool previousY = false;
bool previousOptions = false;

void onConnectedController(ControllerPtr ctl) {
  if (myController == nullptr) {
    myController = ctl;

    Serial.println();
    Serial.println("PS4 Controller connected!");
    Serial.println();
  }
  else {
    Serial.println("Another controller connected, ignoring it.");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  if (myController == ctl) {
    myController = nullptr;

    Serial.println();
    Serial.println("PS4 Controller disconnected!");
    Serial.println();
  }
}

void createFood() {
  while (true) {
    food.x = random(0, WIDTH);
    food.y = random(0, HEIGHT);

    bool occupied = false;

    for (int i = 0; i < snakeLength; i++) {
      if (snake[i].x == food.x &&
          snake[i].y == food.y) {
        occupied = true;
        break;
      }
    }

    if (!occupied) {
      break;
    }
  }
}

void startGame() {
  snakeLength = 3;
  score = 0;

  gameOver = false;
  gamePaused = false;

  direction = RIGHT;

  //initial snake position
  snake[0].x = 10;
  snake[0].y = 6;

  snake[1].x = 9;
  snake[1].y = 6;

  snake[2].x = 8;
  snake[2].y = 6;

  createFood();

  lastMoveTime = millis();
}

//check if position belongs to snake
bool isSnake(int x, int y) {
  for (int i = 0; i < snakeLength; i++) {
    if (snake[i].x == x &&
        snake[i].y == y) {
      return true;
    }
  }

  return false;
}

void clearScreen() {
  Serial.print("\033[2J");
  Serial.print("\033[H");
}

//move cursor to top left for more display
void moveCursorHome() {
  Serial.print("\033[H");
}

//UI
void drawGame() {
  moveCursorHome();

  Serial.println("===== ESP32 SNAKE =====");

  Serial.print("Score: ");
  Serial.println(score);

  Serial.println();

  Serial.print("+");

  for (int x = 0; x < WIDTH; x++) {
    Serial.print("-");
  }

  Serial.println("+");

  for (int y = 0; y < HEIGHT; y++) {
    Serial.print("|");

    for (int x = 0; x < WIDTH; x++) {
      char character = ' ';

      //food @
      if (food.x == x &&
          food.y == y) {
        character = '@';
      }

      //snake
      for (int i = 0; i < snakeLength; i++) {
        if (snake[i].x == x &&
            snake[i].y == y) {

          if (i == 0) {
            character = 'O';
          }
          else {
            character = 'o';
          }

          break;
        }
      }

      Serial.print(character);
    }

    Serial.println("|");
  }

  Serial.print("+");

  for (int x = 0; x < WIDTH; x++) {
    Serial.print("-");
  }

  Serial.println("+");
}

//pause screen
void drawPauseScreen() {
  moveCursorHome();

  Serial.println("===== ESP32 SNAKE =====");

  Serial.println();

  Serial.println("        PAUSED");

  Serial.println();

  Serial.print("Score: ");
  Serial.println(score);

  Serial.println();

  Serial.println("Press OPTIONS to resume.");
}

//game over screen
void drawGameOver() {
  moveCursorHome();

  Serial.println("===== ESP32 SNAKE =====");

  Serial.println();

  Serial.println("       GAME OVER");

  Serial.println();

  Serial.print("Score: ");
  Serial.println(score);

  Serial.println();

  Serial.println("Press X to restart.");
}

void changeDirection(Direction newDirection) {
  if (newDirection == UP) {
    if (direction != DOWN) {
      direction = UP;
    }
  }
  else if (newDirection == DOWN) {
    if (direction != UP) {
      direction = DOWN;
    }
  }
  else if (newDirection == LEFT) {
    if (direction != RIGHT) {
      direction = LEFT;
    }
  }
  else if (newDirection == RIGHT) {
    if (direction != LEFT) {
      direction = RIGHT;
    }
  }
}

void readController() {
  if (myController == nullptr) {
    return;
  }
  
  //PS buttons
  bool currentA = myController->a();
  bool currentB = myController->b();
  bool currentX = myController->x();
  bool currentY = myController->y();

  //Options button
  uint8_t misc = myController->miscButtons();
  bool currentOptions = misc & 0x04;

  //D-pad
  uint8_t dpad = myController->dpad();
  bool dpadUp = dpad & 0x01;
  bool dpadDown = dpad & 0x02;
  bool dpadRight = dpad & 0x04;
  bool dpadLeft = dpad & 0x08;

  //r2
  int r2Value = myController->throttle();
  bool r2Pressed = r2Value > 100;

  //restart after game over
  if (gameOver) {
    if (currentA && !previousA) {
      startGame();
      clearScreen();
      drawGame();
    }
    previousA = currentA;
    previousB = currentB;
    previousX = currentX;
    previousY = currentY;
    previousOptions = currentOptions;

    return;
  }

  if (currentOptions && !previousOptions) {
    gamePaused = !gamePaused;
    if (gamePaused) {
      drawPauseScreen();
    }
    else {
      drawGame();
      lastMoveTime = millis();
    }
  }

  if (gamePaused) {
    previousA = currentA;
    previousB = currentB;
    previousX = currentX;
    previousY = currentY;
    previousOptions = currentOptions;
    return;
  }

  //cross = down
  if (currentA && !previousA) {
    changeDirection(DOWN);
  }
  //circle = right
  if (currentB && !previousB) {
    changeDirection(RIGHT);
  }
  //square = left
  if (currentX && !previousX) {
    changeDirection(LEFT);
  }

  //triangle = up
  if (currentY && !previousY) {
    changeDirection(UP);
  }

  //D-pad
  if (dpadUp) {
    changeDirection(UP);
  }
  else if (dpadDown) {
    changeDirection(DOWN);
  }
  else if (dpadLeft) {
    changeDirection(LEFT);
  }
  else if (dpadRight) {
    changeDirection(RIGHT);
  }

  previousA = currentA;
  previousB = currentB;
  previousX = currentX;
  previousY = currentY;
  previousOptions = currentOptions;
}

void moveSnake() {
  Point newHead = snake[0];

  //new head position
  if (direction == UP) {
    newHead.y--;
  }
  else if (direction == DOWN) {
    newHead.y++;
  }
  else if (direction == LEFT) {
    newHead.x--;
  }
  else if (direction == RIGHT) {
    newHead.x++;
  }

  //wrap screen
  if (newHead.x < 0) {
    newHead.x = WIDTH - 1;
  }
  if (newHead.x >= WIDTH) {
    newHead.x = 0;
  }
  if (newHead.y < 0) {
    newHead.y = HEIGHT - 1;
  }
  if (newHead.y >= HEIGHT) {
    newHead.y = 0;
  }
  
  //self collision
  if (isSnake(newHead.x, newHead.y)) {
    gameOver = true;
    drawGameOver();
    return;
  }

  //check food
  bool ateFood = false;
  if (newHead.x == food.x &&
      newHead.y == food.y) {
    ateFood = true;
  }
  //growing
  if (ateFood) {
    if (snakeLength < MAX_SNAKE_LENGTH) {
      snakeLength++;
    }
  }
  //move body
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  //new head
  snake[0] = newHead;
  //new food
  if (ateFood) {
    score++;
    createFood();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  clearScreen();
  randomSeed(micros());
  BP32.setup(
    &onConnectedController,
    &onDisconnectedController
  );
  startGame();
  drawGame();
}

void loop() {
  BP32.update();
  readController();
  if (!gamePaused && !gameOver) {
    unsigned long currentTime = millis();
    //speed boost
    int r2Value = 0;
    bool r2Pressed = false;
    if (myController != nullptr) {
      r2Value = myController->throttle();
      r2Pressed = r2Value > 100;
    }
    unsigned long moveInterval;
    if (r2Pressed) {
      moveInterval = FAST_MOVE_INTERVAL;
    }
    else {
      moveInterval = NORMAL_MOVE_INTERVAL;
    }
    // Move snake
    if (currentTime - lastMoveTime >= moveInterval) {
      lastMoveTime = currentTime;
      moveSnake();
      if (!gameOver) {
        drawGame();
      }
    }
  }
  // Small delay
  delay(10);
}
