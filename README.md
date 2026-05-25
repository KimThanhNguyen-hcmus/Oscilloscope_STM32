# Command Car: STM32 From Sccratch

![C](https://img.shields.io/badge/Language-C-blue.svg)
![Build](https://img.shields.io/badge/Build-Make-orange.svg)
![MCU](https://img.shields.io/badge/MCU-STM32F108-green.svg)
![Status](https://img.shields.io/badge/Status-Active_Learning-success.svg)

## Documents

| Files                  | Description                                                                             |
| ---------------------- | --------------------------------------------------------------------------------------- |
| [README.md](README.md) | Main project overview, hardware information, firmware features, and operating principle |
| [Build](./Build/)      | Contains files.o, .elf, .map                                                            |
| [Inc](./Inc/)          | Header files, register maps, macros                                                     |
| [Src](./Src/)          | Source C files (main.c, peripheral drivers)                                             |
| [Gui](./Gui/)          | Includes GUI using Python                                                               |

## Project Overview

## Hardware Components

## Schematic

## Features

## System Operating Principle

## Getting Started

Prerequisites
To build this project, you will need a cross-compilation toolchain and build tools:

- `arm-none-eabi-gcc` (GNU Arm Embedded Toolchain)
- `make` (GNU Make)
- A flashing tool (e.g., OpenOCD, ST-Link Utility, or equivalent depending on the programmer used).

### Build Instructions

1. Clone the repository:
   ```bash
   git clone [https://github.com/HotIveTea/Oscilloscope_STM32](https://github.com/HotIveTea/Oscilloscope_STM32)
   cd Command_Car
   ```
2. Build the project:
   ```bash
   make all
   ```
   or
   ```bash
   mingw32-make
   ```
3. Flash the firmware to the board:
   ```bash
   make flash
   ```
   or
   ```bash
   mingw32-make flash
   ```

## Developer's Diary

I document my daily progress, bugs encountered, and architectural concepts learned on Notion. Feel free to follow along with my thought process:
[Bare-metal-coding](https://www.notion.so/Bare-metal-programming-155984656d2c836f813c01230064508a?source=copy_link)
I also document my daily progress, bugs and approachs about RTOS in Notion.
[RTOS from scratch](https://www.notion.so/RTOS-from-scratch-English-ver-33b984656d2c806c9579eb40ed18872a?source=copy_link)

<h3>📫 Contact Me</h3>

<p>
  <a href="https://github.com/HotIveTea">
    <img src="https://img.shields.io/badge/GitHub-HotIveTea-181717?style=for-the-badge&logo=github&logoColor=white"/>
  </a>
  
  <a href="https://www.linkedin.com/in/nguyễn-kim-thành-5ba280389/">
    <img src="https://img.shields.io/badge/LinkedIn-Nguyễn%20Kim%20Thành-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white"/>
  </a>
  
  <a href="mailto:nkimthanh47@gmail.com">
    <img src="https://img.shields.io/badge/Gmail-nkimthanh47%40gmail.com-EA4335?style=for-the-badge&logo=gmail&logoColor=white"/>
  </a>
</p>
