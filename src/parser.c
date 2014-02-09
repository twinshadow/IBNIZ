#include <ctype.h>
#include <stdlib.h>
#include "ibniz.h"
#include "language.h"

int
ibniz_isxdigit(char c) {
	return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'));
}

int
ibniz_c2d(char c) {
	return (c >= 'A' ? c - 'A' + 10 : c - '0');
}

/*** Process data blocks ***/
void
parse_data(char *src) {
	uint8_t digitsz;
	int32_t value, wrap, pad, pad_inc;

	vm.datalgt = 0;
	digitsz = 4;
	vm.parsed_data[0] = 0;
	for (; *src != '\0'; src++) {
		if (*src == COMMENT_START) {
			for(; *src && (*src != '\n'); src++);
			continue;
		}
		switch (*src) {
		case DATA_BINARY:
			digitsz = 1;
			break;
		case DATA_QUARTERNARY:
			digitsz = 2;
			break;
		case DATA_OCTAL:
			digitsz = 3;
			break;
		case DATA_HEXIDECIMAL:
			digitsz = 4;
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			value = ibniz_c2d(*src);
			value &= ((1 << digitsz) - 1);
			wrap = (32 - digitsz - (vm.datalgt & 31));

			if (wrap >= 0) {
				vm.parsed_data[vm.datalgt >> 5] |= value << wrap;
				vm.parsed_data[(vm.datalgt >> 5) + 1] = 0;
			} else {
				vm.parsed_data[vm.datalgt >> 5] |= value >> (0 - wrap);
				vm.parsed_data[(vm.datalgt >> 5) + 1] = value << (32 + wrap);
			}
			vm.datalgt += digitsz;
			break;
		}
	}

	pad = vm.datalgt & 31;
	if (!pad) {
		vm.parsed_data[(vm.datalgt >> 5) + 1] = vm.parsed_data[0];
	} else {
		pad_inc = pad;
		while (pad_inc < 32) {
			vm.parsed_data[vm.datalgt >> 5] |= vm.parsed_data[0] >> pad_inc;
			pad_inc *= 2;
		}
		vm.parsed_data[(vm.datalgt >> 5) + 1] =
		    (vm.parsed_data[0] << (32 - pad)) |
		    (vm.parsed_data[1] >> pad);
	}
}

/* precalculate skip points */
/*
  this goes over all the of the code and pushes to the VM where there are
  inner-expressions.
*/
void
parse_exp(char *vm_c) {
	char seek_end, seek_switch;
	char src_c;
	uint32_t iter, skip;
	int endloop = 0;

	vm.codelgt = vm_c - vm.parsed_code;

	for (iter=0;; iter++) {
		src_c = vm.parsed_code[iter];
		seek_end = seek_switch = 0;
		skip = iter + 1;

		switch (src_c) {
		case '\0':
			seek_end = MEDIASWITCH;
			skip = 0;
			endloop = 1;
			break;
		case MEDIASWITCH:
			seek_end = MEDIASWITCH;
			break;
		case COND_START:
			seek_end = COND_END;
			seek_switch = COND_SWITCH;
			break;
		case COND_SWITCH:
			seek_end = COND_END;
			break;
		case SUBR_START:
			seek_end = SUBR_END;
			break;
		}

		if (seek_end) {
			for (;; skip++) {
				src_c = vm.parsed_code[skip];
				if (src_c == '\0' || src_c == seek_end || src_c == seek_switch) {
					if (skip == iter || src_c == 0) {
						vm.parsed_hints[iter] = 0;
					} else {
						vm.parsed_hints[iter] = skip + 1;
					}
					break;
				}
			}
		}

		if (endloop == 1)
			break;
	}
}

/* parse immediates, skip comments & whitespaces */
void
parse_immediates(char *src) {
	char *vm_code = vm.parsed_code;
	uint32_t *vm_hint = vm.parsed_hints;
	uint32_t number_value;
	int number_shift;
	int number_mode = 0; /* 0=null, 1=int, 2=dec */

	for (; *src != '\0'; src++) {
		if (!(*src) || !isascii(*src)) {
			continue;
		}

		if (*src == IMM_DEC || ibniz_isxdigit(*src)) {
			if (number_mode == 0) {
				number_value = 0;
				number_shift = 16;
				number_mode = 1;
			}
			if (*src == IMM_DEC) {
				if (number_mode == 2) {
					*vm_code++ = OP_LOADIMM;
					*vm_hint++ = number_value;
					number_value = 0;
				}
				number_mode = 2;
				number_shift = 12;
			} else {
				if (number_mode == 1)
					number_value = ROL(number_value, 4);
				number_value |= ibniz_c2d(*src) << number_shift;
				if (number_mode == 2)
					number_shift = (number_shift - 4) & 31;
			}
		}
		else {
			if (number_mode != 0) {
				*vm_code++ = OP_LOADIMM;
				*vm_hint++ = number_value;
				number_mode = 0;
			}
			if (*src == COMMENT_START) {
				for(; *src && (*src != '\n'); src++);
				continue;
			}
			if (*src != IMM_SEP) {
				*vm_code++ = *src;
				*vm_hint++ = 0;
				if (*src == DATA_START) {
					*vm_code++ = '\0';
					*vm_hint++ = 0;
					src++;
					parse_data(src);
					break;
				}
			}
		}
	}
	parse_exp(vm_code);
}

void
compiler_parser(char *src) {
	parse_immediates(src);
}
