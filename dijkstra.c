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
 * Este arquivo executa o Algoritmo de Dijkstra.
 * Deve-se ter atenção à estrutura de dados utilizada já que os autores
 * decidiram utilizar um estrutura modificada para facilitar a execução deste.
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
 * Estrutura que guarda informações do nó adjacente, possuindo
 * um ponteiro para o próximo nó adjacente
 * Esta estrutura depente da estrutura NohsIndividuais.
 */
typedef struct Nohs {
		int noh_id;         // Identificação do Noh
		int peso;           // Peso que esta adjacência tem
		struct Nohs * prox; // Ponteiro para a próxima adjacência
} Noh;

/*
 * Estrutura principal. Ela será um vetor que armazenará informações individuáis
 * de cada vertice além de um ponteiro para todos os seus adjacentes.
 * Assim, quando necessitar de uma informação de determinado noh, poderá ser
 * acessado em O(1). Além de forncer todos os seus adjacentes indicados pelo
 * ponteiro prox.
 */
typedef struct NohsIndividuais {
		char visitado;      // Flag
		int peso_atual;   // Distância deste nó até o nó de origem
		struct Nohs * prox; // Ponteiro para a próxima adjacência
} NohIndividual;


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
 * Função que realiza a criação de um novo Nó
 * preenchendo seus dados de acordo com os paramentros
 */
Noh * criaNovoNoh(int destino, int peso) {

	// Aloca uma nova estrutura Noh
	Noh *noh = calloc(1, sizeof (Noh));

	// Atribui as novas informações
	noh->noh_id     = destino;
	noh->peso       = peso;
	noh->prox    = NULL;

	// retorna seu endereço
	return noh;
}

/*
 * Procedimento que recebe dados para a criação de um novo nó adjacente
 * já adicionando-o na sua respectiva lista colocando-o na primeira posição
 * evitando a necessidade de percorrer a lista
 */
void criaNovaAdjacencia(NohIndividual * lista, int origem, int destino, int peso) {

	// Recebe o primeiro item adjacente
	Noh * primeiro = lista[origem - 1].prox;

	// Cria um novo nó
	Noh * novo = criaNovoNoh(destino, peso);

	// Atribui este novo nó no início da lista
	novo->prox = primeiro;
	lista[origem - 1].prox = novo;
}

/*
 * Procedimento que realiza a criação da base da lista de adjacência que é
 * feita pela struct NohIndividual. Ela guarda informações de cada nó
 * individual como 'se foi visitado' e sua distância da origem no momento.
 * Também tem um ponteiro para a estrutura Noh que representa os Nohs adjacentes
 * deste nó, informando o peso da aresta e seu identificador.
 */
NohIndividual * criaListaAdjacencia(int vertices) {

	// Aloca os nohs individuais que armazenaram as listas de adjacencia
	NohIndividual * lista_adjacencia = calloc(vertices, sizeof(NohIndividual));
	int i;

	// Define os valores iniciais de cada um desses nós
	for (i = 0; i < vertices; i++) {
		lista_adjacencia[i].peso_atual = 0;
		lista_adjacencia[i].visitado = 0;
		lista_adjacencia[i].prox = NULL;
	}

	// Retorna a lista de todos os vértices vazia
	return lista_adjacencia;
}

/*
 * Procedimento que realiza a impressão da lista de ajdacência.
 */
void imprimeAdjacencia(NohIndividual * lista, int vertices, int arestas) {

	Noh * atual = NULL;
	int i;

	printf("\nVértices: %d, Arestas: %d:\n", vertices, arestas);

	for (i = 0; i < vertices; i++) {
		printf("%d ->", i + 1);
		atual =  lista[i].prox;

		while (atual != NULL) {
			printf(" %d", atual->noh_id);

			atual = atual->prox;
		}
		printf("\n");
	}
}

/*
 * Procedimento responsável por ler o arquivo e recolher as informações do
 * grafo nele contido.
 */
