#include <ctype.h>
#include <stdlib.h>
#include "ibniz.h"
#include "language.h"
#include <stdio.h>

#define ISXDIGIT(__char) ((__char >= '0' && __char <= '9') || (__char >= 'A' && __char <= 'F'))
#define CHAR2DIGIT(__char) (__char >= 'A' ? __char - 'A' + 10 : __char - '0')

/*** Process data blocks ***/
void
parse_data(char *src) {
	uint8_t digitsz;
	int32_t value, wrap, pad, pad_inc;

	vm.data_len = 0;
	digitsz = 4;
	vm.parsed_data[0] = 0;
	for (; *src != '\0'; src++) {
		if (*src == COMMENT_START) {
			for(; *src && (*src != '\n'); src++);
			continue;
		}
		if (*src == DATA_BINARY)
			digitsz = 1;
		else if (*src == DATA_QUARTERNARY)
			digitsz = 2;
		else if (*src == DATA_OCTAL)
			digitsz = 3;
		else if (*src == DATA_HEXIDECIMAL)
			digitsz = 4;
		else if (ISXDIGIT(*src)) {
			value = CHAR2DIGIT(*src);
			value &= ((1 << digitsz) - 1);
			wrap = (32 - digitsz - (vm.data_len & 31));

			if (wrap >= 0) {
				vm.parsed_data[vm.data_len >> 5] |= value << wrap;
				vm.parsed_data[(vm.data_len >> 5) + 1] = 0;
			} else {
				vm.parsed_data[vm.data_len >> 5] |= value >> (0 - wrap);
				vm.parsed_data[(vm.data_len >> 5) + 1] = value << (32 + wrap);
			}
			vm.data_len += digitsz;
		}
	}

	pad = vm.data_len & 31;
	if (!pad) {
		vm.parsed_data[(vm.data_len >> 5) + 1] = vm.parsed_data[0];
	} else {
		pad_inc = pad;
		while (pad_inc < 32) {
			vm.parsed_data[vm.data_len >> 5] |= vm.parsed_data[0] >> pad_inc;
			pad_inc *= 2;
		}
		vm.parsed_data[(vm.data_len >> 5) + 1] =
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
parse_skip() {
	char seek_end, seek_switch;
	char src_c;
	uint32_t iter, skip;

	for (iter=0; iter < vm.code_len; iter++) {
		src_c = vm.parsed_code[iter];
		seek_switch = 0;

		if (src_c == MEDIASWITCH)
			seek_end = MEDIASWITCH;
		else if (src_c == COND_START) {
			seek_end = COND_END;
			seek_switch = COND_SWITCH;
		}
		else if (src_c == COND_SWITCH)
			seek_end = COND_END;
		else if (src_c == SUBR_START)
			seek_end = SUBR_END;
		else
			continue;

		for (skip = iter + 1; skip < vm.code_len; skip++) {
			src_c = vm.parsed_code[skip];
			if (src_c == '\0' || src_c == seek_end || (seek_switch && src_c == seek_switch)) {
				vm.parsed_hints[iter] = skip + 1;
				break;
			}
		}
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

	for (; *src && *src != '\0'; src++) {
		if (!(*src) || !isascii(*src)) {
			continue;
		}

		if (*src == IMM_DEC || ISXDIGIT(*src)) {
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
				number_value |= CHAR2DIGIT(*src) << number_shift;
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

			if (*src == IMM_SEP || *src == ' ' || *src == '\n') {
				continue;
			}

			if (*src == DATA_START) {
				*vm_code++ = '\0';
				*vm_hint++ = 0;
				src++;
				parse_data(src);
				break;
			}

			*vm_code++ = *src;
			*vm_hint++ = 0;
		}
	}

	if (vm_code != '\0') {
		*vm_code++ = '\0';
		*vm_hint++ = 0;
	}

	vm.code_len = vm_code - vm.parsed_code;
}

void
compiler_parser(char *src) {
	parse_immediates(src);
	parse_skip();
	puts(vm.parsed_code);
}
