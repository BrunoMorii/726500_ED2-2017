/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com reespalhamento linear
 *
 * RA: 726500
 * Aluno: Bruno Morii Borges
 * ========================================================================== */

 /* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_DESENVOLVEDORA 51
#define TAM_DATA 11
#define TAM_CLASSIFICACAO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51

#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


#define POS_OCUPADA 	"[%d] Ocupado: %s\n"
#define POS_LIVRE 		"[%d] Livre\n"
#define POS_REMOVIDA 	"[%d] Removido\n"

/* Estado das posições da tabela hash */
#define LIVRE       0
#define OCUPADO     1
#define REMOVIDO    2

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA 							 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM						  "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 						"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO					    "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO 			 			"**********************EXCLUIR*********************\n"

#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define ERRO_TABELA_CHEIA 		"ERRO: Tabela Hash esta cheia!\n\n"
#define REGISTRO_INSERIDO 		"Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"


/* Registro do jogo */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char desenvolvedora[TAM_DESENVOLVEDORA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char classificacao[TAM_CLASSIFICACAO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Jogo;

/* Registro da Tabela Hash
 * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
typedef struct {
	int estado;
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
    int tam;
    Chave *v;
  } Hashtable;

  /* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;




/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */


/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe o jogo */
int exibir_registro(int rrn);

/*Função de Hash*/
short hash(const char* chave, int tam);


/*Auxiliar para a função de hash*/
short f(char x);

/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a);

/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela);
void cadastrar(Hashtable* tabela);
int  alterar(Hashtable tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);


/* <<< DECLARE AQUI OS PROTOTIPOS >>> */

//retorna -1 em caso de falha, caso contrario a posicao na tabela
int buscar_pos(char *chave, Hashtable tabela);

void imprimir_tabela(Hashtable tabela);

void criar_tabela(Hashtable *tabela, int tam);

Jogo recuperar_registro(int rrn);



/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main() 
{    
	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d\n", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) 
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo) 
		carregar_tabela(&tabela);

	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
			scanf("%d%*c", &opcao);
			switch(opcao) 
			{
				case 1:
					cadastrar(&tabela);
					break;
				case 2:
					printf(INICIO_ALTERACAO);
					if(alterar(tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
					break;
				case 3:
					printf(INICIO_BUSCA);
					buscar(tabela);
					break;
				case 4:
					printf(INICIO_EXCLUSAO);
					if(	remover(&tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
					break;
				case 5:
					printf(INICIO_LISTAGEM);
					imprimir_tabela(tabela);
					break;
				case 6:
					liberar_tabela(&tabela);
					break;
				case 10:
					printf(INICIO_ARQUIVO);
					printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
					break;
				default:
					printf(OPCAO_INVALIDA);
					break;
			}
	}
	return 0;
}

/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
}

/*Auxiliar para a função de hash*/
short f(char x)
{
	return (x < 59) ? x - 48 : x - 54; 
}

/* Exibe o jogo */
int exibir_registro(int rrn)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Jogo j = recuperar_registro(rrn);
  char *cat, categorias[TAM_CATEGORIA];
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.desenvolvedora);
	printf("%s\n", j.data);
	printf("%s\n", j.classificacao);
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strncpy(categorias, j.categoria, strlen(j.categoria) + 1);
  for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    printf("%s ", cat);
	printf("\n");
	return 1;
}

/*Função de Hash*/
short hash(const char* chave, int tam){
	int i, soma = 0; //soma e a somatoria da funcao de hash

	//realiza somatoria
	for(i = 1; i <= 8; i++){
		soma += i * f(chave[i - 1]);
	}

	//retorna valor valido usando mod tam
	return(soma % tam);
}

/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a){
	int i = a, max_int = pow(2, 31) - 1, j;
	char flag_primo = 'n';

	//se i for 1 ou 2, retorna 2, senao realiza operacao de identificar primo
	if(i == 1 || i == 2){
		return 2;
	}

	//se i for par, i nao e primo, logo o proximo valor deve se iniciar impar
	if(i % 2 == 0)
		i++;

	//impar para otimizar funcao de buscar primo mais proximo
	//loop roda ate encontrar primo ou estourar int
	while(i < max_int && flag_primo != 's'){
		//inicializa flag como verdadeira
		flag_primo = 's';
		//rotina para buscar primo
		for(j = 2; j <= sqrt(i); j++){
			if(i % j == 0){
				flag_primo = 'n';
			}
		}
		//se nao e primo incremente i para proximo impar
		if(flag_primo == 'n')
			i +=2;
	}

	return i;
}