NohIndividual * le_arquivo(char * diretorio, int * vertices, int * arestas) {

	// Define o ponteiro pro arquivo em modo de leitura
	FILE * bench = fopen(diretorio, "r");

	int i, origem_tmp, destino_tmp, peso_tmp;
	NohIndividual * lista_adjacencia = NULL;

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
				// Le a linha inteira de comentario
				while(fgetc(bench) != '\n') ;

				break;

			// Informações iniciais do grafo como número de vértices e
				// arestas
			case 'p':
				if (!(fgetc(bench) == ' ')) {
					printf("Erro na inicializacao!\n");
					exit(-2);
				}
				if (!(fgetc(bench) == 's')) {
					printf("Erro na inicializacao!\n");
					exit(-2);
				}
				if (!(fgetc(bench) == 'p')) {
					printf("Erro na inicializacao!\n");
					exit(-2);
				}

				// Le o número de vertices e arestas
				fscanf(bench, "%d %d", vertices, arestas);

				// Cria a lista de adjacencia pra alimentá-la
				lista_adjacencia = criaListaAdjacencia(*vertices);
				break;

			case 'a':
				// Verifica se ja tenha lido a quantidade de arestas antes de ler
					// cada uma.
				if (*vertices == 0 || * arestas == 00) {
					printf("Nenhuma aresta ou vértice lido\n");
					exit(-1);
				}

				// Le a aresta e seu valor
				count_arestas++;

				fscanf(bench, "%d %d %d", &origem_tmp, &destino_tmp, &peso_tmp);

				// Adiciona a aresta à adjacencia
				criaNovaAdjacencia(lista_adjacencia, origem_tmp, destino_tmp, peso_tmp);

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
	if ((*arestas < 1 || *vertices < 1) ||
			(lista_adjacencia == NULL) ||
			(count_arestas != *arestas)) {
		printf("Leitura de arquivo obteve problemas.\n");
		exit(-1);
	}

	// Fecha o arquivo aberto
	fclose(bench);

	// Retorna a lista
	return lista_adjacencia;
}






/*
 * Procedimento de inicialização do algoritmod e Dijkstra. Ele realiza a
 * inicialização dos valores de distância (peso_atual) de cada nó e também
 * sinalizando que eles ainda não foram visitados.
 * Define que todos os vértices possuem o vertice de origem como o vértice
 * anterior.
 * Também inicializa os valores do vertice fonte que deverá ter propriedades
 * diferente dos demais.
 */
void inicializaDijkstra(int fonte, int vertices, int * vertice_anterior, NohIndividual * lista_adjacencia) {
	int i;
	// Inicializa todos dados individuais dos vertices
	for (i = 0; i < vertices; i++) {

		lista_adjacencia[i].peso_atual = -1;
		lista_adjacencia[i].visitado = 0;

		vertice_anterior[i] = fonte;
	}

	// Redefine as informações do vertice fonte
	lista_adjacencia[fonte - 1].peso_atual = 0;
	vertice_anterior[fonte - 1] = -1;
}

/*
 * Procedimento que lista determinado vertice, passado por parâmetro, como
 * vertice visitado, retirando da lista de disponíves.
 * Procedimento utilizado para definir que o nó origem serja o primeiro a ser
 * descartado de uso.
 */
int extraiVertice(NohIndividual * lista_adjacencia, int id) {

	// Altera o valor de visitado para true.
	lista_adjacencia[id - 1].visitado = 1;

	// Retorna o id do noh utilizado
	return id;
}

/*
 * Procedimento que realiza o relaxamento do algoritmo de Dijkstra.
 * Realiza a verificação dos nos adjacentes alterando as distancias dos seus
 * respectivos a procura de novos caminhos.
 */
void relaxamento(NohIndividual * lista, int vertice_anterior[], int origem_id) {

	// Recebe o primeiro noh adjacente
	Noh * atual  = lista[origem_id - 1].prox;

	float peso_atual_temp;

	// Enquanto tiver adjacente para analisar
	while (atual != NULL) {

		// Recebe a distância do nó atual
		peso_atual_temp = lista[atual->noh_id - 1].peso_atual;

		// Verifica se no novo calculo, existe uma distância menor
		if (peso_atual_temp < 0 ||
				(lista[origem_id - 1].peso_atual + atual->peso
					<
				peso_atual_temp)) {

			// Recebe o novo relaxamento
			lista[atual->noh_id - 1].peso_atual = lista[origem_id - 1].peso_atual + atual->peso;

			// define que o vertice antecessor a esse é o de origem aqui analisado
			vertice_anterior[atual->noh_id - 1] = origem_id;
		}

		atual = atual->prox;
	}
}

