/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 726500
 * Aluno: Bruno Morii Borges
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 	11
#define TAM_NOME 			51
#define TAM_DESENVOLVEDORA 	51
#define TAM_DATA 			11
#define TAM_CLASSIFICACAO 	3
#define TAM_PRECO 			8
#define TAM_DESCONTO 		4
#define TAM_CATEGORIA 		51
#define TAM_STRING_INDICE 	(TAM_DESENVOLVEDORA + TAM_NOME)


#define TAM_REGISTRO 		192
#define MAX_REGISTROS 		1000
#define MAX_ORDEM 			150
#define TAM_ARQUIVO 		(MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 			 "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE	 "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 	 "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 			 "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 		 "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 			 "Arquivo vazio!"
#define INICIO_BUSCA			 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM 		 "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO		 "********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO			 "********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO	 "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 		 "Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 		 "Busca por %s.\nNos percorridos:\n"


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


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_ip *chave;	/* vetor das chaves e rrns [m-1]*/
	int *desc;			/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_is *chave;	/* vetor das chaves e rrns [m-1]*/
	int *desc;			/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_is;

typedef struct {
	int raiz;
} Indice;

/* Variáveis globais */

char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO];
char ARQUIVO_IP[2000*sizeof(Chave_ip) + 40];
char ARQUIVO_IS[2000*sizeof(Chave_is) + 40];
/*Ordem das árvores*/
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

 /********************FUNÇÕES DO MENU*********************/
 void cadastrar(Indice* iprimary, Indice* idev);

 int alterar(Indice iprimary);

 void buscar(Indice iprimary,Indice idev);

 void listar(Indice iprimary,Indice idev);

 /*******************************************************/

 /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
 int carregar_arquivo();

/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);

/* (Re)faz o índice de jogos  */
void criar_idev(Indice *idev) ;

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip);

/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
void *criar_no(char ip);

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);

Jogo recuperar_registro(int rrn);

/*
*	Caro aluno,
*	caso não queira trabalhar com void*, é permitido dividir as funções que utilizam
* em duas, sendo uma para cada índice...
* Um exemplo, a write_btree e read_btree ficariam como:
*
*	void write_btree_ip(node_Btree_ip *salvar, int rrn),  node_Btree_ip *read_btree_ip(int rrn),
*	void write_btree_is(node_Btree_is *salvar, int rrn) e node_Btree_is *read_btree_is(int rrn).
*/

/*------------------------
FUNÇOES DE BUSCA AUXILIARES
------------------------*/
int buscar_rrn(int rrn, char *chave);

void buscar_ip(int rrn, char *chave);

void buscar_is(int raiz_ip, int rrn, char *chave);

/*----------------------------
	FUNÇOES DE  AUXILIARES
----------------------------*/

//baseado nos pseudocodigos disponibilizados pelo professor
void insere_ip(Indice *iprimary, char *chave, int rrn);

int insere_ip_aux(int rrn, char *chave, Chave_ip *chave_promovida);

int divide_no_ip(int rrn, char *chave, int rrn_prom, int filho_dir, Chave_ip **chave_promovida);

void insere_is(Indice *idev, Chave_is chave);

int insere_is_aux(int rrn, Chave_is chave, Chave_is *chave_promovida);

int divide_no_is(int rrn, Chave_is chave, int filho_dir, Chave_is **chave_promovida);

void listar_ip(int rrn, int nivel);

void listar_is(int rrn);

//------------------------

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *idev, Jogo j);

/* Exibe o jogo */
int exibir_registro(int rrn);

