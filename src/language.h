#ifndef LANGUAGE_H
#define LANGUAGE_H

#define BITOP_AND '&'
#define BITOP_OR '|'
#define BITOP_XOR '^'
#define BITOP_RIGHT 'r'
#define BITOP_LEFT 'l'
#define BITOP_NEG '~'

#define BOOL_ZERO '='
#define BOOL_POS '>'
#define BOOL_NEG '<'

#define COND_START '?'
#define COND_SWITCH ':'
#define COND_END ';'

#define DATA_START '$'
#define DATA_GET 'G'
#define DATA_BINARY 'b'
#define DATA_QUARTERNARY 'q'
#define DATA_OCTAL 'o'
#define DATA_HEXIDECIMAL 'h'

#define FUNC_SIN 's'
#define FUNC_ATAN 'a'

#define LOOP_TIMES 'X'
#define LOOP_LOOP 'L'
#define LOOP_INDEX 'i'
#define LOOP_OUTDEX 'j'
#define LOOP_DO '['
#define LOOP_WHILE ']'
#define LOOP_JUMP 'J'

#define MEM_LOAD '@'
#define MEM_STORE '!'

#define OP_ADD '+'
#define OP_SUB '-'
#define OP_DIV '/'
#define OP_MOD '%'
#define OP_MUL '*'
#define OP_SQRT 'q'

#define STACK_DUP 'd'
#define STACK_POP 'p'
#define STACK_EXCHANGE 'x'
#define STACK_TRIROT 'v'
#define STACK_PICK ')'
#define STACK_BURY '('
#define STACK_Z 'z'

#define SUBR_START '{'
#define SUBR_END '}'
#define SUBR_VISIT 'V'

#define IMM_DEC '.'
#define IMM_SEP ','
#define COMMENT_START '\\'
#define USERINPUT 'U'
#define MEDIASWITCH 'M'
#define TERMINATE 'T'
#define WHEREAMI 'w'

#define RST_POP 'R'
#define RST_PUSH 'P'

#endif /* LANGUAGE_H */
