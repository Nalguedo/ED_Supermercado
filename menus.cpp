#include "settings.h"

int i, op = HOLD_OPCAO;

//MENU INICIAL
void menuInicial(SUPERMERCADO *Super)
{
	while (op)
	{
		system("cls");
		horadata(Super);
		printf("\n\n************    MENU INICIAL    ************\n\n");
		printf("1 - SIMULAÇÃO\n");
		printf("2 - DEFINIÇÕES\n");
		printf("3 - AJUDA\n\n");
		printf("***********    **************    ***********\n\n");
		printf("0 - SAIR\n");
		printf("____________________________________________\n\n");
		printf("OPÇÃO: ");
		op = scanfMenu(0, 3); //Devolve apenas valores válidos para op

		switch (op)
		{
		case 0:
			limparMemoria(Super);
			//Super = inicializa();
			system("cls");	
			break;
		case 1:
			limparMemoria(Super);
			Super = inicializa();
			submenuSimulacao(Super);
			break;
		case 2:
			limparMemoria(Super);
			Super = inicializa();
			menuSettings(Super);
			break;
		case 3:
			mostraAjuda(Super);
			break;
		}
	}
}

void submenuSimulacao(SUPERMERCADO *Super)
{
	while (op)
	{
		system("cls");
		horadata(Super);
		printf("\n\n*********    VELOCIDADE DE SIMULAÇÃO    *********\n\n");
		printf("1 - INSTANTÂNEA\n");
		printf("2 - RÁPIDA\n");
		printf("3 - LENTA\n");
		printf("0 - VOLTAR\n");
		printf("_________________________________________________\n\n");
		printf("OPÇÃO: ");
		op = scanfMenu(0, 3); //Devolve apenas valores válidos para op

		switch (op)
		{
		case 0:
			break;
		case 1:
			//Inicio da Simulação
			simulacao(Super, 0);
			menuFim(Super);
			break;
		case 2:
			//Inicio da Simulação
			system("cls");
			horadata(Super);
			printf("\n\n-----------SIMULAÇÃO EM CURSO-----------\n\t---------------------------\n\t| \x1B[36mPRIMA ESPAÇO PARA PARAR\x1B[37m |\n\t---------------------------\n\n");
			mostraArtSuper();
			simulacao(Super, 30);
			menuFim(Super);
			break;
		case 3:
			//Inicio da Simulação
			system("cls");
			horadata(Super);
			printf("\n\n-----------SIMULAÇÃO EM CURSO-----------\n\t---------------------------\n\t| \x1B[36mPRIMA ESPAÇO PARA PARAR\x1B[37m |\n\t---------------------------\n\n");
			mostraArtSuper();
			simulacao(Super, 150);
			menuFim(Super);
			break;
		}
	}
	op = HOLD_OPCAO;
}

