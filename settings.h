#if !defined __SETTINGS__
#define __SETTINGS__
#define _CRT_SECURE_NO_WARNINGS

#define BUFFER 255
#define BUFFER_SMALL 30
//Códigos de Erro
#define SEM_ERRO 0
#define ERRO_MEMORIA 1
#define ERRO_FICHEIRO 2
#define ERRO_DADOS 3
//Menus
#define RESET_OPCAO 0
#define HOLD_OPCAO -1

#define MAX_OPCOES 12 //Numero de opções a ler do ficheiro config.txt

#include "fila.h"
#include "list.h"
#include <stdio.h>
#include <locale.h> //uso de caracteres PT-PT
#include <stdlib.h>
#include <malloc.h>
#include <time.h> //uso -> data e hora
#include <string.h>
#include <math.h>
#include <windows.h>
#include <conio.h>

//ESTRUTURAS
//________________________________________________________________________

//----------------------------------------------------------------------//
//Estruturas relativas aos produtos ______________________________________
typedef struct produto
{
	int idProduto;
	double precoProduto;
	double tempoProduto;
}PRODUTO;

typedef struct no_produto
{
	struct no_produto *prox;
	PRODUTO *info_produto;
}NO_PRODUTO;

typedef struct lista_produtos
{
	NO_PRODUTO *inicio;
	int totalProdutos;
	double precoMedio;
	double produtoMaisCaro;
	double valorFinal;
}LISTA_PRODUTOS;

//----------------------------------------------------------------------//
/*
Estrutura que guarda os dados relativos a cada cliente... usada em todo o "percurso" do cliente no supermercado
*/
typedef struct cliente
{
	char idCliente[6];
	char *nomeCliente;
	int idaAoSuper; //0 - Se não for escolhido para ao Super, 1 se for ao Super
	int caixa;
	int nrProdutos;
	int tempoEntradaOffset; // offset de entrada em relação ao cliente anterior
	int tempoEntrada; //Momento de entrada no supermercado
	int tempoCompras; //Momento em que termina de seleccionar os produtos
	int tempoCaixa; //Tempo que demora até sair do supermercado
	int tempoPagamento; //Tempo que demora a passar os produtos na caixa e a pagar
	LISTA_PRODUTOS *talaoCompras;
}CLIENTE;

typedef struct no_cliente
{
	struct no_cliente *prox;
	CLIENTE *info_cliente;
}NO_CLIENTE;
//________________________________________________________________________

//----------------------------------------------------------------------//
//Lista de funcionários __________________________________________________
typedef struct funcionario
{
	char idFuncionario[5];
	char *nome;
	int estadoCX; // 0 - Sem caixa //Os valores diferentes de zero correspondem à caixa na qual se encontra a trabalhar
	int nrClientesAtendidos;
	int nrProdutosProcessados;
	int minutosPorCaixa[BUFFER_SMALL]; //Tempo total de atendimento = soma dos valores diferentes de zero dos indices
}FUNCIONARIO;

typedef struct no_funcionario
{
	struct no_funcionario *prox;
	FUNCIONARIO *info_funcionario;
}NO_FUNCIONARIO;

typedef struct lista_funcionarios
{
	NO_FUNCIONARIO *inicio;
	int nrFuncionarios;
}LISTA_FUNCIONARIOS;
//________________________________________________________________________

//----------------------------------------------------------------------//
//Estruturas relativas às Caixas _________________________________________
typedef struct fila_clientes_caixa
{
	NO_CLIENTE *cabeca;
	NO_CLIENTE *cauda;
	int nrClientesInst;
	//Falta as médias dos tempos e clientes
}FILA_CLIENTES_CAIXA;

typedef struct caixa
{
	FILA_CLIENTES_CAIXA *filaClientes;
	struct caixa *caixaProx;
	int totalClientesCX;
	int idCaixa;
	FUNCIONARIO *funcionario;
	status_caixa statusCaixa;
	int tempoEsperaCaixa; //Somatório de todos os tempos de espera dos clientes na caixa / dividindo pelo totalClientesCX obtemos o tempo médio de espera
	int totalProdutosCX;
	float mediaClientesCX; // desnecessário........
}CAIXA;

