#include "settings.h"

SUPERMERCADO *inicializa()
{
	char ficheiroClientes[] = "./data/clientes.txt";
	char ficheiroFuncionarios[] = "./data/funcionarios.txt";
	char ficheiroProdutos[] = "./data/produtos.txt";

	//Alocar estrutura Global do SUPERMERCADO
	SUPERMERCADO *Super = (SUPERMERCADO *)malloc(sizeof(SUPERMERCADO));
	if (!Super) erroMemoria();

	//Ler definições da simulação do ficheiro config.txt
	loadSettings(Super);
	
	//Iniciar variáveis de controlo
	Super->nrClientesSuper = 0;
	Super->tempoDecorrido = 0;
	Super->nrProdutosOferta = 0;
	Super->valorProdutosOferta = 0;
	Super->nrProdutosVendidos = 0;
	Super->totalFaturado = 0;
	Super->tempoMedioEspera = 0;
	Super->tempoMedioNoSuper = 0;
	Super->mediaCaixasAbertas = 0;
	//Alocar as diferentes listas
	//Caixas
	Super->lCaixa = (LISTA_CAIXA *)malloc(sizeof(LISTA_CAIXA));
	if (!Super->lCaixa) erroMemoria();
	Super->lCaixa->caixaInicio = NULL;
	Super->lCaixa->nrCXativas = 0;
	Super->lCaixa->totalCaixas = Super->settings[6];
	Super->lCaixa->totalClientesCXs = 0;
	Super->mediaCxAbertas = 0;
	Super->mediaClientesSuper = 0;

	//Clientes em fase de compras
	Super->lCompras = (LISTA_COMPRAS *)malloc(sizeof(LISTA_COMPRAS));
	if (!Super->lCompras) erroMemoria();
	Super->lCompras->inicio = NULL;
	Super->lCompras->nrClientesCompras = 0;

	//Lista completa de clientes
	Super->lGeral = (LISTA_GERAL *)malloc(sizeof(LISTA_GERAL));
	if (!Super->lGeral) erroMemoria();
	Super->lGeral->inicio = NULL;
	Super->lGeral->fim = NULL;

	//Lista de eventos - timeline
	Super->lEventos = (LISTA_EVENTOS *)malloc(sizeof(LISTA_EVENTOS));
	if (!Super->lEventos) erroMemoria();
	Super->lEventos->inicio = NULL;
	Super->lEventos->nrEventos = 0;

	//Lista de produtos (USADA PARA CRIAR UMA LISTA COMPLETA DE PRODUTOS DO SUPERMERCADO)
	Super->lProdutos = (LISTA_PRODUTOS *)malloc(sizeof(LISTA_PRODUTOS));
	if (!Super->lProdutos) erroMemoria();
	Super->lProdutos->inicio = NULL;
	Super->lProdutos->totalProdutos = 0;
	Super->lProdutos->precoMedio = 0;
	Super->lProdutos->produtoMaisCaro = 0;
	Super->lProdutos->valorFinal = 0;

	//Lista completa de clientes depois da simulação
	Super->lFinal = (LISTA_FINAL *)malloc(sizeof(LISTA_FINAL));
	if (!Super->lFinal) erroMemoria();
	Super->lFinal->inicio = NULL;
	Super->lFinal->nrClientesFinal = 0;

	//Lista de funcionários
	Super->lFuncionarios = (LISTA_FUNCIONARIOS *)malloc(sizeof(LISTA_FUNCIONARIOS));
	if (!Super->lFuncionarios) erroMemoria();
	Super->lFuncionarios->inicio = NULL;
	Super->lFuncionarios->nrFuncionarios = 0;

	//Criar lista de produtos
	criaListaProdutos(Super);

	//Criar lista completa de clientes
	listaGeralClientes(Super, ficheiroClientes);

	//Criar lista de Funcionários
	listaFuncionarios(Super, ficheiroFuncionarios);

	//Criar lista de caixas
	criaListaCaixas(Super);

	return Super;
}