int main(){

	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	char *p; //alteraç~ao na main devido a uso e nao declaraçao
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = 4 + (ordem_ip - 1) * 14 + ordem_ip * 3; /* = Deixarei esse cálculo pesado para você aluno... */
	tamanho_registro_is = 4 + (ordem_is - 1) * 111 + ordem_is * 3;/* = E esse também. */

	/* Índice primário */
	Indice iprimary ;
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Jogos */
	Indice idev;
	criar_idev(&idev);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo jogo */
			cadastrar(&iprimary, &idev);
			break;
		case 2: /* Alterar o desconto de um Jogo */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar um jogo */
			printf(INICIO_BUSCA);
			buscar(iprimary, idev);
			break;
		case 4: /* Listar todos os Jogos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, idev);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if(!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IP; *p!='\0'; p+=tamanho_registro_ip)
				{
					fwrite( p , 1 ,tamanho_registro_ip,stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if(!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IS; *p!='\0'; p+=tamanho_registro_is)
				{
					fwrite( p , 1 ,tamanho_registro_is,stdout);
					puts("");

				}
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* Exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
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

 void cadastrar(Indice* iprimary, Indice* idev){
 	char mes[3], registro[192];
 	Jogo novo;
	int i;

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

	//busca existencia do jogo, mensagem de falha em caso de pk existente
	if(iprimary->raiz > -1){
		if(buscar_rrn((iprimary->raiz), novo.pk) != -1){
			printf(ERRO_PK_REPETIDA, novo.pk);
			return;
		}
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

	//insere nos indices
	inserir_registro_indices(iprimary, idev, novo);
 }

 int alterar(Indice iprimary){
 	char pk[11], desconto_novo[192], *p;
 	Jogo j;
 	int rrn_buscado, valor_desconto;

 	scanf("%[^\n]%*c", pk); //recebe pk a ser buscada

 	rrn_buscado = buscar_rrn(iprimary.raiz, pk);

 	if(rrn_buscado > -1){
 		//rotina para receber e verificar novo desconto
		do{
			scanf("%[^\n]%*c", desconto_novo);
			sscanf(desconto_novo, "%d", &valor_desconto);
			if(strlen(desconto_novo) != 3 || valor_desconto > 100 || valor_desconto < 0)
				printf(CAMPO_INVALIDO);
		}while(strlen(desconto_novo) != 3 || valor_desconto > 100 || valor_desconto < 0);

		//rotina para inserir desconto
		p = ARQUIVO + rrn_buscado * 192;

		sscanf(p, "%[^@]@%[^@]@%[^@]", j.pk, j.nome, j.desenvolvedora);
		p += strlen(j.pk) + strlen(j.nome) + strlen(j.desenvolvedora) + 25;

		strncpy(p, desconto_novo, 3);
		return 1;
 	}else{
 		printf(REGISTRO_N_ENCONTRADO);
 		return 0;
 	}
 }

 void buscar(Indice iprimary,Indice idev){
 	int opcao;
 	char pk[TAM_PRIMARY_KEY], sk[TAM_STRING_INDICE], nome[TAM_NOME], desenvolvedora[TAM_DESENVOLVEDORA];

 	scanf("%d%*c", &opcao); //recebe qual o tipo de busca a ser realizada

 	switch(opcao){
 		case 1:
 			//recebe a pk a ser buscada e busca
 			scanf("%[^\n]%*c", pk);
 			printf(NOS_PERCORRIDOS_IP, pk);
 			buscar_ip(iprimary.raiz, pk);
 			break;

 		case 2:
 			//recebe a composicao das chaves e busca
 			scanf("%[^\n]%*c", desenvolvedora);
 			scanf("%[^\n]%*c", nome);
 			strcpy(sk, desenvolvedora);
 			strcat(sk, "$");
 			strcat(sk, nome);
 			printf(NOS_PERCORRIDOS_IS, sk);
 			buscar_is(iprimary.raiz,idev.raiz, sk);
 			break;
 	}
 }

int buscar_rrn(int rrn, char *chave){
	node_Btree_ip *no;
	int i, filho;

	no = (node_Btree_ip*)read_btree(rrn * tamanho_registro_ip, 'p');
	//busca a chave ou ate a mais proxima
	i = 0;
	while(i < no->num_chaves && strcmp(no->chave[i].pk, chave) < 0){
		i++;
	}

	//verifica se e campo valido e se e a chave buscada
	if(i < no->num_chaves && strcmp(no->chave[i].pk, chave) == 0){
		filho = no->chave[i].rrn;
		libera_no(no, 'p');
		return(filho);
	}

	if(no->folha == 'F'){ //se e folha e nao encontrou
		libera_no(no, 'p');
		return -1;
	}else{ //se ainda pode descer em niveis 
		filho = no->desc[i];
		libera_no(no, 'p');
		return buscar_rrn(filho, chave);
	}

}

void buscar_ip(int rrn, char *chave){
	node_Btree_ip *no;
	int i, prox;

	no = (node_Btree_ip*)read_btree(rrn * tamanho_registro_ip, 'p');
	//imprime todos chaves da pagina
	for(i = 0; i < no->num_chaves; i++){
		printf("%s", no->chave[i].pk);
		if(i < no->num_chaves - 1){
			printf(", ");
		}
	}
	printf("\n");

	//busca a chave ou ate a mais proxima
	i = 0;
	while(i < no->num_chaves && strcmp(no->chave[i].pk, chave) < 0){
		i++;
	}

	//verifica se e campo valido e se e a chave buscada
	if(i < no->num_chaves && strcmp(no->chave[i].pk, chave) == 0){
		printf("\n");
		exibir_registro(no->chave[i].rrn * 192);
		libera_no(no, 'p');
		return;
	}

	if(no->folha == 'F'){ //se e folha e nao encontrou
		printf("\n");
		printf(REGISTRO_N_ENCONTRADO);
		libera_no(no, 'p');
		return;
	}else{ //se ainda pode descer em niveis 
		prox = no->desc[i];
		libera_no(no, 'p');
		buscar_ip(prox, chave);
	}
}

void buscar_is(int raiz_ip, int rrn , char *chave){
	node_Btree_is *no;
	int i, prox;

	no = (node_Btree_is*)read_btree(rrn * tamanho_registro_is, 's');
	//imprime todos chaves da pagina
	for(i = 0; i < no->num_chaves; i++){
		printf("%s", no->chave[i].string);
		if(i < no->num_chaves - 1){
			printf(", ");
		}
	}
	printf("\n");

	//busca a chave ou ate a mais proxima
	i = 0;
	while(i < no->num_chaves && strcmp(no->chave[i].string, chave) < 0){
		i++;
	}

	//verifica se e campo valido e se e a chave buscada
	if(i < no->num_chaves && strcmp(no->chave[i].string, chave) == 0){
		printf("\n");
		prox = buscar_rrn(raiz_ip, no->chave[i].pk);
		exibir_registro(prox * 192);
		libera_no(no, 's');
		return;
	}

	if(no->folha == 'F'){ //se e folha e nao encontrou
		printf("\n");
		printf(REGISTRO_N_ENCONTRADO);
		libera_no(no, 's');
		return;
	}else{ //se ainda pode descer em niveis 
		prox = no->desc[i];
		libera_no(no, 's');
		buscar_is(raiz_ip, prox, chave);
	}
}

void listar(Indice iprimary,Indice idev){
	int opc;

	//receb opçao de listagem
	scanf("%d%*c", &opc);

	//escolhe a listagem
	switch(opc){
		case 1:
			listar_ip(iprimary.raiz, 1);
			break;
		case 2:
			listar_is(idev.raiz);
			break;
	}
}

void listar_ip(int rrn, int nivel){
	node_Btree_ip* node;
	int i;

	//leitura do no
	node = (node_Btree_ip*) read_btree(rrn * tamanho_registro_ip, 'p');

	//imprime as chaves
	printf("%d - ", nivel);
	for(i = 0; i < node->num_chaves; i++){
		printf("%s", node->chave[i].pk);
		if(i < node->num_chaves - 1){
			printf(", ");
		}
	}
	printf("\n");

	//se nao for folha, chama os filhos
	if(node->folha != 'F'){
		for(i = 0; i < node->num_chaves + 1; i++){
			listar_ip(node->desc[i], nivel + 1);
		}
	}

	//libera no
	libera_no(node ,'p');
}

void listar_is(int rrn){
	node_Btree_is *node;
	int i, j;
	char *jogo, *desenvolvedora;

	//leitura do no
	node = (node_Btree_is *) read_btree(rrn * tamanho_registro_is ,'s');

	//para nao folha
	if(node->folha == 'N'){
		for(i = 0; i <= node->num_chaves; i++){
			//chama para percorrer em ordem
			listar_is(node->desc[i]);
			//faz a impressao
			if(i < node->num_chaves){
				//recebe o que imprimir
				desenvolvedora = strtok(node->chave[i].string, "$");
				jogo = strtok(NULL, "\0");
				//imprime o que deve junto aos traços
				printf("%s", desenvolvedora);
				for(j = strlen(desenvolvedora); j < 50; j++){
					printf("-");
				}
				printf(" ");
				printf("%s", jogo);
				for(j = strlen(jogo); j < 50; j++){
					printf("-");
				}
				printf("\n");
			}
		}
	}else{
		//se for folha, apenas imprime
		for(i = 0; i < node->num_chaves; i++){
			//recebe o que imprimir
			desenvolvedora = strtok(node->chave[i].string, "$");
			jogo = strtok(NULL, "\0");
			//imprime o que deve junto aos traços
			printf("%s", desenvolvedora);
			for(j = strlen(desenvolvedora); j < 50; j++){
				printf("-");
			}
			printf(" ");
			printf("%s", jogo);
			for(j = strlen(jogo); j < 50; j++){
				printf("-");
			}
			printf("\n");
		}
	}

	libera_no(node,'s');
}

 /* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary){
	int i, nregistros_bkp = nregistros;
	char pk[11], *p = ARQUIVO;

	iprimary->raiz = -1; //declara raiz como "nula"
	nregistros = 1; //para inserir os rrn corretamente na arvore

	for(i = 0; i < nregistros_bkp; i++, nregistros++, p += 192){ //rotina de pegar chaves do arquivo e criar a arvore
		strncpy(pk, p, 10);
		pk[10] = '\0';
		insere_ip(iprimary, pk, i);
	}

	nregistros = nregistros_bkp;
}

/* (Re)faz o índice de jogos  */
void criar_idev(Indice *idev){
	int i;
	Jogo j;
	Chave_is reg_chave;

	idev->raiz = -1; //declara raiz como nula

	for(i = 0; i < nregistros; i++){ //rotina para insercao de chaves
		j = recuperar_registro(i * 192);
		//arruma chave para inserçao
		strcpy(reg_chave.pk, j.pk);
		strcpy(reg_chave.string, j.desenvolvedora);
		strcat(reg_chave.string, "$");
		strcat(reg_chave.string, j.nome);
		insere_is(idev, reg_chave);

	}
}

/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip){
	char reg_ip[tamanho_registro_ip + 1], reg_is[tamanho_registro_is + 1], *seek; //auxiliares para strings
	//mauxiliares para manipular no
	node_Btree_ip *nodep;
	node_Btree_is *nodes;
	int i, j;

	if(ip == 'p'){
		//casting para indice primario
		nodep = (node_Btree_ip *) salvar;
		//salva numero de chaves
		sprintf(reg_ip , "%03d", nodep->num_chaves);
		seek = reg_ip + 3; //anda na string registro ip
		//rotina para salvar chaves no registro ip
		for(i = 0; i < ordem_ip - 1; i++){
			if(nodep->chave[i].rrn > -1){
				//se a rrn e valida, salva
				strncpy(seek, nodep->chave[i].pk, 10); //salva chave
				seek += 10; //move ponteiro por tamanho da chave
				sprintf(seek, "%04d", nodep->chave[i].rrn);
				seek += 4; //move ponteiro por tamanho do rrn
			}else{
				//se nao insere #
				sprintf(seek, "##############");
				seek += 14; //move o ponteiro
			}
		}
		//copia o valor de folha
		seek[0] = nodep->folha;
		seek++;
		//coloca descendentes
		for(i = 0; i < ordem_ip; i++){
			if(nodep->desc[i] > -1){
				//se rrn e valido salva
				sprintf(seek, "%03d", nodep->desc[i]);
				seek += 3;
			}else{
				//se nao, insere ***
				strncpy(seek, "***",3);
				seek += 3;
			}
		}

		//coloca na "arvore"
		seek = ARQUIVO_IP + rrn;
		reg_ip[tamanho_registro_ip] = '\0';
		strncpy(seek, reg_ip, tamanho_registro_ip);
		//ARQUIVO_IP[nregistrosip * tamanho_registro_ip] = '\0';
	}else{
		//casting para indice secundario
		nodes = (node_Btree_is *) salvar;
		//salva numero de chaves
		sprintf(reg_is , "%03d", nodes->num_chaves);
		seek = reg_is + 3; //anda na string registro ip
		//rotina para salvar chaves no registro ip
		for(i = 0; i < ordem_is - 1; i++){
			if(strcmp(nodes->chave[i].pk, "!") != 0){
				//se a pk e valida, salva
				strncpy(seek, nodes->chave[i].pk, 10); //salva chave
				seek += 10; //move ponteiro por tamanho da chave primaria
				strcpy(seek, nodes->chave[i].string);
				for(j = strlen(nodes->chave[i].string); j < 101; j++){ //preenche restante com #
					seek[j] = '#';
				}
				seek += 101;//move para proxima insercao
			}else{
				//se nao insere #
				for(j = 0; j < 111; j++){ //preenche restante com #
					seek[j] = '#';
				}
				seek += 111; //move o ponteiro
			}
		}
		//copia o valor de folha
		seek[0] = nodes->folha;
		seek++;
		//coloca descendentes
		for(i = 0; i < ordem_is; i++){
			if(nodes->desc[i] > -1){
				//se rrn e valido salva
				sprintf(seek, "%03d", nodes->desc[i]);
				seek += 3;
			}else{
				//se nao, insere ***
				strncpy(seek, "***",3);
				seek += 3;
			}
		}

		//coloca na "arvore"
		seek = ARQUIVO_IS + rrn;
		reg_is[tamanho_registro_is] = '\0';
		strncpy(seek, reg_is, tamanho_registro_is);
		//ARQUIVO_IS[nregistrosis * tamanho_registro_is] = '\0';
	}
}

