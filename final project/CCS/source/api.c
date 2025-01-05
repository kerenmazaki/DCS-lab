#include  "../header/bsp.h"
#include  "../header/hal.h"
#include  "../header/api.h"
#include  "../header/LCD.h"
#include "string.h"
#include "stdio.h"


#define PI 3.14159
char script_inst[10];


//*********************************************************************
//            state 1 function
//*********************************************************************

void state1_func(void){
    int relative_steps;
    int i;
    float relative_angle;

    while (state == state1) {
        while (state1_start == 1) step_clockwise();

        while (state1_start == 0 && state == state1){
            get_angle();
            if (angle != -1){
                relative_angle = angle - motor_angle;

                relative_steps =  norm_angle(relative_angle);

                if (relative_steps > 0){
                    for (i = relative_steps; i>0; i--) step_clockwise();
                }
                else{
                    for (i = relative_steps; i<0; i++) step_counter_clockwise();
                }
                motor_angle = angle;
            }
            msecs_delay(1000);
        }
    }

    state1_start = 0;
}
//*********************************************************************
//            state 2 function
//*********************************************************************
void state2_func(){
    int counter = 0;

    //float prev_angle = 0;
    JoyPressIntPend &= ~JoyPress;  //  clear pending interrupts
    JoyPressIntEn |= JoyPress; //enable pb interrupts


    while (state == state2){

        get_angle(); //in radians
        if ((angle != -1) && (counter % 10 == 0)){
            int integer_part = (int)angle;
            int fractional_part = (int)((angle - integer_part) * 100);
            sprintf(out_string, "2,%c,%d.%d\n", state2_mode, integer_part, fractional_part);
    //            strcat(out_string, "\n");
            send_message();
            enterLPM(lpm_mode); //wait until message is done sending
        }
        counter ++;
        //msecs_delay(1000);
    }

    JoyPressIntEn &= ~JoyPress; //disable pb interrupts
    state2_mode = 'w';
}
//*********************************************************************
//            state 3 function
//*********************************************************************
void state3_func(){

    JoyPressIntPend &= ~JoyPress;  //  clear pending interrupts
    JoyPressIntEn |= JoyPress; //enable pb interrupts

    steps = 0;

    while (state3_start == 0)
        step_clockwise();
    while (state3_start == 1){
        step_clockwise();
        steps += 1;
    }
    step_angle = (PI*2)/steps;

    int integer_part = (int)step_angle;
    int fractional_part = (int)((step_angle - integer_part) * 1000);

    sprintf(out_string, "3,%d,%d.0%d\n", steps, integer_part, fractional_part);
//    strcat(out_string, "\n");

    send_message();
    enterLPM(lpm_mode); //wait until message is done sending

    JoyPressIntEn &= ~JoyPress; //disable pb interrupts
    state = state0;

}
//*********************************************************************
//            state 4 functions
//*********************************************************************
void state4_func(void){

}


