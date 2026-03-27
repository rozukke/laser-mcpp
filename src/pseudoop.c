/*
*   Laser- a command line utility to assemble LC3 assembly code
*
*   Copyright Notice:
*
*       Copyright 2018, 2019 Zhiyuan Fan
*
*   License Notice:
*
*       Laser is free software: you can redistribute it and/or modify
*       it under the terms of the GNU General Public License as published by
*       the Free Software Foundation, either version 3 of the License, or
*       (at your option) any later version.
*
*       Laser is distributed in the hope that it will be useful,
*       but WITHOUT ANY WARRANTY; without even the implied warranty of
*       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*       GNU General Public License for more details.
*
*       You should have received a copy of the GNU General Public License
*       along with Laser.  If not, see <https://www.gnu.org/licenses/>.
*/

#define USES_PSEUDOOP
#define USES_OFFSET
#include "laser.h"

int8_t arrcmp (char *str, const char *arr[], uint8_t size);

int8_t ispseudoop (Token *token)
{
    const char *pops[] = {
        ".ALIAS",	// 0
        ".MACRO",	// 1
        ".ORIG",	// 2
        ".END",		// 3
        ".STRINGZ",	// 4
        ".BLKW",	// 5
        ".FILL",	// 6
        ".EXPORT",	// 7
        ".IMPORT"	// 8
    };
    return arrcmp (token->str, pops, 9);
}

uint16_t addrnum (uint8_t popcode, Token *token)
{
    if (popcode == STRINGZ && token != NULL) {
        return token->len + 1;
    } else if (popcode == BLKW && token != NULL) {
        uint8_t offt = offtype (token);
        int16_t off = offset (offt, token);
        if (offt > 0 && off > 0) return off;
        else return 0;
    } else if (popcode == FILL) {
        return 1;
    } else {
        return 0;
    }
}
