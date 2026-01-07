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

#ifndef MAIN_H
#define	MAIN_H

//total number of rows in matrix scan result
//last row is a "dummy" row that hold the special keys
#define MATRIX_N_ROWS   16
//total number of columns in scan result
#define MATRIX_N_COLS   7

// keyboard matrix IO definitions
#define MATRIX_Y0       TRISCbits.TRISC5
#define MATRIX_Y1       TRISCbits.TRISC2
#define MATRIX_Y2       TRISCbits.TRISC7
#define MATRIX_Y3       TRISCbits.TRISC1
#define MATRIX_Y4       TRISDbits.TRISD4
#define MATRIX_Y5       TRISCbits.TRISC0
#define MATRIX_Y6       TRISDbits.TRISD5
#define MATRIX_Y7       TRISAbits.TRISA6
#define MATRIX_Y8       TRISDbits.TRISD6
#define MATRIX_Y9       TRISAbits.TRISA7
#define MATRIX_Y10      TRISDbits.TRISD7
#define MATRIX_Y11      TRISEbits.TRISE2
#define MATRIX_Y12      TRISEbits.TRISE1
#define MATRIX_Y13      TRISEbits.TRISE0
#define MATRIX_Y14      TRISAbits.TRISA5

// special keys
#define LEFT_AMIGA      PORTBbits.RB0
#define LEFT_ALT        PORTBbits.RB1
#define LEFT_SHIFT      PORTBbits.RB2
#define CTRL            PORTBbits.RB3
#define RIGHT_ALT       PORTBbits.RB4
#define RIGHT_SHIFT     PORTBbits.RB5
#define RIGHT_AMIGA     PORTAbits.RA4

// key state structure
typedef struct
{
    uint8_t key_current_states[MATRIX_N_ROWS];
    uint8_t debounce_timer;    
} key_state_t;

// timer macro's
#define timer_us                    4
#define timer_get()                 (TMR0L)
#define timer_diff(timer)           (timer_get()-(uint8_t)timer)
#define timer_until_us(timer,us)    {while(timer_diff((timer))<(((us)/timer_us)+1));}
#define timer_add_us(timer,us)      (timer+(((us)/timer_us)+1))            

#endif	/* MAIN_H */

