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

#ifndef MATRIX_H
#define	MATRIX_H

#include <stdint.h>
#include <stdbool.h>

// total number of rows in matrix scan result
// last row is a "dummy" row that hold the special keys
#define MATRIX_N_ROWS   16

// total number of columns in scan result
#define MATRIX_N_COLS   7

void matrix_scan(void);
void matrix_decode(void);

#endif	/* MATRIX_H */