CLIENTE *criaCliente(SUPERMERCADO *Super)
{
	CLIENTE *cliente = (CLIENTE *)malloc(sizeof(CLIENTE));
	if (!cliente) erroMemoria();

	cliente->caixa = 0;
	cliente->nomeCliente = NULL;
	cliente->idaAoSuper = naoVai;
	cliente->talaoCompras = NULL;
	cliente->nrProdutos = rand() % Super->settings[2] + 0; //GERAR NUMERO RANDOM
	cliente->tempoEntradaOffset = rand() % Super->settings[5] + 1; //GERAR NUMERO RANDOM para substituir o zero pelo tempo até prox cliente
	cliente->tempoEntrada = 0;
	cliente->tempoCompras = 0; //Somatório dos tempos de compras de cada produto (talaoCompras) em segundos
	cliente->tempoCaixa = 0; //Será o somatórios dos tempos de espera dos clientes à frente na fila da caixa, a somar com o tempo de passagem de produtos
	cliente->tempoPagamento = ((rand() % Super->settings[8] + 2) * cliente->nrProdutos) + (rand() % Super->settings[9] + 30); //Numero de produtos a multiplicar por um tempo aleatório médio de passagem de produtos

	return cliente;
}

NO_FUNCIONARIO *criaFuncionario(SUPERMERCADO *Super)
{
	NO_FUNCIONARIO *noFuncionario = (NO_FUNCIONARIO *)malloc(sizeof(NO_FUNCIONARIO));
	if (!noFuncionario) erroMemoria();
	noFuncionario->info_funcionario = (FUNCIONARIO *)malloc(sizeof(FUNCIONARIO));
	if (!noFuncionario->info_funcionario) erroMemoria();

	noFuncionario->prox = NULL;

	noFuncionario->info_funcionario->estadoCX = 0;
	noFuncionario->info_funcionario->nrClientesAtendidos = 0;
	noFuncionario->info_funcionario->nrProdutosProcessados = 0;

	for (int i = 0; i < Super->settings[6]; i++)
	{
		noFuncionario->info_funcionario->minutosPorCaixa[i] = 0;
	}

	return noFuncionario;
}

EVENTO *criaEvento(NO_CLIENTE *cliente)
{
	EVENTO *evento = (EVENTO *)malloc(sizeof(EVENTO));
	if (!evento) erroMemoria();

	evento->eCliente = cliente;
	evento->tipoEvento = em_espera;
	evento->noEventoProx = NULL;

	return evento;
}

CAIXA *criaCaixa()
{
	CAIXA *novaCX = (CAIXA *)malloc(sizeof(CAIXA));
	if (!novaCX) erroMemoria();
	FILA_CLIENTES_CAIXA *filaCX = (FILA_CLIENTES_CAIXA *)malloc(sizeof(FILA_CLIENTES_CAIXA));
	if (!filaCX) erroMemoria();

	novaCX->filaClientes = filaCX;
	novaCX->filaClientes->cabeca = NULL;
	novaCX->filaClientes->cauda = NULL;
	novaCX->filaClientes->nrClientesInst = 0;
	novaCX->caixaProx = NULL;
	novaCX->idCaixa = 0;
	novaCX->statusCaixa = fechada;
	novaCX->totalClientesCX = 0;
	novaCX->tempoEsperaCaixa = 0;
	novaCX->totalProdutosCX = 0;
	novaCX->mediaClientesCX = 0;

	return novaCX;
}

NO_PRODUTO *criaProduto(SUPERMERCADO *Super)
{
	NO_PRODUTO *novoProduto = (NO_PRODUTO *)malloc(sizeof(NO_PRODUTO));
	if (!novoProduto) erroMemoria();
	novoProduto->info_produto = (PRODUTO *)malloc(sizeof(PRODUTO));
	if (!novoProduto->info_produto) erroMemoria();

	double U;

	////Distribuição exponencial negativa
	U = (rand() * 1.0 / RAND_MAX);
	novoProduto->info_produto->precoProduto = (log(U)*(-Super->settings[3]) + 1) / 100; //Média 5 euros minimo 1 centimo

																						//Gerar preço aleatório (ALTERNATIVA)
																						//novoProduto->info_produto->precoProduto = ((rand() % 10000 + 10) / (rand() % 9 + 1) / 100.00); //GERAR NUMERO RANDOM
																						//novoProduto->info_produto->idProduto = idProduto++;

																						//Distribuição exponencial negativa
	U = (rand() * 1.0 / RAND_MAX);
	novoProduto->info_produto->tempoProduto = (log(U)*(-Super->settings[4])) + 1; //Média 20 segundos minimo 1 segundo
																				  //Tempo de produto (ALTERNATIVA)
																				  //novoProduto->tempoProduto = rand() % 45 + 5;
	novoProduto->prox = NULL;

	return novoProduto;
}

