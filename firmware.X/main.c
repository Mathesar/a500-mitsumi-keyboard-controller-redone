/*
 *  A500-Mitsumi-controller-redone firmware
 * 
 *  By Dennis van Weeren (2026)
 *  
 *  A500-Mitsumi-controller-redone is free software: you can redistribute it 
 *  and/or modify it under the terms of the GNU General Public License as 
 *  published by the Free Software Foundation, either version 3 of the License, 
 *  or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with A500-Mitsumi-controller-redone.  
 *  If not, see <https://www.gnu.org/licenses/>.
 */

// CONFIG1H
#pragma config OSC = INTIO67    // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = ON        // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 1         // Brown Out Reset Voltage bits ()

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "main.h"
#include "matrix.h"
#include "keyboard.h"

key_state_t key_state;
uint8_t key_matrix[15];

void update_key_states(void)
{
    //disable debounce timer for now
    key_state.debounce_timer = 0;
        
    if(key_state.debounce_timer == 0)
    {
        // debounce timer expired, check for depressed keys        
        for(uint8_t row=0; row<MATRIX_N_ROWS; row++)
        {
            uint8_t column_mask = 1;
            for(uint8_t column=0; column<MATRIX_N_COLS; column++)
            {              
                uint8_t mask = column_mask & (~key_matrix[row]);            
                if(mask & key_state.key_current_states[row])
                {
                    //send message to computer here
                    keyboard_send_key_code(row, column, true);
                    
                    //key was depressed, update state as it has been handled
                    key_state.key_current_states[row] &= ~mask;                   
                }     
                
                column_mask <<= 1;
            }
        }
        
        // check for pressed keys
        for(uint8_t row=0; row<MATRIX_N_ROWS; row++)
        {
            uint8_t column_mask = 1;
            for(uint8_t column=0; column<MATRIX_N_COLS; column++)
            {              
                uint8_t mask = column_mask & key_matrix[row];                
                if(mask & ~key_state.key_current_states[row])
                {
                    //send message to computer here
                    keyboard_send_key_code(row, column, false);
                    
                    //key was pressed, update state as it has been handled
                    key_state.key_current_states[row] |= mask;                   
                }   

                column_mask <<= 1;
            }
        }        
    }
}

#ifndef NDEBUG

#define DBG_PERIOD_US 833
#define DBG_BIT_OUT(bit)    {   us_timer_set(DBG_PERIOD_US);        \
                                if(!(bit)){DEBUG = 0;}              \
                                if(bit){DEBUG = 1;}                 \
                                us_timer_wait();                    };

// 1200 baud, bit-banged serial debug output
void debug_out(uint8_t data)
{
    // startbit
    DBG_BIT_OUT(0);    
    // data
    DBG_BIT_OUT(data&0x01);
    DBG_BIT_OUT(data&0x02);
    DBG_BIT_OUT(data&0x04);
    DBG_BIT_OUT(data&0x08);
    DBG_BIT_OUT(data&0x10);
    DBG_BIT_OUT(data&0x20);
    DBG_BIT_OUT(data&0x40);
    DBG_BIT_OUT(data&0x80);
    // stopbit
    DBG_BIT_OUT(1);    
}

void putch(char c)
{
    debug_out(c);
}

#endif


void init(void)
{
    OSCCONbits.IRCF = 0b111; // 8MHz
    
    // disable comparators
    CMCONbits.CM = 0b111;
        
    // disable analog inputs    
    ADCON1bits.PCFG = 0b1111;
    
    //configure porta
    LATA = 0x00;
    TRISA = 0xff;
    
    // configure portb
    LATB = 0x00;
    TRISB = 0xff;
    INTCON2bits.RBPU = 0; // enable pullups
    
    // configure portc 
    LATC = 0x48;
    TRISC = 0xb7; // C3 is CAPS LED, C6 is debug/TXD
    
    // configure portd
    LATD = 0x00;
    TRISD = 0xf3; // D2,D3 are AUX outputs
    
    // configure porte
    LATE = 0x00;
    TRISE = 0x07;
    
    //configure timer1 in 16bit mode, 1 micro-second per count
    T1CON = 0b10010001;
}

void main(void) {
    
    // initialize system
    init();
    
    while(1)
    {
        // read matrix
        if(!matrix_read(key_matrix))
        {
            // ghosting detected
            printf("ghost\n");
        }
        else
        {
            // update key states
            update_key_states();
        }  
    }

    // We should never, ever come here
    return;
}



