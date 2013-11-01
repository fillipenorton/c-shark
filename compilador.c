#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RES_PALAVRAS 19 // quantidade de palavras reservadas
#define RES_SIMBOLOS 27 // quantidade de simbolos
#define MAX_IDENT 200 // tamanho máximo do identificador

typedef struct token{
	char token[30];
	char lexema[MAX_IDENT];
	int linha, coluna;
	struct token *prox;
}estruturaToken;

typedef struct erro{
	char tipo[30];
	char lexema[MAX_IDENT];
	int linha, coluna;
	struct erro *prox;
}estruturaErro;

FILE *fonte; // codigo-fonte .csk

estruturaToken *listaTokens; // lista de tokens do codigo-fonte
estruturaErro *listaErros; // lista de erros

char novoLexema[MAX_IDENT]; // armazena cada lexema lido do fonte
char* novoToken; // armazena o token correspondente ao lexema lido
char caractere; // armazena cada caractere lido do fonte
int linha = 1; // linha atual
int coluna = 0; // coluna atual
int colunaLexema = 1; // coluna do ultimo lexema lido
int boolNovoLexema = 0; // 1 se foi lido um novo lexema
int boolError = 0;
int boolTab = 0;

char tokens[50][20] = {"CLASS", "FOR","WHILE", "IF", "ELSE", "SWITCH", "CASE", "VOID", "CHAR", "INT", "FLOAT", "BOOL",
	"CONST", "TRUE", "FALSE", "NULL", "RETURN", "BREAK", "STRUCT", "ADD", "SUB", "MUL", "DIV", "MOD", "IGUAL",
	"MAIOR_IGUAL", "MENOR_IGUAL", "MAIOR", "MENOR", "DIF", "NOT", "AND", "OR", "ATRIB", "ABRE_PAREN",
	"FECHA_PAREN", "ABRE_COLCH", "FECHA_COLCH", "ABRE_CHAV", "FECHA_CHAV", "FIM", "SEPARADOR", "INCREMENTO", "DECREMENTO"};

char res_palavras[RES_PALAVRAS][20] = {"class", "for", "while", "if", "else", "switch", "case", "void", "char", "int",
		"float", "bool", "const", "true", "false", "null", "return", "break", "struct"};

char res_simbolos[RES_SIMBOLOS][3] = {"+", "-", "*", "/", "%", "==", ">=", "<=", ">", "<", "!=", "!", "&&", "||", "=",
		"(", ")", "[", "]", "{", "}", ";", ",", "++", "--"};

void inicializa();
void analisador();
void identificador();
char *verificaTokenIdentificador(char ident[MAX_IDENT]);
void numero();
void addListaTokens();
void imprimeListaTokens(estruturaToken *listaT, int quebraLinha);
void simbolos();
char *getTokenSimbolo(char simb[MAX_IDENT]);
void copiaString(char** to, char* from);
void imprimeListaErros(estruturaErro *listaE);
void addListaErros();
void concatChar(char *to, char from);

int main(int argc, char *argv[]){
	char *codigoFonte;

	copiaString(&codigoFonte, argv[1]);

	inicializa(codigoFonte);

	analisador();

	imprimeListaTokens(listaTokens, 0);
	imprimeListaErros(listaErros);

	fclose(fonte);
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
	listaErros = NULL;
}

