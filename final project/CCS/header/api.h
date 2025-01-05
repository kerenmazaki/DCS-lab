#ifndef _api_H_
#define _api_H_

void state1_func(void);
void state2_func(void);
void state3_func(void);
void state4_func(void);

void inc_lcd(int x);
void dec_lcd(int x);
void rra_lcd(char x);
void stepper_deg(int p);
void stepper_scan(int l, int r);
float norm_angle(float relative_angle);
float deg_to_rad(int a);
int rad_to_deg(float a_rad);
void calc_frac(void);

#endif
