#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
#include "ast.h"
#include <ctype.h>

#define MAX_VARS 100
#define MAX_ESCOPE 50
#define MAX_FUNCS 100

typedef struct {
    char nome[100];
    char tipo[20];
} Variavel;

typedef struct {
    Variavel vars[MAX_VARS];
    int total;
} Escopo;

typedef struct {
    char nome[100];
    char tipoRetorno[20];
} Funcao;

Escopo pilhaEscopo[MAX_ESCOPE];
int topo = -1;

Funcao tabelaFuncoes[MAX_FUNCS];
int totalFuncoes = 0;

// ----------- Gerenciamento de Escopo e Variáveis ------------

void abrir_escopo() {
    if (topo + 1 >= MAX_ESCOPE) {
        printf("Erro: muitos escopos!\n");
        exit(1);
    }
    topo++;
    pilhaEscopo[topo].total = 0;
}

void fechar_escopo() {
    if (topo < 0) {
        printf("Erro: nenhum escopo para fechar!\n");
        exit(1);
    }
    pilhaEscopo[topo].total = 0;
    topo--;
}

int var_declarada_escopo_atual(const char* nome) {
    if (topo < 0) return 0;
    Escopo *esc = &pilhaEscopo[topo];
    for (int i = 0; i < esc->total; i++) {
        if (strcmp(esc->vars[i].nome, nome) == 0) return 1;
    }
    return 0;
}

int var_declarada(const char* nome) {
    for (int i = topo; i >= 0; i--) {
        Escopo *esc = &pilhaEscopo[i];
        for (int j = 0; j < esc->total; j++) {
            if (strcmp(esc->vars[j].nome, nome) == 0) return 1;
        }
    }
    return 0;
}

void declarar_var(const char* nome, const char* tipo) {
    if (topo < 0) {
        printf("Erro: sem escopo aberto pra declarar variável\n");
        exit(1);
    }
    Escopo *esc = &pilhaEscopo[topo];
    if (var_declarada_escopo_atual(nome)) {
        printf("Erro: variável '%s' já declarada nesse escopo\n", nome);
        exit(1);
    }
    if (esc->total >= MAX_VARS) {
        printf("Erro: escopo cheio\n");
        exit(1);
    }
    strcpy(esc->vars[esc->total].nome, nome);
    strcpy(esc->vars[esc->total].tipo, tipo);
    esc->total++;
}

const char* pegar_tipo_var(const char* nome) {
    for (int i = topo; i >= 0; i--) {
        Escopo *esc = &pilhaEscopo[i];
        for (int j = 0; j < esc->total; j++) {
            if (strcmp(esc->vars[j].nome, nome) == 0)
                return esc->vars[j].tipo;
        }
    }
    return NULL;
}

// ----------- Gerenciamento de Funções ------------

void declarar_funcao(const char* nome, const char* tipoRetorno) {
    for (int i = 0; i < totalFuncoes; i++) {
        if (strcmp(tabelaFuncoes[i].nome, nome) == 0) {
            printf("Erro: função '%s' já declarada\n", nome);
            exit(1);
        }
    }
    if (totalFuncoes >= MAX_FUNCS) {
        printf("Erro: tabela de funções cheia\n");
        exit(1);
    }
    strcpy(tabelaFuncoes[totalFuncoes].nome, nome);
    strcpy(tabelaFuncoes[totalFuncoes].tipoRetorno, tipoRetorno);
    totalFuncoes++;
}

int funcao_declarada(const char* nome) {
    for (int i = 0; i < totalFuncoes; i++) {
        if (strcmp(tabelaFuncoes[i].nome, nome) == 0)
            return 1;
    }
    return 0;
}

// ----------- Protótipos parser ------------

extern int yylex();
extern char* yytext;
int token_atual;

void proximo_token() {
    token_atual = yylex();
}

void erro(const char* msg) {
    printf("Erro de sintaxe: %s (token: %s)\n", msg, yytext);
    exit(1);
}

NoArvore* programa();
NoArvore* bloco();
NoArvore* declaracao();
NoArvore* comando();
NoArvore* atribuicao();
NoArvore* atribuicao_com_nome(const char* nome);
NoArvore* comandoIf();
NoArvore* comandoWhile();
NoArvore* comandoFor();
NoArvore* comandoPrint();
NoArvore* comandoInput();
NoArvore* comandoReturn();
NoArvore* funcao();
NoArvore* parametros();
NoArvore* expressao();
NoArvore* termo();
NoArvore* chamada_funcao(const char* nomeFunc);

// ----------- Parser ------------