/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip){
	char *seekp, *seeks, nchaves[4], rrn_no[5];
	node_Btree_ip *nodep;
	node_Btree_is *nodes;
	int i;

	if(ip == 'p'){ //recupera para indice primario
		//cria no de indice primario
		nodep = (node_Btree_ip *) criar_no('p');
		//seek no "arquivo"
		seekp = ARQUIVO_IP + rrn;
		//recupera numero de chaves e anda no (texto)
		strncpy(nchaves, seekp, 3);
		nchaves[3] = '\0';
		seekp += 3;
		sscanf(nchaves, "%d", &nodep->num_chaves);
		//recupera chaves e rrns
		for(i = 0; i < nodep->num_chaves; i++){
			strncpy(nodep->chave[i].pk, seekp, 10);
			nodep->chave[i].pk[10] = '\0';
			seekp += 10; //anda para rrn
			strncpy(rrn_no, seekp, 4);
			rrn_no[4] = '\0';
			sscanf(rrn_no, "%d", &nodep->chave[i].rrn);
			seekp += 4; //anda para proxima chave
		}
		for(;i < ordem_ip - 1; i++){
			strcpy(nodep->chave[i].pk, "!");
			nodep->chave[i].rrn = -1;
			seekp += 14;
		}
		//atribui a variavel folha
		nodep->folha = seekp[0];
		seekp++;
		//recupera descendentes
		for(i = 0; i < ordem_ip; i++, seekp += 3){
			strncpy(rrn_no, seekp, 3);
			rrn_no[3] = '\0';
			if(strcmp(rrn_no, "***") == 0)
				nodep->desc[i] = -1;
			else
				sscanf(rrn_no, "%d", &nodep->desc[i]);
		}

		return(nodep);
	}else{ //recupera para indice secundario
		//cria no de indice secundario
		nodes = (node_Btree_is *) criar_no('s');
		//seek no arquvio
		seeks = ARQUIVO_IS + rrn;
		//recupera numero de chaves e anda no (texto)
		strncpy(nchaves, seeks, 3);
		nchaves[3] = '\0';
		seeks += 3;
		sscanf(nchaves, "%d", &nodes->num_chaves);
		//recupera chaves e strings
		for(i = 0; i < nodes->num_chaves; i++){
			strncpy(nodes->chave[i].pk, seeks, 10);
			nodes->chave[i].pk[10] = '\0';
			seeks += 10; //anda para string
			sscanf(seeks, "%[^#]", nodes->chave[i].string);
			seeks += 101; //pula para proxima chave
		}
		for(;i < ordem_is - 1; i++){
			strcpy(nodes->chave[i].pk, "!");
			strcpy(nodes->chave[i].string, "!");
			seeks += 111;
		}
		//atribui a variavel folha
		nodes->folha = seeks[0];
		seeks++;
		//recupera descendentes
		for(i = 0; i < ordem_is; i++, seeks += 3){
			strncpy(rrn_no, seeks, 3);
			rrn_no[3] = '\0';
			if(strcmp(rrn_no, "***") == 0)
				nodes->desc[i] = -1;
			else
				sscanf(rrn_no, "%d", &nodes->desc[i]);
		}

		return(nodes);
	}
}