EVENTO *calculaProxEvento(SUPERMERCADO *Super)
{
	EVENTO *evento = NULL;
	LISTA_EVENTOS *lEventos = Super->lEventos;
	CAIXA *caixa = Super->lCaixa->caixaInicio;

	int tempoEvento = MAXINT32; //O valor será actualizado com o tempoProxEvento dos primeiros clientes de cada lista
	int caixasAbertas = 0;
	int i;
	tipo_eventos tipo_evento = em_espera;

	//Verificar se o próximo evento está na lista geral
	if (Super->lGeral->inicio)
	{
		evento = criaEvento(Super->lGeral->inicio);
		tempoEvento = evento->eCliente->info_cliente->tempoEntrada;
		tipo_evento = entrada;
	}

	//Verificar se o próximo evento está na lista de compras
	if (Super->lCompras->inicio && (Super->lCompras->inicio->info_cliente->tempoCompras < tempoEvento))
	{
		if (!evento)
			evento = criaEvento(Super->lCompras->inicio);

		evento->eCliente = Super->lCompras->inicio;

		tempoEvento = evento->eCliente->info_cliente->tempoCompras;
		tipo_evento = compras;
	}

	//Verificar se o próximo evento está na fila de espera de alguma das caixas
	for (i = 0; i < Super->settings[6]; i++)
	{
		//Contar numero de caixas abertas
		if (caixa->statusCaixa == aberta)
			caixasAbertas++;

		if (caixa->filaClientes->cabeca && (caixa->filaClientes->cabeca->info_cliente->tempoCaixa < tempoEvento))
		{
			if (!evento)
				evento = criaEvento(caixa->filaClientes->cabeca);

			evento->eCliente = caixa->filaClientes->cabeca;

			tempoEvento = evento->eCliente->info_cliente->tempoCaixa;

			tipo_evento = fila_da_caixa;
		}
		caixa = caixa->caixaProx;
	}

	//Actualizar somatório para média ponderada das caixas abertas
	Super->mediaCaixasAbertas += caixasAbertas * (tempoEvento - Super->tempoDecorrido);

	evento->tipoEvento = tipo_evento;
	return evento;
}

//Contar o total de clientes do ficheiro clientes.txt
int contaClientes(char *nomeFicheiro)
{
	FILE *fClientes = fopen(nomeFicheiro, "r");
	if (!fClientes) erroDados();

	char buffer[BUFFER];
	int totalClientes = 0;

	while (fgets(buffer, BUFFER, fClientes))
	{
		totalClientes++;
	}

	fclose(fClientes);
	return totalClientes;
}

//Lê o ficheiro de cliente e copia para lista geral de clientes - A lista geral fica completa!
void listaGeralClientes(SUPERMERCADO *Super, char *nomeFicheiro)
{
	char buffer[BUFFER], nome[BUFFER];
	int index = 0, tempoTotal = 0;
	LISTA_GERAL *listaClientes = Super->lGeral;
	NO_CLIENTE *cliNovo = NULL;

	//Calcular o tamanho do array de clientes
	Super->nrTotalArrayClientes = contaClientes(nomeFicheiro);

	Super->arrayClientes = (CLIENTE **)malloc(sizeof(CLIENTE *) * Super->nrTotalArrayClientes);
	if (!Super->arrayClientes) erroMemoria();

	FILE *fClientes = fopen(nomeFicheiro, "r");
	if (!fClientes) erroDados();

	while (fgets(buffer, BUFFER, fClientes))
	{
		Super->arrayClientes[index] = criaCliente(Super);

		//Leitura para string de cada linha, os campos separados por "TAB" são apontados para as respetivas variáveis na estrutura
		sscanf(buffer, "%[^\t]\t%[^\n]", Super->arrayClientes[index]->idCliente, nome);

		//String "nome" é alocada dinamicamente
		Super->arrayClientes[index]->nomeCliente = (char *)malloc(sizeof(char) * (strlen(nome) + 1));
		strcpy(Super->arrayClientes[index]->nomeCliente, nome);

		index++;
	}

	fclose(fClientes);

	while (tempoTotal < Super->settings[0] - 600) //Deixar entrar clientes até 10 min antes do fecho da loja
	{
		do
		{
			index = rand() % Super->nrTotalArrayClientes;
		} while (Super->arrayClientes[index]->idaAoSuper);

		tempoTotal += Super->arrayClientes[index]->tempoEntradaOffset;

		cliNovo = (NO_CLIENTE *)malloc(sizeof(NO_CLIENTE));
		if (!cliNovo) erroMemoria();

		cliNovo->prox = NULL;
		cliNovo->info_cliente = Super->arrayClientes[index];

		//Seleccionado para ir ao Supermercado
		cliNovo->info_cliente->idaAoSuper = 1;

		//Ordem de entrada no supermercado
		cliNovo->info_cliente->tempoEntrada = tempoTotal;

		//Cria talão do cliente
		cliNovo->info_cliente->talaoCompras = talaoComprasCliente(Super, cliNovo);

		insereFimListaGeral(Super, cliNovo);

		//Incrementa o numero de clientes
		Super->nrClientesSuper++;
	}
}

