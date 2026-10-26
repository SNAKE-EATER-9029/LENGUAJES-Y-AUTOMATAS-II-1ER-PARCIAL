#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  q_tipo,     // Espera tipo de dato (int, float, etc.)
  q_var,      // Espera el nombre de la variable
  q_asign,    // Espera el operador '='
  q0,         // Inicio de la expresión matemática tras '='
  q_num,      // Leyendo parte entera de un número
  q_dec,      // Leyendo parte decimal de un número
  q_op,       // Espera operando después de +, -, *, /
  q_pcoma,    // Espera el fin de instrucción ';'
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

void evaluar_linea(const char *texto) {
  int i = 0;
  Status estado = q_tipo;
  int parentesis = 0;

  char tipo_detectado[20] = "";
  char variable_detectada[30] = "";
  char palabra_res[30] = "";

  int idx_temp = 0;
  char temp[50] = "";

  while (texto[i] != '\0') {
    char c = texto[i];

    if (c == '\n' || c == '\r') break;

    // Saltar espacios cuando no se está leyendo un identificador
    if (c == ' ' && estado != q_tipo && estado != q_var && estado != q_palabra) {
      i++;
      continue;
    }

    switch (estado) {
      case q_tipo: // Leyendo el tipo de dato (int, float, etc.)
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
          temp[idx_temp++] = c;
        } else if (c == ' ') {
          temp[idx_temp] = '\0';
          if (es_tipo_dato(temp)) {
            strcpy(tipo_detectado, temp);
            estado = q_var; // Pasa a esperar el nombre de la variable
            idx_temp = 0;
          } else {
            estado = q_error;
          }
        } else {
          estado = q_error;
        }
        break;

      case q_var: // Leyendo el nombre de la variable
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
          temp[idx_temp++] = c;
        } else if (c == ' ' || c == '=') {
          temp[idx_temp] = '\0';
          if (strlen(temp) > 0) {
            strcpy(variable_detectada, temp);
            idx_temp = 0;
            if (c == '=') estado = q0; // Pasa directo a la expresión
            else estado = q_asign;    // Espera el signo '='
          } else if (c == '=') {
            estado = q0;
          }
        } else {
          estado = q_error;
        }
        break;

      case q_asign: // Esperando el signo '='
        if (c == '=') {
          estado = q0;
        } else if (c != ' ') {
          estado = q_error;
        }
        break;

      case q0: // Inicio de la expresión matemática tras la asignación
        if (c >= '0' && c <= '9') {
          estado = q_num;
        } else if (c == '(') {
          parentesis++;
          estado = q0;
        } else if (c == '-') {
          estado = q_op;
        } else {
          estado = q_error;
        }
        break;

      case q_num: // Leyendo parte entera de un número
        if (c >= '0' && c <= '9') {
          estado = q_num;
        } else if (c == '.') {
          estado = q_dec;
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
          estado = q_op;
        } else if (c == ')') {
          parentesis--;
          if (parentesis < 0) estado = q_error;
        } else if (c == ';') { // Fin de la expresión/instrucción
          if (parentesis == 0) estado = q_pcoma;
          else estado = q_error;
        } else {
          estado = q_error;
        }
        break;

      case q_dec: // Leyendo parte decimal de un número
        if (c >= '0' && c <= '9') {
          estado = q_dec;
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
          estado = q_op;
        } else if (c == ')') {
          parentesis--;
          if (parentesis < 0) estado = q_error;
        } else if (c == ';') {
          if (parentesis == 0) estado = q_pcoma;
          else estado = q_error;
        } else {
          estado = q_error;
        }
        break;

      case q_op: // Esperando operando tras +, -, *, /
        if (c >= '0' && c <= '9') {
          estado = q_num;
        } else if (c == '(') {
          parentesis++;
          estado = q0;
        } else {
          estado = q_error;
        }
        break;

      case q_pcoma: // Tras encontrar ';', lee la palabra reservada opcional
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
          estado = q_palabra;
          idx_temp = 0;
          temp[idx_temp++] = c;
        } else if (c != ' ') {
          estado = q_error;
        }
        break;

      case q_palabra: // Leyendo la palabra reservada
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

  temp[idx_temp] = '\0';
  if (estado == q_palabra) {
    strcpy(palabra_res, temp);
  }

  // Verificación y reporte final de los elementos
  if ((estado == q_pcoma || estado == q_palabra) && parentesis == 0) {
    printf("-> Expresion SINTACTICAMENTE CORRECTA\n");
    printf("   - Tipo de dato:       %s\n", tipo_detectado);
    printf("   - Variable:           %s\n", variable_detectada);
    printf("   - Asignacion ('='):   SI\n");
    printf("   - Fin de linea (';'): SI\n");

    if (estado == q_palabra && es_palabra_reservada(palabra_res)) {
      printf("   - Palabra reservada:  '%s'\n", palabra_res);
    } else if (estado == q_palabra) {
      printf("   - Identificador:      '%s' (No es palabra reservada)\n", palabra_res);
    }
  } else {
    printf("-> Expresion INCORRECTA (Error sintactico o falta de ';')\n");
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

  printf("=== ANALIZADOR DE DECLARACIONES Y EXPRESIONES ===\n\n");

  while (fgets(buffer, sizeof(buffer), archivo) != NULL) {
    buffer[strcspn(buffer, "\n")] = 0;

    printf("Linea: \"%s\"\n", buffer);
    evaluar_linea(buffer);
    printf("--------------------------------------------------\n");
  }

  fclose(archivo);
  return 0;
}