/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
void *criar_no(char ip){
	int i;
	node_Btree_ip *nodep;
	node_Btree_is *nodes;

	//verifica se deve criar e retornar no primario ou secundario
	if(ip == 'p'){
		//aloca no e os vetores dinamicos de acordo com a ordem
		nodep = (node_Btree_ip *) malloc (sizeof(node_Btree_ip));
		nodep->num_chaves = 0;
		nodep->chave = (Chave_ip *) malloc ((ordem_ip - 1) * sizeof(Chave_ip));
		nodep->desc = (int *) malloc (ordem_ip * sizeof(int));
		//inicializa tudo como -1 e key com !
		for(i = 0; i < ordem_ip; i++){
			if(i < ordem_ip - 1){
				strcpy(nodep->chave[i].pk, "!");
				nodep->chave[i].rrn = -1;
			}
			nodep->desc[i] = -1;
		}
		return(nodep);
	}else{
		//aloca no e os vetores dinamicos de acordo com a ordem
		nodes = (node_Btree_is *) malloc (sizeof(node_Btree_is));
		nodes->num_chaves = 0;
		nodes->chave = (Chave_is *) malloc ((ordem_is - 1) * sizeof(Chave_is));
		nodes->desc = (int *) malloc (ordem_is * sizeof(int));
		//inicializa tudo como -1 e key e string com !
		for(i = 0; i < ordem_is; i++){
			if(i < ordem_is - 1){
				strcpy(nodes->chave[i].string, "!");
				strcpy(nodes->chave[i].pk, "!");
			}
			nodes->desc[i] = -1;
		}
		return(nodes);
	}
}

