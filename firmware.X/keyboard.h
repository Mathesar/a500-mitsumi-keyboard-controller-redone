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

#ifndef KEYBOARD_H
#define	KEYBOARD_H

// special keyboard codes
#define POWERUP_KEY_STREAM      0xFD
#define TERMINATE_KEY_STREAM    0xFE
#define LOST_SYNC               0xF9



bool keyboard_send(uint8_t code);
bool keyboard_synchronize(void);

#endif	/* KEYBOARD_H */

