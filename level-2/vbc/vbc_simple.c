// ===== CALCULADORA DE EXPRESIONES ARITMÉTICAS SIMPLE =====
// Este programa evalúa expresiones aritméticas con +, *, paréntesis y dígitos (0-9)
// Ejemplo: "./programa "2+3*4"" imprime "14" (respeta precedencia: primero * luego +)

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

// ===== VARIABLE GLOBAL =====
char *s;  // Puntero global que apunta a la posición actual en la cadena de entrada
          // Se va moviendo a medida que parseamos la expresión

// ===== DECLARACIONES ANTICIPADAS =====
// Necesarias porque las funciones se llaman recursivamente entre sí
int sum();    // Parsea y evalúa sumas (menor precedencia)

// ===== FUNCIÓN: paren() =====
// Parsea y evalúa PARÉNTESIS y NÚMEROS (elementos básicos)
// Gramática: paren ::= digit | '(' sum ')'
int paren()
{
	int result = 0;
	
	// CASO 1: Si es un dígito (0-9)
	if (isdigit(*s))
	{
		result = *s - '0';  // Convertimos el caracter ASCII a número (ej: '5' -> 5)
		s++;                 // Avanzamos al siguiente caracter
		return result;
	}
	// CASO 2: Si es un paréntesis abierto '('
	else if(*s == '(')
	{
		s++;                 // Saltamos el '('
		result = sum();      // Evaluamos recursivamente la expresión dentro del paréntesis
		s++;                 // Saltamos el ')' (asumimos que está bien formado)
	}
	return result;
}

// ===== FUNCIÓN: mult() =====
// Parsea y evalúa MULTIPLICACIONES (precedencia media)
// Gramática: mult ::= paren ('*' paren)*
// El bucle while maneja cadenas como "2*3*4" (asociatividad izquierda)
int mult()
{
	int result = paren();    // Primero obtenemos el primer factor (número o paréntesis)
	
	// Mientras encontremos el operador '*'
	while(*s == '*')
	{
		s++;                  // Saltamos el '*'
		result *= paren();    // Multiplicamos el resultado acumulado por el siguiente factor
	}
	return result;
}

// ===== FUNCIÓN: sum() =====
// Parsea y evalúa SUMAS (menor precedencia)
// Gramática: sum ::= mult ('+' mult)*
// El bucle while maneja cadenas como "2+3+4" (asociatividad izquierda)
int sum()
{
	int result = mult();     // Primero evaluamos multiplicaciones (mayor precedencia)
	
	// Mientras encontremos el operador '+'.
	while(*s == '+')
	{
		s++;                  // Saltamos el '+'.
		result += mult();     // Sumamos el resultado acumulado con el siguiente término
	}
	return result;
}

// ===== FUNCIÓN: unexpected() =====
// Muestra mensajes de error cuando encuentra tokens inesperados
// Retorna 1 para indicar error
int	unexpected(char c)
{
	if (c)
		printf("Unexpected token '%c'\n", c);  // Error: caracter no permitido
	else
		printf("Unexpected end of input\n");    // Error: la entrada terminó inesperadamente
	return 1;
}

// ===== FUNCIÓN: parse() =====
// Valida la expresión ANTES de evaluarla (pre-procesamiento)
// Verifica:
// 1. Que solo contenga caracteres válidos: dígitos, +, *, (, )
// 2. Que los paréntesis estén balanceados
// 3. Que no termine con un operador (+, *)
int parse(char *a)
{
	int i = 0;
	int par = 0;  // Contador de paréntesis (balance)
	
	// Recorremos toda la cadena
	while(a[i])
	{
		if (a[i] == '(')
			par++;  // Incrementamos el contador con cada '('
		else if (a[i] == ')')
			par--;  // Decrementamos el contador con cada ')'
		else if(!isdigit(a[i]) && a[i] != '+' && a[i] != '*')
			return unexpected(a[i]);  // Caracter inválido
		i++;
	}
	
	// Verificamos el balance de paréntesis
	if (par < 0)
		return unexpected(')');  // Hay más ')' que '('
	if (par > 0)
		return unexpected('(');  // Hay más '(' que ')'
	
	// Verificamos que no termine con un operador
	i--;
	if (a[i] == '+' || a[i] == '*')
		return unexpected(0);     // Error: termina con operador
	
	return 0;  // Todo OK
}

// ===== FUNCIÓN PRINCIPAL =====
int main(int ac, char **av)
{
	// Validamos que haya exactamente 1 argumento (la expresión)
	if(ac != 2 || parse(av[1]))
		return -1;
	
	// Inicializamos el puntero global al inicio de la expresión
	s = av[1];
	
	// Evaluamos la expresión completa (empezando por sum, la menor precedencia)
	int result = sum();
	
	// Imprimimos el resultado
	printf("%d\n", result);
	return 0;
}