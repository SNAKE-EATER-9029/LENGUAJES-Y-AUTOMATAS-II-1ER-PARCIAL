#include <stdio.h>

typedef enum {
  q0,      // Estado inicial
  q1,      // Parte entera (dígitos)
  q2,      // Punto decimal
  q3,      // Parte decimal (dígitos después del punto)
  q_error  // Estado de error
} Status;

int main() {
  char texto[100];
  int i = 0;
  Status estado = q0;
  int es_negativo = 0; 

  printf("Introduce tu informacion para evaluar: ");
  fgets(texto, sizeof(texto), stdin);

  while (texto[i] != '\0') {
    char c = texto[i];

   
    if (c == '\n') {
      break;
    }

    switch (estado) {
      case q0:
        if (c == '-') {
          es_negativo = 1; 
          estado = q1;
        } else if (c == '+') {
          es_negativo = 0; 
          estado = q1;
        } else if (c >= '0' && c <= '9') {
          estado = q1; // Si es número, pasa a parte entera
        } else {
          estado = q_error;
        }
        break;

      case q1:
        if (c >= '0' && c <= '9') {
          estado = q1; // Sigue siendo número entero
        } else if (c == '.') {
          estado = q2; // Encuentra el punto decimal
        } else {
          estado = q_error;
        }
        break;

      case q2:
        if (c >= '0' && c <= '9') {
          estado = q3; // Después del punto viene al menos un número
        } else {
          estado = q_error;
        }
        break;

      case q3:
        if (c >= '0' && c <= '9') {
          estado = q3; // Sigue leyendo la parte decimal
        } else {
          estado = q_error;
        }
        break;

      case q_error:
        // Si entra en error, se queda ahí
        break;
    }
    i++;
  }

  // Verificación del estado final
  if (estado == q3) {
    if (es_negativo) {
      printf("El texto ingresado es un numero decimal negativo valido.\n");
    } else {
      printf("El texto ingresado es un numero decimal positivo valido.\n");
    }
  } else if (estado == q1) {
    if (es_negativo) {
      printf("El texto ingresado es un numero entero negativo (sin decimales).\n");
    } else {
      printf("El texto ingresado es un numero entero positivo (sin decimales).\n");
    }
  } else {
    printf("Error: No es un numero valido.\n");
  }

  return 0;
}
