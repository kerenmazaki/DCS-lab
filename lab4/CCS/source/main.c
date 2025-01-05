#include  "../header/bsp.h"
#include  "../header/hal.h"
#include  "../header/api.h"
#include  "../header/app.h"

#define TXD BIT2
#define RXD BIT1

enum FSMstate state;
enum SYSmode lpm_mode;


int main(void)
{
    state = state9;
    lpm_mode = mode0;
    sysConfig();

    if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
        {
        while(1);                               // do not load, trap CPU!!
        }
    while (1) {
        switch (state) {
            case state1:
                state1_func();
                break;

            case state2:
                state2_func();
                break;

            case state3:
                state3_func();
                break;

            case state4:
                state4_func();
                break;

            case state5:
                state5_func();
                break;

            case state6:
                state6_func();
                break;

            case state7:
                state7_func();
                break;

            case state8:
                state8_func();
                break;

            case state9:
                enterLPM(lpm_mode);
                break;
            }
    }
}