//Cria lista de caixas do Supermercado e inicia as respetivas filas
void criaListaCaixas(SUPERMERCADO *Super)
{
	int i;
	LISTA_CAIXA *listaCX = Super->lCaixa;
	CAIXA *novaCaixa = NULL;

	for (i = 1; i <= Super->settings[6]; i++)
	{
		if (!listaCX->caixaInicio)
		{
			listaCX->caixaInicio = novaCaixa = criaCaixa();
		}
		else
		{
			novaCaixa->caixaProx = criaCaixa();
			novaCaixa = novaCaixa->caixaProx;
		}

		novaCaixa->idCaixa = i;
		novaCaixa->funcionario = escolheFuncionario(Super, novaCaixa);
	}
}

//Cria lista de produtos com id [1 até MAX_PRODUTOS] / Preço aleatório e tempo de compra aleatório
LISTA_PRODUTOS *criaListaProdutos(SUPERMERCADO *Super)
{
	int i;
	LISTA_PRODUTOS *listaProdutos = Super->lProdutos;
	if (!listaProdutos) erroMemoria();
	listaProdutos->totalProdutos = Super->settings[1];
	NO_PRODUTO *novoProduto = NULL;
	int idProduto = 1001;
	//Ciclo para gerar o numero de produtos indicado
	for (i = 1; i <= Super->settings[1]; i++)
	{
		if (!listaProdutos->inicio)
		{
			listaProdutos->inicio = novoProduto = criaProduto(Super);
			novoProduto->info_produto->idProduto = idProduto++;
		}
		else
		{
			novoProduto->prox = criaProduto(Super);
			novoProduto = novoProduto->prox;
			novoProduto->info_produto->idProduto = idProduto++;
		}

		if (novoProduto->info_produto->precoProduto > listaProdutos->produtoMaisCaro)
			listaProdutos->produtoMaisCaro = novoProduto->info_produto->precoProduto;

		listaProdutos->precoMedio += novoProduto->info_produto->precoProduto;
	}

	listaProdutos->valorFinal = listaProdutos->precoMedio;
	listaProdutos->precoMedio = (listaProdutos->precoMedio / listaProdutos->totalProdutos);

	return listaProdutos;
}