typedef struct lista_caixa
{
	CAIXA *caixaInicio;
	int totalClientesCXs;
	int totalCaixas; //Adicionar a àrea respetiva ao gráfico de caixas abertas / dividindo pelo total de tempo passado obtemos o numero médio de caixas abertas
	int nrCXativas;
}LISTA_CAIXA;
//________________________________________________________________________

//----------------------------------------------------------------------//
//Lista de clientes que se encontram nas principais fases da simulação ___
typedef struct lista_compras
{
	NO_CLIENTE *inicio;
	int nrClientesCompras;
}LISTA_COMPRAS;

typedef struct lista_geral
{
	NO_CLIENTE *inicio;
	NO_CLIENTE *fim;
}LISTA_GERAL;

typedef struct lista_final
{
	NO_CLIENTE *inicio;
	int nrClientesFinal;
}LISTA_FINAL;
//________________________________________________________________________

//----------------------------------------------------------------------//
//Estruturas relativas aos eventos _______________________________________
typedef struct evento
{
	tipo_eventos tipoEvento;
	struct evento *noEventoProx;
	NO_CLIENTE *eCliente;
}EVENTO;

typedef struct lista_eventos
{
	EVENTO *inicio;
	int nrEventos;
}LISTA_EVENTOS;
//________________________________________________________________________

//----------------------------------------------------------------------//
/*Estrutura principal do SUPERMERCADO*/
typedef struct supermercado
{
	LISTA_COMPRAS *lCompras; //lista de clientes em processo de escolha de produtos
	LISTA_CAIXA *lCaixa; //lista de caixas de atendimento
	LISTA_GERAL *lGeral; //listagem de todos os clientes
	LISTA_EVENTOS *lEventos; //lista de eventos - *LINHA TEMPORAL*
	LISTA_PRODUTOS *lProdutos; //lista de produtos do supermercado
	LISTA_FINAL *lFinal; //lista de clientes após as compras
	LISTA_FUNCIONARIOS *lFuncionarios; //lista de funcionários
	CLIENTE **arrayClientes; //Lista completa do ficheiro de clientes
	int nrTotalArrayClientes;
	int nrClientesSuper;
	int tempoDecorrido; //Total de unidades de tempo decorrido
	float mediaCxAbertas;
	float mediaClientesSuper;
	int settings[MAX_OPCOES]; //definições da simulação ficheiro "config.txt" - tamanho = numero de definições
	int nrProdutosOferta; //total de produtos oferecidos por execeder tempo de espera definido
	double valorProdutosOferta; //somatório do preço dos produtos oferecidos
	int nrProdutosVendidos;
	double totalFaturado;
	float tempoMedioEspera;
	float tempoMedioNoSuper;
	float mediaCaixasAbertas;
}SUPERMERCADO;


//LISTA DE FUNÇÕES 
//----------------------------------------------------------------------//
void limpaInput();
void horadata(SUPERMERCADO *data);
void pausa();
void pausaBreak(char *x);
void erroDados();
void erroMemoria();
int scanfMenu(int min, int max);
int validaCodigo(char *codigo);
void mostraAjuda(SUPERMERCADO *Super);

void menuInicial(SUPERMERCADO *Super);
void submenuSimulacao(SUPERMERCADO *Super);
int menuPausa(SUPERMERCADO *Super);
void menuFim(SUPERMERCADO *Super);
void menuSettings(SUPERMERCADO *Super);
void menuPesquisa(SUPERMERCADO *Super);
void menuListagens(SUPERMERCADO *Super);

SUPERMERCADO *inicializa();
CLIENTE *criaCliente(SUPERMERCADO *Super);
NO_FUNCIONARIO *criaFuncionario(SUPERMERCADO *Super);
EVENTO *criaEvento(NO_CLIENTE *cliente);
CAIXA *criaCaixa();
NO_PRODUTO *criaProduto(SUPERMERCADO *Super);

