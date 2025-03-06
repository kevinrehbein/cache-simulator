# cache-simulator

Para compilar o arquivo main.c, utilize o seguinte comando via terminal "gcc cache_simulator.c -o cache_simulator -lm".
O "-lm" é necessário para linkar a biblioteca libm, de onde é utilizada a função log2.

Para executar, basta utilizar o comando padrão "./cache_simulator <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada".
