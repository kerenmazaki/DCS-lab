#include  "../header/bsp.h"   		// private library - API layer
#include  "../header/hal.h"   		// private library - API layer
#include "string.h"
#include "stdio.h"
// Global Variables
char KB='k';
char last_KB = 'k';

unsigned int dmaBlockSize;


//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
    GPIOconfig();
	TIMERconfig();
	//ADCconfig();
    lcd_init();
    lcd_clear();
}

//---------------------------------------------------------------------
//            KEY PAD
//---------------------------------------------------------------------
void KB_Char(void){
    KB = 'k';
    KeypadPortOUT = 0x0E;
    delay(debounceVal);
    if ( ( KeypadPortIN & 0x10 ) == 0 ) KB = 'F';
    else if ( ( KeypadPortIN & 0x20 ) == 0 ) KB = 'B';
    else if ( ( KeypadPortIN & 0x40 ) == 0 ) KB = '0';
    else if ( ( KeypadPortIN & 0x80 ) == 0 ) KB = 'A';

    KeypadPortOUT = 0x0D;
    delay(debounceVal);
    if ( ( KeypadPortIN & 0x10 ) == 0 ) KB = 'E';
    else if ( ( KeypadPortIN & 0x20 ) == 0 ) KB = '9';
    else if ( ( KeypadPortIN & 0x40 ) == 0 ) KB = '8';
    else if ( ( KeypadPortIN & 0x80 ) == 0 ) KB = '7';

    KeypadPortOUT = 0x0B;
    delay(debounceVal);
    if ( ( KeypadPortIN & 0x10 ) == 0 ) KB = 'D';
    else if ( ( KeypadPortIN & 0x20 ) == 0 ) KB = '6';
    else if ( ( KeypadPortIN & 0x40 ) == 0 ) KB = '5';
    else if ( ( KeypadPortIN & 0x80 ) == 0 ) KB = '4';

    KeypadPortOUT = 0x07;
    delay(debounceVal);
    if ( ( KeypadPortIN & 0x10 ) == 0 ) KB = 'C';
    else if ( ( KeypadPortIN & 0x20 ) == 0 ) KB = '3';
    else if ( ( KeypadPortIN & 0x40 ) == 0 ) KB = '2';
    else if ( ( KeypadPortIN & 0x80 ) == 0 ) KB = '1';
}


void set_recorder_char(void){
    if ((last_KB == KB) && (KB != 'k')) {
        if (KB == '1') KB = 'G';
        else if (KB == '2') KB = 'H';
        else if (KB == '3') KB = 'I';
        else if (KB == 'C') KB = 'J';
        else if (KB == '4') KB = 'K';
        else if (KB == '5') KB = 'L';
        else if (KB == '6') KB = 'M';
        else if (KB == 'D') KB = 'N';
        else if (KB == '7') KB = 'O';
        else if (KB == '8') KB = 'P';
        else if (KB == '9') KB = 'Q';
        else if (KB == 'E') KB = 'R';
        else if (KB == 'A') KB = 'S';
        else if (KB == '0') KB = 'U';
        else if (KB == 'B') KB = 'W';
        else if (KB == 'F') KB = 'Y';
        i--;
        lcd_cursor_left();
    }
    else if ((last_KB == 'S') && (KB == 'A')){
        KB = 'T';
        i--;
        lcd_cursor_left();
    }
    else if ((last_KB == 'U') && (KB == '0')){
        KB = 'V';
        i--;
        lcd_cursor_left();
    }
    else if ((last_KB == 'W') && (KB == 'B')){
        KB = 'X';
        i--;
        lcd_cursor_left();
    }
    else if ((last_KB == 'Y') && (KB == 'F')){
        KB = 'Z';
        i--;
        lcd_cursor_left();
    }
}


//---------------------------------------------------------------------
//                      DMA
//---------------------------------------------------------------------

void DMA_stop(){
    DMA0CTL &= ~DMAEN + ~DMAIE;
    P9OUT &= ~0xFF;
    TACTL &=MC_0;
}

void splitSentence(char* sentence, char words[10][11], int* numWords) {
    int in_word = 0;
    int word_index = 0;
    int char_index = 0;

    *numWords = 0;

    while (*sentence) {
        if (in_word) {
            if (*sentence == ' ') { // End of a word
                words[word_index][char_index] = *sentence;
                char_index++;
                words[word_index][char_index] = '\0';  // Null-terminate the word
                word_index++;
                char_index = 0;
                in_word = 0;
                (*numWords)++;
            } else {
                // Continue the word
                words[word_index][char_index] = *sentence;
                char_index++;
            }
        } else {
            if (*sentence != ' ') {
                // Start of a new word
                in_word = 1;
                if (char_index < 10) {  // Ensure not to exceed the max word length
                    words[word_index][char_index] = *sentence;
                    char_index++;
                }
            }
        }
        sentence++;
    }
    if (in_word) {
        // End of the last word
        words[word_index][char_index] = ' ';
        char_index++;
        words[word_index][char_index] = '\0';
        (*numWords)++;
    }
}