void simulacao(SUPERMERCADO *Super, int velocidade);
void limparMemoria(SUPERMERCADO *Super);
EVENTO *calculaProxEvento(SUPERMERCADO *Super);

int contaClientes(char *nomeFicheiro);
void listaGeralClientes(SUPERMERCADO *Super, char *nomeFicheiro);
void mostraListaGeral(SUPERMERCADO *Super);
void listaFuncionarios(SUPERMERCADO *Super, char *nomeFicheiro);
void mostraListaFuncionarios(SUPERMERCADO *Super);
void criaListaCaixas(SUPERMERCADO *Super);
void mostraListaCaixas(SUPERMERCADO *Super);
LISTA_PRODUTOS *criaListaProdutos(SUPERMERCADO *Super);
void mostraListaProdutos(SUPERMERCADO *Super);
LISTA_PRODUTOS *talaoComprasCliente(SUPERMERCADO *Super, NO_CLIENTE *cliente);
void mostraTalaoCompras(SUPERMERCADO *Super, NO_CLIENTE *cliente);
void ofertaProdutos(SUPERMERCADO *Super);
CAIXA *dadosCaixa(SUPERMERCADO *Super, int codigo);
CAIXA *encontraCaixa(SUPERMERCADO *Super, int nrCaixa);
NO_FUNCIONARIO *dadosFuncionario(SUPERMERCADO *Super, int codigo);
void mostraInfoMenuPausa(SUPERMERCADO *Super);
void mostraResultadosFim(SUPERMERCADO *Super);
void mostraEstatisticaMenuFim(SUPERMERCADO *Super);
NO_CLIENTE *pesquisaCliente(SUPERMERCADO *Super, char *idCliente);
void mostraInfoCliente(SUPERMERCADO *Super, NO_CLIENTE *cliente);
NO_FUNCIONARIO *pesquisaFuncionario(SUPERMERCADO *Super, char *idFuncionario);
void mostraInfoFuncionario(SUPERMERCADO *Super, NO_FUNCIONARIO *funcionario);
void mostraInfoCaixa(SUPERMERCADO *Super, CAIXA *caixa);
NO_PRODUTO *encontraProduto(SUPERMERCADO *Super, int idProduto);
void mostraInfoProduto(SUPERMERCADO *Super, NO_PRODUTO *produto);
void mostraListaClientesCaixa(SUPERMERCADO *Super, int nrCaixa);

void insereFimListaGeral(SUPERMERCADO *Super, NO_CLIENTE *cliente);
void insereOrdenadoListaCompras(SUPERMERCADO *Super, EVENTO *evento);
void insereFilaCaixa(SUPERMERCADO *Super, EVENTO *evento, int nrCaixa);
void removeFilaCaixa(SUPERMERCADO *Super, EVENTO *evento);
int seleccionaCaixa(SUPERMERCADO *Super);
void encerraCaixaChefe(SUPERMERCADO *Super, int nrCaixa);
void abrirCaixaChefe(SUPERMERCADO *Super, int nrCaixa);
void insereListaFinal(SUPERMERCADO *Super, EVENTO *evento);
void verificarCaixas(SUPERMERCADO *Super, FILE *historicoCSV, FILE *historicoTXT);
void mudaClienteDeCaixa(SUPERMERCADO *Super);

FUNCIONARIO *escolheFuncionario(SUPERMERCADO *Super, CAIXA *caixa);
void actualizaFuncionario(CAIXA *caixa, EVENTO *evento);
void mudaTurnoFuncionarios(SUPERMERCADO *Super);

void loadSettings(SUPERMERCADO *Super);
void mostraSettings(SUPERMERCADO *Super);
void guardaSettings(SUPERMERCADO *Super);
void exportarResultados(SUPERMERCADO *Super);
void mostraArtSuper();

#endif