#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Número de argumentos incorreto. Utilize:\n");
        printf("./cache_simulator <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada\n");
        exit(EXIT_FAILURE);
    }

    int nsets = atoi(argv[1]);
    int bsize = atoi(argv[2]);
    int assoc = atoi(argv[3]);
    char *subst = argv[4];
    int flagOut = atoi(argv[5]);
    char *arquivoEntrada = argv[6];

    printf("nsets = %d\n", nsets);
    printf("bsize = %d\n", bsize);
    printf("assoc = %d\n", assoc);
    printf("subst = %s\n", subst);
    printf("flagOut = %d\n", flagOut);
    printf("arquivo = %s\n", arquivoEntrada);

    FILE *arquivo;
    int endereco, b76, b54, b32, b10;
    int tag, indice;
    float miss_compulsorio = 0, miss_conflito = 0, miss_capacidade = 0;
    float hit = 0;
    float total_acessos = 0;
    int n_bits_offset = log(bsize) / log(2);	//log2 (x) = logy (x) / logy (2)
    int n_bits_indice = log(nsets) / log(2);
    int n_bits_tag = 32 - n_bits_offset - n_bits_indice;

    //estrutura da cache para suportar associatividade
    typedef struct {
        int valid;
        int tag;
        int time; //para FIFO e LRU
    } CacheBlock;

    CacheBlock cache[nsets][assoc];
    for (int i = 0; i < nsets; i++) {
        for (int j = 0; j < assoc; j++) {
            cache[i][j].valid = 0;
            cache[i][j].time = 0;
        }
    }

    //testa bertura de arquivo
    if ((arquivo = fopen(arquivoEntrada, "rb")) == NULL) {
        printf("Não foi possível abrir o arquivo!");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); //inicializa o gerador de números aleatórios para substituição Random

    while (fread(&endereco, 4, 1, arquivo) == 1) {
        total_acessos++;

        //converte little endian para big endian
        b76 = (endereco & 0x000000FF) << 24;
        b54 = (endereco & 0x0000FF00) << 8;
        b32 = (endereco & 0x00FF0000) >> 8;
        b10 = (endereco & 0xFF000000) >> 24;
        endereco = b76 + b54 + b32 + b10;

        //extrai tag e índice do endereço
        indice = (endereco >> n_bits_offset) & ((int)pow(2, n_bits_indice) - 1);
        tag = endereco >> (32 - n_bits_tag);

        int hit_flag = 0;
        int replace_index = 0;
        int oldest_time = total_acessos + 1; //inicializa com um valor grande

        //verifica se há um hit no conjunto
        for (int i = 0; i < assoc; i++) {
            if (cache[indice][i].valid && cache[indice][i].tag == tag) {
                hit++;
                hit_flag = 1;
                if (strcmp(subst, "L") == 0) {
                    cache[indice][i].time = total_acessos; //atualiza o tempo de acesso para LRU
                }
                break;
            }
        }

        if (!hit_flag) {
            //miss - escolhe um bloco para substituir
            if (strcmp(subst, "R") == 0) {
                replace_index = rand() % assoc; //substituição Random
            } else {
                //substituição FIFO ou LRU
                for (int i = 0; i < assoc; i++) {
                    if (!cache[indice][i].valid) {
                        replace_index = i;
                        break;
                    }
                    if (cache[indice][i].time < oldest_time) {
                        oldest_time = cache[indice][i].time;
                        replace_index = i;
                    }
                }
            }

            //atualiza estatísticas de miss
            if (!cache[indice][replace_index].valid) {
                miss_compulsorio++;
            } else {
                miss_conflito++;
            }

            //substitui o bloco
            cache[indice][replace_index].valid = 1;
            cache[indice][replace_index].tag = tag;
            cache[indice][replace_index].time = total_acessos;
        }
    }

    //calcula misses de capacidade
    miss_capacidade = total_acessos - hit - miss_compulsorio - miss_conflito;

    //saída dos resultados
    switch (flagOut) {
        case 0:
            printf("\n-----RESULTADOS-----\n");
            printf("Número de misses compulsórios: %.0f\n", miss_compulsorio);
            printf("Número de misses conflito: %.0f\n", miss_conflito);
            printf("Número de misses capacidade: %.0f\n", miss_capacidade);
            printf("Número de hits: %.0f\n", hit);
            break;

        case 1:
            printf("%.0f ", total_acessos);
            printf("%.2f ", hit / total_acessos);
            printf("%.2f ", (miss_capacidade + miss_compulsorio + miss_conflito) / total_acessos);
            printf("%.2f ", miss_compulsorio / (miss_capacidade + miss_compulsorio + miss_conflito));
            printf("%.2f ", miss_capacidade / (miss_capacidade + miss_compulsorio + miss_conflito));
            printf("%.2f ", miss_conflito / (miss_capacidade + miss_compulsorio + miss_conflito));
            break;
    }

    fclose(arquivo);
    return 0;
}