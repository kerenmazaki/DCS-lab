#include  "../header/app.h"
#include  "../header/api.h"
#include  "../header/hal.h" 

//#ifdef __MSP430FG4619__
#include "../header/bsp_msp430x4xx.h" //lab kit
//#else
//#include "../header/bsp_msp430x2xx.h" //pesronal kit
//#endif

enum FSMstate state;
enum SYSmode lpm_mode;
int Nfreq = 42;

void main(void) {
	state = state0;  // start in idle state on RESET
	lpm_mode = mode0;     // start in idle state on RESET
	sysConfig();

	while (1) {
		switch (state) {
		case state0:
			enterLPM(lpm_mode);
			break;

		case state1:
			disable_interrupts();
			count(LEDs_RATE);
                        enable_interrupts();
			state = state0;
			break;

		case state2:
			disable_interrupts();
			ShiftNum2LEDs(LEDs_RATE);
			enable_interrupts();
			state = state0;
			break;

		case state3:
			//disable_interrupts();
			generatePwm();
			break;

        case state4:
            //disable_interrupts();
               pwm(Nfreq);
            break;
		}
	}
}
