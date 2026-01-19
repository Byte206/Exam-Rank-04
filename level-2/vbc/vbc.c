// Incluimos las librerías necesarias
#include <stdio.h>     // Para printf (imprimir en pantalla)
#include <stdlib.h>    // Para calloc y free (gestión de memoria dinámica)
#include <ctype.h>     // Para isdigit (verificar si un caracter es un dígito)

// ===== VARIABLES GLOBALES =====
// En lugar de pasar punteros complicados, usamos variables globales más simples
char *input_string;    // La cadena de entrada que vamos a parsear
int pos = 0;           // Posición actual en la cadena (índice)

// Definimos una estructura para representar los nodos del árbol sintáctico
typedef struct node {
    enum {              // Enumeración para indicar el tipo de nodo:
        ADD,            // Nodo de suma (+)
        MULTI,          // Nodo de multiplicación (*)
        VAL             // Nodo valor (un dígito)
    }   type;
    int           val;  // El valor numérico (solo se usa si type == VAL)
    struct node   *l;   // Puntero al hijo izquierdo del árbol
    struct node   *r;   // Puntero al hijo derecho del árbol
} node;

// Función para crear un nuevo nodo en memoria dinámica
node *new_node(node n)
{
    node *ret = calloc(1, sizeof(*ret));  // Reservamos memoria inicializada a 0 para un nodo
    if (!ret)                              // Si calloc falla (memoria insuficiente)
        return NULL;                       // Retornamos NULL
    *ret = n;                              // Copiamos los valores del nodo n al espacio reservado
    return ret;                            // Retornamos el puntero al nuevo nodo
}

// Función recursiva para liberar toda la memoria del árbol
void destroy_tree(node *n)
{
    if (!n)                       // Si el nodo es NULL, no hay nada que liberar
        return;
    if (n->type != VAL) {         // Si el nodo NO es un valor (es ADD o MULTI)
        destroy_tree(n->l);       // Liberamos recursivamente el subárbol izquierdo
        destroy_tree(n->r);       // Liberamos recursivamente el subárbol derecho
    }
    free(n);                      // Liberamos la memoria del nodo actual
}

// Función para mostrar un mensaje de error cuando encuentra un token inesperado
void unexpected(char c)
{
    if (c)                                        // Si c es un caracter válido (no '\0')
        printf("Unexpected token '%c'\n", c);     // Imprimimos qué caracter inesperado encontró
    else
        printf("Unexpected end of input\n");      // Si c es '\0', la entrada terminó inesperadamente
}

// Función simplificada que intenta "aceptar" un caracter específico
// Lee de la posición actual (pos) en input_string
// Si el caracter coincide, avanza pos. Retorna 1 si acepta, 0 si no.
int accept(char c)
{
    if (input_string[pos] == c) {  // Si el caracter actual es igual a c
        pos++;                      // Avanzamos la posición global
        return 1;                   // Retornamos 1 (éxito)
    }
    return 0;                       // Retornamos 0 (no coincide)
}

// Función simplificada que "espera" un caracter específico
// Si no lo encuentra, muestra un error. Retorna 1 si encuentra, 0 si no.
int expect(char c)
{
    if (accept(c))                // Intentamos aceptar el caracter
        return 1;                 // Si lo acepta, retornamos 1
    unexpected(input_string[pos]); // Si no, mostramos error con el caracter actual
    return 0;                     // Retornamos 0 (fallo)
}

// Declaración anticipada de las funciones de parsing (análisis sintáctico)
// Esto es necesario porque estas funciones se llaman entre sí recursivamente
static node *parse_expr_r(void);  // Parsea expresiones (suma)
static node *parse_term   (void);  // Parsea términos (multiplicación)
static node *parse_factor (void);  // Parsea factores (números o paréntesis)

/* ADDED: parsing d’un facteur (chiffre ou parenthèse) */
static node *parse_factor(char **s)
{
    if (isdigit((unsigned char)**s)) {
        node n = { .type = VAL, .val = **s - '0', .l = NULL, .r = NULL };
        (*s)++;
        return new_node(n);
    }
    if (accept(s, '(')) {
        node *e = parse_expr_r(s);
        if (!e)
            return NULL;
        if (!expect(s, ')')) {          // MOD: vérification de la parenthèse fermante
            destroy_tree(e);
            return NULL;
        }
        return e;
    }
    unexpected(**s);
    return NULL;
}

