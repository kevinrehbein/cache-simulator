# cache-simulator

Para compilar o arquivo main.c, utilize o seguinte comando via terminal "gcc cache_simulator.c -o cache_simulator -lm".
"-lm" é necessário para linkar a biblioteca libm, de onde é utilizada a função log.

Para executar, basta utilizar o comando padrão no "./cache_simulator <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada". É importante que os arquivos de endereços estejam no mesmo diretório que cache_simulator.