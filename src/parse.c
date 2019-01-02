#include "parse.h"

void parse_file (FILE *fp, char *fname) {

	//==========================================================================
	//	Declarations
	//==========================================================================

	int org_addr = 0, addr = 0, ln = 0, ln_st = 0;
	int bin[16];
	char hex[4];
	char addr_str[5];					// 4 hex chars and null
	int MAX_LEN = MAX_WORD_NUM * MAX_WORD_SIZE;
	char line_buf[MAX_LEN + 1];			// see definitions in laser.h
	char word_buf[MAX_WORD_NUM][MAX_WORD_SIZE + 2];	// room for null + size
	int s_cnt = 0;
	char **symbol = malloc(sizeof(char*));
	int *dec_addr = malloc(sizeof(int));
	char fname_buf[MAX_WORD_SIZE + 5];
	bool orig = false, op = false;
	FILE *fp_sym, *fp_lst, *fp_bin, *fp_hex, *fp_obj;
	fpos_t pos;

	//==========================================================================
	//	Create files and write headers (where applicable)
	//==========================================================================
	for (int i = 0; i <= MAX_WORD_SIZE; i++) {
		if (fname[i] == '.') {
			fname_buf[i] = '.';			// .
			fname_buf[i + 1] = 's';		// s
			fname_buf[i + 2] = 'y';		// y
			fname_buf[i + 3] = 'm';		// m
			fname_buf[i + 4] = '\0';
			fp_sym = fopen (fname_buf, "w");	// create or clear filename.sym
			if (fp_sym == NULL)
				printf ("Error: Unable to open %s!\n", fname_buf);
			fname_buf[i + 1] = 'l';		// l
			fname_buf[i + 2] = 's';		// s
			fname_buf[i + 3] = 't';		// t
			fp_lst = fopen (fname_buf, "w");	// create or clear filename.lst
			if (fp_lst == NULL)
				printf ("Error: Unable to open %s!\n", fname_buf);
			fname_buf[i + 1] = 'b';		// b
			fname_buf[i + 2] = 'i';		// i
			fname_buf[i + 3] = 'n';		// n
			fp_bin = fopen (fname_buf, "w");	// create or clear filename.bin
			if (fp_bin == NULL)
				printf ("Error: Unable to open %s!\n", fname_buf);
			fname_buf[i + 1] = 'h';		// h
			fname_buf[i + 2] = 'e';		// e
			fname_buf[i + 3] = 'x';		// x
			fp_hex = fopen (fname_buf, "w");	// create or clear filename.hex
			if (fp_hex == NULL)
				printf ("Error: Unable to open %s!\n", fname_buf);
			fname_buf[i + 1] = 'o';		// o
			fname_buf[i + 2] = 'b';		// b
			fname_buf[i + 3] = 'j';		// j
			fp_obj = fopen (fname_buf, "w");	// create or clear filename.obj
			if (fp_obj == NULL)
				printf ("Error: Unable to open %s!\n", fname_buf);
			break;
		} else {
			fname_buf[i] = fname[i];
		}
	}

	// symbol file header
	fprintf (fp_sym, "Symbol Name\t\t\t Page Address\n---------------------------------\n");
	
	//==========================================================================
	//	Pass 1 - Generate Symbol file
	//==========================================================================

	printf ("Pass 1: \n");

skip_symbol_parse:
	while(fgets (line_buf, MAX_LEN + 1, fp) != NULL) {
		ln++;
		bool comment = (line_buf[0] == ';');
		bool empty = (line_buf[0] == '\0');

		if (comment || empty)
			goto skip_symbol_parse;

		memset (word_buf, 0, sizeof(word_buf));

		int i = 0, j = 0, k = 0;		// counter inits
		bool prev = false;

		while (i <= MAX_LEN && !(line_buf[i] == '\0') && !(line_buf[i] == ';')) {
			bool space = isspace(line_buf[i]);
			bool comma = line_buf[i] == 0x2C;
			if (!space && !comma) {
				word_buf[j][k] = line_buf[i];
				k++;
				prev = true;
			} else if ((space||comma)&&prev) {		// commas also denote EOW
				word_buf[j][k] = 0x00;
				word_buf[j][MAX_WORD_SIZE + 1] = k;
				j++;
				k = 0;
				prev = false;
			}
			i++;
		}

		//look for .ORIG and subsequent starting address
		if (!orig && (isPseuodoOp (word_buf[0]) == 0)) {
			org_addr = addr = addrToDec (word_buf[1]) - 1;
			ln_st = ln;
			decToTwoComp (addr + 1, bin, 16);
			binToHex (bin, 16, hex, 4);
			fprintIntArr (fp_bin, bin, 16);
			fprintCharArr (fp_hex, hex, 4);
			fgetpos (fp, &pos);
			orig = true;
		}
		if (isPseuodoOp (word_buf[0]) == 1) {
			break;
		}
		
		if (orig && isLabel(word_buf[0])) {
			symbol = realloc (symbol, (s_cnt + 1) * sizeof(char*));
			dec_addr = realloc (dec_addr, (s_cnt + 1) * sizeof(int));
			symbol[s_cnt] = malloc (sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (symbol[s_cnt], word_buf[0], sizeof(char) * (MAX_WORD_SIZE + 2));
			dec_addr[s_cnt] = addr;
			s_cnt++;

			decToAddr (addr_str, addr);
			putSymbol (fp_sym, word_buf[0], addr_str);
			if (word_buf[1][0] != 0x00)
				addr++;
		} else if (orig){
			addr++;
		}
	}

	//==========================================================================
	//	Pass 2 - Generate List, Binary, Hex, and Object files
	//==========================================================================

	printf ("Pass 2:\n");

	fsetpos (fp, &pos);		// sets starting pos to origin pos, save some reads
	ln = ln_st;
	addr = org_addr;

skip_line_parse:
	while(fgets(line_buf, MAX_LEN+1, fp)!=NULL){
		ln++;
		bool comment = (line_buf[0] == ';');
		bool empty = (line_buf[0] == '\0');

		if (comment || empty)
			goto skip_line_parse;

		memset (word_buf, 0, sizeof(word_buf));

		int i = 0, j = 0, k = 0;		// counter inits
		bool prev = false;

		while (i <= MAX_LEN && !(line_buf[i] == '\0') && !(line_buf[i] == ';')) {
			bool space = isspace(line_buf[i]);
			bool comma = line_buf[i] == 0x2C;
			if (!space && !comma) {
				word_buf[j][k] = line_buf[i];
				k++;
				prev = true;
			} else if ((space||comma)&&prev) {		// commas also denote EOW
				word_buf[j][k] = 0x00;
				word_buf[j][MAX_WORD_SIZE + 1] = k;
				j++;
				k = 0;
				prev = false;
			}
			i++;
		}

		if(!(isLabel(word_buf[0]) && (word_buf[1][0] == 0x00)))
			addr++;

		//==================================================================
		//	Generate Binary and Hex Files
		//==================================================================

		memset (bin, 0, sizeof(int) * 16);		// clear bin array
		i = j = 0;								// reset counters
		op = false;							// reset opcode indicator
		int offset_bits = 0;
		int max_operands = 0;
		bool match = false;
		char op1[MAX_WORD_SIZE + 2];
		char op2[MAX_WORD_SIZE + 2];
		char op3[MAX_WORD_SIZE + 2];

		while(isLabel (word_buf[i]))
			i++;

		switch (isKeyword (word_buf[i])) {
		case 0:			// BR, check condition codes
			op = true;
			break;
		case 1:			// ADD
			op = true;
			bin[3] = 1;
			offset_bits = 5;
			max_operands = 3;

			memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op2, word_buf[i + 2], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op3, word_buf[i + 3], sizeof(char) * (MAX_WORD_SIZE + 2));

			OPERAND_REGISTER (op1, 0);
			OPERAND_REGISTER (op2, 1);
			OPERAND_REGISTER_IMMEDIATE (op3, 2);
			break;
		case 2:			// LD
			op = true;
			bin[2] = 1;
			offset_bits = 9;
			max_operands = 2;

			memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op2, word_buf[i + 2], sizeof(char) * (MAX_WORD_SIZE + 2));

			OPERAND_REGISTER (op1, 0);
			OPERAND_OFFSET (op2);
			break;
		case 3:			// ST
			op = true;
			bin[3] = bin[2] = 1;
			offset_bits = 9;
			max_operands = 2;

			memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op2, word_buf[i + 2], sizeof(char) * (MAX_WORD_SIZE + 2));

			OPERAND_REGISTER (op1, 0);
			OPERAND_OFFSET (op2);
			break;
		case 4:			// JSR and JSRR, check addr mode
			op = true;
			bin[1] = 1;
			if (strcmp (word_buf[i], "JSR") == 0 || strcmp (word_buf[i], "jsr") == 0) {
				bin[4] = 1;
				offset_bits = 11;
				max_operands = 1;

				memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));

				OPERAND_OFFSET (op1);
			} else {
				offset_bits = 6;
				max_operands = 2;

				memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
				memcpy (op2, word_buf[i + 2], sizeof(char) * (MAX_WORD_SIZE + 2));

				OPERAND_REGISTER (op1, 1);
				OPERAND_OFFSET (op2);
			}
			break;
		case 5:			// AND
			op = true;
			bin[1] = bin[3] = 1;
			offset_bits = 5;
			max_operands = 3;

			memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op2, word_buf[i + 2], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op3, word_buf[i + 3], sizeof(char) * (MAX_WORD_SIZE + 2));

			OPERAND_REGISTER (op1, 0);
			OPERAND_REGISTER (op2, 1);
			OPERAND_REGISTER_IMMEDIATE (op3, 2);
			break;
		case 6:			// LDR
			op = true;
			bin[1] = bin[2] = 1;
			offset_bits = 6;
			max_operands = 3;
			
			memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op2, word_buf[i + 2], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op3, word_buf[i + 3], sizeof(char) * (MAX_WORD_SIZE + 2));

			OPERAND_REGISTER (op1, 0);
			OPERAND_REGISTER (op2, 1);
			OPERAND_OFFSET (op3);
			break;
		case 7:			// STR
			op = true;
			bin[1] = bin[2] = bin[3] = 1;
			offset_bits = 6;
			max_operands = 3;
			
			memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op2, word_buf[i + 2], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op3, word_buf[i + 3], sizeof(char) * (MAX_WORD_SIZE + 2));

			OPERAND_REGISTER (op1, 0);
			OPERAND_REGISTER (op2, 1);
			OPERAND_OFFSET (op3);
			break;
		case 8:			// RTI
			op = true;
			bin[0] = 1;
			break;
		case 9:			// NOT
			op = true;
			bin[0] = bin[3] = bin[10] = bin[11] = bin[12] = bin[13] = bin[14] = bin[15] = 1;
			max_operands = 2;

			memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op2, word_buf[i + 2], sizeof(char) * (MAX_WORD_SIZE + 2));

			OPERAND_REGISTER (op1, 0);
			OPERAND_REGISTER (op2, 1);
			break;
		case 10:		// LDI
			op = true;
			bin[0] = bin[2] = 1;
			offset_bits = 9;
			max_operands = 2;

			memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op2, word_buf[i + 2], sizeof(char) * (MAX_WORD_SIZE + 2));

			OPERAND_REGISTER (op1, 0);
			OPERAND_OFFSET (op2);
			break;
		case 11:		// STI
			op=true;
			bin[0]=bin[2]=bin[3]=1;
			offset_bits=9;
			max_operands = 2;

			memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op2, word_buf[i + 2], sizeof(char) * (MAX_WORD_SIZE + 2));

			OPERAND_REGISTER (op1, 0);
			OPERAND_OFFSET (op2);
			break;
		case 12:		// JMP and RET, check shortcut
			op=true;
			bin[0]=bin[1]=1;

			if(strcmp(word_buf[i], "RET")==0||strcmp(word_buf[i], "ret")==0){
				max_operands = 0;
				bin[7]=bin[8]=bin[9]=1;
			}
			else{
				max_operands = 1;
				memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
				OPERAND_REGISTER (op1, 1);
			}
			break;
		case 14:		// LEA
			op=true;
			bin[0]=bin[1]=bin[2]=1;
			offset_bits=9;
			max_operands = 2;

			memcpy (op1, word_buf[i + 1], sizeof(char) * (MAX_WORD_SIZE + 2));
			memcpy (op2, word_buf[i + 2], sizeof(char) * (MAX_WORD_SIZE + 2));

			OPERAND_REGISTER (op1, 0);
			OPERAND_OFFSET (op2);
			break;
		case 15:		// TRAP, check shortcuts
			op=true;
			bin[0]=bin[1]=bin[2]=bin[3]=1;
			break;
		default:		// not keyword
			switch(isPseuodoOp(word_buf[i])){
				case 0:			// ORIG
					break;
				case 1:			// END
					break;
				case 2:			// STRINGZ
					break;
				case 3:			// BLKW
					break;
				case 4:			// FILL
					break;
				default:
					break;
			}
			break;
		}

		if(op){
			fprintf(fp_bin, "%d\t%s\t", ln, decToAddr(hex, addr));
			fprintIntArr(fp_bin, bin, 16);	// print only if opcode detected
			fprintf(fp_hex, "%d\t", ln);
			binToHex(bin, 16, hex, 4);
			fprintCharArr(fp_hex, hex, 4);
		}
	}

	//==========================================================================
	//	Cleanup
	//==========================================================================

	//free memory
	for(int i=0; i<s_cnt; i++) free(symbol[i]);
	free(symbol);
	free(dec_addr);
}