/*
 * Função que realiza a procura de uma aresta ainda não utilizada e que
 * tenha o menor custo possível de distância.
 */
int extraiVerticeMenosCustoso(NohIndividual * listaAdjacencia, int vertices, int arestas) {

	NohIndividual * lista = listaAdjacencia;

	int i = 0;

	// salta todos os nós que não podem ser utilizados como:
		// nós já visitados ou arestas inexistentes
	while ((lista[i].visitado == 1 || lista[i].peso_atual < 1) && i < arestas)
		i++;

	// Verifica se excedeu a quantidade de arestas
	if (i != arestas -1) {

		// Se não tiver excedido, define o primeiro disponível como o menor para
			// futuras comparações
		int menor = i;

		// Comparando com o restante dos vértices
		for (++i; i < vertices; i++) {

			// Verifica se existe algum outro vertice disponível com aresta
				// menor que o atual
			if (lista[i].visitado == 0 &&
					lista[i].peso_atual > 0 &&
				lista[i].peso_atual < lista[menor].peso_atual) {

				// Indica qual é o menor para o retorno da função
				menor = i;
			}
		}

		// Define o vertisse como visitado
		lista[menor].visitado = 1;

		// Lembrando que o ID é indexado de 1
		menor++;

		// Retorna o id do vertice
		return menor;
	}
	else {
		// Tratamento de erro
		printf("Não foi encontrado uma nova aresta para operar.\nPrograma Finalizado.\n");
		exit(2);
	}
}

/*
 * Procedimento final que imprime o caminho para melhor visualização do usuário
 * bem como o valor total da distância.
 * IMPRIME O CAMINHO DE FORMA INVERSA: destino <- origem
 */
void imprimeCaminho(int vertices, int vertice_anterior[vertices], NohIndividual lista_adjacencia[vertices],  int origem, int destino) {

	// Diz que o destino é o primeiro nó a ser percorrido anterior
	int anterior = destino;

	// Indica que será descrito o caminho para o usuário
	printf("\nCaminho Inverso:\n\t%d", destino);

	// Imprime o caminho de forma inversa
	while (vertice_anterior[anterior - 1] != origem) {
		printf(" <- %d", vertice_anterior[anterior - 1]);

		anterior = vertice_anterior[anterior - 1];
	}

	// Imprime o último item do caminho (que é a origem)
	printf(" <- %d.", vertice_anterior[anterior - 1]);

	// Imprime também a distância do caminho
	printf("\nDistância pecorrida: %d\n.", lista_adjacencia[destino - 1].peso_atual);
}


/*
 * Procedimento que realiza a impressão dos dados em arquivo para a análise.
 * Para a impressão, utiliza-se a pilha para que o caminho inverso seja impresso
 * de forma natural (origem -> destino)
 */
void imprimeTodosCaminhosArquivo(FILE * file, int vertices, int vertice_anterior[vertices], NohIndividual lista_adjacencia[vertices], int origem) {

	int anterior, i;

	// Pilha para armazenamento do caminho inverso
	Pilha * stack;

	// Para cada vertice diferente da origem
	for(i = 0; i < vertices; i++) {
		if (i != origem -1) {

			// Verifica se o vértice é inválido
			if (vertice_anterior[i] != -1) {

				// Imprime as informações básicas do arquivo como início, fim e
					// custo
				fprintf(file, "[%d,%d](%d)", origem, i + 1, lista_adjacencia[i].peso_atual);

				// Inicializa a pilha para que não exista lixo
				stack = NULL;

				// Coloca o primeiro item na pilha
				pushPilha(&stack, i + 1);

				// Informa qual é o próximo item a ser colocado na pilha
				anterior = vertice_anterior[i];

				// Enquanto não for a origem, adiciona os intermediários na pilha
				while(anterior != origem) {
					pushPilha(&stack, anterior);

					anterior = vertice_anterior[anterior - 1];
				}

				// Imprime a origem no arquivo
				fprintf(file, " %d", origem);

				// Imprime os itens restantes no arquivo
				imprimePilha(stack, file);
			}
		}
	}
}