NoArvore* programa() {
    abrir_escopo(); // escopo global
    NoArvore* raiz = criarNo("PROGRAMA", "");
    while (token_atual == FUNC || token_atual == MAIN) {
        if (token_atual == FUNC) {
            adicionarFilho(raiz, funcao());
        } else if (token_atual == MAIN) {
            NoArvore* mainNode = criarNo("MAIN", "");
            proximo_token();
            if (token_atual == ABRE_PARENTESE) {
                proximo_token();
                if (token_atual == FECHA_PARENTESE) {
                    proximo_token();
                    adicionarFilho(mainNode, bloco());
                } else erro("faltou fechar o parêntese do MAIN");
            } else erro("esperado '(' depois do MAIN");
            adicionarFilho(raiz, mainNode);
        }
    }

    fechar_escopo();
    fprintf(stderr, "Programa sintaticamente correto!\n");
    return raiz;
}

NoArvore* funcao() {
    proximo_token(); 
    if (token_atual != IDENTIFICADOR)
        erro("esperado nome da função após 'func'");
    char nomeFunc[100];
    strcpy(nomeFunc, yytext);
    NoArvore* no = criarNo("FUNCAO", "");
    adicionarFilho(no, criarNo("IDENTIFICADOR", nomeFunc));
    proximo_token();
    if (token_atual != ABRE_PARENTESE)
        erro("esperado '(' depois do nome da função");
    proximo_token();
    abrir_escopo();

    NoArvore* parametrosNode = NULL;
    if (token_atual != FECHA_PARENTESE) {
        parametrosNode = parametros();
    }

    if (token_atual != FECHA_PARENTESE)
        erro("faltou fechar os parênteses dos parâmetros");
    proximo_token();

    if (token_atual != DOIS_PONTOS)
        erro("esperado ':' depois dos parâmetros");
    proximo_token();

    if (token_atual != TIPO)
        erro("esperado tipo após ':' na declaração da função");

    char tipoRetorno[20];
    strcpy(tipoRetorno, yytext);

    declarar_funcao(nomeFunc, tipoRetorno);

    adicionarFilho(no, parametrosNode);
    adicionarFilho(no, criarNo("TIPO", tipoRetorno));

    proximo_token();

    adicionarFilho(no, bloco());

    fechar_escopo();

    return no;
}

NoArvore* parametros() {
    NoArvore* no = criarNo("PARAMETROS", "");

    if (token_atual == IDENTIFICADOR) {
        char nome[100];
        strcpy(nome, yytext);
        proximo_token();

        if (token_atual != DOIS_PONTOS)
            erro("esperado ':' após o nome do parâmetro");
        proximo_token();

        if (token_atual != TIPO)
            erro("esperado tipo do parâmetro");

        char tipo[20];
        strcpy(tipo, yytext);

        declarar_var(nome, tipo);
        adicionarFilho(no, criarNo("IDENTIFICADOR", nome));
        adicionarFilho(no, criarNo("TIPO", tipo));
        proximo_token();

        while (token_atual == VIRGULA) {
            proximo_token();

            if (token_atual != IDENTIFICADOR)
                erro("esperado identificador depois da vírgula");

            strcpy(nome, yytext);
            proximo_token();

            if (token_atual != DOIS_PONTOS)
                erro("esperado ':' depois do identificador");
            proximo_token();

            if (token_atual != TIPO)
                erro("esperado tipo no parâmetro");

            strcpy(tipo, yytext);
            declarar_var(nome, tipo);
            adicionarFilho(no, criarNo("IDENTIFICADOR", nome));
            adicionarFilho(no, criarNo("TIPO", tipo));
            proximo_token();
        }
    }

    return no;
}

NoArvore* bloco() {
    abrir_escopo();
    NoArvore* no = criarNo("BLOCO", "");
    if (token_atual != ABRE_CHAVES)
        erro("esperado '{' para abrir o bloco");
    proximo_token();
    while (token_atual == VAR || token_atual == IDENTIFICADOR || token_atual == IF || token_atual == WHILE || token_atual == FOR || token_atual == PRINT || token_atual == INPUT || token_atual == RETURN) {
        if (token_atual == VAR) {
            adicionarFilho(no, declaracao());
        } else {
            adicionarFilho(no, comando());
        }
    }
    if (token_atual != FECHA_CHAVES)
        erro("faltou fechar o bloco com '}'");
    proximo_token();

    fechar_escopo();
    return no;
}

