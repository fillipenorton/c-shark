#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RESERVADAS 19 // quantidade de palavras reservadas
#define MAX_IDENT 30 // tamanho máximo do identificador

typedef struct token{
	char token[20];
	char lexema[MAX_IDENT];
	int linha, coluna;
	struct token *prox;
}estruturaToken;

FILE *fonte; // codigo-fonte .csk

estruturaToken *listaTokens; // lista de tokens do codigo-fonte

char novoLexema[MAX_IDENT]; // armazena cada lexema lido do fonte
char* novoToken; // armazena o token correspondente ao lexema lido
char caractere; // armazena cada caractere lido do fonte
int linha = 1; // linha atual
int coluna = 1; // coluna atual
int colunaLexema = 1; // coluna do ultimo lexema lido
int boolNovoLexema = 0; // 1 se foi lido um novo lexema

char tokens[50][20] = {"reservada_class", "reservada_for","reservada_while", "reservada_if", "reservada_else",
	"reservada_switch", "reservada_case", "reservada_void", "reservada_char", "reservada_int", "reservada_float",
	"reservada_bool", "reservada_const", "reservada_true", "reservada_false", "reservada_null", "reservada_return",
	"reservada_break", "reservada_struct", "IDENT", "ADD", "SUB", "MUL", "DIV", "MOD", "EQ", "MAIOR_IGUAL", "MENOR_IGUAL",
	"MAIOR", "MENOR", "DIF", "NOT", "AND", "OR", "ATRIB", "ASP_DUPLAS", "ABRE_PAREN", "FECHA_PAREN", "ABRE_COLCH",
	"FECHA_COLCH", "ABRE_CHAV", "FECHA_CHAV", "FIM", "SEPARADOR", "COMENT_CURTO", "ABRE_COMENT_LONGO", "FECHA_COMENT_LONGO",
	"NUM_FLOAT", "NUM_INT"};

char reservadas[RESERVADAS][20] = {"class", "for", "while", "if", "else", "switch", "case", "void", "char", "int",
		"float", "bool", "const", "true", "false", "null", "return", "break", "struct"};

void inicializa();
void analisador();
void identificador();
char *verificaTokenIdentificador(char ident[MAX_IDENT]);
void numero();
void addListaTokens();

int main(int argc, char *argv[]){
	char *codigoFonte = NULL;

	strcpy(codigoFonte, argv[1]);

	inicializa(codigoFonte);

	analisador();

	return 0;
}

/*
 * Inicializa o ponteiro para arquivo do código-fonte
 * e a lista de tokens
 */
void inicializa(char *arq){
	fonte = fopen (arq, "r");

	if (fonte == NULL) {
		printf ("Erro ao abrir arquivo.\n");
		exit(1);
	}

	listaTokens = NULL;
}

void analisador(){

	while(!feof(fonte)){ // enquanto não chegar ao fim do codigo-fonte
		boolNovoLexema = 0;
		novoLexema[0] = '0'; // inicializa a cada loop para poder formar novos lexemas
		caractere = fgetc(fonte);

		if((caractere >= 'a' && caractere <= 'z') || (caractere >= 'A' && caractere <= 'Z')) {
			identificador();
			novoToken = verificaTokenIdentificador(novoLexema);
			boolNovoLexema = 1;
		}

		if(caractere == '\n'){
			linha++;
		}

		if(boolNovoLexema) // se formou novo lexema, adiciona na lista
			addListaTokens();
	}
}

/*
 * Função para o Estado identificador do autômato
 * Armazena o identificador lido na variável global novoLexema
 */
void identificador(){
	char auxCaractere[2];
	novoLexema[0] = '\0';

	while((caractere >= 'a' && caractere <= 'z') || // loop para identificar o lexema
			(caractere >= 'A' && caractere <= 'Z') ||
			(caractere >= '0' && caractere <= '9')){

		auxCaractere[0] = caractere;
		auxCaractere[1] = '\0';
		strcat(novoLexema, auxCaractere);
		caractere = fgetc(fonte);
		coluna++;
		if(caractere == '\n'){
			linha++;
		}
	}
}

/*
 *  Verifica se o identificador é uma palavra reservada
 *
 *  @param ident Um identificador
 *  @return token correspondente ao identificador
*/
char *verificaTokenIdentificador(char ident[MAX_IDENT]){
	int i;

	for(i=0; i<RESERVADAS; i++){
		if(strcmp(reservadas[i],ident)==0){ // se for palavra reservada retorna o token correspondente
			return tokens[i];
		}
	}

	return "IDENT";
}

void numero(){

}

/*
 * Adiciona um token na lista de tokens do código-fonte
*/
void addListaTokens(){
	estruturaToken *aux, *i;

	aux = (estruturaToken *) malloc(sizeof(estruturaToken));

	strcpy(aux->token, novoToken);
	strcpy(aux->lexema, novoLexema);
	aux->linha = linha;
	aux->coluna = colunaLexema;
	aux->prox = NULL;

	if(listaTokens == NULL){
		listaTokens = aux;
		colunaLexema++;
	}else{
		for (i = listaTokens; i->prox != NULL; i=i->prox);
		i->prox = aux;
	}

}