/*
 * Algoritmo de Dijkstra
 * Baseado no pseudocódigo do livro do Cormen.
 */
void dijkstra(int fonte, NohIndividual * lista_adjacencia, int vertices, int arestas, int vertice_anterior[vertices]) {

	// Quantidade de vertices adicionados no vertor resultante.
		// Serve como medida para indicar término do cálculo
	int quantidade_vetor_resultantes = 0;

	// Inicializa as variáveis do algorimo
	inicializaDijkstra(fonte, vertices, vertice_anterior, lista_adjacencia);

	// Extrai o vertice mais leve
	int vertice_mais_proximo = extraiVertice(lista_adjacencia, fonte);

	// Enquanto tiver vertice pra analizar
	while (++quantidade_vetor_resultantes < vertices) {

		// Realiza o relaxamento da fronteira
		relaxamento(lista_adjacencia, vertice_anterior, vertice_mais_proximo);

		// Extrai vertice mais leve
		vertice_mais_proximo = extraiVerticeMenosCustoso(lista_adjacencia, vertices, arestas);
	}
}

/*
 * Procedimento responsável por desalocar todos os dados alocados para a
 * execução do algoritmo de Dijkstra.
 */
void desaloca(NohIndividual ** lista_adjacencia, int vertices) {
	// Cria ponteiros para a exclusão e indicação do próximo
	Noh * deletar, * atual;
	int i;

	// Para cada vertice, exclui seus adjacentes
	for (i = 0; i < vertices; i++) {

		if ((*lista_adjacencia)[i].prox != NULL) {
			deletar = (*lista_adjacencia)[i].prox;
			atual = deletar->prox;

			while (atual != NULL) {
				free(deletar);

				deletar = atual;
				atual = atual->prox;
			}

			free(deletar);
		}
	}

	// Exclui o vetor base de vertices NohIndividual
	free(*lista_adjacencia);
}


int main(int argc, char** argv) {

	if(argc == 2) {
		// Variáveis de cálculo de tempo
		clock_t tempo_inicio, tempo_final;
		double intervalo_real = 0;

		int i, vertices = 0, arestas = 0, origem = 1, destino = 3;
		// Arquivo de saída de dados dos caminhos
		FILE * file = fopen("saida_dijkstra.txt", "w+");
		// Arquivo de tempos de execução
		FILE * tempos = fopen("tempos_dijkstra.txt", "a");

		NohIndividual * lista_adjacencia = le_arquivo(argv[1], &vertices, &arestas);

		if (vertices < 1)
			exit(-1);

		// Vetor com os valores de vértices predecessores
		int vertice_anterior[vertices];

		// Executa o algoritmo de Dijkstra de todos para todos
		for (i = 0; i < vertices; i++) {

			// Calcula o tempo de execução
			tempo_inicio = clock();
				dijkstra(i + 1, lista_adjacencia, vertices, arestas, vertice_anterior);
			tempo_final = clock();

			// Soma o tempo calculado de cada origem
			intervalo_real += (double)(tempo_final - tempo_inicio) / CLOCKS_PER_SEC;

			imprimeTodosCaminhosArquivo(file, vertices, vertice_anterior, lista_adjacencia, i + 1);
		}

		// Persiste o tempo total de execução
		fprintf(tempos, "%f\n", intervalo_real);

		// Desaloca todos os itens utilizados no algoritmo
		desaloca(&lista_adjacencia, vertices);

		// Fecha de forma correta os arquivos abertos
		fclose(file);
		fclose(tempos);
	}
	// caso contário, cancela a execução
	else {
		printf("Argumentos Inválidos!\n");
		exit(-1);
	}

	return (EXIT_SUCCESS);
}
