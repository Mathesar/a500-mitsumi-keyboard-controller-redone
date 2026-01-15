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

#define KEYBOARD_HANDSHAKE_TIMEOUT_MS   143UL

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
    // assert KDAT
    KDAT_WRITE = 0;    
    us_timer_set(20);
    us_timer_wait();

    // send clk pulse
    us_timer_set(20);
    KCLK = 0;
    us_timer_wait();
    us_timer_set(20);
    KCLK = 1;

    // release KDAT
    us_timer_wait();
    KDAT_WRITE = 1;

    // wait for sync
    return keyboard_wait_handshake(); 
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
