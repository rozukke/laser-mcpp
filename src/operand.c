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

#define USES_OPERAND
#include "laser.h"

int8_t arrcmp (char *str, const char *arr[], uint8_t size)
{
    for (uint8_t i = 0; i < size; i++) {
        if (strcasecmp (str, arr[i]) == 0) return i;
    }
    return -1;
}

int8_t brrcmp (char *str, const char *arr[], uint8_t size)
{
    for (uint8_t i = 0; i < size; i++) {
        if (strcasecmp (str, arr[i]) == 0) return i;
    }
    return -1;
}

int8_t isregister (Token *token)
{
    const char *regs[] = {
        "R0",
        "R1",
        "R2",
        "R3",
        "R4",
        "R5",
        "R6",
        "R7",
    };

    return arrcmp (token->str, regs, 8);
}

int8_t isbranch (Token *token)
{
    const char *brs[] = {
        "BR",
        "BRP",
        "BRZ",
        "BRZP",
        "BRN",
        "BRNP",
        "BRNZ",
        "BRNZP",
    };

    int8_t tmp = brrcmp (token->str, brs, 8);
    tmp = (tmp == 0) ? 7 : tmp;
    return tmp;
}

int8_t istrap (Token *token)
{
    const char *traps[] = {
        "GETC",
        "OUT",
        "PUTS",
        "IN",
        "PUTSP",
        "HALT",
        "TRAP",
        "REG",
        "CHAT",
        "GETP",
        "SETP",
        "GETB",
        "SETB",
        "GETH",
    };

    int8_t tmp = arrcmp (token->str, traps, 14);
    if (tmp == -1) return -1;
    else if (tmp == 6) return 0;
    else return 32 + tmp;
}

int8_t isoperand (Token *token)
{
    const char *ops[] = {
        "BR",
        "ADD",
        "LD",
        "ST",
        "JSR",
        "AND",
        "LDR",
        "STR",
        "RTI",
        "NOT",
        "LDI",
        "STI",
        "JMP",
        "",
        "LEA",
        "TRAP",
    };

    int8_t op = arrcmp (token->str, ops, 16);
    if (op < 0) {																// could be branch, trap, or assembler shortcut
        if (isbranch (token) >= 0) {
            op = BR;
        } else if (istrap (token) >= 0) {
            op = TRAPS;
        } else if (strcasecmp (token->str, "JSRR") == 0 ||
                   strcasecmp (token->str, "jsrr") == 0) {
            op = JSRR;
        } else if (strcasecmp (token->str, "RET") == 0 ||
                   strcasecmp (token->str, "ret") == 0) {
            op = RET;
        }
    } else if (op == 13) {														// invalid op
        op = -1;
    }
    return op;
}
