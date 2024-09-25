#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <xmmintrin.h>  // Para instruções SSE

#define LUT_SIZE 10000
#define LUT_MAX 100.0f

// Tabela de consulta para raiz quadrada inversa
float inv_sqrt_lut[LUT_SIZE];

// Função para inicializar a tabela de consulta
void initialize_lut() {    for (int i = 0; i < LUT_SIZE; i++) {        
    float x = ((float)i / (LUT_SIZE - 1)) * LUT_MAX;        
    inv_sqrt_lut[i] = 1.0f / sqrt(x);    
    }
}

// Função para normalizar usando tabela de consulta
void normalize_feature_vector_lut(float* features, int length) {   
    float sum = 0.0f;    for (int i = 0; i < length; i++) {        
        sum += features[i] * features[i];  
        }
    // Limitar o valor de sum ao intervalo da tabela    
    if (sum >= LUT_MAX) sum = LUT_MAX;
    
    // Obter índice aproximado na tabela   
    int idx = (int)((sum / LUT_MAX) * (LUT_SIZE - 1));
    float inv_sqrt = inv_sqrt_lut[idx];

    // Normalizar os vetores  
    for (int i = 0; i < length; i++) {        
       features[i] *= inv_sqrt;    
    }
}

// Função para calcular a raiz quadrada inversa usando a otimização de Quake III
float quake_rsqrt(float number) {    
    long i;    
    float x2, y;    
    const float threehalfs = 1.5F;
    x2 = number * 0.5F;    
    y = number;    
    i = *(long*)&y;                      

    // Acessa bits do float    
    i = 0x5f3759df - (i >> 1);            

    // Estimativa inicial mágica    
    y = *(float*)&i;    y = y * (threehalfs - (x2 * y * y));  

    // 1ª iteração de Newton-Raphson    
    return y;
}

// Função para normalizar usando o método do Quake III
void normalize_feature_vector_quake(float* features, int length) {    
    float sum = 0.0f;    for (int i = 0; i < length; i++) {       
       sum += features[i] * features[i];    
    } 

    float inv_sqrt = quake_rsqrt(sum);
    for (int i = 0; i < length; i++) { 
        features[i] *= inv_sqrt;    
    }
 }

// Função para normalizar usando SSE rsqrtss
void normalize_feature_vector_sse(float* features, int length) {    
    float sum = 0.0f;    
    for (int i = 0; i < length; i++) {        
        sum += features[i] * features[i];   
     }
    __m128 vec = _mm_set_ss(sum);   // Carregar soma em um registrador SSE   
    vec = _mm_rsqrt_ss(vec);        // Calcular raiz quadrada inversa
    
    float inv_sqrt;   
    _mm_store_ss(&inv_sqrt, vec);   // Armazenar o resultado
    
    for (int i = 0; i < length; i++) {        
        features[i] *= inv_sqrt;    
    }
}

// Função para ler dados de um arquivo CSV
float** read_csv(const char* filename, int* num_elements, int* num_dimensions) {    
    FILE* file = fopen(filename, "r");    
    if (!file) {        
        perror("Failed to open file");        
        exit(EXIT_FAILURE);    
    }

    // Determine the number of elements and dimensions   
    *num_elements = 0;   
    *num_dimensions = 0;    
    char line[1024];    
    while (fgets(line, sizeof(line), file)) {        
        if (*num_elements == 0) {            
            char* token = strtok(line, ",");            
            while (token) { (*num_dimensions)++; token = strtok(NULL, ",");
            }        
        } 
        (*num_elements)++;    
    }    
    rewind(file);


    // Allocate memory for the features    
    float** features = (float**)malloc(*num_elements * sizeof(float*));    
        for (int i = 0; i < *num_elements; i++) {        
            features[i] = (float*)malloc(*num_dimensions * sizeof(float));    
        }

    // Read the data   
    int i = 0;    
    while (fgets(line, sizeof(line), file)) { 
        int j = 0;        
        char* token = strtok(line, ",");        
        while (token) {            
            features[i][j++] = atof(token);            
            token = strtok(NULL, ",");        
        } 
        i++;    
    }

    fclose(file);    
    return features;
}