NoArvore* declaracao() {
    NoArvore* no = criarNo("DECLARACAO", "");
    proximo_token();
    if (token_atual != IDENTIFICADOR)
        erro("esperado nome da variável depois de 'var'");
    char nome[100];
    strcpy(nome, yytext);
    proximo_token();
    if (token_atual != DOIS_PONTOS)
        erro("esperado ':' após o nome da variável");
    proximo_token();
    if (token_atual != TIPO)
        erro("esperado tipo após ':'");
    char tipo[20];
    strcpy(tipo, yytext);
    if (var_declarada_escopo_atual(nome)) {
        printf("Erro: variável '%s' já declarada nesse escopo\n", nome);
        exit(1);
    }
    declarar_var(nome, tipo);

    NoArvore* idNo = criarNo("IDENTIFICADOR", nome);
    strcpy(idNo->tipoVariavel, tipo);  // Seta o tipo da variável
    adicionarFilho(no, idNo);
    adicionarFilho(no, criarNo("TIPO", tipo));

    proximo_token();

    if (token_atual == ATRIBUICAO) {
        proximo_token();
        NoArvore* expr = expressao();
        adicionarFilho(no, expr);
    }

    if (token_atual != PONTO_VIRGULA)
        erro("esperado ';' no fim da declaração");
    proximo_token();

    return no;
}


NoArvore* comando() {
    if (token_atual == IDENTIFICADOR) {
        char nomeId[100];
        strcpy(nomeId, yytext);

        proximo_token();

        if (token_atual == ABRE_PARENTESE) {
            proximo_token(); // consome '('
            return chamada_funcao(nomeId);
        } else if (token_atual == ATRIBUICAO) {
            return atribuicao_com_nome(nomeId);
        } else {
            erro("Esperado '(' para chamada de função ou '=' para atribuição após identificador");
        }
    } else if (token_atual == IF) return comandoIf();
    else if (token_atual == WHILE) return comandoWhile();
    else if (token_atual == FOR) return comandoFor();
    else if (token_atual == PRINT) return comandoPrint();
    else if (token_atual == INPUT) return comandoInput();
    else if (token_atual == RETURN) return comandoReturn();
    else erro("comando inválido");
    return NULL;
}

// Implementação da função atribuicao que faltava
NoArvore* atribuicao() {
    if (token_atual != IDENTIFICADOR)
        erro("Esperado identificador no inicio da atribuicao");
    char nome[100];
    strcpy(nome, yytext);
    proximo_token();
    if (token_atual != ATRIBUICAO)
        erro("Esperado '=' na atribuicao");
    proximo_token();
    NoArvore* no = criarNo("ATRIBUICAO", "");
    adicionarFilho(no, criarNo("IDENTIFICADOR", nome));
    adicionarFilho(no, expressao());
    if (token_atual != PONTO_VIRGULA)
        erro("Esperado ';' no fim da atribuicao");
    proximo_token();
    return no;
}

NoArvore* atribuicao_com_nome(const char* nome) {
    NoArvore* no = criarNo("ATRIBUICAO", "");

    if (!var_declarada(nome)) {
        printf("Erro: variável '%s' não declarada\n", nome);
        exit(1);
    }
    const char* tipo_var = pegar_tipo_var(nome);
    NoArvore* idNo = criarNo("IDENTIFICADOR", nome);
    strcpy(idNo->tipoVariavel, tipo_var ? tipo_var : "");
    adicionarFilho(no, idNo);

    if (token_atual != ATRIBUICAO)
        erro("esperado '=' na atribuicao");
    proximo_token();
    NoArvore* expr = expressao();
    adicionarFilho(no, expr);
    if (token_atual != PONTO_VIRGULA)
        erro("esperado ';' depois da atribuicao");
    proximo_token();
    return no;
}

NoArvore* chamada_funcao(const char* nomeFunc) {
    NoArvore* no = criarNo("CHAMADA_FUNCAO", "");
    adicionarFilho(no, criarNo("IDENTIFICADOR", nomeFunc));
    NoArvore* args = criarNo("ARGUMENTOS", "");
    if (token_atual != FECHA_PARENTESE) {
        while (1) {
            adicionarFilho(args, expressao());
            if (token_atual == VIRGULA) {
                proximo_token();
            } else break;
        }
    }

    if (token_atual != FECHA_PARENTESE)
        erro("faltou fechar os parênteses na chamada de função");
    proximo_token();
    adicionarFilho(no, args);
    if (!funcao_declarada(nomeFunc)) {
        printf("Erro: função '%s' não declarada\n", nomeFunc);
        exit(1);
    }
    return no;
}


NoArvore* comandoIf() {
    NoArvore* no = criarNo("IF", "");
    proximo_token();
    if (token_atual != ABRE_PARENTESE)
        erro("esperado '(' depois do IF");
    proximo_token();

    adicionarFilho(no, expressao());
    if (token_atual != FECHA_PARENTESE)
        erro("faltou fechar parêntese do IF");
    proximo_token();
    adicionarFilho(no, bloco());

    if (token_atual == ELSE) {
        proximo_token();
        adicionarFilho(no, bloco());
    }
    return no;
}

