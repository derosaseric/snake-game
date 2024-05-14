# Nexys A7 100T Snake Game

## Overview

This project implements a classic Snake game on the Nexys A7 100T FPGA board. The game is developed using SystemVerilog for creating a System on a Chip (SoC) and C++ for implementing the game's functionality.

## Features

- **VGA display output** for gameplay
- **PS/2 keyboard input** for controlling the snake
- Real-time **scoring** and **time tracking**
- Randomly generated **apples** for the snake to eat
- **Pause functionality**
- **Game over detection**
- **LED indication** for game start

## Getting Started

### Prerequisites

- Nexys A7 100T FPGA board
- Vivado Design Suite
- USB keyboard (PS/2)

### Installation

1. Clone the repository to your local machine:

   ```bash
   git clone https://github.com/derosaseric/snake-game.git
2. Open the project in Vivado Design Suite.
3. Compile the project and generate the bitstream.
4. Program the bitstream onto your Nexys A7 100T FPGA board.

## Usage

1. Connect your Nexys A7 100T board to a VGA monitor and a PS/2 keyboard.
2. Power on the board and reset it.
3. Follow the instructions on the VGA monitor to start the game.
3. Use the arrow keys (or WASD) to control the snake.
4. Eat apples to increase your score and avoid hitting the walls or yourself.
5. Press the 'P' key to pause the game.
6. After the game is over, press the spacebar to restart.

## Code Structure

- `main.cpp`: Contains the C++ code for game logic including snake movement, apple generation, scoring, and game over detection.
- `chu_init.h`: Initialization file for the Nexys A7 100T FPGA board.
- `gpio_cores.h`: Header file for GPIO cores used in the project.
- `vga_core.h`: Header file for VGA display core.
- `sseg_core.h`: Header file for seven-segment display core.
- `ps2_core.h`: Header file for PS/2 keyboard core.
- `xadc_core.h`: Header file for XADC core.

## Contributing

Contributions are welcome! For major changes, please open an issue first to discuss what you would like to change.


## License

This project is licensed under the **MIT License**. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- This project was created by Eric De Rosas and Andre Corona.