//Cria lista de compras (talao) do cliente, seleccionando os produtos da lista de produtos (stock) da loja
LISTA_PRODUTOS *talaoComprasCliente(SUPERMERCADO *Super, NO_CLIENTE *cliente)
{
	LISTA_PRODUTOS *talaoCliente = (LISTA_PRODUTOS *)malloc(sizeof(LISTA_PRODUTOS));
	if (!talaoCliente) erroMemoria();

	talaoCliente->precoMedio = 0;
	talaoCliente->produtoMaisCaro = 0;
	talaoCliente->totalProdutos = cliente->info_cliente->nrProdutos;

	NO_PRODUTO *itemStock = Super->lProdutos->inicio;
	NO_PRODUTO *produtoLista = NULL;

	int i, nrProduto, k;

	for (i = 0; i < cliente->info_cliente->nrProdutos; i++)
	{
		itemStock = Super->lProdutos->inicio;

		k = 1;

		nrProduto = rand() % Super->settings[1] + 1;
		//Percorre o stock da loja até ao numero de produto gerado aleatóriamente
		while (k != nrProduto)
		{
			k++;
			itemStock = itemStock->prox;
		}

		if (!i)
		{
			produtoLista = (NO_PRODUTO *)malloc(sizeof(NO_PRODUTO));
			produtoLista->info_produto = itemStock->info_produto;
			talaoCliente->inicio = produtoLista;
			produtoLista->prox = NULL;
		}
		else
		{
			produtoLista->prox = (NO_PRODUTO *)malloc(sizeof(NO_PRODUTO));
			produtoLista = produtoLista->prox;
			produtoLista->info_produto = itemStock->info_produto;
			produtoLista->prox = NULL;
		}

		if (produtoLista->info_produto->precoProduto > talaoCliente->produtoMaisCaro)
			talaoCliente->produtoMaisCaro = produtoLista->info_produto->precoProduto;

		talaoCliente->precoMedio += produtoLista->info_produto->precoProduto;
		cliente->info_cliente->tempoCompras += (int)produtoLista->info_produto->tempoProduto;
	}

	//Se o cliente sair sem comprar nada, somar tempo aleatório entre 2minutos e 5minutos que corresponde ao tempo de permanencia no supermercado
	if (!cliente->info_cliente->nrProdutos)
		cliente->info_cliente->tempoCompras = rand() % 300 + 120;

	//Calcular o momento em que o cliente acaba as compras e passa para a caixa
	cliente->info_cliente->tempoCompras += cliente->info_cliente->tempoEntrada;

	if (!i)
		talaoCliente->inicio = NULL;

	talaoCliente->valorFinal = talaoCliente->precoMedio;
	talaoCliente->precoMedio = talaoCliente->precoMedio / talaoCliente->totalProdutos;

	return talaoCliente;
}

//Lê o ficheiro de funcionários e copia para lista de funcionários
void listaFuncionarios(SUPERMERCADO *Super, char *nomeFicheiro)
{
	char buffer[BUFFER], nome[BUFFER];
	LISTA_FUNCIONARIOS *listaFuncionarios = Super->lFuncionarios;
	NO_FUNCIONARIO *novoFuncionario = NULL;

	FILE *fFuncionarios = fopen(nomeFicheiro, "r");
	if (!fFuncionarios) erroDados();

	while (fgets(buffer, BUFFER, fFuncionarios))
	{
		if (!listaFuncionarios->inicio)
		{
			listaFuncionarios->inicio = novoFuncionario = criaFuncionario(Super);
		}
		else
		{
			novoFuncionario->prox = criaFuncionario(Super);
			novoFuncionario = novoFuncionario->prox;
		}

		//Leitura para string de cada linha, os campos separados por "TAB" são apontados para as respetivas variáveis na estrutura
		sscanf(buffer, "%[^\t]\t%[^\n]", novoFuncionario->info_funcionario->idFuncionario, nome);

		//String "nome" é alocada dinamicamente
		novoFuncionario->info_funcionario->nome = (char *)malloc(sizeof(char) * (strlen(nome) + 1));
		strcpy(novoFuncionario->info_funcionario->nome, nome);

		//Incrementa o numero de funcionários
		listaFuncionarios->nrFuncionarios++;
	}
	fclose(fFuncionarios);
}

void insereFimListaGeral(SUPERMERCADO *Super, NO_CLIENTE *cliente)
{
	LISTA_GERAL *lGeral = Super->lGeral;

	if (!lGeral->inicio)
	{
		lGeral->inicio = lGeral->fim = cliente;
		lGeral->fim->prox = NULL;
	}
	else
	{
		lGeral->fim->prox = cliente;
		lGeral->fim = cliente;
	}
}

