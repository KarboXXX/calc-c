#include <bits/types/locale_t.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <stdbool.h>
#include <ctype.h>
#include <float.h>
#include <stdlib.h>

#define MAX_LENGTH 60 + 1 // +1 por conta do EOF
#define error_message_no_stdin "não foi possível obter entrada, talvez você esteja usando um terminal sem entrada padrão?\n"

/*
  verifica se todos os characteres de uma string são números, e retorna char = 1 ou char = 0,
  em caso de verdadeiro ou falso respectivamente.

  char foi usado porque é o tipo de variável que ocupa o menor número de bytes.
*/
bool is_number(char *string, char valid_decimal_point) {
  int len = strlen(string);
  if (len == 0)
    return false;

  for (int i=0; i<len; i++) {
    if (!isdigit(string[i]) && string[i] != valid_decimal_point) { // tambem levar em consideração números decimais, avaliando strings com . e , como números.
      return false;
    }
  }
  
  return true;
}

/*
 limpa o restante do standard input fazendo o flush
 dos caracteres até o proximo EOF (end of file, ou enter)
*/
void flush_stdin() {
  int char_code;
  while ((char_code = getc(stdin)) != EOF && char_code != '\n') {};
}

/*
  mostra uma mensagem, pede um input do usuário, guarda no buffer *buf impedindo buffer overflow,
  confirma que o input do usuário contém apenas números,
  substitui todos os pontos decimais inválidos para o válido sob o idioma do usuário,
  impede que o conteúdo do buffer ultrapasse o limite desejado, e converte o resultado para float, e o retorna.
*/
bool input(char *message, char *buf, char decimal_point) {
  printf("%s", message);
  if (fgets(buf, MAX_LENGTH * sizeof(char), stdin) == NULL) { // fgets é mais seguro que gets, e impede buffer overflow, erro caso standard input não existir
    printf("%s", error_message_no_stdin);
    return false; // retornar false em caso de erro.
  } else {
    size_t len = strlen(buf);
    bool got_newline = (len > 0 && buf[len - 1] == '\n');

    // caso o input seja grande demais, a gente limpa o restante do standard input
    // fazendo o flush dos caracteres até o proximo EOF (end of file, ou enter)
    // para em caso de erro, a proxima leitura do stdin esteja vazia.
    if (!got_newline && !feof(stdin))
      flush_stdin();

    if (got_newline) {
      buf[len - 1] = '\0'; // limpar a primeira (e única possível) ocorrencia do caractere de escape do buffer (enter, ou nova linha)
      len--;
    }
      
    return true;
  }  
}

enum regiao { sul, sudeste, norte, nordeste };
const char *regioes[] = {"sul", "sudeste", "norte", "nordeste"};

bool regiao_valida(enum regiao subject) {
  switch (subject) {
  case sul: return true;
  case sudeste: return true;
  case norte: return true;
  case nordeste: return true;
  }
  return false;
}

typedef struct {
    enum regiao regiao;
    double peso;
    double preco;
    char nome[MAX_LENGTH];
    char codigo[MAX_LENGTH];
} product;

double calcular_frete(enum regiao regiao, double peso) {
    if (peso <= 2.0f) {
        switch(regiao) {
            case sul: return 30.0f;  // sul
            case sudeste: return 25.0f;  // sudeste
            case norte: return 35.0f;  // norte
            case nordeste: return 40.0f;  // nordeste
            default: return 0.0f;
        }
    } else {
        switch(regiao) {
            case sul: return 50.0f;  // sul >2kg
            case sudeste: return 45.0f;  // sudeste >2kg
            case norte: return 55.0f;  // norte >2kg
            case nordeste: return 60.0f;  // nordeste >2kg
            default: return 0.0f;
        }
    }
}

void add_dias(struct tm* data, int dias) {
    data->tm_mday += dias;
    mktime(data);  // normaliza a data automaticamente
}

int main() {
    setlocale(LC_ALL, "");
    
    char d_point = localeconv()->decimal_point[0];
 
    product prod;
    
    char codigo[MAX_LENGTH] = "";
    while (strlen(codigo) == 0)
      input("codigo do produto: ", codigo, d_point);    
    strcpy(prod.codigo, codigo);
    
    char nome[MAX_LENGTH] = "";
    while (strlen(nome) == 0)
      input("nome do produto: ", nome, d_point);
    strcpy(prod.nome, nome);
    
    char peso_buf[MAX_LENGTH] = "";
    while (!is_number(peso_buf, d_point) || strlen(peso_buf) == 0)
      input("peso (kg): ", peso_buf, d_point);

    char *end = NULL;
    prod.peso = strtod(peso_buf, &end);
    
    char preco_buf[MAX_LENGTH];
    while (!is_number(preco_buf, d_point) || strlen(preco_buf) == 0)
      input("preço unitário (R$): ", preco_buf, d_point);

    prod.preco = strtod(preco_buf, &end);
    
    printf("região de entrega:\n");
    size_t regioes_length = sizeof(regioes) / sizeof(regioes[0]);
    for (int i=0; i<regioes_length; i++)
      printf("[%d] %s\t", i+1, regioes[i]);
    printf("\n");
    
    char regiao_buf[MAX_LENGTH];
    while (strlen(regiao_buf) == 0 || !is_number(regiao_buf, d_point) || !regiao_valida(prod.regiao)){
      input("escolha (1-4): ", regiao_buf, d_point);
      prod.regiao = strtod(regiao_buf, &end) - 1; // compensar pelo começo sendo 1 e não 0
    }
    
    float frete = calcular_frete(prod.regiao, prod.peso);
    float total = prod.preco + frete;
    
    time_t agora = time(NULL);
    struct tm* data_compra = localtime(&agora);
    struct tm data_entrega = *data_compra;
    
    add_dias(&data_entrega, 6); // adiciona 6d para a data entrega (exemplo estimado)
    
    printf("\ncódigo: %s\n", prod.codigo);
    printf("produto: %s\n", prod.nome);
    printf("peso: %.2f kg\n", prod.peso);
    printf("preço: R$ %.2f\n", prod.preco);
    
    printf("região: %s\n", regioes[prod.regiao]);
    printf("frete: R$ %.2f\n", frete);
    printf("total: R$ %.2f\n", total);
    
    printf("\ndata/hora da compra: %s", asctime(data_compra));
    printf("data prevista entrega: %s", asctime(&data_entrega));
    
    return 0;
}
