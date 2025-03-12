# cache-simulator

No Linux:

Para compilar o arquivo cache_simulator.c, utilize o seguinte comando via terminal no linux: 
gcc cache_simulator.c -o cache_simulator -lm

O "-lm" é necessário para linkar a biblioteca libm, de onde é utilizada a função log2.

Para executar, basta utilizar o comando padrão:
./cache_simulator <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada

O arquivo_de_entrada deve estar contido no diretório "Endereços".


No Windows:

Para compilar o arquivo cache_simulator.c, utilize o seguinte comando via terminal no windows: 
gcc cache_simulator.c -o cache_simulator

Para executar, basta utilizar o comando padrão:
.\cache_simulator <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada

O arquivo_de_entrada deve estar contido no diretório "Endereços".