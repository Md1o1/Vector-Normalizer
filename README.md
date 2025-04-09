# Normalização de Vetores

Objetivo: Avaliar o impacto no desempenho da função normalize_feature_vector otimizando vetores em três métodos distintos:<br>
1. Lookup Table <br>
Nesta abordagem, utiliza-se uma tabela de consulta pré-computada para calcular a raiz quadrada inversa. <br>
A tabela deverá cobrir um intervalo de valores suficientemente grande e ser utilizada para normalizar os vetores. <br>
2. Otimização do Quake III (Newton-Raphson) <br>
Utiliza-se a otimização famosa do Quake III utilizando o método de Newton-Raphson para calcular a raiz quadrada inversa. <br>
Esta técnica envolve manipulação de bits para uma estimativa inicial rápida, seguida de iterações para refinar o resultado. <br>
3. Instruções de Hardware (SSE rsqrtss) <br>
Utiliza as instruções de hardware SSE (rsqrtss) para calcular a raiz quadrada inversa de forma eficiente. <br>
Esta abordagem explora as capacidades do processador para realizar cálculos de ponto flutuante rapidamente, utilizando o header <xmmintrin.h>. <br>

##
