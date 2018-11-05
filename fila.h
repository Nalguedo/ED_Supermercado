#pragma once


//Status Caixas
typedef enum STATUS_CAIXA {
	fechada,
	a_fechar,
	aberta,
	aberta_chefe,
	fechada_chefe
} status_caixa;

//Dados caixas
typedef enum DADOS_CAIXA {
	mais_clientes,
	mais_produtos
} dados_caixa;