//MENU PAUSA
int menuPausa(SUPERMERCADO *Super)
{
	CAIXA *caixa = NULL;
	NO_CLIENTE *cliente = NULL;
	int nrCaixa;
	char buffer[BUFFER];

	while (op)
	{
		system("cls");
		horadata(Super);
		printf("\n\n***********    MENU PAUSA    ***********\n\n");
		printf("1 - CONTINUAR SIMULAÇÃO\n");
		printf("2 - CONCLUIR SIMULAÇÃO\n");
		printf("3 - FECHAR CAIXA\n");
		printf("4 - ABRIR CAIXA\n");
		printf("5 - PESQUISAR CLIENTE\n"); //FALTA IMPLEMENTAR
		printf("6 - RESUMO DE INFORMAÇÕES\n");
		printf("_________________________________________\n\n");
		printf("OPÇÃO: ");
		op = scanfMenu(1, 6); //Devolve apenas valores válidos para op

		switch (op)
		{
		case 1:
			return -1;
			break;
		case 2:
			return 0;
			break;
		case 3:
			//verificar se o numero introduzido corresponde ao range de caixas do Super e se a caixa está aberta para poder ser fechada
			system("cls");
			mostraListaCaixas(Super);

			printf("\n\nTOTAL DE CAIXAS = %d", Super->settings[6]);
			do
			{
				caixa = Super->lCaixa->caixaInicio;

				printf("\nPARA VOLTAR ATRÁS INSIRA (0)");
				printf("\nNUMERO DA CAIXA A ENCERRAR: ");
				nrCaixa = scanfMenu(0, Super->settings[6]);

				while (caixa->idCaixa != nrCaixa && nrCaixa)
					caixa = caixa->caixaProx;
			
				if (caixa->statusCaixa < 2 && caixa->statusCaixa > 3)
				{
					if (nrCaixa)
					{
						nrCaixa = -1;
						printf("\nCAIXA JÁ SE ENCONTRA FECHADA!\n");
					}
				}
			} while (nrCaixa == -1);
			//Apenas devolve numeros de caixa 
			if (nrCaixa)
				encerraCaixaChefe(Super, nrCaixa);
			break;
		case 4://verificar se o numero introduzido corresponde ao range de caixas do Super e se a caixa está fechada
			system("cls");
			mostraListaCaixas(Super);

			printf("\n\nTOTAL DE CAIXAS = %d", Super->settings[6]);
			do
			{
				caixa = Super->lCaixa->caixaInicio;

				printf("\nPARA VOLTAR ATRÁS INSIRA (0)");
				printf("\nNUMERO DA CAIXA A ABRIR: ");
				nrCaixa = scanfMenu(0, Super->settings[6]);

				while (caixa->idCaixa != nrCaixa && nrCaixa)
					caixa = caixa->caixaProx;

				if (caixa->statusCaixa > 1 && caixa->statusCaixa < 4)
				{
					if (nrCaixa)
					{
						nrCaixa = -1;
						printf("\nCAIXA JÁ SE ENCONTRA ABERTA!\n");
					}
				}

			} while (nrCaixa == -1);
			//Apenas devolve numeros de caixa 
			if (nrCaixa)
				abrirCaixaChefe(Super, nrCaixa);
			break;
		case 5:
			system("cls");
			horadata(Super);
			do {
				printf("\n\nINTRODUZA O CÓDIGO DE CLIENTE A PESQUISAR\n\nCÓDIGO: ");
				scanf("%s", buffer);
			} while (strlen(buffer) != 6 || !validaCodigo(buffer)); //Apenas aceita códigos com 6 digitos
			cliente = pesquisaCliente(Super, buffer);

			if (cliente)
			{
				mostraInfoCliente(Super, cliente);
			}
			else
			{
				printf("\n\n\t\x1B[31mCÓDIGO DE CLIENTE NÃO ENCONTRADO!\x1B[37m\n\n");
			}
			pausa();
			break;
		case 6:
			mostraInfoMenuPausa(Super);
			break;
		}
	}
	op = HOLD_OPCAO;
	return -1;
}

//MENU FIM DE SIMULAÇÃO
void menuFim(SUPERMERCADO *Super)
{
	while (op)
	{
		system("cls");
		horadata(Super);
		printf("\n\n*********    FIM DE SIMULAÇÃO    *********\n\n");
		printf("1 - RESULTADOS\n");
		printf("2 - ESTATÍSTICAS\n");
		printf("3 - LISTAGENS\n");
		printf("4 - PESQUISA\n\n");
		printf("5 - GRAVAR SIMULAÇÃO\n\n");
		printf("0 - MENU INICIAL\n");
		printf("__________________________________________\n\n");
		printf("OPÇÃO: ");
		op = scanfMenu(0, 5); //Devolve apenas valores válidos para op

		switch (op)
		{
		case 0:
			break;
		case 1:
			mostraResultadosFim(Super);
			break;
		case 2:
			mostraEstatisticaMenuFim(Super);
			break;
		case 3:
			menuListagens(Super);
			break;
		case 4:
			menuPesquisa(Super);
			break;
		case 5:
			exportarResultados(Super);
			break;
		}
	}
	op = RESET_OPCAO;
}