void mergeSentences(int index1, int index2) {
    char tempWords1[10][11];
    char tempWords2[10][11];
    int numWords1 = 0, numWords2 = 0;
    int merge_add = 0;

    // Split sentences into temporary arrays
    splitSentence(data_matrix[index1], tempWords1, &numWords1);
    splitSentence(data_matrix[index2], tempWords2, &numWords2);

    // Prepare DMA transfer
    int totalWords = numWords1 + numWords2;

    // Configure DMA
    DMA_state2();  // Initialize DMA channel 0

    // Start DMA transfer alternately from tempWords1 and tempWords2 to strMerge
    unsigned int j = 0;  // Index for strMerge
    unsigned int count1 = 1, count2 = 1;
    for (j = 0; j < totalWords; j++) {
            if (count1 <= numWords1){
                // Transfer word from tempWords1 to strMerge[j]
                dmaBlockSize = strlen(tempWords1[j]);  // Size of current word (not including null terminator)
                DMA0SZ = dmaBlockSize;
                DMA0SA = (void (*)( ))tempWords1[j];
                DMA0DA = (void (*)( ))&strMerge[merge_add];
                merge_add += dmaBlockSize;
                DMA0CTL |= DMAEN;                         // Enable DMA0
                DMA0CTL |= DMAREQ;                        // trigger DMA
                count1 ++;
            }
            if (count2 <= numWords2){
                // Transfer word from tempWords2 to strMerge[j]
                dmaBlockSize = strlen(tempWords2[j]);  // Size of current word (not including null terminator)
                DMA0SZ = dmaBlockSize;
                DMA0SA = (void (*)( ))tempWords2[j];
                DMA0DA = (void (*)( ))&strMerge[merge_add];
                merge_add += dmaBlockSize;
                DMA0CTL |= DMAEN;                         // Enable DMA0
                DMA0CTL |= DMAREQ;                        // trigger DMA
                count2 ++;
            }
        }


        // Wait for DMA transfer to complete
        while (DMA0CTL & DMAEN);

        j++;  // Increment strMerge index
    }

//-------------------------------------------------------------------------------------
//            DMA init
//-------------------------------------------------------------------------------------

void DMA_state2(void) {
    DMACTL0 = DMA0TSEL_0; // DMA0 trigger: Software initiated
    DMA0CTL = DMADT_1 | DMASRCINCR_3 | DMADSTINCR_3 | DMASRCBYTE | DMADSTBYTE; // Block transfer, increment both source and destination addresses
}

void DMA_state3(void) {
    DMACTL0 = DMA0TSEL_1;                     // CCR2 trigger
    DMA0CTL = DMADT_4 + DMASRCINCR_3 + DMASBDB; // Rpt, inc src
}

void DMA_state4(void) {
    DMACTL0 = DMA0TSEL_0;                     // DMA0 trigger: Software initiated
    DMA0CTL = DMADT_1 | DMASRCINCR_3 | DMADSTINCR_2 | DMASRCBYTE | DMADSTBYTE; // Block transfer, decrement source and increment destination addresses
    }

//*********************************************************************
//            count half sec
//*********************************************************************
void Half_Sec(){
  TACCR0=0xFFFF;
  TACTL=TASSEL_2+MC_1+ID_3;   //CLK=2=SMCLK,CTL=3=UP/DOWN MODE,DIV=3=/8
  __bis_SR_register(LPM0_bits+GIE);
}

//*********************************************************************
//            count 0.45 sec
//*********************************************************************
void ff_mSec(){
  TACCR0=0xE661;
  TACTL=TASSEL_2+MC_1+ID_3;   //CLK=2=SMCLK,CTL=3=UP/DOWN MODE,DIV=3=/8
  __bis_SR_register(LPM0_bits+GIE);
}

