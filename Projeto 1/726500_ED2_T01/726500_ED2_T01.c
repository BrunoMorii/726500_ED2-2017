/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01 - Indexação
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
#define TAM_DESENVOLVEDORA	51
#define TAM_DATA			11
#define TAM_CLASSIFICACAO 	3
#define TAM_PRECO 			8
#define TAM_DESCONTO 		4
#define TAM_CATEGORIA 		51


#define TAM_REGISTRO 		192
#define MAX_REGISTROS 		1000
#define MAX_CATEGORIAS 		10
#define TAM_ARQUIVO 		(MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 			"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 	"Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO 	"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 			"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 		"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 			"Arquivo vazio!"
#define INICIO_BUSCA 		 	"**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM  		"**********************LISTAR**********************\n"
#define INICIO_ALTERACAO 		"**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO  		"**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO  		"**********************ARQUIVO*********************\n"
#define SUCESSO  				"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					"FALHA AO REALIZAR OPERACAO!\n"

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

/* Registros dos Índices*/

/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;
} Ip;

/*	Você deve optar por utilizar índices secundários simples, ou listas invertidas.
 *  Seguem abaixo sugestões para suas EDs


*Struct para índice secundário comum:
typedef struct secundary_index{
  char pk[TAM_PRIMARY_KEY];
  void *chave; <- Trocar o tipo conforme item desejado.
} Is;*/


//Lista ligada simples para a lista invertida:
typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;

//struct para a lista invertida:
typedef struct reverse_index{
  char *chave; //<- Trocar o tipo conforme item desejado.
  ll* lista;
} Ir;

/* Variavel global que simula o arquivo de dados */
char ARQUIVO[TAM_ARQUIVO];

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 /* Exibe o menu de opções */
void print_menu();

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();

/* Exibe o jogo, utilize com_desconto = 1 apenas para listagem por preço */
int exibir_registro(int rrn, char com_desconto);

/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Jogo */
Jogo recuperar_registro(int rrn);

/*Dada uma certa chave, retorna o rrn dela consultando o ip, senao retorna -1*/
int recuperar_rrn(Ip* iprimary, const char* pk, int nregistros);

/* (Re)faz o índice primário */
void criar_iprimary(Ip *indice_primario, int nregistros);

/* Salva os dados da entrada para a struct jogo*/
void ler_entrada(char* registro, Jogo *novo);


/*------------------------------------------------------
 				FUNÇOES QUE EU IMPLEMENTEI
------------------------------------------------------*/

/*------------------------------
Funcoes de manipulacao de dados
------------------------------*/

//inserçao de novo registro
int insere_registro(char novo_registro[], int nregistros);

//insere nova chave (nao repetida) em iprimary
void insere_pk(Ip *iprimary, char pk[], int pos);

//inserçao de chave nos indices secundarios
int insere_sk(Ir **isecondary, char *sk, char *pk, int nindices);

//insercao de chave primaria na lista invertida
void insere_Ir(Ir *indice, char *pk);

//listar por chave secundaria
void listagem_sk(Ir sk, Ip *iprimary, int nregistros, int desconto);

//listar por dupla chave secundaria
void listagem_2sk(Ir sk, Ir sk2, Ip *iprimary, int nregistros);

//altera campo desconto no arquivo, retorna 1 em caso de sucesso, senao 0
int alterar(Ip *iprimary, int nregistros);

//remove registro no arquivo e rrn = -1 
//retorna 1 em caso de sucesso, senao 0
int remover(Ip *iprimary, int nregistros);

//limpeza do arquivo e retorna quantidade de registros removidos
int limpar_arquivo();


//-------funcoes para criar chaves secundarias

//por titulo
int criar_igame(Ir **igame, Ip *iprimary, int nregistros);

//por desenvolvedora
int criar_idev(Ir **idev, Ip *iprimary, int nregistros);

//por categoria
int criar_icat(Ir **icat, Ip *iprimary, int nregistros);

//por preco com desconto aplicado
int criar_iprice(Ir **iprice, Ip *iprimary, int nregistros);

//remove os nos das listas invertidas
void libera_sk(Ir **isecondary, int nindices);

/*----------------
Funçes auxiliares
----------------*/

//comparaçao para PK
int compare_pri(const void *chave, const void *elem);

//comparacao para SK
int compare_sec(const void *chave, const void *elem);

//Ordenaçao para pk
void ordena_pk(Ip *iprimary, int nregistros);

