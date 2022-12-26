
enum Estado {
	st_aguardando, 
	st_fila1, st_servico1, 
	st_fila2, st_fila2_retry, st_servico2,
	st_finalizado
};

typedef struct {
	Estado estado = st_aguardando;

	double tp_chegada;    // gerado aleatorio
	double tp_servico1;   // gerado aleatorio
	double tp_servico2;   // gerado aleatorio

	double tp_chegada_fila1;
	double tp_chegada_fila2;
	double tp_inicio_servico1;
	double tp_inicio_servico2;
	double tp_acumulado_servico2 = 0.0;
	double tp_final_servico1;
	double tp_final_servico2;
} Evento;