#ifndef _hal_H_
#define _hal_H_


extern int state3_start;
extern char in_string[100];
extern char out_string[50];
extern char state2_mode;
extern int state1_start;
extern float angle;
extern int d;
extern int steps;
extern float step_angle;
extern float motor_angle;
extern int script_number;
extern char state4_mode;

void sysConfig(void);
void step_clockwise(void);
void step_counter_clockwise(void);
void half_step_clockwise(void);
void half_step_counter_clockwise(void);
void DelayUs(unsigned int cnt);
void DelayMs(unsigned int cnt);
void delay(unsigned int t);
void enterLPM(unsigned char LPM_level);
void enable_interrupts();
void disable_interrupts();
void Half_Sec();
void msecs_delay(int X);
void get_angle(void);
void send_message();
void process_message(void);
void write_script(void);
void run_script(void);
void run_func(void);


void lcd_cmd(unsigned char c);
void lcd_data(unsigned char c);
void lcd_puts(const char * s);
void lcd_init();
void lcd_strobe();

#endif
