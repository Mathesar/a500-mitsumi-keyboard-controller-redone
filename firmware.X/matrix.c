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

#include "matrix.h"
#include "main.h"
#include <xc.h>

//select a row by pulling the output low
void matrix_select_row(uint8_t row)
{
    switch(row)
    {
        case 0:     MATRIX_Y0=0;    break;
        case 1:     MATRIX_Y1=0;    break;
        case 2:     MATRIX_Y2=0;    break;
        case 3:     MATRIX_Y3=0;    break;
        case 4:     MATRIX_Y4=0;    break;
        case 5:     MATRIX_Y5=0;    break;
        case 6:     MATRIX_Y6=0;    break;
        case 7:     MATRIX_Y7=0;    break;
        case 8:     MATRIX_Y8=0;    break;
        case 9:     MATRIX_Y9=0;    break;
        case 10:    MATRIX_Y10=0;   break;
        case 11:    MATRIX_Y11=0;   break;
        case 12:    MATRIX_Y12=0;   break;
        case 13:    MATRIX_Y13=0;   break;
        default:    MATRIX_Y14=0;   break;        
    }
}

//deselect a row by making the output high-z
void matrix_deselect_row(uint8_t row)
{
    switch(row)
    {
        case 0:     MATRIX_Y0=1;    break;
        case 1:     MATRIX_Y1=1;    break;
        case 2:     MATRIX_Y2=1;    break;
        case 3:     MATRIX_Y3=1;    break;
        case 4:     MATRIX_Y4=1;    break;
        case 5:     MATRIX_Y5=1;    break;
        case 6:     MATRIX_Y6=1;    break;
        case 7:     MATRIX_Y7=1;    break;
        case 8:     MATRIX_Y8=1;    break;
        case 9:     MATRIX_Y9=1;    break;
        case 10:    MATRIX_Y10=1;   break;
        case 11:    MATRIX_Y11=1;   break;
        case 12:    MATRIX_Y12=1;   break;
        case 13:    MATRIX_Y13=1;   break;
        default:    MATRIX_Y14=1;   break;        
    }
}

// return status of matrix columns
// if a key was pressed the corresponding column bit is set
uint8_t matrix_read_columns(void)
{
    uint8_t columns = (PORTA & 0x0f) ^ 0x0f;
    if(TRISB6 == 0)
        columns |= 0x10;
    if(TRISB7 == 0)
        columns |= 0x20;
         
    return columns;
}

// count the number of bits set in a byte using Brian Kernighan's Algorithm
uint8_t matrix_count_bits_set(uint8_t value)
{
    uint8_t bits_set; // number of bits set counter
    for (bits_set = 0; value; bits_set++)
    {
      value &= value - 1; // clear the least significant bit set
    }    
    
    return bits_set;
}

//Scan the matrix and place the scanned matrix in <result>.
//Returns true if OK.
//Returns false if ghosting detected, result is then invalid.
bool matrix_read(uint8_t *result)
{
    uint8_t row, columns;
    uint8_t possible_ghost_columns = 0;
    
    // scan keyboard matrix
    for(row = 0; row < MATRIX_N_ROWS-1; row++)
    {
        // select row
        matrix_select_row(row);
        
        // allow 160us for row to settle
        us_timer_set(160);
        us_timer_wait();
        // scan a row every 600us
        us_timer_set(440);

        // read pressed keys for this row
        columns = matrix_read_columns();   
        
        // deselect row
        matrix_deselect_row(row);
        
        // check for ghosting
        if(matrix_count_bits_set(columns) >= 2)
        {
            if(columns & possible_ghost_columns)
            {
                //ghosting detected!
                return false;
            }                            
            possible_ghost_columns |= columns;           
        }

        // store result for this row
        result[row] = columns;
        
        // wait for timer to expire before scanning next row     
        us_timer_wait();
    }
    
    // scan special keys and put them in the dummy row
    columns = 0;
    if(LEFT_AMIGA == 0)
        columns |= 0x01;
    if(LEFT_ALT == 0)
        columns |= 0x02;
    if(LEFT_SHIFT == 0)
        columns |= 0x04;
    if(CTRL == 0)
        columns |= 0x08;
    if(RIGHT_ALT == 0)
        columns |= 0x10;
    if(RIGHT_SHIFT == 0)
        columns |= 0x20;
    if(RIGHT_AMIGA == 0)
        columns |= 0x40;
    result[row] = columns;    
    
    //scan OK
    return true;
}
