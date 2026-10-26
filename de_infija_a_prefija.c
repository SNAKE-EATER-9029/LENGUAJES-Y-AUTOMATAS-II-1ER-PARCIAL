#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  q_tipo,     // Espera tipo de dato O inicio de expresión directa
  q_var,      // Espera el nombre de la variable
  q_asign,    // Espera el operador '='
  q0,         // Inicio de la expresión matemática tras '='
  q_num,      // Leyendo parte entera de un número
  q_dec,      // Leyendo parte decimal de un número
  q_op,       // Espera operando después de +, -, *, /
  q_pcoma,    // Espera el fin de instrucción ';' (opcional si es expresión simple)
  q_palabra,  // Espera la palabra reservada en inglés al final
  q_error     // Estado de error sintáctico
} Status;

// Tipos de datos permitidos
const char *tipos_datos[] = {
  "int", "float", "char", "double", "bool", "long", "short"
};
const int num_tipos = 7;

// Palabras reservadas en inglés
const char *palabras_reservadas[] = {
  "if", "else", "while", "for", "return", "print", "fn", "end"
};
const int num_reservadas = 8;

int es_tipo_dato(const char *word) {
  for (int j = 0; j < num_tipos; j++) {
    if (strcmp(word, tipos_datos[j]) == 0) return 1;
  }
  return 0;
}

int es_palabra_reservada(const char *word) {
  for (int j = 0; j < num_reservadas; j++) {
    if (strcmp(word, palabras_reservadas[j]) == 0) return 1;
  }
  return 0;
}

// Función que define la prioridad de los operadores
int prioridad(char op) {
  if (op == '+' || op == '-') return 1;
  if (op == '*' || op == '/') return 2;
  return 0;
}

// Estructura de Pila para el algoritmo de conversión prefija
typedef struct {
  char items[100][50];
  int tope;
} PilaStr;

void push(PilaStr *p, const char *val) {
  p->tope++;
  strcpy(p->items[p->tope], val);
}

void pop(PilaStr *p, char *out) {
  if (p->tope >= 0) {
    strcpy(out, p->items[p->tope]);
    p->tope--;
  }
}

// ALGORITMO CONV-PREFIJA (ExpINF -> ExpPRE)
void infija_a_prefija(char tokens[][50], int num_tokens, char *expPRE) {
  PilaStr pila;
  pila.tope = -1;
  char res_rev[100][50];
  int count_rev = 0;

  for (int k = num_tokens - 1; k >= 0; k--) {
    char *tok = tokens[k];

    if (strcmp(tok, ")") == 0) {
      push(&pila, tok);
    } 
    else if (strcmp(tok, "(") == 0) {
      char elem[50];
      while (pila.tope >= 0 && strcmp(pila.items[pila.tope], ")") != 0) {
        pop(&pila, elem);
        strcpy(res_rev[count_rev++], elem);
      }
      if (pila.tope >= 0) {
        pop(&pila, elem);
      }
    } 
    else if (strchr("+-*/", tok[0]) && strlen(tok) == 1) {
      char elem[50];
      while (pila.tope >= 0 && strcmp(pila.items[pila.tope], ")") != 0 &&
             prioridad(tok[0]) < prioridad(pila.items[pila.tope][0])) {
        pop(&pila, elem);
        strcpy(res_rev[count_rev++], elem);
      }
      push(&pila, tok);
    } 
    else {
      strcpy(res_rev[count_rev++], tok);
    }
  }

  while (pila.tope >= 0) {
    char elem[50];
    pop(&pila, elem);
    strcpy(res_rev[count_rev++], elem);
  }

  expPRE[0] = '\0';
  for (int k = count_rev - 1; k >= 0; k--) {
    strcat(expPRE, res_rev[k]);
    strcat(expPRE, " ");
  }
}

