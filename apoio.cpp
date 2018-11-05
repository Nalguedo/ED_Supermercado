#include "settings.h"

//FUNÇÕES DE APOIO (MENSAGENS, ERROS, LIMPAR HISTÓRICO, SETTINGS, ETC)
//----------------------------------------------------------------------//
/*
Função Data e Hora - usada como cabeçalho do programa - também usada no cálculo aleatório dos numeros de contrato
Usa a estrutura gmtime() - https://linux.die.net/man/3/gmtime
*/
void horadata(SUPERMERCADO *data)
{
	time_t t = time(NULL);
	struct tm *tmp = localtime(&t);
	printf("\x1B[32mSuperMarket \x1B[33mSIM®\x1B[37m\nData: %02d/%02d/%d\t\tHora: %02d:%02d\n", tmp->tm_mday, tmp->tm_mon + 1, tmp->tm_year + 1900, tmp->tm_hour, tmp->tm_min);
	//if (data.dadosNaoGuardados) //Flag de dados por gravar, sempre que esteja "true" surge o aviso no topo do programa
		//printf("\n""\x1B[31m AVISO - EXISTEM ALTERAÇÕES NÃO GUARDADAS \x1B[37m""\n");
}

/*
Função para limpar stdin
*/
void limpaInput()
{
	fseek(stdin, 0, SEEK_END); //Colocamos o ponteiro do stdin na ultima posição, prevenindo que a função gets assuma o "Enter" "\n"
}

/*
Função de Pausa
Pausa com getchar(); e apresenta a mensagem para o utilizador premir o Enter para continuar
*/
void pausa()
{
	printf("\n\n\t------------------------------\n\t| \x1B[36mPrima ENTER para continuar\x1B[37m |\n\t------------------------------\n\n");
	limpaInput();
	while (getchar() != '\n');
}

/*
Função de Pausa com opção para terminar
*/
void pausaBreak(char *x)
{
	printf("\n\n   ----------------------------------------------------\n   |""\x1B[36m [ ENTER ] -> CONTINUAR\t\x1B[31m\t[ X ] -> SAIR\x1B[37m"" |\n   ----------------------------------------------------\n");
	limpaInput();
	do
	{
		scanf("%c", &(*x));
	} while (*x != '\n' && *x != 'x' && *x != 'X' && *x);
}

/*
Função de erro de leitura de dados, apresenta a mensagem para o utilizador premir o Enter para sair da aplicação
*/
void erroDados()
{
	printf("\n\n     -------------------------------------\n     |****   ERRO NA BASE DE DADOS   ****|\n     \n     |****  O PROGRAMA VAI ENCERRAR  ****|\n     -------------------------------------\n");
	pausa();
	exit(0);
}

/*
Função para enviar mensagem de erro de memória
*/
void erroMemoria()
{
	printf("\n\n     -------------------------------------\n     |****      ERRO NA MEMÓRIA      ****|\n     \n     |****  O PROGRAMA VAI ENCERRAR  ****|\n     -------------------------------------\n");
	pausa();
	exit(ERRO_MEMORIA);
}

/*
Função para o ciclo associado ao scanf dos menus - aceita apenas os algarismos definidos para as respetivas opções
*/
int scanfMenu(int min, int max)
{
	int op;

	while (scanf("%d", &op) != 1 || op < min || op > max)
	{
		printf("\nINTRODUZA UMA OPÇÃO VÁLIDA!\n");
		limpaInput();
		printf("OPÇÃO: ");
	}
	return op;
}

int validaCodigo(char *codigo)
{
	unsigned int i;

	for (i = 0; i < strlen(codigo); i++)
	{
		if (!isdigit(codigo[i]))
			return 0;
	}
	return 1;
}

/*
Função para mostrar o menu de ajuda - Mostra o conteúdo do ficheiro "help.txt"
*/
void mostraAjuda(SUPERMERCADO *Super)
{
	FILE *fi = fopen("help.txt", "r");
	char buffer[BUFFER];
	int i = 0;
	system("cls");
	horadata(Super);

	while (fgets(buffer, BUFFER, fi))
	{
		if (i == 21)
		{
			i = 0;
			printf("\n\nPÁGINA 1/2");
			pausa();
			system("cls");
			horadata(Super);
		}
		printf("%s", buffer);
		i++;
	}
	printf("\n\nPÁGINA 2/2");
	pausa();
	fclose(fi);
}

