#pragma once

//Evento dos Clientes
typedef enum TIPO_EVENTOS {
	em_espera,
	entrada,
	compras,
	fila_da_caixa,
	pagamento,
	fim_de_compras
} tipo_eventos;

//Status Ida ao Super
typedef enum IDA_AO_SUPER {
	naoVai,
	vai
} ida_ao_super;

typedef enum DADOS_FUNCIONARIO {
	menos_clientes_F,
	mais_clientes_F
} dados_funcionario;