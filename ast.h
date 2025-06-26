#ifndef AST_H
#define AST_H

typedef struct NoArvore {
    char tipo[50];
    char valor[100];
    char tipoVariavel[20];
    struct NoArvore* filho;
    struct NoArvore* proximo;
} NoArvore;

NoArvore* criarNo(const char* tipo, const char* valor);
void adicionarFilho(NoArvore* pai, NoArvore* filho);
void imprimirArvore(NoArvore* raiz, int nivel);
void liberarArvore(NoArvore* raiz);

#endif
