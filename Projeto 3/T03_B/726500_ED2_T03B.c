/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com encadeamento
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
#define INICIO_EXCLUSAO  "**********************EXCLUIR*********************\n"
#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso.\n\n"



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
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave {
	char pk[TAM_PRIMARY_KEY];
	int rrn;
	struct chave *prox;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
  int tam;
  Chave **v;
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

Chave *buscar_pos(char *chave, Hashtable tabela);

void imprimir_tabela(Hashtable tabela);

void criar_tabela(Hashtable *tabela, int tam);

Jogo recuperar_registro(int rrn);

void inserir_lista(Hashtable *tabela, Chave *nova);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */

int main() 
{
	
	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) 
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d%*c", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo) 
		carregar_tabela(&tabela);
	


	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
		scanf("%d%*c", &opcao);
		switch(opcao) {

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
			printf("%s", (remover(&tabela)) ? SUCESSO : FALHA );
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
	int i;
	char pk[TAM_PRIMARY_KEY], *p = ARQUIVO;
	Chave *nova_chave;

	//atualiza numero de registros
	nregistros = strlen(ARQUIVO) / TAM_REGISTRO;

	for(i = 0; i < nregistros; i++, p+= 192){
		//copia a pk para inserir
		strncpy(pk, p, 10);
		pk[10] = '\0';
		nova_chave = (Chave *) malloc (sizeof(Chave));
		nova_chave->rrn = i;
		strcpy(nova_chave->pk, pk);
		nova_chave->prox = NULL;
		inserir_lista(tabela, nova_chave);
	}
}

void cadastrar(Hashtable* tabela){
	char mes[3], registro[192];
 	Jogo novo;
	int i;
	Chave *nova_chave;

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
	if(buscar_pos(novo.pk, *tabela) != NULL){
		printf(ERRO_PK_REPETIDA, novo.pk);
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
	
	//cria e insere nova chave no indice
	nova_chave = (Chave *) malloc (sizeof(Chave));
	nova_chave->rrn = nregistros;
	strcpy(nova_chave->pk, novo.pk);
	nova_chave->prox = NULL;
	inserir_lista(tabela, nova_chave);
	nregistros++;

	//mensagem de insercao
	printf(REGISTRO_INSERIDO, novo.pk);
}

void inserir_lista(Hashtable *tabela, Chave *nova){
	Chave *aux;
	int pos;

	//recebe primeira posiçao possivel
	pos = hash(nova->pk, tabela->tam);
	aux = tabela->v[pos];

	if(aux == NULL){
		tabela->v[pos] = nova;
	}else{
		//se chave nova e menor que o primeiro, insere e sai da funcao 
		if(strcmp(tabela->v[pos]->pk, nova->pk) > 0){
			nova->prox = tabela->v[pos];
			tabela->v[pos] = nova;
			return;
		}

		//se nao, busca onde colocar percorrendo com aux
		while(aux->prox != NULL && strcmp(aux->prox->pk, nova->pk) < 0){
			aux = aux->prox;
		}

		//insere
		nova->prox = aux->prox;
		aux->prox = nova;
	}
}

int  alterar(Hashtable tabela){
	char pk[11], desconto_novo[192], *p;
 	Jogo j;
 	int valor_desconto = -1;
 	Chave *pos_buscada;

 	scanf("%[^\n]%*c", pk); //recebe pk a ser buscada

 	//busca posicao
 	pos_buscada = buscar_pos(pk, tabela);

 	if(pos_buscada == NULL){
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
		p = ARQUIVO + pos_buscada->rrn * 192;

		sscanf(p, "%[^@]@%[^@]@%[^@]", j.pk, j.nome, j.desenvolvedora);
		p += strlen(j.pk) + strlen(j.nome) + strlen(j.desenvolvedora) + 25;

		strncpy(p, desconto_novo, 3);
		return 1;
 	}
}

void buscar(Hashtable tabela){
	char pk[TAM_PRIMARY_KEY];
	Chave *pos_buscado;

	//le chave a ser buscada
	scanf("%[^\n]%*c", pk);

	//guarda rrn de busca
	pos_buscado = buscar_pos(pk, tabela);

	if(pos_buscado == NULL){
		//caso nao encontre o registro
		printf(REGISTRO_N_ENCONTRADO);
	}else{
		exibir_registro(pos_buscado->rrn);
	}
}

//retorna NULL em caso de falha, caso contrario o no Chave buscado
Chave *buscar_pos(char *chave, Hashtable tabela){
	int hash_pos;
	Chave *aux;

	//acha a posicao inicial da tabel hash
	hash_pos = hash(chave, tabela.tam);

	//busca chave usando aux para eprcorre lista
	aux = tabela.v[hash_pos];

	while(aux != NULL){
		if(strcmp(aux->pk, chave) == 0)
			return aux;
		aux = aux->prox;
	}

	return NULL;
}

int remover(Hashtable* tabela){
	int hash_pos;
	char pk[TAM_PRIMARY_KEY], *p;
	Chave *libera = NULL, *aux;

	//le chave a ser removida
	scanf("%[^\n]%*c", pk);

	//acha a posicao inicial da tabel hash
	hash_pos = hash(pk, tabela->tam);

	//verifica se posicao inicial da tabela esta vazia
	aux = tabela->v[hash_pos];

	if(aux == NULL){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	//verifica se e a primeira posicao
	//caso seja, liber a e sai da funcao
	if(strcmp(tabela->v[hash_pos]->pk, pk) == 0){
		libera = tabela->v[hash_pos];
		tabela->v[hash_pos] = tabela->v[hash_pos]->prox;
		//marca a remoçao no arquivo
		p = ARQUIVO + libera->rrn * TAM_REGISTRO;
		strncpy(p, "*|", 2);
		//libera
		free(libera);
		return 1;
	}

	//busca chave anterior usando aux para eprcorre lista

	while(aux->prox != NULL){
		//se o prox for a chave a ser removida, faz as devidas alteraçoes
		//e sai da funcao
		if(strcmp(aux->prox->pk, pk) == 0){
			//pega a Chave para liberar e arruma a lista
			libera = aux->prox;
			aux->prox = aux->prox->prox;
			//marca a remoçao no arquivo
			p = ARQUIVO + libera->rrn * TAM_REGISTRO;
			strncpy(p, "*|", 2);
			//libera de fato
			free(libera);
			return 1;
		}
		aux = aux->prox;
	}

	//se nao encontrar na lista
	printf(REGISTRO_N_ENCONTRADO);
	return 0;
}

void liberar_tabela(Hashtable* tabela){
	int i;
	Chave *libera, *aux;

	for(i = 0; i < tabela->tam; i++){
		//rotina para liberar possiveis chaves na posicao
		aux = tabela->v[i];
		//aux percorre as chaves a serem liberadas
		while(aux != NULL){
			libera = aux;
			aux = aux->prox;
			free(libera);
		}
	}

	//libera tabela
	free(tabela->v);
}

void imprimir_tabela(Hashtable tabela){
	int i;
	Chave *aux;

	//imprime de acordo com o estado
	for(i = 0; i < tabela.tam; i++){
		//imprime posicao da tabela hash
		printf("[%d]", i);
		//rotina para imprimir chaves naquela posicao
		aux = tabela.v[i];
		while(aux != NULL){
			printf(" %s", aux->pk);
			aux = aux->prox;
		}
		printf("\n");
	}
}

//optando por criar e ja inicializar
void criar_tabela(Hashtable *tabela, int tam){
	int i;

	//inicializa nregistros
	nregistros = 0;

	//aloca e inicializa tabela
	tabela->tam = tam;
	tabela->v = (Chave **) malloc (tam * sizeof(Chave*));

	for(i = 0; i < tam; i++){
		tabela->v[i] = (Chave *) malloc (sizeof(Chave));
		tabela->v[i] = NULL;
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