#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

NoArvore* criarNo(const char* tipo, const char* valor) {
    NoArvore* no = (NoArvore*) malloc(sizeof(NoArvore));
    strcpy(no->tipo, tipo);
    strcpy(no->valor, valor);
    no->filho = NULL;
    no->proximo = NULL;
    no->tipoVariavel[0] = '\0';  
    return no;
}


void adicionarFilho(NoArvore* pai, NoArvore* filho) {
    if (pai->filho == NULL) {
        pai->filho = filho;
    } else {
        NoArvore* temp = pai->filho;
        while (temp->proximo != NULL) {
            temp = temp->proximo;
        }
        temp->proximo = filho;
    }
}

void imprimirArvore(NoArvore* no, int nivel) {
    if (no == NULL) return;
    for (int i = 0; i < nivel; i++) printf("  ");
    printf("%s", no->tipo);
    if (strlen(no->valor) > 0)
        printf(": %s", no->valor);
    if (strlen(no->tipoVariavel) > 0)
        printf(" [tipo: %s]", no->tipoVariavel);
    printf("\n");

    NoArvore* filho = no->filho;
    while (filho) {
        imprimirArvore(filho, nivel + 1);
        filho = filho->proximo;
    }
}

void liberarArvore(NoArvore* raiz) {
    if (raiz == NULL) return;
    liberarArvore(raiz->filho);
    liberarArvore(raiz->proximo);
    free(raiz);
}