// Função para medir o tempo de execução usando a biblioteca 'resources'
void get_resource_usage(struct rusage* usage) {    
    getrusage(RUSAGE_SELF, usage);
    }
void print_resource_usage(const char* label, struct rusage* usage) {    
    printf("%s\n", label);    
    printf("User time: %ld.%06ld seconds\n", usage->ru_utime.tv_sec, usage->ru_utime.tv_usec);    
    printf("System time: %ld.%06ld seconds\n", usage->ru_stime.tv_sec, usage->ru_stime.tv_usec);    
    printf("Maximum resident set size: %ld kilobytes\n", usage->ru_maxrss);
    }

int main() {    
    int num_elements, num_dimensions;    
    float** features = read_csv("data.csv", &num_elements, &num_dimensions);
    
    struct rusage start_usage, end_usage;

    // Tabela de consulta    
    initialize_lut();    
    get_resource_usage(&start_usage);    
    for (int i = 0; i < num_elements; i++) {        
        normalize_feature_vector_lut(features[i], num_dimensions);    
    }    
    get_resource_usage(&end_usage);    
    printf("LUT normalization:\n");    
    print_resource_usage("Start", &start_usage);    
    print_resource_usage("End", &end_usage);

    // Otimização Quake III    
    get_resource_usage(&start_usage);    
    for (int i = 0; i < num_elements; i++) {        
        normalize_feature_vector_quake(features[i], num_dimensions);    
    }    
    get_resource_usage(&end_usage);    
    printf("Quake III normalization:\n");    
    print_resource_usage("Start", &start_usage);    
    print_resource_usage("End", &end_usage);

    // SSE rsqrtss    
    get_resource_usage(&start_usage);    
    for (int i = 0; i < num_elements; i++) {        
        normalize_feature_vector_sse(features[i], num_dimensions);    
    }    
    get_resource_usage(&end_usage);    
    printf("SSE normalization:\n");    
    print_resource_usage("Start", &start_usage);    
    print_resource_usage("End", &end_usage);

    // Liberação da memória    
    for (int i = 0; i < num_elements; i++) {        
        free(features[i]);    
    }    
    free(features);
    
    return 0;
}
    
#!/bin/bash
echo "Executando testes de desempenho para diferentes otimizações..."
# Compilar o programa
gcc -o normalize normalize.c -lm -lrt -O2
# Executar o programa e salvar saída em 
output.txt./normalize > output.txt
echo "Resultados salvos em output.txt"


esse script facilita a execução dos testes de desempenho.
Ele compila o código e executa o programa, redirecionando a saída para um arquivo output.txt
Instruções para Compilação e Execução
Coloque todos os arquivos no mesmo diretório:
normalize.c (o código fonte)run_tests.sh (script de execução)data.csv (arquivo CSV com dados de entrada)
Para compilar e rodar o programa, execute os comandos:
bash Copiar 
códigochmod +x run_tests.sh 
# Para tornar o script executável./run_tests.sh         
# Executa o script para compilar e rodar o programa
Os resultados serão salvos no arquivo output.txt.

Entregáveis
Código Fonte:
Código fonte incluindo as três versões otimizadas do método normalize_feature_vector.
Código para leitura dos dados do arquivo CSV.
Exemplo de como compilar e executar o trabalho.
Scripts de teste utilizados para medir o desempenho.
Os fontes, bem como demais arquivos necessários para compilação e execução, bem como um arquivo de entrada de exemplo, deverão ser enviados num arquivo compactado único no link do trabalho no canvas.
OBSERVAÇÃO: O código deve ser compatível com arquitetura GNU/Linux. Trabalhos compatíveis apenas com outros sitemas operacionais NÃO serão avaliados