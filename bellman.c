/*
 * Trabalho de Projeto e Análise de Algoritmo
 * Mestrado em Ciência da Computação - Turma 16.1
 *
 * Alunos (nome, matricula, e-mail):
 * 	Conrado
 * 	Danilo Santos Souza                16.1.10149 - danilo.gdc@gmail.com
 * 	Rodolfo Labiapari Mansur Guimarães 16.1.10163 - rodolfolabiapari@gmail.com
 * 	Thiago Schons                      16.1.10186 - thiagoschons2@gmail.com
 *
 * Este arquivo executa o Algoritmo de Bellman Ford.
 *
 *
 * Para executar o arquivo utilize o comando:
 * 	"./nomeDoPrograma benchmark"
 *
 *
 * Saída está descrita da seguinte forma: [origem, destino](distancia) caminho
 * Abaixo é exibido um exemplo
 * [1,2](8) 1 4 2
 * [1,3](9) 1 4 2 3
 * [1,4](5) 1 4
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/*
 * Estrutura que armazena os valores de uma aresta
 */
typedef struct Edges {
	int origem;
	int destino;
	float peso;
} Edge;


/*
 * Utilizou-se de uma pilha para imprimir a ordem de saída na forma
 * origem -> destino. O algoritmo naturalmente imprime de forma inversa, e por
 * isso necessitou de uma pilha.
 */
typedef struct Pilhas {
    int noh;
    struct Pilhas * prox;
} Pilha;


/*
 * Procedimento de empilhar um novo item na pilha
 */
void pushPilha (Pilha ** s, int noh) {
	Pilha * noh_pilha = calloc(1, sizeof(Pilha));

	noh_pilha->noh = noh;
	noh_pilha->prox = *s;

	*s = noh_pilha;
}

/*
 * Procedimento de retirar um item da pilha
 */
int popPilha (Pilha ** s) {
	int retorno;
	Pilha * proximo;

	if (s != NULL) {
		retorno = (*s)->noh;
		proximo = (*s)->prox;

		free(*s);

		*s = proximo;

		return retorno;

	} else {
		return -1;
	}
}

/*
 * Procedimento que realiza a retirada dos dados da pilha imprimindo cada um
 * deles.
 */
void imprimePilha(Pilha * s, FILE * f) {

	if (s == NULL) {
		return;
	} else {

		while (s != NULL) {
			fprintf(f, " %d", s->noh);
			popPilha(&s);
		}
	}
	fprintf(f, "\n");
}


/*
 * Procedimento que retira todos os itens da pilha
 */
void esvaziaPilha(Pilha * s) {

	Pilha * atual, * proximo;

	if (s == NULL) {
		return;
	} else {
		atual = s;
		proximo = s->prox;

		while (proximo != NULL) {
			free (atual);

			atual = proximo;
			proximo = atual->prox;
		}

		free (atual);
	}
}

/*
 * Procedimento responsável por ler o arquivo e recolher as informações do
 * grafo nele contido.
 */
void le_arquivo(char * diretorio, Edge ** lista_arestas, int * vertices, int * arestas, int * maior_peso) {

	// Define o ponteiro pro arquivo em modo de leitura
	FILE * bench = fopen(diretorio, "r");

	int i = 0, origem_tmp = 0, destino_tmp = 0, peso_tmp = 0;
	char criou_lista = 0;

	// Contador de quantas arestas foram lidas
	int count_arestas = 0;


	// Lê do arquivo o comando da linha
	char comando = fgetc(bench);

	// Enquando não for final de arquivo
	while (comando != EOF) {

		// Verifica qual comando é o comando
		switch (comando) {
			// Comentários serão ignorados
			case 'c':
					while(fgetc(bench) != '\n') ;

				break;

			// Informações iniciais do grafo como número de vértices e
				// arestas
			case 'p':
				if (!(fgetc(bench) == ' ')) {
					printf("Erro na inicializacao!\n");
					exit(2);
				}
				if (!(fgetc(bench) == 's')) {
					printf("Erro na inicializacao!\n");
					exit(2);
				}
				if (!(fgetc(bench) == 'p')) {
					printf("Erro na inicializacao!\n");
					exit(2);
				}

				// Le o número de vertices e arestas
				fscanf(bench, "%d %d", vertices, arestas);

				// Flag indicando que a lista foi criada
				criou_lista = 1;

				// Variável indicando o maior peso encontrado no momento
				* maior_peso = -1;

				// Aloca a quantidade exata de arestas lida previamente pelo
					// arquivo
				*lista_arestas = calloc(*arestas, sizeof(Edge));

				break;

			case 'a':

				// Verifica a flag de criaçao da lista
				if (criou_lista == 0) {
					printf("Lista não criada!\n");
					exit(-1);
				}

				// Lê a aresta
				fscanf(bench, "%d %d %d", &origem_tmp, &destino_tmp, &peso_tmp);

				// Atribou as informações no vetor, na primeira posição vazia
				(*lista_arestas)[count_arestas].origem  = origem_tmp;
				(*lista_arestas)[count_arestas].destino = destino_tmp;
				(*lista_arestas)[count_arestas].peso    = peso_tmp;

				// Verifica se encontrou algum peso maior que o já encontrado
				if (peso_tmp > * maior_peso)
					* maior_peso = peso_tmp;

				// Le a aresta e seu valor
				count_arestas++;

				// Quebra a linha
				fgetc(bench);

				break;

			default:
				break;
		}

		// Le o proximo comando
		comando = fgetc(bench);
	} //while

	// Verifica se a contagem de leitura de arestas foi realmente exato
	if ((criou_lista == 0) || (count_arestas != *arestas)) {
		printf("Numero de arestas está incorreto em relação ao arquivo.\n");
		exit(-1);
	}

	// Para a representação do infinito, utilizou-se o maior peso encontrado ao
	// quadrado
	*maior_peso = *maior_peso * *maior_peso;

	// Fecha o arquivo aberto
	fclose(bench);
}

