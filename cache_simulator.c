#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//estrutura da cache
typedef struct {
    int validade;
    int tag;
    int momento_do_acesso; //para FIFO e LRU
} CacheBlock;

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
    FILE *arquivo;
    char caminho[100];
    int endereco, b76, b54, b32, b10;
    int tag, indice;
    float miss_compulsorio = 0, miss_conflito = 0, miss_capacidade = 0;
    float hit = 0;
    float total_acessos = 0;
    int n_bits_offset = log2(bsize);
    int n_bits_indice = log2(nsets);
    int n_bits_tag = 32 - n_bits_offset - n_bits_indice;

    sprintf(caminho, "Endereços/%s", arquivoEntrada);

    //testa bertura de arquivo
    if ((arquivo = fopen(caminho, "rb")) == NULL) {
        printf("Não foi possível abrir o arquivo!");
        exit(EXIT_FAILURE);
    }

    //Inicializa cache com bits de validade e momento do acesso zerado
    CacheBlock cache[nsets][assoc];
    for (int i = 0; i < nsets; i++) {
        for (int j = 0; j < assoc; j++) {
            cache[i][j].validade = 0;
            cache[i][j].momento_do_acesso = 0;
        }
    }

    srand(time(NULL)); //inicializa o gerador de números aleatórios para substituição Random

    //lê um valor de 4 Bytes do arquivo por vez e armazena em endereço
    while (fread(&endereco, 4, 1, arquivo) == 1) {
        int hit_flag = 0;
        int bloco = 0;
        int momento_acesso_mais_antigo = total_acessos + 1; //inicializa com um valor maior que o total de acessos até o momento.
        int espaco_vazio = 0;
        
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

        //verifica se há um hit no conjunto
        for (int i = 0; i < assoc; i++) {
            if (cache[indice][i].validade && cache[indice][i].tag == tag) {
                hit++;
                hit_flag = 1;

                //atualiza o momento de acesso se for LRU
                if (strcmp(subst, "L") == 0){
                    cache[indice][i].momento_do_acesso = total_acessos;
                }
                break;
            }
        }

        if (!hit_flag) { 

            //miss compulsório
            for(int i = 0; i < assoc; i++){
                if (!cache[indice][i].validade) { 
                    miss_compulsorio++;     //atualiza estatística de miss
                    espaco_vazio = 1;

                    //insere o bloco na cache
                    cache[indice][i].validade = 1;      
                    cache[indice][i].tag = tag;
                    cache[indice][i].momento_do_acesso = total_acessos;     // registra o momento do acesso para FIFO e LRU 
                    break;
                }
            }
            
            //miss conflito ou capacidade
            if (!espaco_vazio){    

                if (assoc == 1) {
                    miss_conflito++;    //atualiza estatística de miss - map. direto todos os misses são de conflito

                } else if (nsets == 1){
                    miss_capacidade++;  //atualiza estatística de miss - map. total. assoc. todos os misses são de capacidade
                                        
                } else {

                    //percorre cache e verfica ausência de capacidade
                    for(int i = 0; i < nsets; i++){
                        for (int j = 0; j < assoc; j++) {   
                            if(!cache[i][j].validade){      
                                espaco_vazio = 1;
                                break;
                            }
                        }
                        if(espaco_vazio)
                            break;
                    }
                    
                    //atualiza estatística de miss conforme capacidadde
                    if (espaco_vazio) {
                        miss_conflito++; 
                    } else miss_capacidade++;
                }
                
                //escolhe o bloco a ser substituido
                if (strcmp(subst, "R") == 0) {
                    //substituição Random
                    bloco = rand() % assoc;     
                } else {
                    //substituição LRU e FIFO
                    for (int i = 0; i < assoc; i++) {
                        if (cache[indice][i].momento_do_acesso < momento_acesso_mais_antigo) {
                            momento_acesso_mais_antigo = cache[indice][i].momento_do_acesso;
                            bloco = i;      // resulta o bloco mais antigo inserido(FIFO) ou utilizado(LRU)
                        }
                    }
                }

                //substitui o bloco
                cache[indice][bloco].tag = tag;
                cache[indice][bloco].momento_do_acesso = total_acessos;     // registra o momento do acesso para FIFO e LRU
            }
        }
    }

    //saída dos resultados
    switch (flagOut) {
        case 0:
            printf("-----RESULTADOS-----\n");
            printf("Numero de misses: %.0f\n", miss_capacidade + miss_compulsorio + miss_conflito);
            printf("Taxa de misses: %.4f\n", (miss_capacidade + miss_compulsorio + miss_conflito) / total_acessos);
            printf("Numero de misses compulsorios: %.0f\n", miss_compulsorio);
            printf("Taxa de misses compulsorios: %.4f\n", miss_compulsorio/(miss_capacidade + miss_compulsorio + miss_conflito));
            printf("Numero de misses de conflito: %.0f\n", miss_conflito);
            printf("Taxa de misses de conflito: %.4f\n", miss_conflito/(miss_capacidade + miss_compulsorio + miss_conflito));
            printf("Numero de misses de capacidade: %.0f\n", miss_capacidade);
            printf("Taxa de misses de capacidade: %.4f\n", miss_capacidade/(miss_capacidade + miss_compulsorio + miss_conflito));
            printf("Numero de hits: %.0f\n", hit);
            printf("Taxa de hits: %.4f\n", hit/total_acessos);
            break;

        case 1:
            printf("%.0f ", total_acessos);
            printf("%.4f ", hit / total_acessos);
            printf("%.4f ", (miss_capacidade + miss_compulsorio + miss_conflito) / total_acessos);
            printf("%.2f ", miss_compulsorio / (miss_capacidade + miss_compulsorio + miss_conflito));
            printf("%.2f ", miss_capacidade / (miss_capacidade + miss_compulsorio + miss_conflito));
            printf("%.2f ", miss_conflito / (miss_capacidade + miss_compulsorio + miss_conflito));
            break;
    }

    fclose(arquivo);
    return 0;
}