//MENU DE PESQUISA FINAL
void menuPesquisa(SUPERMERCADO *Super)
{
	CAIXA *caixa = NULL;
	NO_CLIENTE *cliente = NULL;
	NO_FUNCIONARIO *funcionario = NULL;
	NO_PRODUTO *produto = NULL;
	char buffer[BUFFER];
	int i;

	while (op)
	{
		system("cls");
		horadata(Super);
		printf("\n\n*************    PESQUISA    *************\n\n");
		printf("1 - CLIENTE\n");
		printf("2 - FUNCIONÁRIO\n");
		printf("3 - CAIXA\n");
		printf("4 - PRODUTO\n\n");
		printf("0 - VOLTAR\n");
		printf("_________________________________________\n\n");
		printf("OPÇÃO: ");
		op = scanfMenu(0, 4); //Devolve apenas valores válidos para op

		switch (op)
		{
		case 0:
			break;
		case 1:
			system("cls");
			horadata(Super);
			do {
				printf("\n\nINTRODUZA O CÓDIGO DE CLIENTE A PESQUISAR (ex: 123456)\n\nCÓDIGO: ");
				scanf("%s", buffer);
			} while (strlen(buffer) != 6 || !validaCodigo(buffer)); //Apenas aceita códigos com 6 digitos
			cliente = pesquisaCliente(Super, buffer);

			if (cliente)
			{
				mostraInfoCliente(Super, cliente);
			}
			else
			{
				printf("\n\n\t\x1B[31mCÓDIGO DE CLIENTE NÃO ENCONTRADO!\x1B[37m\n\n");
			}
			pausa();
			break;
		case 2:
			system("cls");
			horadata(Super);
			do {
				printf("\n\nINTRODUZA O ID DE FUNCIONÁRIO A PESQUISAR (ex: 1234)\n\nID: ");
				scanf("%s", buffer);
			} while (strlen(buffer) != 4 || !validaCodigo(buffer)); //Apenas aceita códigos com 4 digitos
			funcionario = pesquisaFuncionario(Super, buffer);

			if (funcionario)
			{
				mostraInfoFuncionario(Super, funcionario);
			}
			else
			{
				printf("\n\n\t\x1B[31mFUNCIONÁRIO NÃO ENCONTRADO!\x1B[37m\n\n");
			}
			pausa();
			break;
		case 3:
			system("cls");
			horadata(Super);
			do {
				printf("\n\nINTRODUZA O NUMERO DA CAIXA A PESQUISAR\n\nNUMERO: ");
				scanf("%d", &i);
			} while (i < 1 || i > Super->settings[6]);
			caixa = encontraCaixa(Super, i);

			mostraInfoCaixa(Super, caixa);
			pausa();

			mostraListaClientesCaixa(Super, i);
			pausa();
			break;
		case 4:
			system("cls");
			horadata(Super);
			
			printf("\n\nINTRODUZA O ID DE PRODUTO A PESQUISAR (ex: 1111)\n\nID: ");
			i = scanfMenu(1001, (Super->settings[1] + 1000));
			
			produto = encontraProduto(Super, i);

			mostraInfoProduto(Super, produto);

			pausa();
			break;
		}
	}
	op = HOLD_OPCAO;
}

//MENU DE APRESENTAÇÃO DE LISTAGENS
void menuListagens(SUPERMERCADO *Super)
{
	while (op)
	{
		system("cls");
		horadata(Super);
		printf("\n\n************    LISTAGENS    ************\n\n");
		printf("1 - CLIENTES\n");
		printf("2 - FUNCIONÁRIOS\n");
		printf("3 - PRODUTOS\n");
		printf("4 - CAIXAS\n\n");
		printf("0 - VOLTAR\n");
		printf("_________________________________________\n\n");
		printf("OPÇÃO: ");
		op = scanfMenu(0, 4); //Devolve apenas valores válidos para op

		switch (op)
		{
		case 0:
			break;
		case 1:
			mostraListaGeral(Super);
			pausa();
			break;
		case 2:
			mostraListaFuncionarios(Super);
			pausa();
			break;
		case 3:
			mostraListaProdutos(Super);
			pausa();
			break;
		case 4:
			mostraListaCaixas(Super);
			pausa();
			break;
		}
	}
	op = HOLD_OPCAO;
}

