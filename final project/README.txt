# UART-based Communication System with MSP430G2553

This project implements UART-based communication between a PC and an MSP430G2553 microcontroller. The PC acts as the user interface, sending commands to the microcontroller, which then executes them and interacts with various hardware peripherals.

## Overview

- **PC (User Interface)**: Sends commands to the microcontroller via a serial connection. Commands include controlling LEDs, playing tones, setting delay times, and more.
- **Microcontroller (MSP430G2553)**: Executes commands by managing peripherals like RGB LEDs, a buzzer, a potentiometer, and an LCD. A finite state machine (FSM) is used to manage command execution and transitions between states.

## System States

The system is built around a finite state machine (FSM) that handles different modes based on the received commands. Each state corresponds to a specific command sent from the PC application.

### 1. Blink RGB LED
Cycles through RGB LED colors (Red, Green, Blue) with a configurable delay time (X milliseconds, default: 500 ms). The system continues blinking the LEDs until a new command is received. The delay time can be dynamically updated.

### 2. Count Up
Displays an incrementing integer value on the LCD screen. The counter value is retained across states and updates at intervals defined by the delay time (X).

### 3. Circular Tone Series
Plays a series of tones through the buzzer, cycling through frequencies from 1kHz to 2.5kHz. Each tone lasts for X milliseconds. The tone sequence repeats until interrupted by a new command.

### 4. Set Delay Time
Allows the user to input a new delay time (X milliseconds) to be used by other commands like blinking LEDs and tone playback. The input is finalized with the ENTER key.

### 5. Read Potentiometer Value
Reads the potentiometer’s voltage and displays it on the LCD as a three-digit value (e.g., 3.45V). The value continuously updates on the LCD as the potentiometer’s position changes.

### 6. Clear LCD
Clears the LCD and resets the counter value to zero. Executes once and transitions to the idle state.

### 7. Show Menu
Displays a list of available commands and their corresponding input keys. Waits for a new command after displaying the menu.

### 8. Sleep Mode
Puts the microcontroller into a low-power idle state. The system wakes up upon receiving a new command, helping conserve power when inactive.

## Setup and Usage

1. **Connect the PC to the Microcontroller**: Establish a serial connection between the PC and the MSP430G2553.
2. **Launch the PC Application**: Start the user interface on the PC to interact with the microcontroller.
3. **Send Commands**: Use the menu-driven interface to send commands to the microcontroller. The microcontroller will execute the commands and provide feedback via the connected peripherals (LEDs, buzzer, LCD, etc.).
4. **Adjust Settings**: Set the delay time or toggle between states as required using the PC interface.

## Conclusion

This project demonstrates the use of UART communication between a PC and an embedded microcontroller, allowing for real-time control of various hardware peripherals using simple commands. The finite state machine architecture ensures efficient handling of commands and transitions between different operational modes.
