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

#include <xc.h>
#include "main.h"
#include <stdbool.h>

#define KEYBOARD_HANDSHAKE_TIMEOUT_MS   50UL
#define KEYBOARD_SYNC_RETRIES           10

const uint8_t key_codes[MATRIX_N_ROWS][MATRIX_N_COLS] = 
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

// wait for host computer handshake
bool keyboard_wait_handshake(void)
{
    us_timer_set(KEYBOARD_HANDSHAKE_TIMEOUT_MS*1000UL);    
    while(KDAT_READ)
    {
        if(us_timer_expired())
            return false;
    }
    return true;    
}

// synchronize with host computer
bool keyboard_synchronize(void)
{
    for(uint8_t retries = 0; retries<KEYBOARD_SYNC_RETRIES; retries++)
    {
        // release KDAT
        KDAT_WRITE = 1;    
        us_timer_set(20);
        us_timer_wait();

        // send clk pulse
        us_timer_set(20);
        KCLK = 0;
        us_timer_wait();
        KCLK = 1;

        //wait for sync
        if(keyboard_wait_handshake())
            return true; // succes! We are synchronized
    }
    
    // synchronization failed
    return false;
}

// send a byte to the host computer
bool keyboard_send(uint8_t code)
{
    // check for KDAT released
    us_timer_set(KEYBOARD_HANDSHAKE_TIMEOUT_MS*1000);    
    while(!KDAT_READ)
    {
        if(us_timer_expired())
            return false;   // KDAT is stuck low
    }
    
    // original Mitsumi keyboard sends a short pulse on
    // KDAT which doesn't seem necessary. We do it as well.   
    us_timer_set(20)
    KDAT_WRITE = 0;
    us_timer_wait();  
    us_timer_set(100);
    KDAT_WRITE = 1;
    
    // prepare data (rotate and complement)
    uint8_t tx_code = (code << 1) & 0xfe;
    if(code & 0x80)
        tx_code++;    
    tx_code = ~tx_code;
        
    // now send all data in loop
    uint8_t count = 8;
    do
    {
        us_timer_wait();
        us_timer_set(20);
        if(tx_code&0x80)
            KDAT_WRITE = 1;
        if(!(tx_code&0x80))
            KDAT_WRITE = 0;
        code <<= 1;
        us_timer_wait();
        us_timer_set(20);
        KCLK = 0;
        us_timer_wait();
        us_timer_set(20);
        KCLK = 1;
    }
    while(--count);      
    
    // release KDAT
    us_timer_wait();
    KDAT_WRITE = 1;
    
    // wait for sync pulse
    return keyboard_wait_handshake(); 
}

// send a key hit/released code to the host computer
void keyboard_send_key_code(uint8_t row, uint8_t column, bool released)
{
#ifndef NDEBUG
    if(released)
        printf("[%2d,%d] released\n", row, column);
    else
        printf("[%2d,%d] hit\n", row, column);
#endif
       
    // look up code here
        
    // and send keycode
    keyboard_send(row);   
}