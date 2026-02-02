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
#include "matrix.h"
#include "keyboard.h"
#include "main.h"

#define KEYBOARD_BUFFER_SIZE            32
#define KEYBOARD_HANDSHAKE_TIMEOUT_MS   143UL
#define KEYBOARD_SERIAL_TIME_STEP_US    20
#define WAIT_KDAT_HIGH()                {while(!KDAT_READ);}

struct
{
    uint8_t buffer[KEYBOARD_BUFFER_SIZE];
    uint8_t read_index;
    uint8_t size;
} keyboard_buffer;

// wait for host computer handshake
bool keyboard_wait_handshake(void)
{
    us_timer_set(KEYBOARD_HANDSHAKE_TIMEOUT_MS*1000UL);    

    // check for KDAT released
    WAIT_KDAT_HIGH();
     
    // now check for handshake pulse
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
    WAIT_KDAT_HIGH();
    
    // original Mitsumi keyboard sends a short pulse on
    // KDAT which doesn't seem necessary. We do it as well.   
    us_timer_set(KEYBOARD_SERIAL_TIME_STEP_US-5);
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
        us_timer_set(KEYBOARD_SERIAL_TIME_STEP_US-2);
        if(tx_code&0x80)
            KDAT_WRITE = 1;
        if(!(tx_code&0x80))
            KDAT_WRITE = 0;
        tx_code <<= 1;
        us_timer_wait();
        us_timer_set(KEYBOARD_SERIAL_TIME_STEP_US-5);   
        KCLK = 0;
        us_timer_wait();
        us_timer_set(KEYBOARD_SERIAL_TIME_STEP_US-7)
        KCLK = 1;
    }
    while(--count);      
    
    // release KDAT
    us_timer_wait();
    KDAT_WRITE = 1;
    
    // wait for sync pulse
    return keyboard_wait_handshake(); 
}

// put a key code into the FIFO buffer
void keyboard_put_buffer(uint8_t code)
{
    // return if buffer is completely full
    if(keyboard_buffer.size == KEYBOARD_BUFFER_SIZE)
        return;
    
    // when buffer is almost full post buffer overflow code
    if(keyboard_buffer.size == KEYBOARD_BUFFER_SIZE-1)
        code = KEYBOARD_BUFFER_OVERFLOW;
    
    // compute write index
    uint8_t write_index = keyboard_buffer.read_index + keyboard_buffer.size;
    if(write_index >= KEYBOARD_BUFFER_SIZE)
        write_index -= KEYBOARD_BUFFER_SIZE;
    
    // store in buffer
    keyboard_buffer.buffer[write_index] = code;    
    keyboard_buffer.size++;
}

// get a keycode from the FIFO buffer
uint8_t keyboard_get_buffer(void)
{    
    uint8_t code = KEYBOARD_BUFFER_EMPTY;
    
    if(keyboard_buffer.size)
    {
        code = keyboard_buffer.buffer[keyboard_buffer.read_index++];
        if(keyboard_buffer.read_index >= KEYBOARD_BUFFER_SIZE)
            keyboard_buffer.read_index = 0;
        keyboard_buffer.size--;
    }

    return code;
}