NoArvore* comandoWhile() {
    NoArvore* no = criarNo("WHILE", "");
    proximo_token();
    if (token_atual != ABRE_PARENTESE)
        erro("esperado '(' depois do WHILE");
    proximo_token();
    adicionarFilho(no, expressao());
    if (token_atual != FECHA_PARENTESE)
        erro("esperado ')' depois da condição do WHILE");
    proximo_token();
    adicionarFilho(no, bloco());
    return no;
}

NoArvore* comandoFor() {
    NoArvore* no = criarNo("FOR", "");
    proximo_token();
    if (token_atual != ABRE_PARENTESE)
        erro("esperado '(' depois do FOR");
    proximo_token();
    adicionarFilho(no, atribuicao());
    adicionarFilho(no, expressao());
    if (token_atual != PONTO_VIRGULA)
        erro("esperado ';' no meio do FOR");
    proximo_token();
    adicionarFilho(no, atribuicao());
    if (token_atual != FECHA_PARENTESE)
        erro("faltou ')' no FOR");
    proximo_token();
    adicionarFilho(no, bloco());
    return no;
}

NoArvore* comandoPrint() {
    NoArvore* no = criarNo("PRINT", "");
    proximo_token();
    if (token_atual != ABRE_PARENTESE)
        erro("esperado '(' depois do PRINT");
    proximo_token();
    adicionarFilho(no, expressao());
    if (token_atual != FECHA_PARENTESE)
        erro("esperado ')' no PRINT");
    proximo_token();
    if (token_atual != PONTO_VIRGULA)
        erro("esperado ';' depois do PRINT");
    proximo_token();
    return no;
}

NoArvore* comandoInput() {
    NoArvore* no = criarNo("INPUT", "");
    proximo_token();
    if (token_atual != ABRE_PARENTESE)
        erro("esperado '(' depois do INPUT");
    proximo_token();
    if (token_atual != IDENTIFICADOR)
        erro("esperado identificador no INPUT");
    if (!var_declarada(yytext)) {
        printf("Erro: variável '%s' não declarada no INPUT\n", yytext);
        exit(1);
    }
    adicionarFilho(no, criarNo("IDENTIFICADOR", yytext));
    proximo_token();
    if (token_atual != FECHA_PARENTESE)
        erro("esperado ')' no INPUT");
    proximo_token();
    if (token_atual != PONTO_VIRGULA)
        erro("esperado ';' depois do INPUT");
    proximo_token();
    return no;
}

NoArvore* comandoReturn() {
    NoArvore* no = criarNo("RETURN", "");
    proximo_token();
    adicionarFilho(no, expressao());
    if (token_atual != PONTO_VIRGULA)
        erro("esperado ';' depois do RETURN");
    proximo_token();
    return no;
}

NoArvore* expressao() {
    NoArvore* no = termo();
    while (token_atual == SOMA || token_atual == SUBTRACAO) {
        int operador = token_atual;
        proximo_token();
        NoArvore* noOp = criarNo((operador == SOMA) ? "SOMA" : "SUBTRACAO", "");
        adicionarFilho(noOp, no);
        NoArvore* termoDir = termo();
        adicionarFilho(noOp, termoDir);
        no = noOp;
    }
    return no;
}

NoArvore* termo() {
    NoArvore* no = NULL;

    if (token_atual == IDENTIFICADOR) {
        char nome[100];
        strcpy(nome, yytext);
        proximo_token();

        if (token_atual == ABRE_PARENTESE) {
            proximo_token();
            no = chamada_funcao(nome);
        } else {
            no = criarNo("IDENTIFICADOR", nome);
            const char* tipoVar = pegar_tipo_var(nome);
            if (tipoVar) strcpy(no->tipoVariavel, tipoVar);
        }
    } else if (token_atual == NUMERO) {
        no = criarNo("NUMERO", yytext);
        strcpy(no->tipoVariavel, "int");  
        proximo_token();
    } else if (token_atual == STRING_LITERAL) {
        no = criarNo("STRING_LITERAL", yytext);
        strcpy(no->tipoVariavel, "string");
        proximo_token();
    } else {
        erro("expressao invalida");
    }

    return no;
}

int main() {
    proximo_token();  // inicia a leitura dos tokens
    NoArvore* raiz = programa();  // analisa o programa e constrói a AST

    printf("\n--- ÁRVORE SINTÁTICA ABSTRATA (AST) ---\n");
    imprimirArvore(raiz, 0);  // imprime a árvore com indentação

    liberarArvore(raiz);  // libera memória
    return 0;
}