// Parsea un TÉRMINO: maneja multiplicaciones (*) con precedencia mayor que sumas
// Ejemplo: "3*4*5" genera un árbol de multiplicaciones
static node *parse_term(char **s)
{
    node *left = parse_factor(s);   // Parseamos el primer factor
    if (!left)                       // Si falla, retornamos NULL
        return NULL;
    // Mientras encontremos el operador '*'
    while (accept(s, '*')) {
        node *right = parse_factor(s);  // Parseamos el siguiente factor
        if (!right) {                    // Si falla
            destroy_tree(left);          // Liberamos el árbol izquierdo
            return NULL;
        }
        // Creamos un nodo MULTI que une left y right
        node n = { .type = MULTI, .l = left, .r = right };
        left = new_node(n);              // El nuevo nodo se convierte en el nuevo "left"
        if (!left)                       // Si falla la creación del nodo
            return NULL;
    }
    return left;  // Retornamos el árbol de multiplicaciones construido
}

// Parsea una EXPRESIÓN: maneja sumas (+) con precedencia menor que multiplicaciones
// Ejemplo: "2+3*4" parsea primero "3*4" y luego suma "2" al resultado
// Esta función es similar a parse_term, pero para el operador '+'
static node *parse_expr_r(char **s)
{
    node *left = parse_term(s);     // Parseamos el primer término
    if (!left)                       // Si falla, retornamos NULL
        return NULL;
    // Mientras encontremos el operador '+'
    while (accept(s, '+')) {
        node *right = parse_term(s);    // Parseamos el siguiente término
        if (!right) {                    // Si falla
            destroy_tree(left);          // Liberamos el árbol izquierdo
            return NULL;
        }
        // Creamos un nodo ADD que une left y right
        node n = { .type = ADD, .l = left, .r = right };
        left = new_node(n);              // El nuevo nodo se convierte en el nuevo "left"
        if (!left)                       // Si falla la creación del nodo
            return NULL;
    }
    return left;  // Retornamos el árbol de sumas construido
}

// Función principal de parsing: parsea toda la expresión y verifica que no haya caracteres sobrantes
node *parse_expr(char *s)
{
    char *p = s;                    // Creamos un puntero local que apunta al inicio de la cadena
    node *ret = parse_expr_r(&p);   // Parseamos la expresión completa
    if (!ret)                        // Si falla el parsing
        return NULL;
    if (*p) {                        // Si después de parsear aún quedan caracteres
        unexpected(*p);              // Mostramos error por caracter inesperado
        destroy_tree(ret);           // Liberamos el árbol creado
        return NULL;
    }
    return ret;  // Retornamos el árbol sintáctico completo
}

// Evalúa recursivamente el árbol sintáctico y calcula el resultado
int eval_tree(node *tree)
{
    switch (tree->type) {
        case ADD:   return eval_tree(tree->l) + eval_tree(tree->r);  // Suma: evalúa izq + der
        case MULTI: return eval_tree(tree->l) * eval_tree(tree->r);  // Multiplicación: evalúa izq * der
        case VAL:   return tree->val;                                 // Valor: retorna el número
    }
    return 0;  // No debería ocurrir, pero por seguridad retornamos 0
}

// Función principal del programa
int main(int argc, char **argv)
{
    if (argc != 2)                       // Verificamos que haya exactamente 1 argumento (la expresión)
        return 1;                        // Si no, retornamos error
    node *tree = parse_expr(argv[1]);    // Parseamos la expresión (primer argumento)
    if (!tree)                            // Si el parsing falla
        return 1;                         // Retornamos error
    printf("%d\n", eval_tree(tree));    // Evaluamos el árbol e imprimimos el resultado
    destroy_tree(tree);                   // Liberamos toda la memoria del árbol
    return 0;                             // Terminamos con éxito
}