/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela){
	int i, j, hash_pos;
	char pk[TAM_PRIMARY_KEY], *p = ARQUIVO, inseriu;

	//atualiza numero de registros
	nregistros = strlen(ARQUIVO) / TAM_REGISTRO;

	for(j = 0; j < nregistros; j++, p+= 192){
		//copia a pk para inserir
		strncpy(pk, p, 10);
		pk[11] = '\0';
		inseriu = 'n';
		//busca onde inserir
		hash_pos = hash(pk, tabela->tam);
		for(i = 0; i < tabela->tam && inseriu == 'n'; i++){
			if(tabela->v[(hash_pos + i) % tabela->tam].estado != OCUPADO){
				tabela->v[(hash_pos + i) % tabela->tam].estado = OCUPADO;
				tabela->v[(hash_pos + i) % tabela->tam].rrn = j;
				strcpy(tabela->v[(hash_pos + i) % tabela->tam].pk, pk);
				inseriu = 's';
			}
		}
	}
}

void cadastrar(Hashtable* tabela){
	char mes[3], registro[192], inseriu = 'n';
 	Jogo novo;
	int i, hash_pos, colisoes;

	/* parte para leitura do novo jogo */
	//leitura dos dados do novo jogo
	scanf("%[^\n]%*c", novo.nome);
	scanf("%[^\n]%*c", novo.desenvolvedora);
	scanf("%[^\n]%*c", novo.data);
	scanf("%[^\n]%*c", novo.classificacao);
	scanf("%[^\n]%*c", novo.preco);
	scanf("%[^\n]%*c", novo.desconto);
	scanf("%[^\n]%*c", novo.categoria);

	//leitura do mes em data para compor a pk
	sscanf(novo.data, "%*[^'/']/%[^'/']", mes);

	//criando chave primaria
	strncpy(novo.pk, novo.nome, 2);
	novo.pk[2] = '\0';
	strncat(novo.pk, novo.desenvolvedora, 2);
	strncat(novo.pk, novo.data, 2);
	strcat(novo.pk, mes);
	strncat(novo.pk, novo.classificacao, 2);
	/* fim leitura e criaçao struct jogo */

	//busca existencia do jogo, mensagem de falha em caso de pk existente e sai da funcao
	if(buscar_pos(novo.pk, *tabela) != -1){
		printf(ERRO_PK_REPETIDA, novo.pk);
		return;
	}

	//busca onde inserir, se estiver cheia, mensagem de erro e return
	hash_pos = hash(novo.pk, tabela->tam);
	for(i = 0; i < tabela->tam && inseriu == 'n'; i++){
		if(tabela->v[(hash_pos + i) % tabela->tam].estado != OCUPADO){
			tabela->v[(hash_pos + i) % tabela->tam].estado = OCUPADO;
			tabela->v[(hash_pos + i) % tabela->tam].rrn = nregistros;
			strcpy(tabela->v[(hash_pos + i) % tabela->tam].pk, novo.pk);
			inseriu = 's';
			colisoes = i;
		}
	}

	//mensagem de erro de tabela cheia
	if(inseriu == 'n'){
		printf(ERRO_TABELA_CHEIA);
		return;
	}

	//criando registro
	strcpy(registro, novo.pk);
	registro[strlen(novo.pk)] = '\0';
	strcat(registro, "@");
	strcat(registro, novo.nome);
	strcat(registro, "@");
	strcat(registro, novo.desenvolvedora);
	strcat(registro, "@");
	strcat(registro, novo.data);
	strcat(registro, "@");
	strcat(registro, novo.classificacao);
	strcat(registro, "@");
	strcat(registro, novo.preco);
	strcat(registro, "@");
	strcat(registro, novo.desconto);
	strcat(registro, "@");
	strcat(registro, novo.categoria);
	strcat(registro, "@");

	//preenchendo espaço do registro com '#'
	for(i = strlen(registro); i < 192; i++){
		registro[i] = '#';
	}

	//insere o novo registro
	strncat(ARQUIVO, registro, 192);
	nregistros++;

	//mensagem de insercao
	printf(REGISTRO_INSERIDO, novo.pk, colisoes);
}

