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

// set below define to disable debugging
//#define NDEBUG

#ifndef NDEBUG
#include <stdio.h>
#endif

// keyboard matrix row IO definitions
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

// special key IO definitions
#define LEFT_AMIGA      PORTBbits.RB0
#define LEFT_ALT        PORTBbits.RB1
#define LEFT_SHIFT      PORTBbits.RB2
#define CTRL            PORTBbits.RB3
#define RIGHT_ALT       PORTBbits.RB4
#define RIGHT_SHIFT     PORTBbits.RB5
#define RIGHT_AMIGA     PORTAbits.RA4

// other IO
#define DEBUG           LATCbits.LATC6
#define KDAT_WRITE      TRISDbits.TRISD0
#define KDAT_READ       PORTDbits.RD0
#define KCLK            TRISDbits.TRISD1
#define CAPS_LOCK       LATCbits.LATC3
#define HOST_RESET      TRISCbits.TRISC4

// micro-second timer macro's, resolution=4us, max=255ms
#define us_timer_set(us)            {TMR1 = (uint16_t)(65536UL-(((us)+3UL)/4UL));PIR1bits.TMR1IF = 0;}
#define us_timer_expired()          (PIR1bits.TMR1IF)
#define us_timer_wait()             {while(!PIR1bits.TMR1IF);}

// shared timer macro's
#define timer_get()                 (TMR0)
#define ms_to_timer(ms)             (((uint32_t)ms*1000UL)/128UL)
typedef uint16_t timer_t;

#endif	/* MAIN_H */