//---------------------------------------------------------------------
//            LCD
//---------------------------------------------------------------------
//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
            LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
    }
    else
    {
            LCD_DATA_WRITE = c;
            lcd_strobe();
    }

    LCD_RS(0);
}
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){
    int place = 0;
    while(*s){
        lcd_data(*s++);
        place ++;
        if (place == 16) {
            lcd_new_line();
        }
        if (place == 32) {
        break;
        }
    }
}
//******************************************************************
//    write frequency template to LCD
//******************************************************************
void write_freq_tmp_LCD(void){
   lcd_clear();
   lcd_home();
    const char SquareWaveFreq[] = "fin=";
    const char Hz[] = "Hz";
     lcd_puts(SquareWaveFreq);
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_cursor_right();
     lcd_puts(Hz);
}
//******************************************************************
//    write signal shape template to LCD
//******************************************************************
void write_signal_shape_tmp_LCD(void){
   lcd_clear();
   lcd_home();
    const char signal_shape[] = "signal shape: ";
     lcd_puts(signal_shape);
     lcd_new_line();
}
//******************************************************************
// initialize the LCD
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);
    
    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}
//******************************************************************
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  __no_operation();
  __no_operation();
//  asm("NOP");
//  asm("NOP");
  LCD_EN(0);
}
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) __no_operation();;//asm("nop"); // the command asm("nop") takes 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){
  
	unsigned char i;
        for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec
}
//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	
	for(i=t; i>0; i--);
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

//*********************************************************************
//            keypad Service Routine
//*********************************************************************
#pragma vector=PORT2_VECTOR  // For KeyPad
  __interrupt void KeyPad_ISR(void){
      //delay(debounceVal);
      KB_Char();
      KeypadPortOUT &= ~0x0F;  // Reset Row1-4
      //KeypadIRQIntEn &= ~0x02;  //disable interrupt
      KeypadIRQIntPend &= ~BIT1; // Reset Flag
//---------------------------------------------------------------------
//            Exit from a given LPM
//---------------------------------------------------------------------

      switch(lpm_mode){
          case mode0:
              LPM0_EXIT; // must be called from ISR only
              break;

          case mode1:
              LPM1_EXIT; // must be called from ISR only
              break;

          case mode2:
              LPM2_EXIT; // must be called from ISR only
              break;

          case mode3:
              LPM3_EXIT; // must be called from ISR only
              break;

          case mode4:
              LPM4_EXIT; // must be called from ISR only
              break;
      }

  }



//*********************************************************************
//            Port1 Interrupt Service Routine
//*********************************************************************
#pragma vector=PORT1_VECTOR
  __interrupt void PBs_handler(void){
	delay(debounceVal);
  
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
	if(PBsArrIntPend & PB0){
	  state = state1;
	  PBsArrIntPend &= ~PB0;
        }
        else if(PBsArrIntPend & PB1){
	  state = state2;
	  PBsArrIntPend &= ~PB1; 
        }
	else if(PBsArrIntPend & PB2){ 
	  state = state3;                       
	  PBsArrIntPend &= ~PB2;
        }
    else if(PBsArrIntPend & PB3){
      state = state4;
      PBsArrIntPend &= ~PB3;
        }
//---------------------------------------------------------------------
//            Exit from a given LPM 
//---------------------------------------------------------------------	
        switch(lpm_mode){
		case mode0:
		 LPM0_EXIT; // must be called from ISR only
		 break;
		 
		case mode1:
		 LPM1_EXIT; // must be called from ISR only
		 break;
		 
		case mode2:
		 LPM2_EXIT; // must be called from ISR only
		 break;
                 
		case mode3:
		 LPM3_EXIT; // must be called from ISR only
		 break;

		case mode4:
		 LPM4_EXIT; // must be called from ISR only
		 break;
	}
  }

////*********************************************************************
////            Port2 Interrupt Service Routine
////*********************************************************************
//#pragma vector=PORT2_VECTOR
//  __interrupt void PB3_handler(void){
//	delay(debounceVal);
//
////---------------------------------------------------------------------
////            selector of transition between states
////---------------------------------------------------------------------
//        if(PB3ArrIntPend & PB3){
//	  state = state4;
//	  PB3ArrIntPend &= ~PB3;
//        }
//
////---------------------------------------------------------------------
////            Exit from a given LPM
////---------------------------------------------------------------------
//        switch(lpm_mode){
//		case mode0:
//		 LPM0_EXIT; // must be called from ISR only
//		 break;
//
//		case mode1:
//		 LPM1_EXIT; // must be called from ISR only
//		 break;
//
//		case mode2:
//		 LPM2_EXIT; // must be called from ISR only
//		 break;
//
//                case mode3:
//		 LPM3_EXIT; // must be called from ISR only
//		 break;
//
//                case mode4:
//		 LPM4_EXIT; // must be called from ISR only
//		 break;
//	}
//  }
