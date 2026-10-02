# ESP32 PS4 Snake

A simple Snake game running on an ESP32 and controlled using a PlayStation 4 DualShock 4 controller over Bluetooth.

The game is displayed in the macOS terminal.

## Features

- ESP32-based Snake game
- PS4 DualShock 4 Bluetooth controller
- Bluepad32 for controller communication
- 20 × 12 game board
- Snake grows when eating food
- Score system
- Screen wrap-around
- Self-collision detection
- Game over state
- Cross button to restart after game over
- OPTIONS button to pause/resume
- R2 speed boost
- PS4 face buttons and D-pad for movement
- Fixed Serial Monitor screen using ANSI escape sequences (we cannot use the serial monitor of the arduinoIDE, must use the terminal)

---

## Hardware

### Required

- ESP32 Dev kit v1 (the ESP32-WROOM-32)
- PlayStation 4 DualShock 4 controller
- USB cable for programming the ESP32
- Computer running Arduino IDE

---

## Software

- Arduino IDE
  - ESP32
  - Bluepad32 through this github link: https://raw.githubusercontent.com/ricardoquesada/bluepad32-arduino/master/package_bluepad32_index.json

The PS4 controller communicates with the ESP32 over Bluetooth.

---

## Controls

| PS4 Button | Action |
|---|---|
| Triangle / D-pad Up | Move Up |
| Square / D-pad Left | Move Left |
| Circle / D-pad Right | Move Right |
| Cross / D-pad Down | Move Down / Restart after Game Over(Cross only) |
| R2 | Speed the snake up |
| OPTIONS | Pause / Resume |

---

The game board is represented by a 20 × 12 grid.

```text
+--------------------+
|                    |
|                    |
|        @           |
|                    |
|             Oooo   |
|                    |
|                    |
+--------------------+
