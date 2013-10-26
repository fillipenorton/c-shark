#include <stdio.h>
#include <stdlib.h>

#define RESERVADAS 19

typedef struct token{
	char token[30];
	char lexema[30];
	int linha, coluna;
	struct token *prox;
}token;

FILE *fonte; // codigo-fonte .csk

token *listaTokens; // lista de tokens do codigo-fonte

char tokens[50][20] = {"reservada_class", "reservada_for","reservada_while", "reservada_if", "reservada_else"
	"reservada_switch", "reservada_case", "reservada_void", "reservada_char", "reservada_int", "reservada_float",
	"reservada_bool", "reservada_const", "reservada_true", "reservada_false", "reservada_null", "reservada_return",
	"reservada_break", "reservada_struct", "ID", "ADD", "SUB", "MUL", "DIV", "MOD", "EQ", "MAIOR_IGUAL", "MENOR_IGUAL",
	"MAIOR", "MENOR", "DIF", "NOT", "AND", "OR", "ATRIB", "ASP_DUPLAS", "ABRE_PAREN", "FECHA_PAREN", "ABRE_COLCH",
	"FECHA_COLCH", "ABRE_CHAV", "FECHA_CHAV", "FIM", "SEPARADOR", "COMENT_CURTO", "ABRE_COMENT_LONGO", "FECHA_COMENT_LONGO",
	"NUM_FLOAT", "NUM_INT"};

char reservadas[RESERVADAS] = {"class", "for", "while", "if", "else", "switch", "case", "void", "char", "int",
		"float", "bool", "const", "true", "false", "null", "return", "break", "struct"};

int main(){


	return 0;
}