void loadSettings(SUPERMERCADO *Super)
{
	FILE *fsettings = fopen("config.txt", "r");
	if (!fsettings) erroDados();
	
	int i = 0;
	char op[10], buffer[BUFFER];

	while (!feof(fsettings) && i < MAX_OPCOES)
	{
		//Leitura para array das settings
		fscanf(fsettings, "%s%[^\n]", op, buffer);
		Super->settings[i] = atoi(op);
		i++;
	}

	fclose(fsettings);
}

void mostraSettings(SUPERMERCADO *Super)
{
	FILE *fsettings = fopen("config.txt", "r");
	if (!fsettings) erroDados();
	char buffer[BUFFER];

	while (fgets(buffer, BUFFER, fsettings))
	{
		printf("%s\n", buffer);
	}

	fclose(fsettings);
}

void guardaSettings(SUPERMERCADO *Super)
{
	FILE *fsettings = fopen("config.txt", "w");
	if (!fsettings) erroDados();

	int i;

	for (i = 0; i < MAX_OPCOES; i++)
	{
		fprintf(fsettings, "%d\n", Super->settings[i]);
	}
	fclose(fsettings);
}

void exportarResultados(SUPERMERCADO *Super)
{
	time_t t = time(NULL);
	struct tm *tmp = localtime(&t);

	char nomeBackupTXT[BUFFER];
	sprintf_s(nomeBackupTXT, "./export/SIMULACAO_%02d%02d%d_%02d%02d%02d.txt", tmp->tm_mday, tmp->tm_mon + 1, tmp->tm_year + 1900, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	char nomeBackupCSV[BUFFER];
	sprintf_s(nomeBackupCSV, "./export/SIMULACAO_%02d%02d%d_%02d%02d%02d.csv", tmp->tm_mday, tmp->tm_mon + 1, tmp->tm_year + 1900, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

	FILE *fiSim = fopen(nomeBackupTXT, "w");
	if (!nomeBackupTXT) erroDados();

	int flag = 1;

	CAIXA *caixa = NULL;
	NO_FUNCIONARIO *funcionario = NULL;

	int seconds, hours, minutes;
	seconds = Super->tempoDecorrido % 60;
	minutes = (Super->tempoDecorrido / 60) % 60;
	hours = (Super->tempoDecorrido / 60 / 60) % 60;

	int tempoMedioSuperF = (int)(Super->tempoMedioNoSuper / Super->lFinal->nrClientesFinal);

	while (flag && flag < 3)
	{
		if (flag == 2)
		{
			FILE *fiSim = fopen(nomeBackupCSV, "w");
			if (!nomeBackupCSV) erroDados();
		}

		fprintf(fiSim, "SuperMarketSIM®\nDATA: %02d/%02d/%d\t\tHORA: %02d:%02d\n\n", tmp->tm_mday, tmp->tm_mon + 1, tmp->tm_year + 1900, tmp->tm_hour, tmp->tm_min);
		fprintf(fiSim, "______________________________________________________________________________________________________\n");
		fprintf(fiSim, "|#####################################################################################################\n");
		fprintf(fiSim, "| RESULTADOS DA SIMULAÇÃO:                                                                            \n");
		fprintf(fiSim, "|                                                                                                     \n");
		fprintf(fiSim, "| TEMPO DE SIMULAÇÃO: %02d:%02d:%02d                                                                  \n", hours, minutes, seconds);
		fprintf(fiSim, "| CLIENTES ATENDIDOS: %d                                                                              \n", Super->lFinal->nrClientesFinal);
		fprintf(fiSim, "|                                                                                                     \n");
		fprintf(fiSim, "| PRODUTOS VENDIDOS: %d                                                                               \n", Super->nrProdutosVendidos);
		fprintf(fiSim, "| TOTAL FATURADO: %.2lf EUROS                                                                         \n", Super->totalFaturado);
		fprintf(fiSim, "|                                                                                                     \n");
		fprintf(fiSim, "| TEMPO MÉDIO DE ESPERA NA FILA: %.0f SEGUNDOS                                                        \n", Super->tempoMedioEspera / Super->lFinal->nrClientesFinal);
		fprintf(fiSim, "| TEMPO MÉDIO DE PERMANÊNCIA NO SUPERMERCADO: %02d:%02d                                               \n", (tempoMedioSuperF / 60) % 60, tempoMedioSuperF % 60);
		fprintf(fiSim, "|                                                                                                     \n");
		fprintf(fiSim, "| NUMERO DE PRODUTOS OFERECIDOS = %d                                                                  \n", Super->nrProdutosOferta);
		fprintf(fiSim, "| TOTAL DAS OFERTAS = %.2lf EUROS                                                                     \n", Super->valorProdutosOferta);
		fprintf(fiSim, "|                                                                                                     \n");
		fprintf(fiSim, "|                                                                                                     \n");
		fprintf(fiSim, "| CAIXAS:                                                                                             \n");
		fprintf(fiSim, "|                                                                                                     \n");
		caixa = dadosCaixa(Super, mais_clientes);
		fprintf(fiSim, "| CAIXA QUE ATENDEU MAIS CLIENTES: CAIXA %d                                                           \n", caixa->idCaixa);
		caixa = dadosCaixa(Super, mais_produtos);
		fprintf(fiSim, "| CAIXA QUE VENDEU MAIS PRODUTOS: CAIXA %d                                                            \n", caixa->idCaixa);
		fprintf(fiSim, "| MÉDIA DE CAIXAS ABERTAS: %.2f                                                                       \n", Super->mediaCaixasAbertas / Super->tempoDecorrido);
		fprintf(fiSim, "|                                                                                                     \n");
		fprintf(fiSim, "|                                                                                                     \n");
		fprintf(fiSim, "| FUNCIONÁRIOS:                                                                                       \n");
		fprintf(fiSim, "|                                                                                                     \n");
		funcionario = dadosFuncionario(Super, menos_clientes_F);
		fprintf(fiSim, "| MENOS CLIENTES ATENDIDOS:                                                                           \n");
		fprintf(fiSim, "| ID: %s\tNOME: %-30s\tCLIENTES ATENDIDOS: %d                                                         \n", funcionario->info_funcionario->idFuncionario, funcionario->info_funcionario->nome, funcionario->info_funcionario->nrClientesAtendidos);
		funcionario = dadosFuncionario(Super, mais_clientes_F);
		fprintf(fiSim, "| MAIS CLIENTES ATENDIDOS:                                                                            \n");
		fprintf(fiSim, "| ID: %s\tNOME: %-30s\tCLIENTES ATENDIDOS: %d                                                         \n", funcionario->info_funcionario->idFuncionario, funcionario->info_funcionario->nome, funcionario->info_funcionario->nrClientesAtendidos);
		fprintf(fiSim, "|_____________________________________________________________________________________________________\n");
		fprintf(fiSim, "|#####################################################################################################\n");
		fprintf(fiSim, "|                                                                                                     \n");
		fprintf(fiSim, "| DEFINIÇÕES DA SIMULAÇÃO:                                                                            \n");
		fprintf(fiSim, "|                                                                                                     \n");
		fprintf(fiSim, "| TOTAL DE HORAS DE FUNCIONAMENTO ----------------------------------------------- [%d]                \n", Super->settings[0]);
		fprintf(fiSim, "| NÚMERO DE PRODUTOS GERADOS ---------------------------------------------------- [%d]                \n", Super->settings[1]);
		fprintf(fiSim, "| NÚMERO MÁXIMO DE PRODUTOS COMPRADOS POR CLIENTE ------------------------------- [%d]                \n", Super->settings[2]);
		fprintf(fiSim, "| MÉDIA DO CUSTO DOS PRODUTOS --------------------------------------------------- [%d]                \n", Super->settings[3] / 100);
		fprintf(fiSim, "| MÉDIA DO TEMPO DISPENDIDO EM ESCOLHA DE PRODUTOS ------------------------------ [%d]                \n", Super->settings[4]);
		fprintf(fiSim, "| INTERVALO DE ENTRADA DE CLIENTES ---------------------------------------------- [%d]                \n", Super->settings[5]);
		fprintf(fiSim, "| NUMERO MAXIMO DE CAIXAS ------------------------------------------------------- [%d]                \n", Super->settings[6]);
		fprintf(fiSim, "| TEMPO MAXIMO DE ESPERA -------------------------------------------------------- [%d]                \n", Super->settings[7]);
		fprintf(fiSim, "| TEMPO MÁXIMO DE UM PRODUTO SER PASSADO NA CAIXA ------------------------------- [%d]                \n", Super->settings[8]);
		fprintf(fiSim, "| TEMPO MÁXIMO DE PAGAMENTO ----------------------------------------------------- [%d]                \n", Super->settings[9]);
		fprintf(fiSim, "| NUMERO MÉDIO DE CLIENTES EM FILAS DE CAIXA PARA PROCEDER À ABERTURA DE OUTRA -- [%d]                \n", Super->settings[10]);
		fprintf(fiSim, "| NUMERO MÉDIO DE CLIENTES EM FILAS PARA PROCEDER AO FECHO DE CAIXA ------------- [%d]                \n", Super->settings[11]);
		fprintf(fiSim, "|                                                                                                     \n");
		fprintf(fiSim, "|#####################################################################################################\n");
		fprintf(fiSim, "|_____________________________________________________________________________________________________\n");

		fclose(fiSim);
		flag++;
	}
	printf("\n\n\n\tFICHEIRO EXPORTADO COM SUCESSO!\n\n");
	pausa();
}

void mostraInfoMenuPausa(SUPERMERCADO *Super)
{
	system("cls");
	horadata(Super);
	int seconds, hours, minutes;
	seconds = Super->tempoDecorrido % 60;
	minutes = (Super->tempoDecorrido / 60) % 60;
	hours = (Super->tempoDecorrido / 60 / 60) % 60;
	printf("\n\n************    DADOS DA SIMULAÇÃO    ************\n\n");
	printf("TEMPO DECORRIDO: %02d:%02d:%02d\n\n", hours, minutes, seconds);
	printf("CLIENTES ATENDIDOS: %d\n", Super->lFinal->nrClientesFinal);
	printf("PRODUTOS VENDIDOS: %d\n", Super->nrProdutosVendidos);
	printf("TOTAL FATURADO: %.2lf EUROS\n", Super->totalFaturado);
	printf("TEMPO MÉDIO DE ESPERA: %.2f SEGUNDOS\n", Super->tempoMedioEspera / Super->lFinal->nrClientesFinal);

	int tempoMedioSuperF = (int)(Super->tempoMedioNoSuper / Super->lFinal->nrClientesFinal);
	printf("TEMPO MÉDIO DE PERMANÊNCIA NO SUPERMERCADO: %02d:%02d \n", (tempoMedioSuperF / 60) % 60, tempoMedioSuperF % 60);
	pausa();
}

void mostraResultadosFim(SUPERMERCADO *Super)
{
	system("cls");
	horadata(Super);
	int seconds, hours, minutes;
	seconds = Super->tempoDecorrido % 60;
	minutes = (Super->tempoDecorrido / 60) % 60;
	hours = (Super->tempoDecorrido / 60 / 60) % 60;
	printf("\n\n*************    RESULTADOS DA SIMULAÇÃO    *************\n\n");
	printf("TEMPO DE SIMULAÇÃO: %02d:%02d:%02d\n\n", hours, minutes, seconds);
	printf("CLIENTES ATENDIDOS: %d\n\n", Super->lFinal->nrClientesFinal);
	printf("PRODUTOS VENDIDOS: %d\n", Super->nrProdutosVendidos);
	printf("TOTAL FATURADO: %.2lf EUROS\n", Super->totalFaturado);
	printf("NUMERO DE PRODUTOS OFERECIDOS = %d\nTOTAL DAS OFERTAS = %.2lf EUROS\n\n", Super->nrProdutosOferta, Super->valorProdutosOferta);
	pausa();
}

void mostraEstatisticaMenuFim(SUPERMERCADO *Super)
{
	CAIXA *caixa = NULL;
	NO_FUNCIONARIO *funcionario = NULL;

	system("cls");
	horadata(Super);
	int seconds, hours, minutes;
	seconds = Super->tempoDecorrido % 60;
	minutes = (Super->tempoDecorrido / 60) % 60;
	hours = (Super->tempoDecorrido / 60 / 60) % 60;
	printf("\n\n*************    ESTATISTICAS DA SIMULAÇÃO    *************\n");
	printf("\x1B[36mCLIENTES\x1B[37m\n");
	printf("CLIENTES ATENDIDOS: %d\n", Super->lFinal->nrClientesFinal);
	printf("TEMPO MÉDIO DE ESPERA NA FILA: %.0f SEGUNDOS\n", Super->tempoMedioEspera / Super->lFinal->nrClientesFinal);

	int tempoMedioSuperF = (int)(Super->tempoMedioNoSuper / Super->lFinal->nrClientesFinal);
	printf("TEMPO MÉDIO DE PERMANÊNCIA NO SUPERMERCADO: %02d:%02d \n\n", (tempoMedioSuperF / 60) % 60, tempoMedioSuperF % 60);

	printf("\x1B[36mCAIXAS\x1B[37m\n");
	caixa = dadosCaixa(Super, mais_clientes);
	printf("CAIXA QUE ATENDEU MAIS CLIENTES: CAIXA %d\n", caixa->idCaixa);

	caixa = dadosCaixa(Super, mais_produtos);
	printf("CAIXA QUE VENDEU MAIS PRODUTOS: CAIXA %d\n", caixa->idCaixa);
	printf("MÉDIA DE CAIXAS ABERTAS: %.2f\n\n", Super->mediaCaixasAbertas / Super->tempoDecorrido);

	printf("\x1B[36mFUNCIONÁRIOS\x1B[37m\n");
	funcionario = dadosFuncionario(Super, menos_clientes_F);
	printf("FUNCIONÁRIO QUE ATENDEU MENOS CLIENTES:\nID: %s\tNOME: %-30s\tCLIENTES ATENDIDOS: %d\n", funcionario->info_funcionario->idFuncionario, funcionario->info_funcionario->nome, funcionario->info_funcionario->nrClientesAtendidos);

	funcionario = dadosFuncionario(Super, mais_clientes_F);
	printf("FUNCIONÁRIO QUE ATENDEU MAIS CLIENTES:\nID: %s\tNOME: %-30s\tCLIENTES ATENDIDOS: %d\n\n", funcionario->info_funcionario->idFuncionario, funcionario->info_funcionario->nome, funcionario->info_funcionario->nrClientesAtendidos);

	printf("\x1B[36mOFERTAS\x1B[37m\n");
	printf("NUMERO DE PRODUTOS OFERECIDOS = %d\nTOTAL DAS OFERTAS = %.2lf EUROS", Super->nrProdutosOferta, Super->valorProdutosOferta);

	pausa();
}

void mostraArtSuper()
{
	printf("\n\n            _ _                         ");
	printf("\n             \\ \\ ___________            ");
	printf("\n              |# # # # # # #/             ");
	printf("\n              | # # # # # #/              ");
	printf("\n     >>>>>>   |#_#_#_#_#_#/    >>>>>>     ");
	printf("\n              |________                   ");
	printf("\n ______________O ___ O __________________\n\n ");
}

void mostraInfoFuncionario(SUPERMERCADO *Super, NO_FUNCIONARIO *funcionario)
{
	system("cls");
	horadata(Super);

	int seconds, hours, minutes, index;

	index = funcionario->info_funcionario->estadoCX - 1;

	printf("\n\n\x1B[36mDADOS FUNCIONÁRIO:\x1B[37m\n\n");
	printf("ID: %s\n", funcionario->info_funcionario->idFuncionario);
	printf("NOME: %s\n\n", funcionario->info_funcionario->nome);
	printf("CAIXA: %d\n", funcionario->info_funcionario->estadoCX);
	seconds = funcionario->info_funcionario->minutosPorCaixa[index] % 60;
	minutes = (funcionario->info_funcionario->minutosPorCaixa[index] / 60) % 60;
	hours = (funcionario->info_funcionario->minutosPorCaixa[index] / 60 / 60) % 60;
	printf("TEMPO EM CAIXA: %02d:%02d:%02d\n", hours, minutes, seconds);
	printf("CLIENTES ATENDIDOS: %d\n", funcionario->info_funcionario->nrClientesAtendidos);
	printf("PRODUTOS PROCESSADOS: %d\n", funcionario->info_funcionario->nrProdutosProcessados);
}

void mostraInfoCliente(SUPERMERCADO *Super, NO_CLIENTE *cliente)
{
	system("cls");
	horadata(Super);

	printf("\n\n\x1B[36mDADOS CLIENTE:\x1B[37m\n\n");
	printf("ID: %s\n", cliente->info_cliente->idCliente);
	printf("NOME: %s\n", cliente->info_cliente->nomeCliente);
	printf("CAIXA: %d\n\n", cliente->info_cliente->caixa);
	pausa();
	system("cls");
	mostraTalaoCompras(Super, cliente);
}

void mostraInfoCaixa(SUPERMERCADO *Super, CAIXA *caixa)
{
	system("cls");
	horadata(Super);

	printf("\n\n\x1B[36mDADOS CAIXA:\x1B[37m\n\n");
	printf("ID: %d\n", caixa->idCaixa);
	printf("TOTAL DE CLIENTES: %d\n", caixa->totalClientesCX);
	printf("TOTAL DE PRODUTOS: %d\n", caixa->totalProdutosCX);
}

void actualizaFuncionario(CAIXA *caixa, EVENTO *evento)
{
	caixa->funcionario->estadoCX = caixa->idCaixa;
	caixa->funcionario->minutosPorCaixa[caixa->idCaixa - 1] += evento->eCliente->info_cliente->tempoPagamento;
	caixa->funcionario->nrClientesAtendidos++;
	caixa->funcionario->nrProdutosProcessados += evento->eCliente->info_cliente->nrProdutos;
}

void mudaTurnoFuncionarios(SUPERMERCADO *Super)
{
	int i;
	CAIXA *caixa = Super->lCaixa->caixaInicio;

	for (i = 1; i <= Super->settings[6]; i++)
	{
		caixa->funcionario = escolheFuncionario(Super, caixa);
		caixa = caixa->caixaProx;
	}
}

void mostraInfoProduto(SUPERMERCADO *Super, NO_PRODUTO *produto)
{
	system("cls");
	horadata(Super);

	printf("\n\n\x1B[36mDADOS PRODUTO:\x1B[37m\n\n");
	printf("ID: %d\n", produto->info_produto->idProduto);
	printf("PREÇO: %.2lf EUROS\n", produto->info_produto->precoProduto);
	printf("TEMPO DE AQUISIÇÃO: %.2lf SEGUNDOS\n", produto->info_produto->tempoProduto);
}

void mostraListaClientesCaixa(SUPERMERCADO *Super, int nrCaixa)
{
	NO_CLIENTE *cliente = Super->lFinal->inicio;
	int i = 1;

	CAIXA *caixa = encontraCaixa(Super, nrCaixa);

	system("cls");
	horadata(Super);
	printf("\t\t\t\x1B[36mCAIXA %d\x1B[37m", nrCaixa);
	int pag = 1, totalPag = caixa->totalClientesCX / 19;

	while (cliente)
	{
		if (i == 20)
		{
			i = 1;
			printf("\n\nPÁGINA %d/%d", pag, totalPag + 1);
			pag++;
			pausa();
			system("cls");
			horadata(Super);
			printf("\t\t\t\x1B[36mCAIXA %d\x1B[37m", nrCaixa);
		}
		if (cliente->info_cliente->caixa == nrCaixa)
		{
			printf("\nID: %-5s\tNOME: %-30s\tPRODUTOS: %d\tMONTANTE: %.2lf", cliente->info_cliente->idCliente, cliente->info_cliente->nomeCliente, cliente->info_cliente->nrProdutos, cliente->info_cliente->talaoCompras->valorFinal);
			i++;
		}
		cliente = cliente->prox;
	}
	printf("\n\nPÁGINA %d/%d", pag, totalPag + 1);
}

//Mostra a lista de Clientes que frequentou a loja
void mostraListaGeral(SUPERMERCADO *Super)
{
	NO_CLIENTE *cliente = Super->lFinal->inicio;

	int i;
	char x;

	system("cls");
	horadata(Super);
	int pag = 1, totalPag = Super->lFinal->nrClientesFinal / 20;

	for (i = 0; i < Super->lFinal->nrClientesFinal; i++)
	{
		printf("\nID: %-5s\tNOME: %-30s\tPRODUTOS: %d\tMONTANTE: %.2lf", cliente->info_cliente->idCliente, cliente->info_cliente->nomeCliente, cliente->info_cliente->nrProdutos, cliente->info_cliente->talaoCompras->valorFinal);
		if (i > 1 && i % 20 == 0) //Numero máximo de linhas a apresentar - 20
		{
			printf("\nPÁGINA %d / %d", pag, totalPag + 1);
			pausaBreak(&x);
			if (x == 'x' || x == 'X') //X para sair
				return;

			system("cls");
			horadata(Super);
			pag++;
		}
		cliente = cliente->prox;
	}
	printf("\nPÁGINA %d / %d", pag, totalPag + 1);
}

//Mostra a lista de caixas e informações
void mostraListaCaixas(SUPERMERCADO *Super)
{
	CAIXA *caixa = Super->lCaixa->caixaInicio;
	int i = 1;
	char estadoCx[15];

	system("cls");
	horadata(Super);

	while (caixa)
	{
		if (i == 15)
		{
			i = 1;
			pausa();
			system("cls");
			horadata(Super);
		}
		//Converter os valores do enum do estado_caixa para strings a apresentar
		switch (caixa->statusCaixa)
		{
		case 0:
			strcpy(estadoCx, "FECHADA");
			break;
		case 1:
			strcpy(estadoCx, "A FECHAR");
			break;
		case 2:
			strcpy(estadoCx, "ABERTA");
			break;
		case 4:
			strcpy(estadoCx, "FECHADA-CHEFE");
			break;
		}
		printf("\nNUMERO DA CAIXA: %d\tESTADO: %-15s\tCLIENTES NA FILA: %d\tTOTAL DE CLIENTES: %d", caixa->idCaixa, estadoCx, caixa->filaClientes->nrClientesInst, caixa->totalClientesCX);
		caixa = caixa->caixaProx;
	}
}

//Mostra a lista de produtos e detalhes criados aleatoriamente
void mostraListaProdutos(SUPERMERCADO *Super)
{
	NO_PRODUTO *produto = Super->lProdutos->inicio;

	int i;
	char x;

	system("cls");
	horadata(Super);
	int pag = 1;

	for (i = 0; i < Super->lProdutos->totalProdutos; i++)
	{
		printf("\nID_PRODUTO: %d\tPREÇO: %.2lf\tTEMPO DE COMPRA: %.0lf", produto->info_produto->idProduto, produto->info_produto->precoProduto, produto->info_produto->tempoProduto);
		if (i > 1 && i % 20 == 0) //Numero máximo de linhas a apresentar - 20
		{
			printf("\nPÁGINA %d / %.0f", pag, (Super->lProdutos->totalProdutos * 1.0) / (20));
			pausaBreak(&x);
			if (x == 'x' || x == 'X') //X para sair
				return;

			system("cls");
			horadata(Super);
			pag++;
		}
		produto = produto->prox;
	}
	printf("\nPÁGINA %d / %.0f", pag, (Super->lProdutos->totalProdutos * 1.0) / (20));
	pausa();
	system("cls");
	horadata(Super);
	printf("\n\nPREÇO MÉDIO = %.2lf euros\nPRODUTO MAIS CARO = %.2lf euros\nTOTAL DE PRODUTOS = %d\n\n", Super->lProdutos->precoMedio, Super->lProdutos->produtoMaisCaro, Super->lProdutos->totalProdutos);
}

void mostraTalaoCompras(SUPERMERCADO *Super, NO_CLIENTE *cliente)
{
	NO_PRODUTO *produtoComprado = cliente->info_cliente->talaoCompras->inicio;

	int i = 1;
	int pag = 1, totalPag = cliente->info_cliente->nrProdutos / 19;

	printf("\t\x1B[36mTALÃO DE COMPRAS\x1B[37m\n");

	while (produtoComprado)
	{
		if (i == 20)
		{
			i = 1;
			printf("\n\nPÁGINA %d/%d", pag, totalPag + 1);
			pag++;
			pausa();
			system("cls");
			horadata(Super);
		}
		printf("PRODUTO: %d\tPREÇO: %.2lf euros\n", produtoComprado->info_produto->idProduto, produtoComprado->info_produto->precoProduto);
		produtoComprado = produtoComprado->prox;

	}
	printf("\n\nPÁGINA %d/%d", pag, totalPag + 1);
	printf("\n\nTotal: %.2lf", cliente->info_cliente->talaoCompras->valorFinal);
}

//Mostra a lista de funcionários importada do ficheiro
void mostraListaFuncionarios(SUPERMERCADO *Super)
{
	NO_FUNCIONARIO *funcionario = Super->lFuncionarios->inicio;

	int i;
	char x;

	system("cls");
	horadata(Super);
	int pag = 1;

	for (i = 0; i < Super->lFuncionarios->nrFuncionarios; i++)
	{
		printf("\nID: %-5s\tNOME: %-30s\tCAIXA: %d\tCLIENTES: %d", funcionario->info_funcionario->idFuncionario, funcionario->info_funcionario->nome, funcionario->info_funcionario->estadoCX, funcionario->info_funcionario->nrClientesAtendidos);
		if (i > 1 && i % 20 == 0) //Numero máximo de linhas a apresentar - 20
		{
			printf("\nPÁGINA %d / %.0f", pag, (Super->lFuncionarios->nrFuncionarios * 1.0) / (20));
			pausaBreak(&x);
			if (x == 'x' || x == 'X') //X para sair
				return;

			system("cls");
			horadata(Super);
			pag++;
		}
		funcionario = funcionario->prox;
	}
	printf("\nPÁGINA %d / %.0f", pag, (Super->lFuncionarios->nrFuncionarios * 1.0) / (20));
}

void limparMemoria(SUPERMERCADO *Super)
{
	NO_CLIENTE *cli = NULL;
	NO_CLIENTE *auxCli = NULL;
	NO_PRODUTO *produto = NULL;
	NO_PRODUTO *auxProduto = NULL;
	CAIXA *caixa = NULL;
	CAIXA *auxCaixa = NULL;
	EVENTO *evento = NULL;
	EVENTO *auxEvento = NULL;
	NO_FUNCIONARIO *funcionario = NULL;
	NO_FUNCIONARIO *auxFuncionario = NULL;

	//Lista de compras está vazia quando termina a simulação, só é libertado o ponteiro da lista
	free(Super->lCompras);
	Super->lCompras = NULL;

	caixa = Super->lCaixa->caixaInicio;
	while (caixa)
	{
		auxCaixa = caixa->caixaProx;
		cli = caixa->filaClientes->cabeca;
		while (cli)
		{
			auxCli = cli->prox;
			free(cli);
			cli = auxCli;
		}
		free(caixa->filaClientes);
		free(caixa);
		caixa = auxCaixa;
	}
	free(Super->lCaixa);

	cli = Super->lGeral->inicio;
	while (cli)
	{
		auxCli = cli->prox;
		free(cli);
		cli = auxCli;
	}
	free(Super->lGeral);

	//Lista Eventos fica apenas com o elemento inicial
	free(Super->lEventos);

	produto = Super->lProdutos->inicio;
	while (produto)
	{
		auxProduto = produto->prox;
		free(produto->info_produto);
		free(produto);
		produto = auxProduto;
	}
	free(Super->lProdutos);

	cli = Super->lFinal->inicio;
	while (cli)
	{
		auxCli = cli->prox;
		free(cli);
		cli = auxCli;
	}
	free(Super->lFinal);

	funcionario = Super->lFuncionarios->inicio;
	while (funcionario)
	{
		auxFuncionario = funcionario->prox;
		free(funcionario->info_funcionario);
		free(funcionario);
		funcionario = auxFuncionario;
	}
	free(Super->lFuncionarios);


	for (int i = 0; i < Super->nrTotalArrayClientes; i++)
	{

		produto = Super->arrayClientes[i]->talaoCompras ? Super->arrayClientes[i]->talaoCompras->inicio : NULL;

		while (produto)
		{
			auxProduto = produto->prox;
			free(produto);
			produto = auxProduto;
		}
		free(Super->arrayClientes[i]->talaoCompras);
		free(Super->arrayClientes[i]->nomeCliente);
		free(Super->arrayClientes[i]);
	}
	free(Super->arrayClientes);

	free(Super);
}

