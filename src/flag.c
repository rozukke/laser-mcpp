#define USES_FLAG
#define USES_FILE
#define USES_NOTIFY
#include "laser.h"

const char *about = "Copyright Notice:\n\n"

					"\tCopyright 2018, 2019 Zhiyuan Fan\n\n"

					"License Notice:\n\n"

					"\tThis program is free software: you can redistribute it and/or modify\n"
					"\tit under the terms of the GNU General Public License as published by\n"
					"\tthe Free Software Foundation, either version 3 of the License, or\n"
					"\t(at your option) any later version.\n\n"

					"\tThis program is distributed in the hope that it will be useful,\n"
					"\tbut WITHOUT ANY WARRANTY; without even the implied warranty of\n"
					"\tMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
					"\tGNU General Public License for more details.\n\n"

					"\tYou should have received a copy of the GNU General Public License\n"
					"\talong with this program.  If not, see <https://www.gnu.org/licenses/>.\n";

const char *version_num = "2.0.0";

const char *usage = "laser: a command line utility to assemble LC3 assembly code\n\n"
					"Usage:\n"
					"\t'laser -v' to display version number\n"
					"\t'laser -h' to display a more detailed help message\n"
					"\t'laser -a <file>' to assemble a file\n"
					"\t'laser -c <file>' to remove all files generated by 'laser -a <file>'\n";

const char *help = "Description:\n"
					"\tAn assembler for the LC3 assembly language written entirely in C by Zhiyuan Fan.\n"
					"Usage:\n"
					"\t-v: 'laser -v' displays version number\n"
					"\t-h: 'laser -h' displays this message again\n"
					"\t-q: 'laser -q' suppresses all warning messages during assembly\n"
					"\t-s: 'laser -s' suppresses all warnings and errors during assembly\n"
					"\t-a: 'laser -a <file>' assembles the specified file\n"
					"\t    'laser -a *.asm' assembles all assembly files in current directory\n"
					"\t-c: 'laser -c <file>' removes all files generated by 'laser -a <file'\n"
					"\t    'laser -c *.asm' removes all files generated by 'laser -a *.asm'\n"
					"Configuration:\n"
					"\tConfiguration options can be found in the 'config.h' header file.\n"
					"\tSee 'https://github.com/PaperFanz/laser' for more details.\n";

const char *flags[] = { "-v", "-h", "-q", "-s", "-a", "-c", "-l", "-p"};

static int8_t LOGENABLED = 0;
static int8_t HIPERFMODE = 0;

int8_t checkflags (char *f)
{
	for (int8_t i = 0; i < 8; i++) {
		if (strcmp (f, flags[i]) == 0)
			return i;
	}

	return -1;
}

int8_t islogging (void)
{
	return LOGENABLED;
}

int8_t ishiperf (void)
{
	return HIPERFMODE;
}

int8_t parseflag (int8_t flag)
{
	switch (flag) {
		case VERSION: {
			printf ("laser version %s\n", version_num);
			break;
		}		
		case HELP: {
			printf ("%s", help);
			break;
		}
		case QUIET: {
			setVerbosity (noWarn);
			break;
		}
		case SILENT: {
			setVerbosity (noErrs);
			break;
		}
		case ASSEMBLE: {
			break;
		}
		case CLEAN: {
			break;
		}
		case LOG: {
			LOGENABLED = 1;
			break;
		}
		case HIPERF: {
			HIPERFMODE = 1;
			break;
		}
		default: {
			return -1;
		}
	}

	return flag;
}