/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip){
	node_Btree_ip *nodep;
	node_Btree_is *nodes;

	//libera de acordo com o tipo de indice
	if(ip == 'p'){
		nodep = (node_Btree_ip *) node;
		free(nodep->chave);
		free(nodep->desc);
		free(nodep);
	}else{
		nodes = (node_Btree_is *) node;
		free(nodes->chave);
		free(nodes->desc);
		free(nodes);
	}
}

/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *idev, Jogo j){
	Chave_is reg_chave;

	//chama para inserir no primario
	insere_ip(iprimary, j.pk, nregistros - 1);

	//arruma chave para inserçao
	strcpy(reg_chave.pk, j.pk);
	strcpy(reg_chave.string, j.desenvolvedora);
	strcat(reg_chave.string, "$");
	strcat(reg_chave.string, j.nome);
	insere_is(idev, reg_chave);
}

void insere_ip(Indice *iprimary, char *chave, int rrn){
	node_Btree_ip *nodep;
	Chave_ip chave_promovida;
	int filho_dir;

	chave_promovida.rrn = -1;

	//caso arvore vazia
	if(iprimary->raiz == -1){
		//cria no
 		nodep = (node_Btree_ip *) criar_no('p');
 		//atribui os valores na raiz
 		nodep->folha = 'F';
 		nodep->num_chaves++;
 		strcpy(nodep->chave[0].pk, chave);
 		nodep->chave[0].rrn = rrn;
 		//coloca a raiz e o rrn, incrementa
 		iprimary->raiz = 0;
 		nregistrosip++;
 		write_btree(nodep, 0, 'p');
 		//libera no
 		libera_no(nodep, 'p');
	}else{
		filho_dir = insere_ip_aux(iprimary->raiz, chave, &chave_promovida);
	}

	if(chave_promovida.rrn != -1){
		//cria no
 		nodep = (node_Btree_ip *) criar_no('p');
 		//coloca os valores no novo no
 		nodep->folha = 'N';
 		nodep->num_chaves = 1;
 		strcpy(nodep->chave[0].pk, chave_promovida.pk);
 		nodep->chave[0].rrn = chave_promovida.rrn;

 		//atualiza os descendentes
 		nodep->desc[0] = iprimary->raiz;
 		nodep->desc[1] = filho_dir;

 		//atualiza a raiz
 		iprimary->raiz = nregistrosip;
 		nregistrosip++;

		//escreve o novo no e libera
		write_btree(nodep, (nregistrosip - 1) * tamanho_registro_ip, 'p');
		libera_no(nodep, 'p');
	}
}

