#include <stdio.h>

typedef enum {
  q0,      // Espera inicio de número o '('
  q_num,   // Leyendo la parte entera de un número
  q_dec,   // Leyendo la parte decimal de un número
  q_op,    // Espera operando después de +, -, *, /
  q_error  // Estado de error syntax
} Status;

int main() {
  char texto[100];
  int i = 0;
  Status estado = q0;
  int parentesis = 0; // Contador para balancear paréntesis

  printf("Introduce tu expresion para evaluar: ");
  fgets(texto, sizeof(texto), stdin);

  while (texto[i] != '\0') {
    char c = texto[i];

    // Ignorar salto de línea al final
    if (c == '\n') {
      break;
    }

    // Ignorar espacios en blanco para no romper la secuencia
    if (c == ' ') {
      i++;
      continue;
    }

    switch (estado) {
      case q0: // Inicio de la expresión o tras un '('
        if (c >= '0' && c <= '9') {
          estado = q_num;
        } else if (c == '(') {
          parentesis++;
          estado = q0;
        } else if (c == '-') { // Permite número negativo inicial
          estado = q_op;
        } else {
          estado = q_error;
        }
        break;

      case q_num: // Leyendo dígitos de un número
        if (c >= '0' && c <= '9') {
          estado = q_num;
        } else if (c == '.') {
          estado = q_dec;
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
          estado = q_op;
        } else if (c == ')') {
          parentesis--;
          if (parentesis < 0) estado = q_error; // Cierra sin haber abierto
        } else {
          estado = q_error;
        }
        break;

      case q_dec: // Leyendo parte decimal tras el punto
        if (c >= '0' && c <= '9') {
          estado = q_dec;
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
          estado = q_op;
        } else if (c == ')') {
          parentesis--;
          if (parentesis < 0) estado = q_error;
        } else {
          estado = q_error;
        }
        break;

      case q_op: // Tras un operador +, -, *, /
        if (c >= '0' && c <= '9') {
          estado = q_num;
        } else if (c == '(') {
          parentesis++;
          estado = q0;
        } else {
          // Si viene otro operador (ej: *-), cae en error
          estado = q_error;
        }
        break;

      case q_error:
        // Se mantiene en error
        break;
    }

    i++;
  }

  // Para ser válida, debe terminar en un número o ')' y con paréntesis balanceados (0)
  if ((estado == q_num || estado == q_dec) && parentesis == 0) {
    printf("Expresion CORRECTA\n");
  } else {
    printf("Expresion INCORRECTA\n");
  }

  return 0;
}
