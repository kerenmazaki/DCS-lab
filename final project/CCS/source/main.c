#include  "../header/bsp.h"
#include  "../header/hal.h"
#include  "../header/api.h"
#include  "../header/app.h"

#define TXD BIT2
#define RXD BIT1

enum FSMstate state;
enum SYSmode lpm_mode;
FlashFileSystem script_handler;


int main(void)
{
    script_handler.file_count = 0;

    script_handler.file_names[0] = "script1";
    script_handler.file_names[1] = "script2";
    script_handler.file_names[2] = "script3";

    script_handler.file_pointers[0] = (char*)0xC000;
    script_handler.file_pointers[1] = (char*)0xC200;
    script_handler.file_pointers[2] = (char*)0xC400;

    state = state0;
    lpm_mode = mode0;
    sysConfig();

    if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
        {
        while(1);                               // do not load, trap CPU!!
        }
    while (1) {
        switch (state) {
            case state0:
                enterLPM(lpm_mode);
                break;

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

            }
    }
}