int insere_ip_aux(int rrn, char *chave, Chave_ip *chave_promovida){
	node_Btree_ip *nodep;
	int i, filho_dir = -1;
	Chave_ip promocao;

	nodep = (node_Btree_ip *) read_btree(rrn * tamanho_registro_ip, 'p');

	if(nodep->folha == 'F'){
		if(nodep->num_chaves < ordem_ip - 1){
			//percorre a folha para saber onde inderir
			i = nodep->num_chaves - 1;
			while(i >= 0 && strcmp(nodep->chave[i].pk, chave) > 0){
				nodep->chave[i + 1] = nodep->chave[i];
				i--;
			}
			//insere na folha
			strcpy(nodep->chave[i + 1].pk, chave);
			nodep->chave[i + 1].rrn = (nregistros - 1);
			nodep->num_chaves++;
			write_btree(nodep, rrn * tamanho_registro_ip, 'p');
			libera_no(nodep, 'p');
			return -1;
		}else{
			//faz a divisao
			libera_no(nodep, 'p');
			return divide_no_ip(rrn, chave, nregistros - 1,filho_dir, &chave_promovida);
		}
	}else{
		//percorre a folha para saber onde inserir
		i = nodep->num_chaves - 1;
		while(i >= 0 && strcmp(nodep->chave[i].pk, chave) > 0){
			i--;
		}
		i++;

		//insere novamente
		filho_dir = insere_ip_aux(nodep->desc[i], chave, chave_promovida);

		//verifica descendencias
		if(chave_promovida->rrn > -1){
			promocao = *chave_promovida;

			if(nodep->num_chaves < ordem_ip - 1){
				i = nodep->num_chaves - 1;
				while(i >= 0 && strcmp(nodep->chave[i].pk, chave) > 0){
					nodep->chave[i + 1] = nodep->chave[i];
					nodep->desc[i + 2] = nodep->desc[i + 1];
					i--;
				}
				nodep->chave[i + 1] = promocao;
				nodep->desc[i + 2] = filho_dir;
				nodep->num_chaves++;

				chave_promovida->rrn = -1;
				write_btree(nodep, rrn * tamanho_registro_ip, 'p');
				libera_no(nodep, 'p');
				return -1;
			}else{
				libera_no(nodep, 'p');
				return divide_no_ip(rrn, chave_promovida->pk, chave_promovida->rrn, filho_dir, &chave_promovida);
			}
		}else{
			libera_no(nodep, 'p');
			return -1;
		}
	}
}

