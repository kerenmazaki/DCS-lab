#include  "../header/bsp.h"   		// private library - API layer
#include  "../header/hal.h"   		// private library - API layer
#include  "../header/api.h"   		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;


void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();      // Configure GPIO, Timers, LCD
  
  
    while(1){
        switch(state){
            case state0:                         //idl
                enterLPM(lpm_mode);
                break;

            case state1:                        //PB0
                idiom_printer();
                //state = state0;
                break;

            case state2:                       //PB1
                merge_strings();
                state = state0;
                break;

            case state3:                       //PB2
                Show_nums();
                break;

            case state4:                       //PB3
                flip_sentence();
                state = state0;
                break;
        }
    }
}
