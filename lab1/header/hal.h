#ifndef _HAL_H_
#define _HAL_H

enum FSMstate {state0, state1, state2, state3, state4}; // FSM states
enum SYSmode {mode0, mode1, mode2, mode3}; // Sleep modes
extern enum FSMstate state;
extern enum SYSmode lpm_mode;

void GPIOconfig(void);
void TIMERconfig(void);
void ADCconfig(void);

#endif
