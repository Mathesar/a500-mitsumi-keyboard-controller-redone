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

// key state structure
typedef struct
{
    uint8_t key_current_states[MATRIX_N_ROWS]; 
} key_state_t;

key_state_t key_state;
uint8_t matrix[MATRIX_N_ROWS];
uint8_t key_codes[MATRIX_N_EVENTS];
bool caps_lock = false;

// matrix to key code mapping
const uint8_t matrix_to_code_map[MATRIX_N_ROWS][MATRIX_N_COLS] = 
{
//      2     4     1     0     5     3   extra     (column)
    { 0x4f, 0x4d, 0x4c, 0x5f, 0x4a, 0x4e, 0x00 },   // row 0
    { 0x44, 0x41, 0x0d, 0x59, 0x0f, 0x46, 0x00 },   // row 1
    { 0x1b, 0x40, 0x0c, 0x58, 0x1d, 0x2b, 0x00 },   // row 2
    { 0x1a, 0x00, 0x0b, 0x57, 0x2d, 0x2a, 0x00 },   // row 3
    { 0x19, 0x3a, 0x0a, 0x56, 0x3d, 0x29, 0x00 },   // row 4
    { 0x18, 0x39, 0x09, 0x5c, 0x43, 0x28, 0x00 },   // row 5
    { 0x17, 0x38, 0x08, 0x55, 0x1e, 0x27, 0x00 },   // row 6
    { 0x16, 0x37, 0x07, 0x5b, 0x2e, 0x26, 0x00 },   // row 7
    { 0x15, 0x36, 0x06, 0x54, 0x3e, 0x25, 0x00 },   // row 8
    { 0x14, 0x35, 0x05, 0x53, 0x3c, 0x24, 0x00 },   // row 9
    { 0x13, 0x34, 0x04, 0x52, 0x1f, 0x23, 0x00 },   // row 10
    { 0x12, 0x33, 0x03, 0x51, 0x2f, 0x22, 0x00 },   // row 11
    { 0x11, 0x32, 0x02, 0x50, 0x3f, 0x21, 0x00 },   // row 12
    { 0x10, 0x31, 0x01, 0x5a, 0x5e, 0x20, 0x00 },   // row 13
    { 0x42, 0x30, 0x00, 0x45, 0x5d, 0x62, 0x00 },   // row 14
    { 0x66, 0x64, 0x60, 0x63, 0x65, 0x61, 0x67 }    // row 15 (extra)
};

// CAPS-LOCK key code
#define CAPS_LOCK_CODE  0x62

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
    if(PORTBbits.RB6 == 0)
        columns |= 0x10;
    if(PORTBbits.RB7 == 0)
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

// Toggle CAPS-LOCK on or off
// returns correct key code
uint8_t matrix_handle_caps_lock(void)
{
    if(caps_lock)
    {
        // turn caps lock off
        caps_lock = false;
        CAPS_LOCK = 1;
        return CAPS_LOCK_CODE | 0x80;
    }
    else
    {
        // turn caps lock on
        caps_lock = true;
        CAPS_LOCK = 0;
        return CAPS_LOCK_CODE;
    }     
}

//Scan the matrix and place the scanned matrix in global <matrix>.
//Returns true if OK.
//Returns false if ghosting detected, result is then invalid.
bool matrix_scan(void)
{
    uint8_t row, columns;
    
    // scan keyboard matrix
    for(row = 0; row < MATRIX_N_ROWS-1; row++)
    {
        // select row
        matrix_select_row(row);
        
        // allow 160us for row to settle
        us_timer_set(160);
        us_timer_wait();
        // scan a row every 400us
        us_timer_set(240);

        // read pressed keys for this row
        columns = matrix_read_columns();   
        
        // deselect row
        matrix_deselect_row(row);

        // store result for this row
        matrix[row] = columns;
        
        // wait for timer to expire before scanning next row     
        us_timer_wait();
    }
    
    // scan special keys and put them in the extra row
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
    matrix[row] = columns;    
    
    //check for ghosting
    for(row = 0; row < MATRIX_N_ROWS-1; row++)
    {
        // if there is any row with more than 1 column set
        // and these columns overlap with a colum in any 
        // other row we have ghosting
        columns = matrix[row];
        if( matrix_count_bits_set(columns) > 1 )
        {
            // >1 column set, check against other rows
            for(uint8_t x = 0; x < MATRIX_N_ROWS-1; x++)
            {                
                if(x != row)
                {
                    if(columns & matrix[x])
                    {
                        // overlap! ghosting detected
                        return false;
                    }                       
                }
            }    
        }        
    }
    
    //scan OK
    return true;
}

// takes the scanned matrix in global <matrix> and scans for 
// key pressed / released events
// store events as key codes in global <keycodes>
// returns the number of decoded key events
uint8_t matrix_decode(void)
{ 
    uint8_t n = 0;

    // go through all rows
    for(uint8_t row=0; row<MATRIX_N_ROWS; row++)
    {
        // check for pressed or released events
        uint8_t pressed  =  matrix[row] & ~key_state.key_current_states[row];
        uint8_t released = ~matrix[row] &  key_state.key_current_states[row];
        
        // go through all columns   
        uint8_t column_mask = 1;
        for(uint8_t column=0; column<MATRIX_N_COLS; column++)
        {              
            uint8_t code = matrix_to_code_map[row][column];
            
            if(code == CAPS_LOCK_CODE)
            {
                // CAPS-LOCK is special, toggles on "pressed" events
                if(pressed&column_mask)
                {
                    key_codes[n++] = matrix_handle_caps_lock();
                }               
            }
            else if(pressed&column_mask)
            {            
                key_codes[n++] = code;
#ifndef NDEBUG
                printf("[%u,%u] DOWN, CODE:0x%02X\n", row, column, matrix_to_code_map[row][column]);
#endif
            }
            else if(released&column_mask)
            {            
                key_codes[n++] = code | 0x80;     
#ifndef NDEBUG
                printf("[%u,%u] UP\n", row, column);
#endif
            }
            
            // update states
            key_state.key_current_states[row] ^= column_mask&(pressed|released);
            
            // next column
            column_mask <<= 1;
            
            // check for buffer full
            if(n >= MATRIX_N_EVENTS)
                return MATRIX_N_EVENTS;
        }    
    }
        
    // return number of key pressed/released events detected
    return n;
}