void insereOrdenadoListaCompras(SUPERMERCADO *Super, EVENTO *evento)
{
	NO_CLIENTE *clienteAnt = NULL, *clienteAct = NULL;
	int flagSearch = 1;


	//Tirar o cliente do inicio da lista geral 
	Super->lGeral->inicio = Super->lGeral->inicio->prox;

	//Colocar por ordem do tempo do próximo evento na lista das compras
	if (!Super->lCompras->inicio)
	{
		Super->lCompras->inicio = evento->eCliente;
		evento->eCliente->prox = NULL;
	}
	else
	{
		//Introduzir cliente ordenado
		clienteAnt = clienteAct = Super->lCompras->inicio;
		while (flagSearch)
		{
			if (!clienteAct)
				flagSearch = 0;
			else
			{
				if (clienteAct->info_cliente->tempoCompras >= evento->eCliente->info_cliente->tempoCompras)
					flagSearch = 0;
				else
				{
					clienteAnt = clienteAct;
					clienteAct = clienteAct->prox;
				}
			}
		}

		if (clienteAct == Super->lCompras->inicio) //Insere no inicio
		{
			evento->eCliente->prox = Super->lCompras->inicio;
			Super->lCompras->inicio = evento->eCliente;
		}
		else //Insere no meio ou no fim
		{
			evento->eCliente->prox = clienteAct;
			clienteAnt->prox = evento->eCliente;
		}
	}
	Super->lCompras->nrClientesCompras++;
}

//Saída do cliente do supermercado, genericamente será depois de ser atendido, no entanto, 
//se o nrProdutos comprado for zero, deverá passar para a lista final sem passar pelas caixas
void insereListaFinal(SUPERMERCADO *Super, EVENTO *evento)
{
	LISTA_FINAL *lFinal = Super->lFinal;

	int tempoEspera = 0;
	int tempoNoSuper = 0;

	if (evento->eCliente->info_cliente->nrProdutos)
	{
		tempoEspera = evento->eCliente->info_cliente->tempoCaixa - evento->eCliente->info_cliente->tempoCompras;
		tempoNoSuper = evento->eCliente->info_cliente->tempoCaixa - evento->eCliente->info_cliente->tempoEntrada;
	}

	evento->eCliente->prox = lFinal->inicio;
	lFinal->inicio = evento->eCliente;

	lFinal->nrClientesFinal++;
	//Calcular resultados a apresentar
	Super->nrProdutosVendidos += evento->eCliente->info_cliente->nrProdutos;
	Super->totalFaturado += evento->eCliente->info_cliente->talaoCompras->valorFinal;
	Super->tempoMedioEspera += tempoEspera;
	Super->tempoMedioNoSuper += tempoNoSuper;
	//TODO: CRIAR ARRAY COM LISTA COMPLETA DE CLIENTES FINAL PARA FÁCIL ORDENAÇÃO
}

void ofertaProdutos(SUPERMERCADO *Super)
{
	LISTA_FINAL *lFinal = Super->lFinal;
	NO_CLIENTE *cliente = NULL;
	NO_PRODUTO *produto = NULL;

	int i, j, index;
	int maxTempo = Super->settings[7];

	cliente = lFinal->inicio;

	for (i = 0; i < lFinal->nrClientesFinal; i++)
	{
		if ((cliente->info_cliente->tempoCaixa - cliente->info_cliente->tempoCompras) > maxTempo)
		{
			index = rand() % cliente->info_cliente->nrProdutos; //Produto aleatório do carrinho oferecido
			produto = cliente->info_cliente->talaoCompras->inicio;
			for (j = 0; j < index; j++)
			{
				produto = produto->prox;
			}

			Super->nrProdutosOferta++;
			Super->valorProdutosOferta += produto->info_produto->precoProduto;
		}
		cliente = cliente->prox;
	}
}

NO_FUNCIONARIO *dadosFuncionario(SUPERMERCADO *Super, int codigo)
{
	LISTA_FUNCIONARIOS *lFuncionarios = Super->lFuncionarios;
	NO_FUNCIONARIO *funcionario = lFuncionarios->inicio;
	NO_FUNCIONARIO *auxFuncionario = NULL;

	int i;

	if (codigo == menos_clientes_F)
	{
		int minClientes = 9999;
		for (i = 0; i < lFuncionarios->nrFuncionarios; i++)
		{
			if (funcionario->info_funcionario->nrClientesAtendidos && funcionario->info_funcionario->nrClientesAtendidos < minClientes)
			{
				minClientes = funcionario->info_funcionario->nrClientesAtendidos;
				auxFuncionario = funcionario;
			}
			funcionario = funcionario->prox;
		}
		return auxFuncionario;
	}

	if (codigo == mais_clientes_F)
	{
		int maxClientes = 0;
		for (i = 0; i < lFuncionarios->nrFuncionarios; i++)
		{
			if (funcionario->info_funcionario->nrClientesAtendidos > maxClientes)
			{
				maxClientes = funcionario->info_funcionario->nrClientesAtendidos;
				auxFuncionario = funcionario;
			}
			funcionario = funcionario->prox;
		}
		return auxFuncionario;
	}

	return NULL;
}