int  alterar(Hashtable tabela){
	char pk[11], desconto_novo[192], *p;
 	Jogo j;
 	int pos_buscada, valor_desconto = -1;

 	scanf("%[^\n]%*c", pk); //recebe pk a ser buscada

 	//busca posicao
 	pos_buscada = buscar_pos(pk, tabela);

 	if(pos_buscada == -1){
 		//caso nao encontre a chave
 		printf(REGISTRO_N_ENCONTRADO);
 		return 0;
 	}else{
 		//rotina para receber e verificar novo desconto
		do{
			scanf("%[^\n]%*c", desconto_novo);
			sscanf(desconto_novo, "%d", &valor_desconto);
			if(strlen(desconto_novo) != 3 || valor_desconto > 100 || valor_desconto < 0)
				printf(CAMPO_INVALIDO);
		}while(strlen(desconto_novo) != 3 || valor_desconto > 100 || valor_desconto < 0);

		//rotina para inserir desconto
		p = ARQUIVO + tabela.v[pos_buscada].rrn * 192;

		sscanf(p, "%[^@]@%[^@]@%[^@]", j.pk, j.nome, j.desenvolvedora);
		p += strlen(j.pk) + strlen(j.nome) + strlen(j.desenvolvedora) + 25;

		strncpy(p, desconto_novo, 3);
		return 1;
 	}
}

void buscar(Hashtable tabela){
	char pk[TAM_PRIMARY_KEY];
	int pos_buscado;

	//le chave a ser buscada
	scanf("%[^\n]%*c", pk);

	//guarda rrn de busca
	pos_buscado = buscar_pos(pk, tabela);

	if(pos_buscado == -1){
		//caso nao encontre o registro
		printf(REGISTRO_N_ENCONTRADO);
		return;
	}else{
		//se encontrado, exibe registro
		exibir_registro(tabela.v[pos_buscado].rrn);
	}
}

//retorna -1 em caso de falha, caso contrario a posicao na tabela
int buscar_pos(char *chave, Hashtable tabela){
	int hash_pos, i;

	//acha a posicao inicial da tabel hash
	hash_pos = hash(chave, tabela.tam);

	//busca linearmente a chave
	for(i = 0; i < tabela.tam; i++){
		//se espaco e ocupado e a chave e correpsondente, retorna o rrn
		if(strcmp(chave, tabela.v[(hash_pos + i) % tabela.tam].pk) == 0 && 
		tabela.v[(hash_pos + i) % tabela.tam].estado == OCUPADO){
			return((hash_pos + i) % tabela.tam);
		}
	}

	return -1;
}

int remover(Hashtable* tabela){
	char pk[TAM_PRIMARY_KEY], *p;
	int pos_buscado;

	//le chave a ser removida
	scanf("%[^\n]%*c", pk);

	//guarda pos de busca
	pos_buscado = buscar_pos(pk, *tabela);

	if(pos_buscado == -1){
		//caso nao encontre o registro
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}else{
		//marca estado como removido
		tabela->v[pos_buscado].estado = REMOVIDO;
		//remove no arquivo
		p = ARQUIVO + tabela->v[pos_buscado].rrn * TAM_REGISTRO;
		strncpy(p, "*|", 2);
	}

	return 1;
}

void liberar_tabela(Hashtable* tabela){
	free(tabela->v);
}

void imprimir_tabela(Hashtable tabela){
	int i;

	//imprime de acordo com o estado
	for(i = 0; i < tabela.tam; i++){
		switch(tabela.v[i].estado){
			case LIVRE:
				printf(POS_LIVRE, i);
				break;

			case REMOVIDO:
				printf(POS_REMOVIDA, i);
				break;

			case OCUPADO:
				printf(POS_OCUPADA, i, tabela.v[i].pk);
				break;
		}
	}
}

//optando por criar e ja inicializar
void criar_tabela(Hashtable *tabela, int tam){
	int i;

	//inicializa nregistros
	nregistros = 0;

	//aloca e inicializa tabela
	tabela->tam = tam;
	tabela->v = (Chave *) malloc (tam * sizeof(Chave));

	for(i = 0; i < tam; i++){
		tabela->v[i].rrn = -1;
		tabela->v[i].estado = LIVRE;
		strcpy(tabela->v[i].pk, "!");

	}
}

Jogo recuperar_registro(int rrn){
	Jogo jogo_busca; //jogo a ser reornado
	char *comeco_registro = ARQUIVO + rrn * TAM_REGISTRO; //auxiliar para compor a chave primaria e localizar o registro correto

	//rotina de leitura do registro
	sscanf(comeco_registro, "%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]", jogo_busca.pk, jogo_busca.nome,
		   jogo_busca.desenvolvedora, jogo_busca.data, jogo_busca.classificacao, 
		   jogo_busca.preco, jogo_busca.desconto, jogo_busca.categoria);

	return(jogo_busca);
}