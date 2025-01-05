#include  "../header/bsp.h"   		// private library - API layer
#include  "../header/hal.h"   		// private library - API layer
#include  "../header/api.h"   		// private library - API layer
#include "string.h"
#include "stdio.h"


#define ASCII_offset 48

char data_matrix[10][40] = {
    "An apple a day keeps the doctor away",
    "Climb on the bandwagon",
    "Dot the i's and cross the t's",
    "He who pays the piper calls the tune",
    "The pen is mightier than the sword",
    "The pot calling the kettle black",
    "Shed crocodile tears",
    "Close but no cigar",
    "Cut from the same cloth",
    "Strike while the iron’s hot"
};
char idiom_recorder[32];
const char led_num_arr[9] = {128, 64, 32, 16, 8, 4, 23, 13, 40};
const char Str[160] = "Google Colaboratory is a free Jupyter notebook environment that runs on Google’s cloud servers, letting the user leverage backend hardware like GPUs and TPUs";

int i = 0;

char strMerge[80];
char strMirror[160];

// DMA control variables
unsigned int dmaBlockSize;



//*********************************************************************
//            state 1 function
//*********************************************************************
void idiom_printer(void){
    i = 0;
    lcd_clear();
    lcd_home();
    char const * instr="write an idiom";
    lcd_puts(instr);
    while (state == state1){
        enterLPM(mode0);
        if (i==0) {
            lcd_clear();
            lcd_home();
        }
        if (i==16){
            lcd_new_line();
        }
        if (i == 32){
            i = 0;
            lcd_clear();
            lcd_home();
        }
        set_recorder_char();

        if (KB != 'k'){
            lcd_putchar(KB);
            idiom_recorder[i] = KB;
            last_KB = KB;
            i++;
        }
        //KeypadIRQIntEn |= 0x02;  //enable keypad interrupt
        //else idiom_recorder[i] = ' ';
    }
    //lcd_clear();
    i = 0;
    KB = 'k';
}

//*********************************************************************
//            state 2 function
//*********************************************************************
void merge_strings(void){
    i=0;
    int row = 1;
    int index1 = -1, index2 = -1;

    memset(strMerge, '\0', sizeof(strMerge));
    lcd_clear();
    lcd_home();
    char const * instr="choose 2 indices";
    lcd_puts(instr);
    lcd_new_line();
    while (state == state2){
         enterLPM(mode0);
         if (KB != 'k'){
             if (i == 0){
                 lcd_putchar(KB);
                 index1 = KB - '0';
                 i++;
             }
             else if (i==1){
                 lcd_putchar(',');
                 lcd_putchar(KB);
                 index2 = KB - '0';
                 break;
             }
         }
     }
    mergeSentences(index1,index2);
    lcd_clear();
    lcd_home();
    lcd_puts(&strMerge);

    while (state == state2){
        if (KB == 'F'){
            lcd_clear();
            lcd_home();
            if (32*row > strlen(strMerge) - 1){
                row = 0;
            }
            lcd_puts(&strMerge[32*row]);
            row ++;
        }
        enterLPM(mode0);
    }
    //lcd_clear();
}


//*********************************************************************
//            state 3 function
//*********************************************************************
void Show_nums(void){

    DMA_state3();
    while (state == state3){
        DMA0SA = (void (*)())led_num_arr;              // Source block address
        DMA0DA = (void (*)())&LEDsArrPort;             // Destination single address
        DMA0SZ = 0x09;                                // Block size
        DMA0CTL |= DMAEN;
        TACCR0=0xFFFF;
        TACTL=TASSEL_2+MC_1+ID_3;   //CLK=2=SMCLK,CTL=3=UP/DOWN MODE,DIV=3=/8
        //TACTL = TASSEL_2 + MC_2;     // timer config to compare mode for 0.5 secs.
        enterLPM(mode0);       // Enter LPM0
    }
    while (1) {
        if (LEDsArrPort == 40){
            //DMA0CTL = ~DMAEN;
            DMA_stop();
            LEDsArrPort = 0x00;
            break;
        }
    }
}

//*********************************************************************
//            state 4 function
//*********************************************************************
void flip_sentence(void){
    DMA_state4();
    dmaBlockSize = strlen(Str);  // Size of current word (not including null terminator)
    DMA0SZ = dmaBlockSize;
    DMA0SA = (void (*)( ))Str;
    DMA0DA = (void (*)( ))&strMirror[dmaBlockSize - 1];
    DMA0CTL |= DMAEN;                         // Enable DMA0
    DMA0CTL |= DMAREQ;                        // trigger DMA

    // Wait for DMA transfer to complete
    while (DMA0CTL & DMAEN);
    lcd_clear();
    lcd_home();
    lcd_puts(&strMirror);
    enterLPM(mode0);
    //lcd_clear();
}