//Ordenaçao para sk
void ordena_sk(Ir *isecondary, int nindices);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * ======================================================================= */
int main(){
  /* Arquivo */


	/* --------AVISOS:
		.tal codigo e comentado sem uso de acentos pois o programa usado (Sublime) em conjunto ao sistema operacional Xubutu
			possui conflitos com esse tipo de entrada, foi tentado resolver com formatacao do documento e outro metodos e 
			nenhum resultado positivo
		.comentarios normalmente explicam segmentos de codigo apos sua apariçao
		.a implementaçao das funcoes que tem seus prototipos acima foram separadas em quatro seçoes (sem contar as ja implementadas):
			- ROTINAS DE REFAZER INDICE POR ARQUIVO
			- ROTINAS DE BUSCA E AUXILIARES
			- ROTINAS DE ALTERAÇAO E AUXILIARES
			- ROTINAS DE REMOCAO E AUXILIARES
		.sobre os calculos envolvendo o iprice, necessario para desviar dos problemas com arredondamento, quanto a isso:
			- tudo que envolve o calculo com desconto utili as duas linhas de calculo com casting
			- conhecimento passado pelo grupo da sala para evitar os erros de arredondamento
			- avisos direto e agradecimento especial pelo aviso:
				->Caroline Aparecida - RA: 726506
				->Breno Vinicius - RA: 726498
		. Agradecimento especial ao Nathan Eloy - RA 726575:
			.lembrar verificar se o casting na funcao compare estava correto (erro bobo que impediu funcionamento do codigo)
	*/

	//i e iterador para loops, nx sao quantidades de cada tipo de chave
	int carregarArquivo = 0, nregistros = 0, ncat = 0, ngame = 0, ndev = 0, nprice = 0, opcao2 = 0, rrn_busca = 0, i = 0, flag_remocao = 0;
	//auxiliares de busca e insercao de jogos
	Jogo novo_jogo, jogo_busca;
	//strings auxiliares para busca, preco com desconto e tokkens
	char novo_registro[TAM_REGISTRO], pk_busca[TAM_PRIMARY_KEY], sk_busca[TAM_NOME], sk2_busca[TAM_NOME], *cat, price[TAM_PRECO];
	//auxiliares de busca de lista ligada
	Ir *busca_secundaria, *busca_secundaria2;
	//auxiliar de calculo do preco com desconto
	float preco, desconto;
	
	scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	/* Índice primário */
	Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
  	if (!iprimary) {
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	criar_iprimary(iprimary, nregistros);

	/* Alocar e criar índices secundários */
	Ir *igame = (Ir *) malloc (sizeof(Ir));
	if(!igame){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}

	ngame = criar_igame(&igame, iprimary, nregistros);

	Ir *idev = (Ir *) malloc (sizeof(Ir));
	if(!idev){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	ndev = criar_idev(&idev, iprimary, nregistros);

	Ir *icat = (Ir *) malloc (sizeof(Ir));
	if(!icat){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	ncat = criar_icat(&icat, iprimary, nregistros);

	Ir *iprice = (Ir *) malloc (sizeof(Ir));
	if(!iprice){
		perror(MEMORIA_INSUFICIENTE);
		exit(1);
	}
	nprice = criar_iprice(&iprice, iprimary, nregistros);


    /* Execução do programa */
	int opcao = 0;
	while(1) {
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1:
			/* inserir */
			//recebe o novo jogo a ser inserido
			ler_entrada(novo_registro, &novo_jogo);

			/*busca se o jogo ja existe usando 
			se existe, imprime mensagem
			senao, insere jogo e as respectivas chaves*/
			if(recuperar_rrn(iprimary ,novo_jogo.pk, nregistros) >= 0){
				printf(ERRO_PK_REPETIDA, novo_jogo.pk);
			}else{
				/*confere se nao atingiu o num max de registros
					em caso afirmativo, mensagem de erro
					senao, insere */
				if(nregistros == 1000){
					printf(FALHA);
				}else{
					//insercao do novo registro e suas chaves nos indices
					nregistros = insere_registro(novo_registro, nregistros);

					//atualizacao do primario
					insere_pk(iprimary, novo_jogo.pk, nregistros - 1);

					//atualizacao de indices secundarios

					//nome e desenvolvedora
					ngame = insere_sk(&igame, novo_jogo.nome, novo_jogo.pk, ngame);
					ndev = insere_sk(&idev, novo_jogo.desenvolvedora, novo_jogo.pk, ndev);

					//rotina para categorias
					cat = strtok(novo_jogo.categoria, "|\0");
					while(cat != NULL){
						ncat = insere_sk(&icat, cat, novo_jogo.pk, ncat);
						cat = strtok(NULL, "|\0");
					}

					//rotina para preço com desconto
					sscanf(novo_jogo.preco, "%f", &preco);
					sscanf(novo_jogo.desconto, "%f", &desconto);

					preco = preco * (100 - desconto);
					preco = (int)preco / (float)100;
					sprintf(price, "%07.2f", preco);
					nprice = insere_sk(&iprice, price , novo_jogo.pk, nprice);
				}
			}

			break;
		case 2:
			/* alterar */
			printf(INICIO_ALTERACAO);

			//se alterar retornar 1, houve alteraçao
			//caso contrario, houve falha
			if(alterar(iprimary, nregistros)){
				printf(SUCESSO);
				libera_sk(&iprice, nprice);
				nprice = criar_iprice(&iprice, iprimary, nregistros);
			}else{
				printf(FALHA);
			}
			break;
		case 3:
			/* excluir */
			printf(INICIO_EXCLUSAO);

			//se excluir retornar 1, houve alteraçao
			//caso contrario, houve falha
			if(remover(iprimary, nregistros)){
				printf(SUCESSO);
				//flag que indica se houve uma remocao
				flag_remocao = 1;
			}else{
				printf(FALHA);
			}
			break;
		case 4:
			/* buscar */
			printf(INICIO_BUSCA);

			//recebe parametros de busca
			scanf("%d%*c", &opcao2);
			//opçoes de busca
			switch(opcao2){
				case 1:
					//por codigo (pk)
					scanf("%[^\n]%*c", pk_busca);
					//busca rrn
					rrn_busca = recuperar_rrn(iprimary, pk_busca, nregistros);
					/*se nao encontrado, mensagem de erro
						caso contrario, imprime dados */
					if(rrn_busca == -1){
						printf(REGISTRO_N_ENCONTRADO);
					}else{
						//exibe sem desconto
						exibir_registro(rrn_busca, 0);
					}
					break;

				case 2:
					//por nome de jogo
					scanf("%[^\n]%*c", sk_busca);
					//busca pelo nome
					busca_secundaria = (Ir *) bsearch(sk_busca, igame, ngame, sizeof(Ir), compare_sec);
					/*se nao encontrado, mensagem de erro
						caso contrario, imprime dados */
					if(!busca_secundaria){
						printf(REGISTRO_N_ENCONTRADO);
					}else{
						listagem_sk(*busca_secundaria, iprimary, nregistros, 0);
					}
					break;

				case 3:
					//por desenvolvedora e categoria
					scanf("%[^\n]%*c", sk_busca);
					scanf("%[^\n]%*c", sk2_busca);

					busca_secundaria = (Ir *) bsearch(sk_busca, idev, ndev, sizeof(Ir), compare_sec);
					busca_secundaria2 = (Ir *) bsearch(sk2_busca, icat, ncat, sizeof(Ir), compare_sec);
					/*se nao encontrado, mensagem de erro
						caso contrario, imprime dados */
					if(busca_secundaria != NULL && busca_secundaria2 != NULL){
						listagem_2sk(*busca_secundaria, *busca_secundaria2, iprimary, nregistros);
					}else{
						printf(REGISTRO_N_ENCONTRADO);
					}

					break;
			}
			break;
		case 5:
			/* listar */
			printf(INICIO_LISTAGEM);

			//se aqruivo vazio, imprime mensagem e sai
			if(nregistros == 0){
				printf(REGISTRO_N_ENCONTRADO);
			}

			//recebe parametros de busca
			scanf("%d%*c", &opcao2);
			//opçoes de busca
			switch(opcao2){
				case 1:
					//por codigo (pk)					
					//apenas precisa passar por todo vetor iprimary
					//com o rrn de cada um
					//e sem desconto
					for(i = 0; i < nregistros; i++){
						if((exibir_registro(iprimary[i].rrn, 0)) && i < nregistros - 1){
							printf("\n");
						}
					}
					
					break;

				//por chave secundaria necessita de funçao auxiliar

				case 2:
					//por categoria
					//recebe categoria desejada 
					scanf("%[^\n]%*c", sk_busca);
					//busca pela categoria
					busca_secundaria = (Ir *) bsearch(sk_busca, icat, ncat, sizeof(Ir), compare_sec);
					/*se nao encontrado, mensagem de erro
						caso contrario, lista jogos */
					if(!busca_secundaria){
						printf(REGISTRO_N_ENCONTRADO);
					}else{
						listagem_sk(*busca_secundaria, iprimary, nregistros, 0);
					}

					break;

				case 3:
					//por desenvolvedora 
					for(i = 0; i < ndev; i++){
						listagem_sk(idev[i], iprimary, nregistros, 0);
						if(i < ndev - 1  && idev[i].lista != NULL)
							printf("\n");
					}

					break;

				case 4:
					//por desconto aplicado
					for(i = 0; i < nprice; i++){
						listagem_sk(iprice[i], iprimary, nregistros, 1);
						if(i < nprice - 1 && iprice[i].lista != NULL)
							printf("\n");

					}

					break;
			}

			break;
		case 6:
			/* liberar espaco */
			//executa apenas quando houve alguma remocao, indicada pela flag
			//menos execuçoes de um processo lento
			if(flag_remocao){
				//limpa as listas invertidas
				libera_sk(&igame, ngame);
				free(igame);
				libera_sk(&idev, ndev);
				free(idev);
				libera_sk(&icat, ncat);
				free(icat);
				libera_sk(&iprice, nprice);
				free(iprice);
				//retira lixos do arquvio
				nregistros -= limpar_arquivo();
				//refaz os indices


				criar_iprimary(iprimary, nregistros);
				
				// Alocar e criar índices secundários 
				igame = (Ir *) malloc (sizeof(Ir));
				ngame = criar_igame(&igame, iprimary, nregistros);
				idev = (Ir *) malloc (sizeof(Ir));
				ndev = criar_idev(&idev, iprimary, nregistros);
				icat = (Ir *) malloc (sizeof(Ir));
				ncat = criar_icat(&icat, iprimary, nregistros);
				iprice = (Ir *) malloc (sizeof(Ir));
				nprice = criar_iprice(&iprice, iprimary, nregistros);

				flag_remocao = 0;
			}
		break;
		case 7:
			/* imprimir arquivo de dados*/
			printf(INICIO_ARQUIVO);
			if(nregistros == 0)
				printf(ARQUIVO_VAZIO);
			else
				printf("%s\n", ARQUIVO);
			break;
		case 8:
    		/* Liberar memoria e finalizar */
			free(iprimary);
			libera_sk(&igame, ngame);
			free(igame);
			libera_sk(&idev, ndev);
			free(idev);				
			libera_sk(&icat, ncat);
			free(icat);
			libera_sk(&iprice, nprice);
			free(iprice);
			return (0);
			break;
		default:
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return (0);
}



/* Exibe o jogo, utilize com_desconto = 1 para listagem por preço */
int exibir_registro(int rrn, char com_desconto) {

	if(rrn < 0)
		return (0);

	float preco;
	short int desconto;
	Jogo j = recuperar_registro(rrn);
	char *cat, categorias[TAM_CATEGORIA];
	
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.desenvolvedora);
	printf("%s\n", j.data);

	if(com_desconto){
		sscanf(j.preco, "%f", &preco);
		sscanf(j.desconto, "%hd", &desconto);
		preco = preco * (100 - desconto);
		preco = (int) preco / (float) 100;
		printf("%07.2f\n", preco);
	}
	else{
		printf("%s\n", j.preco);
		printf("%s\n", j.desconto);
	}

	strncpy(categorias, j.categoria, strlen(j.categoria));
	categorias[strlen(j.categoria)] = '\0';
  	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	
	printf("\n");
	return (1);
}

/* Imprime o menu para o usuário */
void print_menu(){
		printf("\n 1. Cadastro");
		printf("\n 2. Alteração");
		printf("\n 3. Remoção");
		printf("\n 4. Busca");
		printf("\n 5. Listagem");
		printf("\n 6. Liberar espaço");
		printf("\n 7. Finalizar");
		printf("\n\nEntre com uma das opções acima: ");
}

/* Carrega o arquivo de dados */
int carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
	return (strlen(ARQUIVO) / TAM_REGISTRO);
}

/*-----------------------------------------
			O QUE EU IMPLEMENTEI:
-----------------------------------------*/


/*-------------------------------------
 ROTINAS DE REFAZER INDICE POR ARQUIVO
------------------------------------*/
/* (Re)faz o índice primário */
void criar_iprimary(Ip *indice_primario, int nregistros){
	int i;
	Jogo jogo_recuperado;

	//roda todo arquivo recebndo jogos e indicando rrns
	for(i = 0; i < nregistros; i++){
		//recupera o jogo por rrn no arquivo
		jogo_recuperado = recuperar_registro(i * 192);
		//insere na lista de chaves primarias
		insere_pk(indice_primario, jogo_recuperado.pk, i);
	}
}

/* (Re)faz o índices secundarios */

//por titulo
int criar_igame(Ir **igame, Ip *iprimary, int nregistros){
	int ngame = 0, i;
	Jogo jogo_recuperado;

	//roda todo iprimary recebndo chaves primarias e indicando rrns e suas sk
	for(i = 0; i < nregistros; i++){
		//recupera o jogo por rrn no arquivo
		jogo_recuperado = recuperar_registro(iprimary[i].rrn);
		//insere na lista de chaves secundarias
		ngame = insere_sk(igame, jogo_recuperado.nome, jogo_recuperado.pk, ngame);
	}

	return(ngame);
}

//por desenvolvedora
int criar_idev(Ir **idev, Ip *iprimary, int nregistros){
	int ndev = 0, i;
	Jogo jogo_recuperado;

	//roda todo iprimary recebndo chaves primarias e indicando rrns e suas sk
	for(i = 0; i < nregistros; i++){
		//recupera o jogo por rrn no arquivo
		jogo_recuperado = recuperar_registro(iprimary[i].rrn);
		//insere na lista de chaves secundarias
		ndev = insere_sk(idev, jogo_recuperado.desenvolvedora, jogo_recuperado.pk, ndev);
	}

	return(ndev);
}

//por categoria
int criar_icat(Ir **icat, Ip *iprimary, int nregistros){
	int ncat = 0, i;
	Jogo jogo_recuperado;
	//auxiliar para strtok
	char *cat;

	//roda todo iprimary recebndo chaves primarias e indicando rrns e suas sk
	for(i = 0; i < nregistros; i++){
		//recupera o jogo por rrn no arquivo
		jogo_recuperado = recuperar_registro(iprimary[i].rrn);
		//insere na lista de chaves secundarias
		cat = strtok(jogo_recuperado.categoria, "|\0");
		while(cat != NULL){
			ncat = insere_sk(icat, cat, jogo_recuperado.pk, ncat);
			cat = strtok(NULL, "|\0");
		}
		
	}

	return(ncat);
}

//por preco com desconto aplicado
int criar_iprice(Ir **iprice, Ip *iprimary, int nregistros){
	int nprice = 0, i;
	Jogo jogo_recuperado;
	//auxiliares para preco
	float preco, desconto;
	char price[TAM_PRECO];

	//roda todo iprimary recebndo chaves primarias e indicando rrns e suas sk
	for(i = 0; i < nregistros; i++){
		//recupera o jogo por rrn no arquivo
		jogo_recuperado = recuperar_registro(iprimary[i].rrn);
		//calcula o preco com os casting para evitar erros no ava
		sscanf(jogo_recuperado.preco, "%f", &preco);
		sscanf(jogo_recuperado.desconto, "%f", &desconto);
		preco = preco * (100 - desconto);
		preco = (int)preco / (float)100;
		sprintf(price, "%07.2f", preco);
		//insere na lista de chaves secundarias
		nprice = insere_sk(iprice, price, jogo_recuperado.pk, nprice);
	}

	return(nprice);
}

/* implementaçao de ler_entrada como sugerido*/
void ler_entrada(char* registro, Jogo *novo){
	char mes[3];
	int i;

	//leitura dos dados do novo jogo
	scanf("%[^\n]%*c", novo->nome);
	scanf("%[^\n]%*c", novo->desenvolvedora);
	scanf("%[^\n]%*c", novo->data);
	scanf("%[^\n]%*c", novo->classificacao);
	scanf("%[^\n]%*c", novo->preco);
	scanf("%[^\n]%*c", novo->desconto);
	scanf("%[^\n]%*c", novo->categoria);

	//leitura do mes em data para compor a pk
	sscanf(novo->data, "%*[^'/']/%[^'/']", mes);

	//criando chave primaria
	strncpy(novo->pk, novo->nome, 2);
	novo->pk[2] = '\0';
	strncat(novo->pk, novo->desenvolvedora, 2);
	strncat(novo->pk, novo->data, 2);
	strcat(novo->pk, mes);
	strncat(novo->pk, novo->classificacao, 2);

	//criando registro
	strcpy(registro, novo->nome);
	registro[strlen(novo->nome)] = '\0';
	strcat(registro, "@");
	strcat(registro, novo->desenvolvedora);
	strcat(registro, "@");
	strcat(registro, novo->data);
	strcat(registro, "@");
	strcat(registro, novo->classificacao);
	strcat(registro, "@");
	strcat(registro, novo->preco);
	strcat(registro, "@");
	strcat(registro, novo->desconto);
	strcat(registro, "@");
	strcat(registro, novo->categoria);
	strcat(registro, "@");

	//preenchendo espaço do registro com '#'
	for(i = strlen(registro); i < 192; i++){
		registro[i] = '#';
	}
}

/*--------------------------------
 ROTINAS DE INSERÇAO E AUXILIARES
--------------------------------*/

//Insere novo registro e retorna a nova quantidade de registros
int insere_registro(char novo_registro[], int nregistros){
	if(nregistros == 0){
		strncpy(ARQUIVO, novo_registro, 192);
		ARQUIVO[192] = '\0';
	}else{
		strncat(ARQUIVO, novo_registro, 192);
	}

	return (nregistros + 1);
}

//insere nova chave (nao repetida) em iprimary
void insere_pk(Ip *iprimary, char pk[], int pos){
	//insere na ultima posicao e ordena
	strcpy(iprimary[pos].pk, pk);
	iprimary[pos].pk[11] = '\0';
	iprimary[pos].rrn = pos * 192;
	ordena_pk(iprimary, pos + 1);
}

/*Ordenaçao para pk (Selection)
  Conhecimento desde Alg1, futuras pesquisas se pensar em melhorar*/
void ordena_pk(Ip *iprimary, int nregistros){
	int menor = 0, i = 0, j = 0;
	Ip swap; //variavel auxiliar para troca

	//funcao basica deselection sort
	//futuramente: tentar implementar um algoritmo melhor

	for(i = 0; i < nregistros; i++){
		menor = i;
		for(j = i + 1; j < nregistros; j++){
			if(strcmp(iprimary[menor].pk, iprimary[j].pk) > 0){
				menor = j;
			}
		}
		if(menor != i){
			swap = iprimary[menor];
			iprimary[menor] = iprimary[i];
			iprimary[i] = swap;
		}
	}
}


/*Entendimento sobre bsearch e comparacoes necessarias inspiradas no exemplo encontrado 
  na pagina sobre bsearch de C++ Reference:
  Link: http://www.cplusplus.com/reference/cstdlib/bsearch/ */

//inserçao de chave nos indices secundarios, generalizado para todos
int insere_sk(Ir **isecondary, char *sk, char *pk, int nindices){
	//declara ret_s para receber null ou chave secundaria ja existente
	Ir *ret_s = (Ir *) bsearch(sk, *isecondary, nindices, sizeof(Ir), compare_sec);

	/*se nao existir a chave, realoca o vetor de chaves secundarias para inserçao do novo
	  o retorno e o novo numero de chaves secundarias nesse indice
	  caso contrario, apenas insere na lista invertida, o retorno e nindices*/
	if(!ret_s){
		//realoca o vetor isecondary passado e aloca para a chave
		*isecondary = (Ir *) realloc (*isecondary, (nindices + 1) * sizeof(Ir));
		((*isecondary)[nindices]).chave = (char *) malloc (strlen(sk) + 1);
		//copia a chave secundaria
		strcpy(((*isecondary)[nindices]).chave, sk);
		((*isecondary)[nindices]).chave[strlen(sk)] = '\0';
		//chama rotina de insercao na lista invertida
		((*isecondary)[nindices]).lista = NULL;
		insere_Ir(&((*isecondary)[nindices]), pk);
		ordena_sk(*isecondary, nindices + 1);
		//retorna nindices + novo
		return(nindices + 1);
	}else{
		insere_Ir(ret_s, pk);
		ordena_sk(*isecondary, nindices);
		//retorna nidices
		return(nindices);
	}
}

void insere_Ir(Ir *indice, char *pk){
	//cria novo no para inserir na lista ligada
	//aux serve para percorrer a lista ligada
	ll *novo = (ll *) malloc (sizeof(ll)), *aux = NULL;

	//copia chave primaria
	strcpy(novo->pk, pk);
	novo->pk[11] = '\0';

	//insere ordenado

	//verifica se a lista esta vazia
	if(indice->lista == NULL){
		novo->prox = NULL;
		indice->lista = novo;
	}else{
		//verifica se deve ser o primeiro a ser inserido
		if(strcmp(indice->lista->pk, pk) > 0){
			novo->prox = indice->lista;
			indice->lista = novo;
		}else{
			//faz uma rotina para encontrar onde inserir
			aux = indice->lista;
			while(aux->prox != NULL){
				//se a pk se encaixa entre aux e seu prox e sai da funçao
				if(strcmp(aux->prox->pk, pk) > 0){
					novo->prox = aux->prox;
					aux->prox = novo;
					return;
				}
				aux = aux->prox;
			}
			//se rodar toda a lista, sera o ultimo
			novo->prox = NULL;
			aux->prox = novo;
		}
	}
}

//Ordena sk por Selectio Sort
void ordena_sk(Ir *isecondary, int nindices){
	int menor = 0, i = 0, j = 0;
	Ir swap; //variavel auxiliar para troca

	//funcao basica deselection sort
	//futuramente: tentar implementar um algoritmo melhor

	for(i = 0; i < nindices; i++){
		menor = i;
		for(j = i + 1; j < nindices; j++){
			if(strcmp(isecondary[menor].chave, isecondary[j].chave) > 0){
				menor = j;
			}
		}
		if(menor != i){
			swap = isecondary[menor];
			isecondary[menor] = isecondary[i];
			isecondary[i] = swap;
		}
	}
}

/*-----------------------------
 ROTINAS DE BUSCA E AUXILIARES
-----------------------------*/

//recupera rrn por pk, caso contrario retorna -1
int recuperar_rrn(Ip* iprimary, const char* pk, int nregistros){
	//variavel que recebe o retorno de bsearch
	Ip *ret_b = (Ip *) bsearch(pk, iprimary, nregistros, sizeof(Ip), compare_pri);

	//se encontrado retorna rrn, senao retorna -1 
	if(!ret_b){
		return -1;
	}else{
		return (ret_b->rrn);
	}
}

//comparaçao para PK
int compare_pri(const void *chave, const void *elem){

	return strcmp((char*)chave, (*(Ip*)elem).pk);
}

//comparacao para SK
int compare_sec(const void *chave, const void *elem){

	return strcmp((char*)chave, (*(Ir*)elem).chave);
}


/* Recupera do arquivo o registro com o rrn informado e retorna os dados na
 * struct Jogo */
Jogo recuperar_registro(int rrn){
	Jogo jogo_busca; //jogo a ser reornado
	char mes[3], *comeco_registro = ARQUIVO + rrn;; //auxiliar para compor a chave primaria e localizar o registro correto

	//rotina de leitura do registro
	sscanf(comeco_registro, "%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]", jogo_busca.nome, 
		   jogo_busca.desenvolvedora, jogo_busca.data, jogo_busca.classificacao, 
		   jogo_busca.preco, jogo_busca.desconto, jogo_busca.categoria);


	//leitura do mes em data para compor a pk
	sscanf(jogo_busca.data, "%*[^'/']/%[^'/']", mes);

	//criando chave primaria
	strncpy(jogo_busca.pk, jogo_busca.nome, 2);
	jogo_busca.pk[2] = '\0';
	strncat(jogo_busca.pk, jogo_busca.desenvolvedora, 2);
	strncat(jogo_busca.pk, jogo_busca.data, 2);
	strcat(jogo_busca.pk, mes);
	strncat(jogo_busca.pk, jogo_busca.classificacao, 2);

	return(jogo_busca);
}


//processo para listagem de jogos por chave secundaria
void listagem_sk(Ir sk, Ip *iprimary, int nregistros, int desconto){
	//auxiliar para rodar lista ligada
	ll *aux = sk.lista;
	//flag_quebralinha serve para identificar quando inserir uma quebra de linha
	int rrn_recuperado, flag_quebralinha = 0;

	//roda a lista ligada para imprimir registro
	while(aux != NULL){
		//recuprea rrn
		rrn_recuperado = recuperar_rrn(iprimary, aux->pk, nregistros);
		//decide se e com desconto ou nao
		if(desconto){
			//verifica se e necessaria a quebra de linha e depois exibe
			if(flag_quebralinha && rrn_recuperado >= 0)
				printf("\n");
			exibir_registro(rrn_recuperado, 1);
		}
		else{
			//verifica se e necessaria a quebra de linha e depois exibe
			if(flag_quebralinha && rrn_recuperado >= 0)
				printf("\n");
			exibir_registro(rrn_recuperado, 0);
		}
		if(rrn_recuperado >= 0)
			flag_quebralinha = 1;
		aux = aux->prox;
	}
}

void listagem_2sk(Ir sk, Ir sk2, Ip *iprimary, int nregistros){
	//auxiliares para rodar listas ligadas
	ll *aux = sk.lista, *aux2 = sk2.lista;
	//flag_quebralinha serve para identificar quando inserir uma quebra de linha
	int rrn_recuperado, flag_quebralinha = 0;

	//roda ambas listas em busca de matching
	while(aux != NULL){
		aux2 = sk2.lista;
		while(aux2 != NULL){
			if(strcmp(aux->pk, aux2->pk) == 0){
				//recupera rrn
				rrn_recuperado = recuperar_rrn(iprimary, aux->pk, nregistros);
				//verifica se e necessaria a quebra de linha
				if(flag_quebralinha && rrn_recuperado >= 0)
					printf("\n");
				//tenta exibir registro por rrn
				exibir_registro(rrn_recuperado, 0);
				//flag quebralinha para 1 em caso de rrn valido pela primeira vez
				if(rrn_recuperado >= 0)
					flag_quebralinha = 1;
			}
			aux2 = aux2->prox;
		}
		aux = aux->prox;
	}
}

/*---------------------------------
 ROTINAS DE ALTERAÇAO E AUXILIARES
---------------------------------*/

//altera campo desconto no arquivo, retorna 1 em caso de sucesso, senao 0
int alterar(Ip *iprimary, int nregistros){
	//aux e o pronteiro para atingir o rrn certo
	char pk_busca[TAM_PRIMARY_KEY], desconto_novo[4], *aux;
	int valor_desconto = 0, rrn_busca = 0;
	//auxiliar para alterar apenas desconto
	Jogo jogo_aux;

	//recebe chave primaria
	scanf("%[^\n]%*c", pk_busca);

	//busca rrn
	rrn_busca = recuperar_rrn(iprimary, pk_busca, nregistros);

	//se encontrou, executa alteraçao e retorna 1, senao retorna 0
	if(rrn_busca >= 0){
		//rotina para receber e verificar novo desconto
		do{
			scanf("%s%*c", desconto_novo);
			sscanf(desconto_novo, "%d", &valor_desconto);
			if(strlen(desconto_novo) != 3 || valor_desconto > 100 || valor_desconto < 0)
				printf(CAMPO_INVALIDO);
		}while(strlen(desconto_novo) != 3 || valor_desconto > 100 || valor_desconto < 0);

		//alterar campo de desconto
		aux = ARQUIVO + rrn_busca;
		//leitura para saber a quantidade de char nos campos variaveis antes do campo desconto
		sscanf(aux, "%[^@]@%[^@]", jogo_aux.nome, jogo_aux.desenvolvedora);
		//movimentando ponteiro somando campos variaveis com estaticos e delimitadors
		aux += strlen(jogo_aux.nome) + strlen(jogo_aux.desenvolvedora) + 24;
		//coloca novo desconto
		strncpy(aux, desconto_novo, 3);
		return 1;
	}else{
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}
}

/*-------------------------------
 ROTINAS DE REMOÇAO E AUXILIARES
-------------------------------*/

//remove registro no arquivo e rrn = -1 
//retorna 1 em caso de sucesso, senao 0
int remover(Ip *iprimary, int nregistros){
	//aux e o pronteiro para atingir o rrn correntamente
	char pk_busca[TAM_PRIMARY_KEY], *aux;
	int rrn_busca = 0;
	//auxiliar para armazenar struct do que sera removido
	Ip *pk;

	//recebe chave primaria
	scanf("%[^\n]%*c", pk_busca);

	//busca rrn
	rrn_busca = recuperar_rrn(iprimary, pk_busca, nregistros);

	if(rrn_busca >= 0){
		//move ponteiro para registro
		aux = ARQUIVO + rrn_busca;

		//altera registro
		strncpy(aux, "*|", 2);

		//coloca -1 no indice da chave removida
		pk = (Ip *) bsearch(pk_busca, iprimary, nregistros, sizeof(Ip), compare_pri);
		pk->rrn = -1;

		return 1;
	}else{
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

}

//limpeza do arquivo e retorna quantidade de itens removidos
int limpar_arquivo(){
	//auxiliar que recebe registro a registro e move ponteiro em ARQUIVO
	char *aux = ARQUIVO, arquivo_temporario[TAM_ARQUIVO];
	int nremovidos = 0;

	//inicio do arquivo temporario como \0 para strncat
	arquivo_temporario[0] = '\0';

	//roda todo o arquivo
	while(*aux != '\0'){
		//se nao e removido concatena o registro, caso contrario, o mesmo e pulado e acrescenta-se +1 na quantidade removida
		if(!(aux[0] == '*' && aux[1] == '|'))
			strncat(arquivo_temporario, aux, 192);
		else
			nremovidos++;

		aux = aux + 192;
	}

	//copiando novo arquivo para a variavel padrao
	strcpy(ARQUIVO, arquivo_temporario);

	return(nremovidos);
}

//remove os nos das listas invertidas
void libera_sk(Ir **isecondary, int nindices){
	int i;
	//auxiliares para usar free e rodas a lista ligada
	ll *aux_free, *aux;

	//vasculha todo vetor
	for(i = 0; i < nindices; i++){
		//rotina para liberar memoria da ll
		aux = ((*isecondary)[i]).lista;
		while(aux != NULL){
			aux_free = aux;
			aux = aux->prox;
			free(aux);
		}
		free((*isecondary)[i].chave);
	}
}