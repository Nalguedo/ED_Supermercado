#include "settings.h"

/*
------------------ GESTÃO DE CAIXAS ----------------------
*/
//Seleccionar a caixa que o cliente irá escolher para ser atendido
int seleccionaCaixa(SUPERMERCADO *Super)
{
	LISTA_CAIXA *lCaixa = Super->lCaixa;
	CAIXA *caixa = lCaixa->caixaInicio;
	int i, maxClientes = -1, caixaEscolhida = -1;

	for (i = 0; i < Super->settings[6]; i++)
	{
		if (caixa->statusCaixa != fechada && caixa->statusCaixa != a_fechar && caixa->statusCaixa != fechada_chefe)
		{
			if (maxClientes == -1 || caixa->filaClientes->nrClientesInst < maxClientes)
			{
				maxClientes = caixa->filaClientes->nrClientesInst;
				caixaEscolhida = i + 1;
			}
		}
		caixa = caixa->caixaProx;
	}

	return caixaEscolhida;
}

CAIXA *abrirCaixa(SUPERMERCADO *Super)
{
	LISTA_CAIXA *lCaixa = Super->lCaixa;
	CAIXA *caixa = lCaixa->caixaInicio;

	for (int i = 0; i < Super->settings[6]; i++)
	{
		if (caixa->statusCaixa == fechada_chefe)
		{

		}
		else
			if (caixa->statusCaixa == fechada)
			{
				caixa->statusCaixa = aberta;
				return caixa;
			}
		caixa = caixa->caixaProx;
	}
	return NULL;
}

void verificarCaixas(SUPERMERCADO *Super, FILE *historicoCSV, FILE *historicoTXT)
{
	LISTA_CAIXA *lCaixa = Super->lCaixa;
	CAIXA *caixa = lCaixa->caixaInicio;
	CAIXA *auxCaixa = NULL;

	CAIXA *caixaMenosClientes = NULL;
	int caixasAbertas = 0;
	int totalClientes = 0;
	int minClientes = -1;
	int i, flag = 0;
	float mediaClienteCaixa;

	for (i = 0; i < Super->settings[6]; i++)
	{
		if (caixa->statusCaixa == a_fechar) //Fechar caixas que não têm clientes
		{
			if (caixa->filaClientes->nrClientesInst == 0)
			{
				caixa->statusCaixa = fechada;
				fprintf(historicoCSV, "%d;CAIXA FECHADA;;;%d;;;%s\n", Super->tempoDecorrido, caixa->idCaixa, caixa->funcionario->idFuncionario);
				fprintf(historicoTXT, "%d\tCAIXA FECHADA\t%d\t%s\n", Super->tempoDecorrido, caixa->idCaixa, caixa->funcionario->idFuncionario);
			}
		}
		else if (caixa->statusCaixa != fechada && caixa->statusCaixa != fechada_chefe) //Contar caixas abertas e total de clientes nas filas
		{
			caixasAbertas++;
			totalClientes += caixa->filaClientes->nrClientesInst;

			if (minClientes == -1 || caixa->filaClientes->nrClientesInst < minClientes)
			{
				minClientes = caixa->filaClientes->nrClientesInst;
				caixaMenosClientes = caixa;
			}
		}
		caixa = caixa->caixaProx;
	}

	//Se todas as caixas estiverem fechadas abre uma caixa
	if (!caixasAbertas)
	{
		auxCaixa = lCaixa->caixaInicio;
		for (i = 0; i < Super->settings[6]; i++)
		{
			if (auxCaixa->statusCaixa == fechada)
			{
				auxCaixa->statusCaixa = aberta;
				flag = 1;
			}
			if (flag)
			{
				caixasAbertas++;
				break;
			}
			auxCaixa = auxCaixa->caixaProx;
		}
		if (!caixasAbertas)
		{
			auxCaixa = encontraCaixa(Super, 1);
			auxCaixa->statusCaixa = aberta;
			caixasAbertas++;
		}
	}

	mediaClienteCaixa = (float)totalClientes / caixasAbertas;

	//Criar hipotese de o cliente mudar de caixa se outra fila estiver consideravelmente mais curta do que aquela em que se encontra
	if (mediaClienteCaixa > 5)
		mudaClienteDeCaixa(Super);

	if (mediaClienteCaixa < Super->settings[11] && caixasAbertas > 1)
	{
		caixaMenosClientes->statusCaixa = a_fechar;
	}
	else if (mediaClienteCaixa > Super->settings[10] && caixasAbertas < Super->settings[6])
	{
		CAIXA *caixaAberta = abrirCaixa(Super);

		if (caixaAberta)
		{
			fprintf(historicoCSV, "%d;CAIXA ABERTA;;;%d;;;%s\n", Super->tempoDecorrido, caixaAberta->idCaixa, caixaAberta->funcionario->idFuncionario);
			fprintf(historicoTXT, "%d\tCAIXA ABERTA\t%d\t%s\n", Super->tempoDecorrido, caixaAberta->idCaixa, caixaAberta->funcionario->idFuncionario);
		}
	}
}