void analisador(){

	while((caractere = fgetc(fonte)) != EOF){ // enquanto não chegar ao fim do codigo-fonte
		boolNovoLexema = 0;
		boolError = 0;
		boolTab = 0;
		novoLexema[0] = '\0'; // inicializa a cada loop para poder formar novos lexemas

		coluna++;
		colunaLexema = coluna;

		if((caractere >= 'a' && caractere <= 'z') || (caractere >= 'A' && caractere <= 'Z')) { // identificador
			identificador();
			if(!boolError){
				boolNovoLexema = 1;
				copiaString(&novoToken, verificaTokenIdentificador(novoLexema));
			}else{
				addListaErros();
			}
		}else if((caractere >= '0' && caractere <= '9')){ // numero
			numero();
			if(!boolError)
				boolNovoLexema = 1;
			else
				addListaErros();
		}else if(caractere == '\n'){
			linha++;
			coluna = 0;
		}else if(caractere != ' ' && caractere != '\t'){ //não-letra, não-número, então simbolo
			simbolos();
			if(!boolError){
				boolNovoLexema = 1;
			}else{
				addListaErros();
			}
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
	novoLexema[0] = '\0';

	while((caractere >= 'a' && caractere <= 'z') || // loop para identificar o lexema
			(caractere >= 'A' && caractere <= 'Z') ||
			(caractere >= '0' && caractere <= '9')){

		concatChar(novoLexema, caractere);
		caractere = fgetc(fonte); // lê o proximo caractere
		coluna++;

		if(caractere == '\n'){
			linha++;
		}
	}

	if(caractere != ' ' && caractere != '(' && caractere != ')' && caractere != '{' && caractere != '*' && caractere != '/' &&
			caractere != '+' && caractere != '-' && caractere != '[' && caractere != ']' && caractere != ';' && caractere != ','){
		boolError = 1;
		while(caractere != ' ' && caractere != ';' && caractere != ',' && caractere != '\n' &&
				caractere != '{' && caractere != '(' && caractere != ')' && caractere != ']'){
			concatChar(novoLexema, caractere);
			caractere = fgetc(fonte);
		}
		ungetc(caractere, fonte);
		copiaString(&novoToken, "caractere invalido em IDENT");
		return;
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
	int boolTokenAtribuido = 0;
	int boolAsteriscoAntes = 0;
	novoLexema[0] = '\0';

	if(caractere == '"'){ // string
		boolTokenAtribuido = 1;
		strcat(novoLexema, "\\\"");
		caractere = fgetc(fonte);
		coluna++;
		do{
			concatChar(novoLexema, caractere);

			caractere = fgetc(fonte);
			coluna++;
		}while(caractere != '"');
		if(caractere=='"'){
			strcat(novoLexema, "\\\"");
		}
		copiaString(&novoToken, "CADEIA");
	}else if(caractere == '<' || caractere == '>' || caractere == '=' || caractere == '!'){
		concatChar(novoLexema, caractere);
		caractere = fgetc(fonte);
		coluna++;

		if(caractere == '='){
			concatChar(novoLexema, caractere);
		}
	}else if(caractere == '/'){
		concatChar(novoLexema, caractere);
		caractere = fgetc(fonte);
		coluna++;

		if(caractere == '/'){ // Comentário curto
			while(caractere != '\n'){
				concatChar(novoLexema, caractere);

				caractere = fgetc(fonte);
				coluna++;
			}
			linha++;
			coluna = 1;
			copiaString(&novoToken, "COMENT_CURTO");
			boolTokenAtribuido = 1;
		}else if(caractere == '*'){ // Comentário longo
			while(boolAsteriscoAntes != 1){
				concatChar(novoLexema, caractere);

				caractere = fgetc(fonte);
				coluna++;
				if(caractere == '\n'){
					caractere = ' '; // salvando quebra de linha em comentários como espaço
					linha++;
				}
				if(caractere == '*'){
					concatChar(novoLexema, caractere);

					caractere = fgetc(fonte);
					coluna++;
					if(caractere == '/'){
						boolAsteriscoAntes = 1;

						concatChar(novoLexema, caractere);
					}
				}
			}
			copiaString(&novoToken, "COMENT_LONGO");
			boolTokenAtribuido = 1;
		}
		
	}else if(caractere == '&'){
		concatChar(novoLexema, caractere);
		caractere = fgetc(fonte);
		coluna++;

		if(caractere == '&'){ // Operador lógico AND
			concatChar(novoLexema, caractere);
		}else{ // ERRO

		}
	}else if(caractere == '|'){
		concatChar(novoLexema, caractere);
		caractere = fgetc(fonte);
		coluna++;

		if(caractere == '|'){ // Operador lógico OR
			concatChar(novoLexema, caractere);
		}else{ // ERRO

		}
	}else if(caractere == '+'){
		concatChar(novoLexema, caractere);
		caractere = fgetc(fonte);
		coluna++;

		if(caractere == '+'){ // Incremento
			concatChar(novoLexema, caractere);
		}else{
			ungetc(caractere, fonte);
			coluna--;
		}
	}else if(caractere == '-'){
		concatChar(novoLexema, caractere);
		caractere = fgetc(fonte);
		coluna++;

		if(caractere == '-'){ // Decremento
			concatChar(novoLexema, caractere);
		}else{
			ungetc(caractere, fonte);
			coluna--;
		}
	}else{
		concatChar(novoLexema, caractere);
	}
	if(!boolTokenAtribuido){
		novoToken = getTokenSimbolo(novoLexema);
	}
	if(strcmp(novoToken, "ERRO LEXICO") == 0){
		boolError = 1;
		while(caractere != ' ' && caractere != ';' && caractere != '\n' &&
				caractere != '{' && caractere != '(' && caractere != ')'){
			caractere = fgetc(fonte);
			concatChar(novoLexema, caractere);
		}
		ungetc(caractere, fonte);
		copiaString(&novoToken, "símbolo inválido");
	}
}

/*
 *  Retorna o token de um simbolo da linguagem.
 *
 *  @param lex Um simbolo
 *  @return token correspondente ao simbolo
*/
char *getTokenSimbolo(char simb[3]){
	int i;

	for(i=0; i<RES_SIMBOLOS; i++){
		if(strcmp(res_simbolos[i],simb) == 0){
			return tokens[RES_PALAVRAS+i];
		}
	}

	return "ERRO LEXICO";
}

/*
 * Função para o Estado numero do autômato
 * Armazena o numero lido na variável global novoLexema
 */
void numero(){
	int isFloat = 0;
	novoLexema[0] = '\0';

	while( (caractere >= '0' && caractere <= '9') || caractere == '.'){ // loop para identificar o lexema
		concatChar(novoLexema, caractere);

		caractere = fgetc(fonte); // lê o proximo caractere
		coluna++;

		if(caractere == '.'){
			isFloat = 1;
		}

		if(caractere == '\n'){
			linha++;
		}
	}

	if(caractere != ' ' && caractere != ')' && caractere != '*' && caractere != '/' && caractere != '+' &&
			caractere != '-' && caractere != '[' && caractere != ']' && caractere != ';' && caractere != ','){
		boolError = 1;
		while(caractere != ' ' && caractere != ';' && caractere != ',' && caractere != '\n'){
			concatChar(novoLexema, caractere);
			caractere = fgetc(fonte);
		}
		copiaString(&novoToken, "esperado número");
		return;
	}

	if(isFloat){
		copiaString(&novoToken, "NUM_FLOAT");
	}else{
		copiaString(&novoToken, "NUM_INT");
	}

	if(caractere != ' '){ // caso de identificador seguido de simbolo, volta para não perder caractere
		ungetc(caractere, fonte);
		coluna--;
	}
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
	}else{
		for (i = listaTokens; i->prox != NULL; i = i->prox);
		i->prox = aux; // adiciona na ultima posicao da lista
	}
}

/*
 * Imprime uma lista de tokens de código-fonte
 *
 * @param listaT Uma lista de tokens
 * @param quebraLinha Ativa ou desativa quebra de linha entre tokens
*/
void imprimeListaTokens(estruturaToken *listaT, int quebraLinha){
	estruturaToken *i;

	for (i = listaT; i != NULL; i = i->prox){
		if(quebraLinha)
			printf("<%s,\"%s\">\n", i->token, i->lexema);
		else
			printf("<%s,\"%s\">", i->token, i->lexema);
	}
}

/*
 * Adiciona um erro na lista de erros do código-fonte
*/
void addListaErros(){
	estruturaErro *aux, *i;

	aux = (estruturaErro *) malloc(sizeof(estruturaErro));

	strcpy(aux->tipo, novoToken);
	strcpy(aux->lexema, novoLexema);
	aux->linha = linha;
	aux->coluna = colunaLexema;
	aux->prox = NULL;

	if(listaErros == NULL){
		listaErros = aux;
	}else{
		for (i = listaErros; i->prox != NULL; i = i->prox);
		i->prox = aux; // adiciona na ultima posicao da lista
	}

}

/*
 * Imprime uma lista de erros de código-fonte
 *
 * @param listaE Uma lista de erros
*/
void imprimeListaErros(estruturaErro *listaE){
	estruturaErro *i;

	for (i = listaE; i != NULL; i = i->prox){
		printf("\n%d:%d: '%s' %s", i->linha, i->coluna, i->lexema, i->tipo);
	}
}

/*
 * Função utilizada quando o tamanho da string 'to'
 * não foi previamente definida.
 * Copia da string 'from'para a string 'to'.
 *
 * @param to Endereço de uma string
 * @param from Uma string
*/
void copiaString(char **to, char *from){
	int tam;
	tam = strlen(from) + 1;

	*to = (char *) malloc(sizeof(char) * tam);

	strcpy(*to, from);
}

/*
 * Função utilizada para concatenar um char em uma string
 * Cocatena o char 'from'na string 'to'.
 *
 * @param to Uma string
 * @param from Um char
*/
void concatChar(char *to, char from){
	char aux[2];

	aux[0] = from;
	aux[1] = '\0';

	strcat(to, aux);
}
