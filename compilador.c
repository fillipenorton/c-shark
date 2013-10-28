#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RES_PALAVRAS 19 // quantidade de palavras reservadas
#define RES_SIMBOLOS 27 // quantidade de simbolos
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
int coluna = 0; // coluna atual
int colunaLexema = 1; // coluna do ultimo lexema lido
int boolNovoLexema = 0; // 1 se foi lido um novo lexema

char tokens[50][20] = {"CLASS", "FOR","WHILE", "IF", "ELSE", "SWITCH", "CASE", "VOID", "CHAR", "INT", "FLOAT", "BOOL", // 12
	"CONST", "TRUE", "FALSE", "NULL", "RETURN", "BREAK", "STRUCT", "ADD", "SUB", "MUL", "DIV", "MOD", "IGUAL", // 25
	"MAIOR_IGUAL", "MENOR_IGUAL", "MAIOR", "MENOR", "DIF", "NOT", "AND", "OR", "ATRIB", "ABRE_PAREN",
	"FECHA_PAREN", "ABRE_COLCH", "FECHA_COLCH", "ABRE_CHAV", "FECHA_CHAV", "FIM", "SEPARADOR", "COMENT_CURTO",
	"ABRE_COMENT_LONGO", "FECHA_COMENT_LONGO", "NUM_FLOAT", "NUM_INT"};

char res_palavras[RES_PALAVRAS][20] = {"class", "for", "while", "if", "else", "switch", "case", "void", "char", "int",
		"float", "bool", "const", "true", "false", "null", "return", "break", "struct"};

char res_simbolos[RES_SIMBOLOS][2] = {"+", "-", "*", "/", "%", "==", ">=", "<=", ">", "<", "!=", "!", "&&", "||", "=",
		"(", ")", "[", "]", "{", "}", ";", ",", "//", "/*", "*/"};

void inicializa();
void analisador();
void identificador();
char *verificaTokenIdentificador(char ident[MAX_IDENT]);
void numero();
void addListaTokens();
void imprimeListaTokens(estruturaToken *listaT);
void simbolos();
char *getTokenSimbolo(char simb[MAX_IDENT]);

int main(int argc, char *argv[]){
	char *codigoFonte = NULL;

	strcpy(codigoFonte, argv[1]);

	inicializa(codigoFonte);

	analisador();

	imprimeListaTokens(listaTokens);

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

	while((caractere = fgetc(fonte)) != EOF){ // enquanto não chegar ao fim do codigo-fonte
		boolNovoLexema = 0;
		novoLexema[0] = '0'; // inicializa a cada loop para poder formar novos lexemas

		coluna++;
		colunaLexema = coluna;

		if((caractere >= 'a' && caractere <= 'z') || (caractere >= 'A' && caractere <= 'Z')) { // identificadores
			identificador();
			novoToken = verificaTokenIdentificador(novoLexema);
			boolNovoLexema = 1;
		}else if((caractere >= '0' && caractere <= '9')){ // numeros

		}else if(caractere == '\n'){
			linha++;
			coluna = 0;
			boolNovoLexema = 1;
			strcpy(novoLexema,"\n");
		}else if(caractere == '\t'){
			boolNovoLexema = 1;
			strcpy(novoLexema,"\t");
		}else if(caractere != ' '){ //não-letra, não-número, então simbolo
			simbolos();
			boolNovoLexema = 1;
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
		strcat(novoLexema, auxCaractere); // concatena caracteres para formar identificador

		caractere = fgetc(fonte); // lê o proximo caractere
		coluna++;

		if(caractere == '\n'){
			linha++;
		}
	}

	if(caractere != ' '){ // caso de identificador seguido de simbolo
		ungetc(caractere, fonte);
		coluna--;
	}
}

/*
 *  Verifica se o identificador é uma palavra reservada. Retorna o
 *  token do identificador, caso seja.
 *
 *  @param ident Um identificador
 *  @return token correspondente ao identificador
*/
char *verificaTokenIdentificador(char ident[MAX_IDENT]){
	int i;

	for(i=0; i<RES_PALAVRAS; i++){
		if(strcmp(res_palavras[i],ident)==0){ // se for palavra reservada retorna o token correspondente
			return tokens[i];
		}
	}

	return "IDENT";
}

/*
 * Função para o Estado simbolos do autômato
 * Armazena o simbolo lido na variável global novoLexema
 */
void simbolos(){
	char auxCaractere[2];

	auxCaractere[0] = caractere;
	auxCaractere[1] = '\0';

	if(caractere == '"'){ // string
		novoLexema[0] = '\0';
		strcat(novoLexema, "\\\"");
		caractere = fgetc(fonte);
		coluna++;
		do{
			auxCaractere[0] = caractere;
			auxCaractere[1] = '\0';

			strcat(novoLexema, auxCaractere);

			caractere = fgetc(fonte);
			coluna++;
		}while(caractere != '"');
		if(caractere=='"'){
			strcat(novoLexema, "\\\"");
		}
		strcpy(novoToken, "CADEIA");
	}else{
		strcpy(novoLexema, auxCaractere);
		novoToken = getTokenSimbolo(novoLexema);
	}
}

/*
 *  Retorna o token de um simbolo da linguagem.
 *
 *  @param lex Um simbolo
 *  @return token correspondente ao simbolo
*/
char *getTokenSimbolo(char simb[MAX_IDENT]){
	int i;

	for(i=0; i<RES_SIMBOLOS; i++){
		if(strcmp(res_simbolos[i],simb) == 0){
			return tokens[RES_PALAVRAS+i];
		}
	}

	return "ERROR";
}

void numero(){

}

/*
 * Adiciona um token na lista de tokens do código-fonte
*/
void addListaTokens(){
	estruturaToken *aux, *i;

	aux = (estruturaToken *) malloc(sizeof(estruturaToken));

	if(strcmp(novoLexema, "\n") == 0){
		strcpy(aux->token, "FDL"); // FimDeLinha
		strcpy(aux->lexema, novoLexema);
		aux->linha = linha;
		aux->coluna = colunaLexema;
		aux->prox = NULL;
	}else if(strcmp(novoLexema, "\t") == 0){
		strcpy(aux->token, "TAB"); // Tabulação
		strcpy(aux->lexema, novoLexema);
		aux->linha = linha;
		aux->coluna = colunaLexema;
		aux->prox = NULL;
	}else{
		strcpy(aux->token, novoToken);
		strcpy(aux->lexema, novoLexema);
		aux->linha = linha;
		aux->coluna = colunaLexema;
		aux->prox = NULL;
	}

	if(listaTokens == NULL){
		listaTokens = aux;
	}else{
		for (i = listaTokens; i->prox != NULL; i = i->prox);
		i->prox = aux; // adiciona na ultima posicao da lista
	}

}

/*
 * Imprime uma lista de tokens de código-fonte
*/
void imprimeListaTokens(estruturaToken *listaT){
	estruturaToken *i;

	for (i = listaT; i != NULL; i = i->prox){
		if(strcmp(i->lexema, "\n") == 0)
			printf("<%s>\n", i->token); // APAGAR \n
		else if(strcmp(i->lexema, "\t") == 0)
			printf("<%s>", i->token);
		else
			printf("<%s,\"%s\">", i->token, i->lexema);
	}
}