void insereFilaCaixa(SUPERMERCADO *Super, EVENTO *evento, int nrCaixa)
{
	CAIXA *caixa = Super->lCaixa->caixaInicio;
	NO_CLIENTE *clienteAux = NULL;

	int i;

	//Tirar o cliente do inicio da lista das compras 
	Super->lCompras->inicio = Super->lCompras->inicio->prox;
	Super->lCompras->nrClientesCompras--;

	//Seleccionar caixa com o numero escolhido (nrCaixa) na lista de caixas
	for (i = 1; i < nrCaixa; i++)
	{
		caixa = caixa->caixaProx;
	}

	//Se a fila estiver vazia
	if (!caixa->filaClientes->cabeca)
	{
		caixa->filaClientes->cabeca = caixa->filaClientes->cauda = evento->eCliente;
		//Ao tempoCompras adicionamos o tempoCompras e o tempoPagamento para obter o tempoCaixa (saída)
		evento->eCliente->info_cliente->tempoCaixa = evento->eCliente->info_cliente->tempoCompras;
		evento->eCliente->info_cliente->tempoCaixa += evento->eCliente->info_cliente->tempoPagamento;
	}
	else
	{	//Insere na cauda
		caixa->filaClientes->cauda->prox = evento->eCliente;
		caixa->filaClientes->cauda = evento->eCliente;

		caixa->filaClientes->cauda->prox = NULL;

		clienteAux = caixa->filaClientes->cabeca;
		evento->eCliente->info_cliente->tempoCaixa = clienteAux->info_cliente->tempoCaixa;

		for (int i = 0; i < caixa->filaClientes->nrClientesInst; i++)
		{
			evento->eCliente->info_cliente->tempoCaixa += clienteAux->info_cliente->tempoPagamento;
			clienteAux = clienteAux->prox;
		}
	}

	caixa->filaClientes->nrClientesInst++;

	Super->lCaixa->totalClientesCXs++;
}

void removeFilaCaixa(SUPERMERCADO *Super, EVENTO *evento)
{
	CAIXA *caixa = Super->lCaixa->caixaInicio;

	int i;

	//Selecciona a caixa em que o cliente está à espera
	for (i = 1; i < evento->eCliente->info_cliente->caixa; i++)
	{
		caixa = caixa->caixaProx;
	}

	if (caixa->filaClientes->cabeca == NULL)
		return;

	//Actualizar dados funcionario à saída
	actualizaFuncionario(caixa, evento);

	//Tirar o cliente da cabeça da fila de espera da caixa
	if (caixa->filaClientes->cabeca == caixa->filaClientes->cauda)
		caixa->filaClientes->cabeca = caixa->filaClientes->cauda = NULL;
	else
		caixa->filaClientes->cabeca = caixa->filaClientes->cabeca->prox;

	caixa->filaClientes->nrClientesInst--;
	caixa->totalProdutosCX += evento->eCliente->info_cliente->nrProdutos;
	caixa->totalClientesCX++;
}

void encerraCaixaChefe(SUPERMERCADO *Super, int nrCaixa)
{
	LISTA_EVENTOS *lEventos = Super->lEventos;
	EVENTO *evento = lEventos->inicio;
	LISTA_CAIXA *lCaixa = Super->lCaixa;
	CAIXA *caixa = lCaixa->caixaInicio;
	NO_CLIENTE *cliente = NULL;
	NO_CLIENTE *auxCliente = NULL;

	int i, nrClientes;

	//Seleccionar caixa a fechar
	caixa = encontraCaixa(Super, nrCaixa);
	//Mudar estado e obrigar a fechar
	caixa->statusCaixa = fechada_chefe;

	cliente = caixa->filaClientes->cabeca;
	nrClientes = caixa->filaClientes->nrClientesInst;

	for (i = 0; i < nrClientes; i++)
	{
		auxCliente = cliente;
		//Tirar o cliente da cabeça da fila de espera da caixa
		if (cliente == caixa->filaClientes->cauda)
		{
			//Volta à lista de compras
			auxCliente->info_cliente->caixa = 0;
			auxCliente->info_cliente->tempoCaixa = 0;
			auxCliente->info_cliente->tempoCompras = Super->tempoDecorrido;
			//Inserir no inicio da lista de Compras
			auxCliente->prox = Super->lCompras->inicio;
			Super->lCompras->inicio = auxCliente;

			Super->lCompras->nrClientesCompras++;

			caixa->filaClientes->cabeca = caixa->filaClientes->cauda = NULL;

		}
		else
		{
			cliente = cliente->prox;

			//Volta à lista de compras
			auxCliente->info_cliente->caixa = 0;
			auxCliente->info_cliente->tempoCaixa = 0;
			auxCliente->info_cliente->tempoCompras = Super->tempoDecorrido;
			//Inserir no inicio da lista de Compras
			auxCliente->prox = Super->lCompras->inicio;
			Super->lCompras->inicio = auxCliente;

			Super->lCompras->nrClientesCompras++;
		}

		caixa->filaClientes->nrClientesInst--;
		Super->lCaixa->totalClientesCXs--;
	}
}