//MENU DEFINIÇÕES
void menuSettings(SUPERMERCADO *Super)
{
	int setts;

	while (op)
	{
		system("cls");
		horadata(Super);
		printf("\n\n*************************************    DEFINIÇÕES    **********************************\n\n");
		printf("1  - TOTAL DE HORAS DE FUNCIONAMENTO [%d]\n", Super->settings[0] / 60 / 60);
		printf("2  - NÚMERO MÁXIMO DE PRODUTOS [%d]\n", Super->settings[1]);
		printf("3  - NÚMERO MÁXIMO DE PRODUTOS COMPRADOS POR CLIENTE [%d]\n", Super->settings[2]);
		printf("4  - MÉDIA DO CUSTO DOS PRODUTOS [%d]\n", Super->settings[3] / 100);
		printf("5  - MÉDIA DO TEMPO DISPENDIDO EM ESCOLHA DE PRODUTOS [%d]\n", Super->settings[4]);
		printf("6  - INTERVALO DE ENTRADA DE CLIENTES [%d]\n", Super->settings[5]);
		printf("7  - NUMERO MAXIMO DE CAIXAS [%d]\n", Super->settings[6]);
		printf("8  - TEMPO MAXIMO DE ESPERA [%d]\n", Super->settings[7]);
		printf("9  - TEMPO MÁXIMO DE UM PRODUTO SER PASSADO NA CAIXA [%d]\n", Super->settings[8]);
		printf("10 - TEMPO MÁXIMO DE PAGAMENTO [%d]\n", Super->settings[9]);
		printf("11 - NUMERO MÉDIO DE CLIENTES EM FILAS DE CAIXA PARA PROCEDER À ABERTURA DE OUTRA [%d]\n", Super->settings[10]);
		printf("12 - NUMERO MÉDIO DE CLIENTES EM FILAS PARA PROCEDER AO FECHO DE CAIXA [%d]\n\n", Super->settings[11]);
		printf("0  - VOLTAR\n");
		printf("_________________________________________________________________________________________\n\n");
		printf("OPÇÃO: ");
		op = scanfMenu(0, 12); //Devolve apenas valores válidos para op

		switch (op)
		{
		case 0:
			break;
		case 1:
			printf("\n\nTOTAL DE HORAS DE FUNCIONAMENTO\n");
			do
			{
				printf("HORAS (MIN 1H / MAX 24H): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 1 || setts > 24);
			Super->settings[op-1] = setts * 60 * 60;
			break;
		case 2:
			printf("\n\nNÚMERO MÁXIMO DE PRODUTOS\n");
			do
			{
				printf("PRODUTOS (MIN 10 / MAX 10000): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 10 || setts > 10000);
			Super->settings[op - 1] = setts;
			break;
		case 3:
			printf("\n\nNÚMERO MÁXIMO DE PRODUTOS COMPRADOS POR CLIENTE\n");
			do
			{
				printf("PRODUTOS (MIN 1 / MAX 100): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 1 || setts > 100);
			Super->settings[op - 1] = setts;
			break;
		case 4:
			printf("\n\nMÉDIA DO PREÇO DOS PRODUTOS\n");
			do
			{
				printf("MÉDIA (MIN 1 EURO / MAX 50 EUROS): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 1 || setts > 50);
			Super->settings[op - 1] = setts * 100; //Passar para centimos
			break;
		case 5:
			printf("\n\nMÉDIA DO TEMPO DISPENDIDO EM ESCOLHA DE PRODUTOS\n");
			do
			{
				printf("MÉDIA (MIN 1s / MAX 120s): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 1 || setts > 120);
			Super->settings[op - 1] = setts;
			break;
		case 6:
			printf("\n\nINTERVALO DE ENTRADA DE CLIENTES\n");
			do
			{
				printf("TEMPO MÁXIMO ENTRE CLIENTES (20s / 500s): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 20 || setts > 500);
			Super->settings[op - 1] = setts;
			break;
		case 7:
			printf("\n\nNUMERO MAXIMO DE CAIXAS\n");
			do
			{
				printf("NUMERO DE CAIXAS (MIN 1 / MAX 30): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 1 || setts > 30);
			Super->settings[op - 1] = setts;
			break;
		case 8:
			printf("\n\nTEMPO MAXIMO DE ESPERA NA CAIXA\n");
			do
			{
				printf("TEMPO (60s / 500s): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 60 || setts > 500);
			Super->settings[op - 1] = setts;
			break;
		case 9:
			printf("\n\nTEMPO MÁXIMO DE UM PRODUTO SER PASSADO NA CAIXA\n");
			do
			{
				printf("TEMPO (2s / 50s): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 2 || setts > 50);
			Super->settings[op - 1] = setts;
			break;
		case 10:
			printf("\n\nTEMPO MÁXIMO DE PAGAMENTO\n");
			do
			{
				printf("TEMPO (10s / 240s): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 10 || setts > 240);
			Super->settings[op - 1] = setts;
			break;
		case 11:
			printf("\n\nNUMERO MÉDIO DE CLIENTES EM FILAS DE CAIXA PARA PROCEDER À ABERTURA DE OUTRA\n");
			do
			{
				printf("NUMERO DE CLIENTES (2 / 15): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 2 || setts > 15);
			Super->settings[op - 1] = setts;
			break;
		case 12:
			printf("\n\nNUMERO MÉDIO DE CLIENTES EM FILAS PARA PROCEDER AO FECHO DE CAIXA\n");
			do
			{
				printf("NUMERO DE CLIENTES (1 / 5): ");
				scanf("%d", &setts);
				limpaInput();
			} while (!setts || setts < 1 || setts > 5);
			Super->settings[op - 1] = setts;
			break;
		}
	}
	op = HOLD_OPCAO;	

	guardaSettings(Super);
}