void write_script(void){
  char *Flash_ptr;                          // Flash pointer
  unsigned int i;

  Flash_ptr = script_handler.file_pointers[script_number - 1];      // Initialize Flash pointer
  script_handler.file_sizes[script_number - 1] = strlen(in_string);

  FCTL1 = FWKEY + ERASE;                    // Set Erase bit
  FCTL3 = FWKEY;                            // Clear Lock bit
  *Flash_ptr = 0;                           // Dummy write to erase Flash segment

  FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

  for (i=5; i<strlen(in_string); i++)
  {
    *Flash_ptr++ = in_string[i];              // Write value to flash
  }

  *Flash_ptr = '\0';

  FCTL1 = FWKEY;                            // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

void run_script(void){
    char *Flash_ptr;                          // Flash pointer
     int i = 0;

    Flash_ptr = script_handler.file_pointers[script_number - 1];      // Initialize Flash pointer
    FCTL1 = FWKEY + ERASE;                    // Set Erase bit
    FCTL3 = FWKEY;                            // Clear Lock bit

    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

    while (state == state4){
        script_inst[i++] = *Flash_ptr++;              // Write value to flash
        if (*Flash_ptr == '\0') break;
        if (*Flash_ptr == ','){
            script_inst[i] = *Flash_ptr++;
            i = 0;
            run_func();
        }
    }
    run_func();

    FCTL1 = FWKEY;                            // Clear WRT bit
    FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}

void run_func(void){
    char opcode[3];
    int operand = 0; // To store single operand
    int operand1 = 0; // For dual operand cases

    // Extract opcode (first 2 characters)
    opcode[0] = script_inst[0];
    opcode[1] = script_inst[1];
    opcode[2] = '\0';

    operand = str_to_int(script_inst[2]);
    operand = operand * 16;
    operand += str_to_int(script_inst[3]);

    operand1 = str_to_int(script_inst[4]);
    operand1 = operand1 * 16;
    operand1 += str_to_int(script_inst[5]);

    // Convert opcode to instruction
    if (opcode[1] == '1') {
        inc_lcd(operand);
    }
    else if (opcode[1] == '2') {
        dec_lcd(operand);
    }
    else if (opcode[1] == '3') {
        rra_lcd(operand);
    }
    else if (opcode[1] == '4') {
        d = operand;
    }
    else if (opcode[1] == '5') {
        lcd_clear();
    }
    else if (opcode[1] == '6') {
        stepper_deg(operand);
    }
    else if (opcode[1] == '7') {
        stepper_scan(operand, operand1);
    }
    else if (opcode[1] == '8') {
//        enterLPM(lpm_mode);
    }
}

void inc_lcd(int x){
    char buffer[16];
    int count;
    cursor_off();
    lcd_home();
    for (count = 0; count <= x; count ++){
        lcd_clear();
        sprintf(buffer, "%d", count);
        lcd_home();
        lcd_puts(buffer);
        msecs_delay(10*d);
    }
}

void dec_lcd(int x){
    char buffer[16];
    int count;
    cursor_off();
    lcd_home();
    for (count = x; count >= 0; count --){
        lcd_clear();
        sprintf(buffer, "%d", count);
        lcd_home();
        lcd_puts(buffer);
        msecs_delay(10*d);
    }
}

void rra_lcd(char x){
    int count;
    lcd_clear();
    lcd_home();
    cursor_off();
    for (count = 0; count <= 31; count ++){
        lcd_putchar(x);
        msecs_delay(10*d);
        lcd_cursor_left();
        lcd_putchar(' ');
        if (count == 15) lcd_new_line();
    }
}


void stepper_deg(int p){
    float relative_angle;
    int relative_steps;
    int i;

    float p_rad = deg_to_rad(p);

    relative_angle = p_rad - motor_angle;

    relative_steps =  norm_angle(relative_angle);

    if (relative_steps > 0){
        for (i = relative_steps; i>0; i--){
            step_clockwise();
            motor_angle += step_angle;


            if (i % 40 == 0){
                calc_frac();
                send_message();
            }
        }
    }
    else{
        for (i = relative_steps; i<0; i++){
            step_counter_clockwise();
            motor_angle -= step_angle;

            if (i % 40 == 0){
                calc_frac();
                send_message();
            }
        }
    }


    calc_frac();
    send_message();

}

void calc_frac(void){
    int integer_part = (int)motor_angle;
    int fractional_part = (int)(motor_angle * 1000);
    if (fractional_part >= 100) sprintf(out_string, "4,%d.%d\n", integer_part, fractional_part);
    else if (fractional_part >= 10) sprintf(out_string, "4,%d.0%d\n", integer_part, fractional_part);
    else sprintf(out_string, "4,%d.00%d\n", integer_part, fractional_part);
}

void stepper_scan(int l, int r){
    float relative_angle;
    int relative_steps;
    int i;
    char buffer[16];

    float l_rad = deg_to_rad(l);
    float r_rad = deg_to_rad(r);


    // move to angle l
    relative_angle = l_rad - motor_angle;

    relative_steps =  norm_angle(relative_angle);

    if (relative_steps > 0){
        for (i = relative_steps; i>0; i--) step_clockwise();
    }
    else{
        for (i = relative_steps; i<0; i++) step_counter_clockwise();
    }

    motor_angle += relative_angle;

    //print l to LCD
    lcd_home();
    lcd_clear();
    lcd_puts("left: ");
    sprintf(buffer, "%d", l);
    lcd_puts(buffer);
    lcd_puts("[deg.]");


    //move to angle r
    relative_angle = r_rad - motor_angle;

    relative_steps =  norm_angle(relative_angle);

    for (i = relative_steps; i>0; i--) step_clockwise();

    motor_angle += relative_angle;

    //print r to LCD
    lcd_new_line();
    lcd_puts("right: ");
    sprintf(buffer, "%d", r);
    lcd_puts(buffer);
    lcd_puts("[deg.]");
}

float norm_angle(float relative_angle){
    float relative_steps;

    if (relative_angle > PI + 0.1) relative_angle = relative_angle - 2*PI;
    if (relative_angle < -PI - 0.1) relative_angle = relative_angle + 2*PI;

    relative_steps =  relative_angle/step_angle;

    return relative_steps;

}

float deg_to_rad(int a){
    float a_rad = a*PI;
    a_rad = a_rad/180.0;
    return a_rad;
}

int rad_to_deg(float a_rad){
    int a = a_rad*180;
    a = a/PI;
    return a;
}