NO_CLIENTE *pesquisaCliente(SUPERMERCADO *Super, char *idCliente)
{
	LISTA_GERAL *lGeral = Super->lGeral;
	LISTA_COMPRAS *lCompras = Super->lCompras;
	LISTA_CAIXA *lCaixa = Super->lCaixa;
	LISTA_FINAL *lFinal = Super->lFinal;

	NO_CLIENTE *cliente = NULL;
	CAIXA *caixa = lCaixa->caixaInicio;

	int i;

	cliente = lGeral->inicio;
	//Percorre a lista geral
	while (cliente)
	{
		if (strcmp(cliente->info_cliente->idCliente, idCliente) == 0)
			return cliente;
		cliente = cliente->prox;
	}

	cliente = lCompras->inicio;
	//Percorre a lista de compras
	while (cliente)
	{
		if (strcmp(cliente->info_cliente->idCliente, idCliente) == 0)
			return cliente;
		cliente = cliente->prox;
	}

	cliente = caixa->filaClientes->cabeca;
	//Percorrer a lista de caixas e respetivas filas
	for (i = 0; i < Super->settings[6]; i++) //setts[6] numero de caixas
	{
		while (cliente)
		{
			if (strcmp(cliente->info_cliente->idCliente, idCliente) == 0)
				return cliente;
			cliente = cliente->prox;
		}

		if (i != (Super->settings[6] - 1))
		{
			caixa = caixa->caixaProx;
			cliente = caixa->filaClientes->cabeca;
		}
	}

	cliente = lFinal->inicio;
	//Percorre a lista final
	while (cliente)
	{
		if (strcmp(cliente->info_cliente->idCliente, idCliente) == 0)
			return cliente;
		cliente = cliente->prox;
	}

	return NULL;
}

NO_FUNCIONARIO *pesquisaFuncionario(SUPERMERCADO *Super, char *idFuncionario)
{
	LISTA_FUNCIONARIOS *lFuncionarios = Super->lFuncionarios;

	NO_FUNCIONARIO *funcionario = NULL;

	funcionario = lFuncionarios->inicio;
	//Percorre a lista de funcionarios
	while (funcionario)
	{
		if (strcmp(funcionario->info_funcionario->idFuncionario, idFuncionario) == 0)
			return funcionario;
		funcionario = funcionario->prox;
	}

	return NULL;
}

NO_PRODUTO *encontraProduto(SUPERMERCADO *Super, int idProduto)
{
	LISTA_PRODUTOS *lProdutos = Super->lProdutos;

	NO_PRODUTO *produto = NULL;

	produto = lProdutos->inicio;
	//Percorre a lista de produtos
	while (produto)
	{
		if (produto->info_produto->idProduto == idProduto)
			return produto;
		produto = produto->prox;
	}

	return NULL;
}

FUNCIONARIO *escolheFuncionario(SUPERMERCADO *Super, CAIXA *caixa)
{
	int index;
	NO_FUNCIONARIO *noFuncionario = Super->lFuncionarios->inicio;

	//Seleccionar um numero de funcionario aleatorio da lista de funcionarios
	do
	{	//Numero de funcionário aleatório
		index = rand() % Super->lFuncionarios->nrFuncionarios;
		noFuncionario = Super->lFuncionarios->inicio;
		for (int i = 1; i <= index; i++)
		{
			noFuncionario = noFuncionario->prox;
		}
	} while (noFuncionario->info_funcionario->estadoCX); //Se já está atribuido a uma caixa selecciona outro

	noFuncionario->info_funcionario->estadoCX = caixa->idCaixa;

	return noFuncionario->info_funcionario;
}
