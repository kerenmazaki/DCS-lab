# Lab 1 - FSM Implementation

This lab demonstrates the implementation of a Finite State Machine (FSM) using the MSP430 microcontroller. The FSM transitions between various states based on input from switches.

## FSM States

- **State 0 (Idle)**: The system is in a low-power mode, awaiting input.
- **State 1 (LED Blink)**: Blinks LEDs at a defined rate.
- **State 2 (Shift LEDs)**: Shifts the state of the LEDs.
- **State 3 (Generate PWM)**: Generates a Pulse Width Modulation (PWM) signal.
- **State 4 (Adjust PWM Frequency)**: Adjusts the frequency of the PWM signal.
