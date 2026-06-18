# OSCILLOSCOPE From Scratch

![C](https://img.shields.io/badge/Language-C-blue.svg)
![Build](https://img.shields.io/badge/Build-Make-orange.svg)
![MCU](https://img.shields.io/badge/MCU-STM32F108-green.svg)
![MCU](https://img.shields.io/badge/MCU-ESP32-red.svg)
![Status](https://img.shields.io/badge/Status-Active_Learning-success.svg)

## Documents

| Files                  | Description                                                                             |
| ---------------------- | --------------------------------------------------------------------------------------- |
| [README.md](README.md) | Main project overview, hardware information, firmware features, and operating principle |
| [Atilum](./Atilum/)    | Contains PCB schematic                                                                  |
| [ESP32](./ESP32/)      | Contains related ESP32 files and folders                                                |
| [STM32](./STM32/)      | Contains related STM32 files and folders                                                |

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
   git clone https://github.com/KimThanhNguyen-hcmus/Oscilloscope_STM32.git
   cd Oscilloscope_STM32
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

<h3>Contact Me</h3>

<p>
  <a href="https://github.com/KimThanhNguyen-hcmus">
    <img src="https://img.shields.io/badge/GitHub-KimThanhNguyen-181717?style=for-the-badge&logo=github&logoColor=white"/>
  </a>
  
  <a href="https://www.linkedin.com/in/nguyenkimthanh-hcmus/">
    <img src="https://img.shields.io/badge/LinkedIn-Nguyễn%20Kim%20Thành-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white"/>
  </a>
  
  <a href="mailto:nkimthanh47@gmail.com">
    <img src="https://img.shields.io/badge/Gmail-nkimthanh47%40gmail.com-EA4335?style=for-the-badge&logo=gmail&logoColor=white"/>
  </a>
</p>