void abrirCaixaChefe(SUPERMERCADO *Super, int nrCaixa)
{
	CAIXA *caixa = NULL;

	//Seleccionar caixa a abrir
	caixa = encontraCaixa(Super, nrCaixa);
	//Mudar estado e obrigar a abrir
	caixa->statusCaixa = aberta;
}

/*Devolve a caixa com mais clientes atendidos ou mais produtos vendidos de acordo com o codigo "passado" na funcao
CODIGO 5 = Caixa com mais clientes na fila*/
CAIXA *dadosCaixa(SUPERMERCADO *Super, int codigo)
{
	LISTA_CAIXA *lCaixa = Super->lCaixa;
	CAIXA *caixa = lCaixa->caixaInicio;
	CAIXA *caixaAux = NULL;
	int i;

	if (codigo == mais_clientes)
	{//Seleccionar caixa
		int maxClientes = 0;
		for (i = 0; i < lCaixa->totalCaixas; i++)
		{
			if (caixa->totalClientesCX > maxClientes)
			{
				maxClientes = caixa->totalClientesCX;
				caixaAux = caixa;
			}
			caixa = caixa->caixaProx;
		}
		return caixaAux;
	}

	if (codigo == mais_produtos)
	{//Seleccionar caixa
		int maxProdutos = 0;
		for (i = 0; i < lCaixa->totalCaixas; i++)
		{
			if (caixa->totalProdutosCX > maxProdutos)
			{
				maxProdutos = caixa->totalProdutosCX;
				caixaAux = caixa;
			}
			caixa = caixa->caixaProx;
		}
		return caixaAux;
	}

	//Caixa com fila mais longa
	if (codigo == 5)
	{//Seleccionar caixa
		int maxClientes = 0;
		for (i = 0; i < lCaixa->totalCaixas; i++)
		{
			if (caixa->filaClientes->nrClientesInst > maxClientes && (caixa->statusCaixa == aberta))
			{
				maxClientes = caixa->filaClientes->nrClientesInst;
				caixaAux = caixa;
			}
			caixa = caixa->caixaProx;
		}
		return caixaAux;
	}

	return NULL;
}

//FUNÇÃO PARA DEVOLVER CAIXA DANDO O NUMERO DA CAIXA
CAIXA *encontraCaixa(SUPERMERCADO *Super, int nrCaixa)
{
	LISTA_CAIXA *lCaixa = Super->lCaixa;
	CAIXA *caixa = lCaixa->caixaInicio;

	int i;

	for (i = 0; i < nrCaixa - 1; i++)
		caixa = caixa->caixaProx;

	return caixa;
}

void mudaClienteDeCaixa(SUPERMERCADO *Super)
{
	LISTA_CAIXA *lCaixa = Super->lCaixa;
	NO_CLIENTE *cliente = NULL;
	NO_CLIENTE *auxCliente = NULL;
	CAIXA *caixa = NULL;

	//Procura caixa com mais clientes na fila
	caixa = dadosCaixa(Super, 5);
	if (!caixa || caixa->filaClientes->nrClientesInst < 3)
		return;

	cliente = caixa->filaClientes->cauda;
	//Se a caixa escolhida for a mesma em que o cliente já se encontra não altera
	if (cliente->info_cliente->caixa == caixa->idCaixa)
		return;

	auxCliente = caixa->filaClientes->cabeca;

	while (auxCliente->prox->prox)
	{
		auxCliente = auxCliente->prox;
	}
	//Tornar o penultimo cliente da fila em ultimo (cauda)
	auxCliente->prox = NULL;
	caixa->filaClientes->cauda = auxCliente;
	
	caixa->filaClientes->nrClientesInst--;
	Super->lCaixa->totalClientesCXs--;

	//Volta à lista de compras
	cliente->info_cliente->caixa = 0;
	cliente->info_cliente->tempoCaixa = 0;
	cliente->info_cliente->tempoCompras = Super->lCompras->inicio->info_cliente->tempoCompras;
	//Inserir no inicio da lista de Compras
	cliente->prox = Super->lCompras->inicio;
	Super->lCompras->inicio = cliente;

	Super->lCompras->nrClientesCompras++;


}