int divide_no_ip(int rrn, char *chave, int rrn_prom, int filho_dir, Chave_ip **chave_promovida){
	int chave_alocada = 0, i, j;
	node_Btree_ip *nodep, *novo;

	nodep = (node_Btree_ip *) read_btree(rrn * tamanho_registro_ip, 'p');
	novo = (node_Btree_ip *) criar_no('p');

	//copia e altera status da nova folha
	i = nodep->num_chaves - 1;
	novo->folha = nodep->folha;
	novo->num_chaves = (ordem_ip - 1) / 2;

	//copia e tenta inserir nova chave
	for(j = novo->num_chaves - 1; j >= 0; j--){
		if((!chave_alocada) && strcmp(nodep->chave[i].pk, chave) < 0){
			strcpy(novo->chave[j].pk, chave);
			novo->chave[j].rrn = rrn_prom;
			novo->desc[j + 1] = filho_dir;
			chave_alocada = 1;
		}else{
			novo->chave[j] = nodep->chave[i];
			novo->desc[j+1] = nodep->desc[i+1];
			i--;
		}
	}

	//se nao inseriu ainda
	if(!chave_alocada){
		while(i >= 0 && strcmp(nodep->chave[i].pk, chave) > 0){
			nodep->chave[i + 1] = nodep->chave[i];
			nodep->desc[i + 2] = nodep->desc[i + 1];
			i--;
		}
		strcpy(nodep->chave[i + 1].pk, chave);
		nodep->chave[i + 1].rrn = rrn_prom;
		nodep->desc[i + 2] = filho_dir;
	}

	**chave_promovida = nodep->chave[ordem_ip/2];
	novo->desc[0] = nodep->desc[ordem_ip/2 + 1];
	nodep->num_chaves = ordem_ip/2;

	//salva novos registros
	write_btree(novo, nregistrosip * tamanho_registro_ip, 'p');
	nregistrosip++;

	//arruma no nodep
	for(i = ordem_ip / 2; i < ordem_ip - 1; i++){
		nodep->chave[i].rrn = -1;
		strcpy(nodep->chave[i].pk, "!");
		nodep->desc[i + 1] = -1;
	}

	write_btree(nodep, rrn * tamanho_registro_ip, 'p');

	libera_no(nodep, 'p');
	libera_no(novo, 'p');

	//retorna rrn do novo registro
	return (nregistrosip - 1);
}

void insere_is(Indice *idev, Chave_is chave){
	node_Btree_is *nodes;
	Chave_is chave_promovida;
	int filho_dir;

	strcpy(chave_promovida.pk, "!");

	//caso arvore vazia
	if(idev->raiz == -1){
		//cria no
 		nodes = (node_Btree_is *) criar_no('s');
 		//atribui os valores na raiz
 		nodes->folha = 'F';
 		nodes->num_chaves++;
 		strcpy(nodes->chave[0].pk, chave.pk);
 		strcpy(nodes->chave[0].string, chave.string);
 		//coloca a raiz e o rrn, incrementa
 		idev->raiz = 0;
 		nregistrosis++;
 		write_btree(nodes, 0, 's');
 		//libera no
 		libera_no(nodes, 's');
	}else{
		filho_dir = insere_is_aux(idev->raiz, chave, &chave_promovida);
	}

	if(strcmp(chave_promovida.pk, "!") != 0){
		//cria no
 		nodes = (node_Btree_is *) criar_no('s');
 		//coloca os valores no novo no
 		nodes->folha = 'N';
 		nodes->num_chaves = 1;
 		strcpy(nodes->chave[0].pk, chave_promovida.pk);
 		strcpy(nodes->chave[0].string, chave_promovida.string);

 		//atualiza os descendentes
 		nodes->desc[0] = idev->raiz;
 		nodes->desc[1] = filho_dir;

 		//atualiza a raiz
 		idev->raiz = nregistrosis;
 		nregistrosis++;

		//escreve o novo no e libera
		write_btree(nodes, (nregistrosis - 1) * tamanho_registro_is, 's');
		libera_no(nodes, 's');
	}
}