/*
 * Procedimento simples para impressão de das distâncias encontradas
 */
void imprimeDistancias(int * distancia, int vertices) {
	int i;
	printf("\n\n\n");
	for (i = 0; i < vertices; i++)  {
		printf("%4d ", i);
	}

	printf("\n");

	for (i = 0; i < vertices; i++)  {
		printf("%4d ", distancia[i]);
	}

}

/*
 * Procedimento que imprime os predecessores de cada vértice
 */
void imprimePredecessores(int * predecessor, int vertices) {
	int i;
	printf("\n\n");
	for (i = 0; i < vertices; i++)  {
		printf("%4d ", i);
	}

	printf("\n");

	for (i = 0; i < vertices; i++)  {
		printf("%4d ", predecessor[i]);
	}
}


/*
 * Algoritmo De Belmman-Ford.
 * Baseado no pseudocódigo do livro do Cormen.
 */
void bellmanFord(Edge * lista_arestas, int ** distancia, int ** predecessor, int vertices, int arestas, int origem, int maior_peso) {
	int i, j, peso;

	// Aloca um vetor de distâncias e de predecessores temporários
	int * distancia_temp   = calloc(vertices, sizeof(int));
	int * predecessor_temp = calloc(vertices, sizeof(int));

	// Inicializa os vetores temporários com
	for (i = 0; i < vertices; i++) {
		// Distância como 'infinito'
		distancia_temp[i] = maior_peso;
		// predecessor como inexistente no momento
		predecessor_temp[i] = -1;
	}

	// Informa que a distância da origem pra ela mesma é 0
	distancia_temp[origem - 1] = 0;

	// Executa o algoritmo
	for (i = 0; i < vertices; i++) {
		for (j = 0; j < arestas; j++) {
			peso = lista_arestas[j].peso;

			// Verifica se existe uma distância menor por outro caminho
			if (distancia_temp[lista_arestas[j].origem - 1] + peso < distancia_temp[lista_arestas[j].destino - 1] ){
				// Se sim, realiza as atualizações
				distancia_temp[lista_arestas[j].destino - 1] = distancia_temp[lista_arestas[j].origem - 1] + peso;

				predecessor_temp[lista_arestas[j].destino - 1] = lista_arestas[j].origem - 1;
			}
		}
	}

	// Realiza uma verificação final de ciclos negativos
	for (i = 0; i < arestas; i++) {
		peso = lista_arestas[i].peso;

		if (distancia_temp[lista_arestas[i].origem - 1] + peso < distancia_temp[lista_arestas[i].destino - 1]) {
			printf("[%d,%d] Graph contains a negative-weight cycle\n", lista_arestas[i].origem, lista_arestas[i].destino);
			exit(-1);
		}
	}

	// Retorna os vetores criados
	*distancia = distancia_temp;
	*predecessor = predecessor_temp;
}



/*
 * Procedimento final que imprime o caminho para melhor visualização do usuário
 * bem como o valor total da distância.
 */
void imprimeTodosCaminhos(FILE * file, int * distancia, int * predecessor, int vertices, int origem) {

	int anterior, i;

	Pilha * stack;

	for(i = 0; i < vertices; i++) {
		if (i != origem -1) {
			if (predecessor[i] != -1) {

				fprintf(file, "[%d,%d](%d)", origem, i + 1, distancia[i]);

				stack = NULL;

				pushPilha(&stack, i + 1);

				anterior = predecessor[i];
				while(anterior != origem - 1) {
					pushPilha(&stack, anterior + 1);

					anterior = predecessor[anterior];
				}

				fprintf(file, " %d", origem);

				imprimePilha(stack, file);
			}
		}
	}
}

/*
 * Desalocas as memórias alocadas pelo algoritmo
 */
void desaloca(Edge ** lista_arestas, int ** distancia, int ** predecessor) {

	free(*lista_arestas);
	free(*distancia);
	free(*predecessor);
}


int main(int argc, char** argv) {

	if(argc == 2) {
		// Variáveis de cálculo de tempo
		clock_t tempo_inicio, tempo_final;
		double intervalo_real = 0;

		Edge * lista_arestas = NULL;
		int i, vertices = 0, arestas = 0, * distancia = 0, * predecessor = 0, maior_peso = 0;
		// Arquivo de saída de dados dos caminhos
		FILE * file = fopen("saida_bellman.txt", "w+");
		// Arquivo de tempos de execução
		FILE * tempos = fopen("tempos_bellman.txt", "a");

		le_arquivo(argv[1], &lista_arestas, &vertices, &arestas, &maior_peso);

		for (i = 0; i < vertices; i++) {

			tempo_inicio = clock();
				bellmanFord(lista_arestas, &distancia, &predecessor, vertices, arestas, i + 1, maior_peso);
			tempo_final = clock();

			intervalo_real += (double)(tempo_final - tempo_inicio) / CLOCKS_PER_SEC;

			imprimeTodosCaminhos(file, distancia, predecessor, vertices, i + 1);
		}

		fprintf(tempos, "%f\n", intervalo_real);

		fclose(file);
		fclose(tempos);

		desaloca(&lista_arestas, &distancia, &predecessor);

	}
	else {
		printf("Argumentos Inválidos!\n");
		exit(-1);
	}

	return (EXIT_SUCCESS);
}
