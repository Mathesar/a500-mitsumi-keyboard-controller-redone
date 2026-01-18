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
#pragma config WDT = ON         // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 256      // Watchdog Timer Postscale Select bits (1:32768)

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

#define MATRIX_SCAN_INTERVAL_MS     11
#define RESET_DEBOUNCE_TIME_MS      50
#define RESET_PULSE_DURATION_MS     500

#ifndef NDEBUG

#define DBG_PERIOD_US 832   // ~1200 baud
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

// initialize system
void init(void)
{
    // 8MHz
    OSCCONbits.IRCF = 0b111; 
    
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
    LATC = 0x40;  // debug/TXD high, CAPS LED on  
    TRISC = 0xb7; // C3 is CAPS LED, C6 is debug/TXD
    
    // configure portd
    LATD = 0x00;
    TRISD = 0xf3; // D2,D3 are AUX outputs
    
    // configure porte
    LATE = 0x00;
    TRISE = 0x07;
    
    // configure timer0 in 16bit mode, 128 micro-second per count
    T0CON = 0b10000111;
    
    //configure timer1 in 16bit mode, 4 micro-second per count
    T1CON = 0b10110001;
}

void main(void) 
{
    timer_t matrix_timer;
    timer_t reset_debounce_timer;
    uint8_t key_code;
    uint8_t  n_events = 0; 
    
    enum state_t
    {
        POWERUP = 0,
        POWERUP_SYNCED,
        START_SCAN,
        SCAN,
        SEND,
        OUT_OF_SYNC,
        RESET_HOST,
        RESET_HOST_WAIT_PULSE,
        RESET_HOST_WAIT_RELEASE
    };
    
    enum state_t state = POWERUP;
    
    // initialize system
    init();
    
#ifndef NDEBUG
    printf("A500 Mitsumi redone\n");
#endif
                
    // main loop
    while(1)       
    {
        // check for ctrl-amiga-amiga reset outside main state machine
        if( (!CTRL) && (!LEFT_AMIGA) && (!RIGHT_AMIGA) )
        {
            if( (timer_get() - reset_debounce_timer) > ms_to_timer(RESET_DEBOUNCE_TIME_MS) )
            {
                state = RESET_HOST;
            }               
        }
        else
        {
            reset_debounce_timer = timer_get();
        }
                
        // main state machine
        switch(state)
        {
            // We come here after a reset.
            case POWERUP:
                // send sync pulse and wait for host to respond      
                if(keyboard_synchronize())
                    state = POWERUP_SYNCED;
                
                break;
                
            // Power-up synchronization achieved.
            case POWERUP_SYNCED:
                // send "ïnitiate power-up key stream" code
                if(!keyboard_send(POWERUP_KEY_STREAM))
                    state = POWERUP;
                 
                // send "terminate key stream" code
                if(!keyboard_send(TERMINATE_KEY_STREAM))
                    state = POWERUP;
                
                // turn off CAPS-LOCK LED
                CAPS_LOCK = 1;
                
                // we are now ready to scan the keyboard matrix
                state = START_SCAN;
                                
                break;  

            // Start scanning.
            case START_SCAN:
                // init scan timer
                matrix_timer = timer_get();
                
                if(n_events)
                    state = SEND;
                else
                    state = SCAN;
                
                break;
                
            // Scan matrix.
            case SCAN:
                if( (timer_get() - matrix_timer) > ms_to_timer(MATRIX_SCAN_INTERVAL_MS) )
                {
                    matrix_timer += ms_to_timer(MATRIX_SCAN_INTERVAL_MS);
                    
                    // scan keyboard
                    if(matrix_scan())
                    {
                        // extract key codes
                        n_events = matrix_decode();
                        if(n_events)
                        {
                            state = SEND;
                        }
                    }                               
                }                  
                break;
                
            // Send received key codes to host.    
            case SEND:                             
                if(n_events)
                {
                    key_code = key_codes[--n_events];
                    if(!keyboard_send(key_code))
                        state = OUT_OF_SYNC;    
                }     
                else
                {
                    // all key events sent
                    state = SCAN;
                }                    
                break;

            // We are out of sync with the host computer.           
            case OUT_OF_SYNC:                
                // send sync pulse and wait for host to respond                
                if(keyboard_synchronize())
                {
                    // we are synchronized again, send "lost sync" code
                    if(keyboard_send(LOST_SYNC))
                    {
                        // resend garbled code
                        if(keyboard_send(key_code))
                        {
                            state = START_SCAN;
                        }
                    }                     
                }               
                break;
                
            // Reset requested.
            case RESET_HOST:                
                // assert reset to host computer                
                HOST_RESET = 0;
                
                // turn on CAPS-LOCK LED
                CAPS_LOCK = 0;
                
                // set reset pulse timer
                timer_t reset_timer = timer_get();
                
                state = RESET_HOST_WAIT_PULSE;
                
                break;
            
            // Wait for reset pulse to finish.
            case RESET_HOST_WAIT_PULSE:                
                if( (timer_get() - reset_timer) < ms_to_timer(RESET_PULSE_DURATION_MS) )
                {
                    state = RESET_HOST_WAIT_RELEASE;
                }
                break;
                                        
            // Wait for user to release ctrl-amiga-amiga.   
            case RESET_HOST_WAIT_RELEASE:      
                if( CTRL || LEFT_AMIGA || RIGHT_AMIGA )
                {
                    // Now reset ourself as well.
                    // This will also release the host reset.
                    RESET();
                }         
                break;
                
            // We should never, ever come here.
            default:    
                RESET();                
                break;
        }     
        
        // clear watchdog
        CLRWDT();        
    }

    // And here too.
    RESET();
}