int insere_is_aux(int rrn, Chave_is chave, Chave_is *chave_promovida){
	node_Btree_is *nodes;
	int i, filho_dir = -1;
	Chave_is promocao;

	nodes = (node_Btree_is *) read_btree(rrn * tamanho_registro_is, 's');

	if(nodes->folha == 'F'){
		if(nodes->num_chaves < ordem_is - 1){
			//percorre a folha para saber onde inderir
			i = nodes->num_chaves - 1;
			while(i >= 0 && strcmp(nodes->chave[i].string, chave.string) > 0){
				nodes->chave[i + 1] = nodes->chave[i];
				i--;
			}
			//insere na folha
			strcpy(nodes->chave[i + 1].pk, chave.pk);
			strcpy(nodes->chave[i + 1].string, chave.string);
			nodes->num_chaves++;
			write_btree(nodes, rrn * tamanho_registro_is, 's');
			libera_no(nodes, 's');
			return -1;
		}else{
			//faz a divisao
			libera_no(nodes, 's');
			return divide_no_is(rrn, chave, filho_dir, &chave_promovida);
		}
	}else{
		//percorre a folha para saber onde inderir
		i = nodes->num_chaves - 1;
		while(i >= 0 && strcmp(nodes->chave[i].string, chave.string) > 0){
			i--;
		}
		i++;

		//insere novamente
		filho_dir = insere_is_aux(nodes->desc[i], chave, chave_promovida);

		//verifica descendencias
		if(strcmp(chave_promovida->pk, "!") != 0){
			promocao = *chave_promovida;

			if(nodes->num_chaves < ordem_is - 1){
				i = nodes->num_chaves - 1;
				while(i >= 0 && strcmp(nodes->chave[i].string, promocao.string) > 0){
					nodes->chave[i + 1] = nodes->chave[i];
					nodes->desc[i + 2] = nodes->desc[i + 1];
					i--;
				}
				nodes->chave[i + 1] = promocao;
				nodes->desc[i + 2] = filho_dir;
				nodes->num_chaves++;

				strcpy(chave_promovida->pk , "!");
				write_btree(nodes, rrn * tamanho_registro_is, 's');
				libera_no(nodes, 's');
				return -1;
			}else{
				libera_no(nodes, 's');
				return divide_no_is(rrn, *chave_promovida, filho_dir, &chave_promovida);
			}
		}else{
			libera_no(nodes, 's');
			return -1;
		}
	}
}

int divide_no_is(int rrn, Chave_is chave, int filho_dir, Chave_is **chave_promovida){
	int chave_alocada = 0, i, j;
	node_Btree_is *nodes, *novo;

	nodes = (node_Btree_is *) read_btree(rrn * tamanho_registro_is, 's');
	novo = (node_Btree_is *) criar_no('s');

	//copia e altera status da nova folha
	i = nodes->num_chaves - 1;
	novo->folha = nodes->folha;
	novo->num_chaves = (ordem_is - 1) / 2;

	//copia e tenta inserir nova chave
	for(j = novo->num_chaves - 1; j >= 0; j--){
		if((!chave_alocada) && strcmp(nodes->chave[i].string, chave.string) < 0){
			strcpy(novo->chave[j].pk, chave.pk);
			strcpy(novo->chave[j].string, chave.string);
			novo->desc[j + 1] = filho_dir;
			chave_alocada = 1;
		}else{
			novo->chave[j] = nodes->chave[i];
			novo->desc[j+1] = nodes->desc[i+1];
			i--;
		}
	}

	//se nao inseriu ainda
	if(!chave_alocada){
		while(i >= 0 && strcmp(nodes->chave[i].string, chave.string) > 0){
			nodes->chave[i + 1] = nodes->chave[i];
			nodes->desc[i + 2] = nodes->desc[i + 1];
			i--;
		}
		strcpy(nodes->chave[i + 1].pk, chave.pk);
		strcpy(nodes->chave[i + 1].string, chave.string);
		nodes->desc[i + 2] = filho_dir;
	}

	**chave_promovida = nodes->chave[ordem_is/2];
	novo->desc[0] = nodes->desc[ordem_is/2 + 1];
	nodes->num_chaves = ordem_is/2;

	//salva novos registros
	write_btree(novo, nregistrosis * tamanho_registro_is, 's');
	nregistrosis++;

	//arruma no nodes
	for(i = ordem_is / 2; i < ordem_is - 1; i++){
		strcpy(nodes->chave[i].pk, "!");
		strcpy(nodes->chave[i].string, "!");
		nodes->desc[i + 1] = -1;
	}

	write_btree(nodes, rrn * tamanho_registro_is, 's');

	libera_no(nodes, 's');
	libera_no(novo, 's');

	//retorna rrn do novo registro
	return (nregistrosis - 1);
}

//Recupera o jogo dado um rrn
Jogo recuperar_registro(int rrn){
	Jogo jogo_busca; //jogo a ser reornado
	char *comeco_registro = ARQUIVO + rrn; //auxiliar para compor a chave primaria e localizar o registro correto

	//rotina de leitura do registro
	sscanf(comeco_registro, "%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]", jogo_busca.pk, jogo_busca.nome,
		   jogo_busca.desenvolvedora, jogo_busca.data, jogo_busca.classificacao, 
		   jogo_busca.preco, jogo_busca.desconto, jogo_busca.categoria);

	return(jogo_busca);
}