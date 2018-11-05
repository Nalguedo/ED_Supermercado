/*-------------------------------------------|
|PROJETO - ESTRUTURAS DE DADOS 2017/2018	 |
|		"SUPERMARKET SIMULATOR"				 |
|											 |
|Grupo:										 |
|Catarina Morgana	nº 17439				 |
|Pedro Baptista		nº 17058			  	 |
|Pedro Gomes		nº 16839			 	 |
|-------------------------------------------*/

#include "settings.h"

int main()
{
	//Utilização de caracteres PT-PT
	setlocale(LC_ALL, "Portuguese");

	//Inicializa a função rand() - seed time
	time_t t;
	srand((unsigned int)time(&t));

	//Alocar estrutura do Supermercado
	SUPERMERCADO *Super = inicializa();

	menuInicial(Super);

	return 0;
}

//Função principal da simulação, responsável pelo "percurso" de todos os clientes do supermercado
void simulacao(SUPERMERCADO *Super, int velocidade)
{
	LISTA_EVENTOS *lEventos = Super->lEventos;
	LISTA_GERAL *lGeral = Super->lGeral;
	LISTA_COMPRAS *lCompras = Super->lCompras;
	LISTA_CAIXA *lCaixa = Super->lCaixa;
	LISTA_FINAL *lFinal = Super->lFinal;
	LISTA_PRODUTOS *lProdutos = Super->lProdutos;

	int caixaSeleccionada;
	int i, k;

	//Nó de cada evento
	EVENTO *novoEvento = NULL;
	CLIENTE *clienteAux = NULL;

	FILE *historicoCSV = fopen("historico.csv", "w");
	if (!historicoCSV) erroDados();
	FILE *historicoTXT = fopen("historico.txt", "w");
	if (!historicoTXT) erroDados();

	fprintf(historicoCSV, "TEMPO;TIPO DE EVENTO;ID CLIENTE;NOME CLIENTE;NUMERO CAIXA;NR PRODUTOS;PRECO;FUNCIONARIO\n");
	fprintf(historicoTXT, "TEMPO\tTIPO DE EVENTO\tID CLIENTE\tNOME CLIENTE\t\t\tNUMERO CAIXA\tNR PRODUTOS\tPRECO\tFUNCIONARIO\n");

	while (lFinal->nrClientesFinal != Super->nrClientesSuper) //Enquanto a lista final não tiver o numero igual à lista de clientes que vai entrar
	{
		
		//Novo evento

		lEventos->inicio = novoEvento = calculaProxEvento(Super);
		lEventos->nrEventos++;

		//Mudança de turnos nas caixas para o numero de horas em caixa não ser exagerado
		if (lEventos->nrEventos == (Super->nrClientesSuper * 2))
			mudaTurnoFuncionarios(Super);

		verificarCaixas(Super, historicoCSV, historicoTXT);
		clienteAux = novoEvento->eCliente->info_cliente;

		switch (novoEvento->tipoEvento)
		{
		case entrada:

			//Actualizar o relógio da simulação
			Super->tempoDecorrido = clienteAux->tempoEntrada;
			//Cliente passa para a lista de compras
			insereOrdenadoListaCompras(Super, novoEvento);

			//tempo;tipo;id cliente;nome cliente;
			fprintf(historicoCSV, "%d;ENTRADA SUPER;%s;%s\n", Super->tempoDecorrido, clienteAux->idCliente, clienteAux->nomeCliente);
			fprintf(historicoTXT, "%d\tENTRADA SUPER\t%s\t\t%s\n", Super->tempoDecorrido, clienteAux->idCliente, clienteAux->nomeCliente);

			break;

		case compras:

			//Se o cliente não comprar produtos, encaminha diretamente para a lista final sem passar pela caixa
			if (!novoEvento->eCliente->info_cliente->nrProdutos)
			{
				//Tirar o cliente do inicio da lista das compras 
				lCompras->inicio = lCompras->inicio->prox;
				lCompras->nrClientesCompras--;

				insereListaFinal(Super, novoEvento);
				Super->tempoDecorrido = clienteAux->tempoCompras;

				//tempo;tipo;id cliente;nome cliente;
				fprintf(historicoCSV, "%d;SAIDA SUPER;%s;%s\n", Super->tempoDecorrido, clienteAux->idCliente, clienteAux->nomeCliente);
				fprintf(historicoTXT, "%d\tSAIDA SUPER\t%s\t\t%s\n", Super->tempoDecorrido, clienteAux->idCliente, clienteAux->nomeCliente);
				break;
			}

			//Inicia o processo de ida para caixa
			caixaSeleccionada = seleccionaCaixa(Super);
			insereFilaCaixa(Super, novoEvento, caixaSeleccionada);
			//Atribui caixa de atendimento
			novoEvento->eCliente->info_cliente->caixa = caixaSeleccionada;

			Super->tempoDecorrido = novoEvento->eCliente->info_cliente->tempoCompras;

			//tempo;tipo;id cliente;nome cliente;caixa selecionada;nr produtos;preco final
			fprintf(historicoCSV, "%d;ENTRADA CAIXA;%s;%s;%d;%d;%.2lf\n", Super->tempoDecorrido, clienteAux->idCliente, clienteAux->nomeCliente, clienteAux->caixa, clienteAux->nrProdutos, clienteAux->talaoCompras->valorFinal);
			fprintf(historicoTXT, "%d\tENTRADA CAIXA\t%s\t\t%-30s\t%-15d\t%-10d\t%-5.2lf\n", Super->tempoDecorrido, clienteAux->idCliente, clienteAux->nomeCliente, clienteAux->caixa, clienteAux->nrProdutos, clienteAux->talaoCompras->valorFinal);
			break;

		case fila_da_caixa:
			removeFilaCaixa(Super, novoEvento);
			insereListaFinal(Super, novoEvento);
			Super->tempoDecorrido = clienteAux->tempoCaixa;

			//tempo;tipo;id cliente;nome cliente;caixa selecionada;nr produtos;preco final;
			fprintf(historicoCSV, "%d;SAIDA SUPER;%s;%s;%d;%d;%.2lf\n", Super->tempoDecorrido, clienteAux->idCliente, clienteAux->nomeCliente, clienteAux->caixa, clienteAux->nrProdutos, clienteAux->talaoCompras->valorFinal);
			fprintf(historicoTXT, "%d\tSAIDA SUPER\t%s\t\t%-30s\t%-15d\t%-10d\t%-5.2lf\n", Super->tempoDecorrido, clienteAux->idCliente, clienteAux->nomeCliente, clienteAux->caixa, clienteAux->nrProdutos, clienteAux->talaoCompras->valorFinal);
			break;
		}
		if (velocidade > 0)
		{
			int seconds, hours, minutes;
			seconds = Super->tempoDecorrido % 60;
			minutes = (Super->tempoDecorrido / 60) % 60;
			hours = (Super->tempoDecorrido / 60 / 60) % 60;
			printf("  %02d:%02d:%02d  \r", hours, minutes, seconds);
			Sleep(velocidade);
			if (_kbhit())
				if (_getch() == ' ') {
					k = menuPausa(Super);
					if (k)
					{
						system("cls");
						horadata(Super);
						printf("\n\n-----------SIMULAÇÃO EM CURSO-----------\n\t---------------------------\n\t| \x1B[36mPRIMA ESPAÇO PARA PARAR\x1B[37m |\n\t---------------------------\n\n");
						mostraArtSuper();
					}
					if (!k)
						velocidade = 0;
				}
		}
		//Libertar o evento, não implementamos lista de eventos
		free(novoEvento->noEventoProx);
		free(novoEvento);
	}

	//No fim da simulação libertamos o inicio da lista de compras porque fica vazia
	free(Super->lCompras->inicio);
	Super->lCompras->inicio = NULL;

	//Fechar o supermercado
	CAIXA *caixa = lCaixa->caixaInicio;

	for (i = 0; i < Super->settings[6]; i++)
	{
		//Verificar que todas as caixas estão vazias e podem ser encerradas
		if (caixa->statusCaixa != fechada && !caixa->filaClientes->cabeca && !caixa->filaClientes->nrClientesInst)
			caixa->statusCaixa = fechada;

		caixa = caixa->caixaProx;
	}

	//Calcular os produtos oferecidos e o seu valor total
	ofertaProdutos(Super);

	fclose(historicoCSV);
	fclose(historicoTXT);
}
