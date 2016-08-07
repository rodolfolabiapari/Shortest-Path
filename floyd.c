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
 * Este arquivo executa o Algoritmo de Floyd Warshall.
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
#include <time.h>


/*
 * Procedimento que realiza a alocação das matrizes que o algoritmo necessita
 * para executar.
 */
void criaMatrizes(int *** distancia, int *** proximo, int vertices) {
	// Instancia o primeiro nível das matrizes
	*distancia = (int **) calloc(vertices, sizeof(int*));
	*proximo   = (int **) calloc(vertices, sizeof(int*));
	int i, j;

	// Instancia os outros níveis de cada matriz
	for (i = 0; i < vertices; i++){
		(*distancia)[i] = (int *) calloc(vertices, sizeof(int));
		(*proximo)[i]   = (int *) calloc(vertices, sizeof(int));
	}

	// Inicializa cada um de suas células
	for (i = 0; i < vertices; i++){
		for (j = 0; j < vertices; j++){
			(*distancia)[i][j] = -1;
			(*proximo)[i][j] = -1;
		}

		(*distancia)[i][i] = 0;
	}
}

/*
 * Procedimento responsável por desalocar todas as matrizes utilizadas pelo
 * programa
 */
void destroiMatrizes(int *** distancia, int *** proximo, int vertices) {
	int i;
	for (i = 0; i < vertices; i++) {
		free((*distancia)[i]);
		free((*proximo)[i]);
	}

	free(*distancia);
	free(*proximo);
}

/*
 * Procedimento simples para a impressão dos dados de distancia
 */
void imprimeDistancias(int ** d, int vertices) {
	int i, j;
	printf("\n");
	printf("      0:   1:   2:   3:   4:\n");
	for (i = 0; i < vertices; i++){
		for (j = 0; j < vertices; j++){
			if (j == 0) {
				printf("\033[1m\033[37m");
				printf("%3.d:  ", i);
				printf("\033[0m");
			}
			printf("%2d   ", d[i][j]);
		}
		printf("\n");
	}
}


/*
 * Procedimento simples para a impressão dos dados de proximos
 */
void imprimeProximos(int ** p, int vertices) {
	int i, j;
	printf("\n");

	printf("      0:   1:   2:   3:   4:\n");
	for (i = 0; i < vertices; i++){
		for (j = 0; j < vertices; j++){
			if (j == 0) {
				printf("\033[1m\033[37m");
				printf("%3.d: ", i);
				printf("\033[0m");
			}
			printf("%3d ", p[i][j]);
		}
		printf("\n");
	}
}

/*
 * Procedimento responsável por ler o arquivo e recolher as informações do
 * grafo nele contido.
 */
void le_arquivo(char * diretorio, int *** matriz_distancia, int *** matriz_proximo, int * vertices) {

	int ** m_distancia = * matriz_distancia;
	int ** m_proximo   = * matriz_proximo;
	int maior_distancia = -1;
	char criou_matriz = 0;

	// Define o ponteiro pro arquivo em modo de leitura
	FILE * bench = fopen(diretorio, "r");

	int i, j, origem_tmp, destino_tmp, peso_tmp, arestas_temp = 0, count_arestas = 0;

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
				fscanf(bench, "%d %d", vertices, &arestas_temp);

				// Cria a lista de adjacencia pra alimentá-la
				criaMatrizes(&m_distancia, &m_proximo, *vertices);

				// Flag indicando crianção de matriz
				criou_matriz = 1;

				break;

			case 'a':
				// Verifica se a matriz já foi criada
				if (criou_matriz == 0) {
					printf("Matriz não criada!\n");
					exit(-1);
				}

				count_arestas++;

				// Lê a aresta do arquivo
				fscanf(bench, "%d %d %d", &origem_tmp, &destino_tmp, &peso_tmp);

				// adiciona as informações na estrutura
				m_distancia[origem_tmp - 1][destino_tmp - 1] = peso_tmp;

				m_proximo[origem_tmp - 1][destino_tmp - 1] = destino_tmp;

				if (peso_tmp > maior_distancia)
					maior_distancia = peso_tmp;

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
	if ((criou_matriz == 0) || (count_arestas != arestas_temp)) {
		printf("Numero de arestas está incorreto em relação ao arquivo.\n");
		exit(-1);
	}

	// Para a representação do infinito, utilizou-se o maior peso encontrado ao
	// quadrado
	maior_distancia = maior_distancia * maior_distancia;

	// Inicializa a matriz distância com o valor infinito
	for (i = 0; i < *vertices; i++) {
		for (j = 0; j < *vertices; j++) {
			if (m_distancia[i][j] == -1)
				m_distancia[i][j] = maior_distancia;
		}
	}

	*matriz_distancia = m_distancia;
	*matriz_proximo = m_proximo;

	// Fecha o arquivo aberto
	fclose(bench);
}


/*
 * Procedimento final que imprime o caminho para melhor visualização do usuário
 * bem como o valor total da distância.
 */
void imprimeCaminho(FILE * file, int ** proximo, int ** distancia, int origem, int destino) {
	int caminho;

	if (proximo[origem - 1][destino - 1] == -1) {
		printf("\nErros nos resultados [%d,%d]!\n", origem, destino);
		exit(-1);
	}
	else {
		fprintf(file, "[%d,%d](%d)", origem, destino, distancia[origem - 1][destino - 1]);

		caminho = origem;
		fprintf(file, " %d", caminho);

		while (caminho != destino) {
			caminho = proximo[caminho - 1][destino - 1];
			fprintf(file, " %d", caminho);
		}

		fprintf(file, "\n");
	}
}


/*
 * Algoritmo de Floyd Warshall.
 * Baseado no livro do Cormen
 */
void floydWarshall(int ** m_distancia, int ** m_proximo, int vertices) {
	int i, j, k;

	for (k = 0; k < vertices; k++) {
		for (i = 0; i < vertices; i++) {
			for (j = 0; j < vertices; j++) {

				if (m_distancia[i][k] + m_distancia[k][j] < m_distancia[i][j]) {

					m_distancia[i][j] = m_distancia[i][k] + m_distancia[k][j];
					m_proximo[i][j]  = m_proximo[i][k];
				}

			}
		}
	}
}

int main(int argc, char** argv) {

	if(argc == 2) {
		// Variáveis de cálculo de tempo
		clock_t tempo_inicio, tempo_final;
		double intervalo_real;

		int i, j, ** m_proximo = NULL, vertices, ** m_distancia = NULL;
		// Arquivo de saída de dados dos caminhos
		FILE * tempos = fopen("tempos_floyd.txt", "a");
		// Arquivo de tempos de execução
		FILE * file_resultados = fopen("saida_floyd.txt", "w+");

		le_arquivo(argv[1], &m_distancia, &m_proximo, &vertices);

		tempo_inicio = clock();
			floydWarshall(m_distancia, m_proximo, vertices);
		tempo_final = clock();

		intervalo_real = (double)(tempo_final - tempo_inicio) / CLOCKS_PER_SEC;

		fprintf(tempos, "%f\n", intervalo_real);

		for (i = 0; i < vertices; i++) {
			for (j = 0; j < vertices; j++) {
					if (i != j)
					imprimeCaminho(file_resultados, m_proximo, m_distancia, i + 1, j + 1);
			}
		}

		fclose(file_resultados);
		fclose(tempos);

		destroiMatrizes( &m_distancia, &m_proximo, vertices);

	}
	else {
		printf("Argumentos Inválidos!\n");
		exit(-1);
	}

	return (EXIT_SUCCESS);
}