void evaluar_linea(const char *texto) {
  int i = 0;
  Status estado = q_tipo;
  int parentesis = 0;
  int tiene_asignacion = 0;

  char tipo_detectado[20] = "";
  char variable_detectada[30] = "";
  char palabra_res[30] = "";

  int idx_temp = 0;
  char temp[50] = "";

  char tokens_exp[50][50];
  int num_tokens = 0;
  char num_buffer[50] = "";
  int idx_num = 0;

  while (texto[i] != '\0') {
    char c = texto[i];

    if (c == '\n' || c == '\r') break;

    if (c == ' ' && estado != q_tipo && estado != q_var && estado != q_palabra) {
      i++;
      continue;
    }

    switch (estado) {
      case q_tipo:
        // Si empieza directamente con un número o '(', es una expresión simple
        if ((c >= '0' && c <= '9') || c == '(' || c == '-') {
          estado = q0;
          continue; // Revalida este carácter en q0 sin avanzar 'i'
        }
        else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
          temp[idx_temp++] = c;
        } else if (c == ' ') {
          temp[idx_temp] = '\0';
          if (es_tipo_dato(temp)) {
            strcpy(tipo_detectado, temp);
            estado = q_var;
            idx_temp = 0;
            tiene_asignacion = 1;
          } else {
            estado = q_error;
          }
        } else {
          estado = q_error;
        }
        break;

      case q_var:
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
          temp[idx_temp++] = c;
        } else if (c == ' ' || c == '=') {
          temp[idx_temp] = '\0';
          if (strlen(temp) > 0) {
            strcpy(variable_detectada, temp);
            idx_temp = 0;
            if (c == '=') estado = q0;
            else estado = q_asign;
          } else if (c == '=') {
            estado = q0;
          }
        } else {
          estado = q_error;
        }
        break;

      case q_asign:
        if (c == '=') {
          estado = q0;
        } else if (c != ' ') {
          estado = q_error;
        }
        break;

      case q0:
        if (c >= '0' && c <= '9') {
          estado = q_num;
          num_buffer[idx_num++] = c;
        } else if (c == '(') {
          parentesis++;
          estado = q0;
          strcpy(tokens_exp[num_tokens++], "(");
        } else if (c == '-') {
          estado = q_op;
          strcpy(tokens_exp[num_tokens++], "-");
        } else {
          estado = q_error;
        }
        break;

      case q_num:
        if (c >= '0' && c <= '9') {
          estado = q_num;
          num_buffer[idx_num++] = c;
        } else if (c == '.') {
          estado = q_dec;
          num_buffer[idx_num++] = c;
        } else {
          if (idx_num > 0) {
            num_buffer[idx_num] = '\0';
            strcpy(tokens_exp[num_tokens++], num_buffer);
            idx_num = 0;
          }

          if (c == '+' || c == '-' || c == '*' || c == '/') {
            estado = q_op;
            char op_str[2] = {c, '\0'};
            strcpy(tokens_exp[num_tokens++], op_str);
          } else if (c == ')') {
            parentesis--;
            strcpy(tokens_exp[num_tokens++], ")");
            if (parentesis < 0) estado = q_error;
          } else if (c == ';') {
            if (parentesis == 0) estado = q_pcoma;
            else estado = q_error;
          } else {
            estado = q_error;
          }
        }
        break;

      case q_dec:
        if (c >= '0' && c <= '9') {
          estado = q_dec;
          num_buffer[idx_num++] = c;
        } else {
          if (idx_num > 0) {
            num_buffer[idx_num] = '\0';
            strcpy(tokens_exp[num_tokens++], num_buffer);
            idx_num = 0;
          }

          if (c == '+' || c == '-' || c == '*' || c == '/') {
            estado = q_op;
            char op_str[2] = {c, '\0'};
            strcpy(tokens_exp[num_tokens++], op_str);
          } else if (c == ')') {
            parentesis--;
            strcpy(tokens_exp[num_tokens++], ")");
            if (parentesis < 0) estado = q_error;
          } else if (c == ';') {
            if (parentesis == 0) estado = q_pcoma;
            else estado = q_error;
          } else {
            estado = q_error;
          }
        }
        break;

      case q_op:
        if (c >= '0' && c <= '9') {
          estado = q_num;
          num_buffer[idx_num++] = c;
        } else if (c == '(') {
          parentesis++;
          estado = q0;
          strcpy(tokens_exp[num_tokens++], "(");
        } else {
          estado = q_error;
        }
        break;

      case q_pcoma:
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
          estado = q_palabra;
          idx_temp = 0;
          temp[idx_temp++] = c;
        } else if (c != ' ') {
          estado = q_error;
        }
        break;

      case q_palabra:
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
          temp[idx_temp++] = c;
        } else if (c == ' ') {
          temp[idx_temp] = '\0';
          break;
        } else {
          estado = q_error;
        }
        break;

      case q_error:
        break;
    }

    i++;
  }

  if (idx_num > 0) {
    num_buffer[idx_num] = '\0';
    strcpy(tokens_exp[num_tokens++], num_buffer);
  }

  temp[idx_temp] = '\0';
  if (estado == q_palabra) {
    strcpy(palabra_res, temp);
  }

  // Verificación válida para:
  // 1) Instrucción con ';' o palabra reservada
  // 2) Expresión aritmética simple que finaliza en número o decimal sin ';'
  if (((estado == q_pcoma || estado == q_palabra) || (!tiene_asignacion && (estado == q_num || estado == q_dec))) && parentesis == 0) {
    printf("-> Expresion SINTACTICAMENTE CORRECTA\n");

    if (tiene_asignacion) {
      printf("   - Tipo de dato:       %s\n", tipo_detectado);
      printf("   - Variable:           %s\n", variable_detectada);
      printf("   - Asignacion ('='):   SI\n");
      printf("   - Fin de linea (';'): SI\n");
    } else {
      printf("   - Modo:               Expresion Aritmetica Simple\n");
    }

    if (estado == q_palabra && es_palabra_reservada(palabra_res)) {
      printf("   - Palabra reservada:  '%s'\n", palabra_res);
    } else if (estado == q_palabra) {
      printf("   - Identificador:      '%s' (No es palabra reservada)\n", palabra_res);
    }

    // CONVERSIÓN A NOTACIÓN PREFIJA
    char expPRE[200] = "";
    infija_a_prefija(tokens_exp, num_tokens, expPRE);

    printf("   - Notacion Infija:   ");
    for (int k = 0; k < num_tokens; k++) printf("%s ", tokens_exp[k]);
    printf("\n");
    printf("   - Notacion Prefija:  %s\n", expPRE);

  } else {
    printf("-> Expresion INCORRECTA (Error sintactico o parentesis desbalanceados)\n");
  }
}

int main() {
  FILE *archivo;
  char buffer[256];

  archivo = fopen("expresiones.txt", "r");

  if (archivo == NULL) {
    printf("Error al abrir el archivo.\n");
    return 1;
  }

  printf("=== ANALIZADOR Y CONVERTIDOR INFIJA -> PREFIJA ===\n\n");

  while (fgets(buffer, sizeof(buffer), archivo) != NULL) {
    buffer[strcspn(buffer, "\n")] = 0;

    printf("Linea: \"%s\"\n", buffer);
    evaluar_linea(buffer);
    printf("--------------------------------------------------\n");
  }

  fclose(archivo);
  return 0;
}