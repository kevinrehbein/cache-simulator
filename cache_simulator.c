#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[ ] ) {
    
	if (argc != 7){
		printf("Numero de argumentos incorreto. Utilize:\n");
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
    int endereço, b76, b54, b32, b10;
	int tag, indice;
	float miss_compulsorio = 0, miss_conflito = 0, miss_capacidade = 0;
	float hit = 0;
	float total_acessos = 0;
	int cache_val [nsets * assoc];
	int cache_tag [nsets * assoc];
	int n_bits_offset = log(bsize) / log(2);	//log2 (x) = logy (x) / logy (2)
	int n_bits_indice = log(nsets) / log(2);
	int n_bits_tag = 32 - n_bits_offset - n_bits_indice;

	//preenche cache_val com 0
	for(int i=0; i < nsets * assoc; i++){
		cache_val[i] = 0;
	}

	//testa bertura de arquivo
	if ((arquivo = fopen(arquivoEntrada, "rb")) == NULL) {
        printf("Nao foi possível abrir o arquivo!");
        exit(EXIT_FAILURE);
    }

    // Lê 4 bytes do arquivo binário de cada vez até o fim do arquivo
    while (fread(&endereço, 4, 1, arquivo) == 1) {
		total_acessos++;

		//converte little endian para big endian
		b76 = (endereço & 0x000000FF) << 24;
		b54 = (endereço & 0x0000FF00) << 8;
		b32 = (endereço & 0x00FF0000) >> 8;
		b10 = (endereço & 0xFF000000) >> 24;
		endereço = b76 + b54 + b32 + b10;

		//extrai tag e indice do endereço
		indice = (endereço >> n_bits_offset) & ((int)pow(2, n_bits_indice) - 1);
		tag = endereço >> (32 - n_bits_tag);

		// para o mapeamento direto
		if (cache_val[indice] == 0) {
			miss_compulsorio++;
			cache_val[indice] = 1;
			cache_tag[indice] = tag;
			// estas duas últimas instruções representam o tratamento da falta.
			}
		else if (cache_tag[indice] == tag)
				hit++;
			else {
				miss_conflito++;
				//conflito ou capacidade?
				cache_val[indice] = 1;
				cache_tag[indice] = tag;
			}

		//mapamento associativo
	}

	switch (flagOut)
	{
	case 0:
		printf("\n-----RESULTADOS-----\n");
		printf("Numero de misses compulsorios: %.0f\n", miss_compulsorio);
		printf("Numero de misses conflito: %.0f\n", miss_conflito);
		printf("Numero de misses capacidade: %.0f\n", miss_capacidade);
		printf("Numero de hits: %.0f\n", hit);
		break;

	case 1:
		printf("%.0f ", total_acessos);
		printf("%.2f ", hit/total_acessos);
		printf("%.2f ", (miss_capacidade + miss_compulsorio + miss_conflito)/total_acessos);
		printf("%.2f ", miss_compulsorio/(miss_capacidade + miss_compulsorio + miss_conflito));
		printf("%.2f ", miss_capacidade/(miss_capacidade + miss_compulsorio + miss_conflito));
		printf("%.2f ", miss_conflito/(miss_capacidade + miss_compulsorio + miss_conflito));
		break;
	}

	fclose(arquivo);
	return 0;
}
