
# Compilador MACSLang

## 📌 1. Descrição Geral

Este projeto é um **compilador simples para a linguagem MACSLang**, implementado em **C com parser manual**. Ele realiza:

- ✅ Análise léxica
- ✅ Análise sintática
- ✅ Análise semântica básica

### Funcionalidades suportadas:

- Declaração de variáveis e funções com tipo básico (`int`)
- Estruturas de controle: `if`, `else`, `while`, `for`
- Comandos de entrada/saída: `print`, `input`
- Operações aritméticas simples: soma (`+`) e subtração (`-`)
- Escopos aninhados para variáveis locais e globais
- Verificação de erros simples (uso de variáveis/funções não declaradas ou duplicadas)
- Geração de código para funções, `main`, e expressões

---

## ✨ 2. Alfabeto e Tokens

O analisador léxico reconhece os seguintes tokens:

| Token          | Representação                 | Exemplo   |
|----------------|-------------------------------|-----------|
| IDENTIFICADOR  | Nome de variáveis ou funções  | soma, x   |
| NUMERO         | Números inteiros              | 42, 0     |
| VAR            | Palavra reservada             | var       |
| FUNC           | Palavra reservada             | func      |
| MAIN           | Palavra reservada             | main      |
| IF             | Palavra reservada             | if        |
| ELSE           | Palavra reservada             | else      |
| WHILE          | Palavra reservada             | while     |
| FOR            | Palavra reservada             | for       |
| PRINT          | Palavra reservada             | print     |
| INPUT          | Palavra reservada             | input     |
| RETURN         | Palavra reservada             | return    |
| TIPO           | Tipo de dados básico          | int       |
| ABRE_PARENTESE | Símbolo                       | (         |
| FECHA_PARENTESE| Símbolo                       | )         |
| ABRE_CHAVES    | Símbolo                       | {         |
| FECHA_CHAVES   | Símbolo                       | }         |
| DOIS_PONTOS    | Símbolo                       | :         |
| PONTO_VIRGULA  | Símbolo                       | ;         |
| VIRGULA        | Símbolo                       | ,         |
| ATRIBUICAO     | Símbolo                       | =         |
| SOMA           | Operador                      | +         |
| SUBTRACAO      | Operador                      | -         |

---

## 📚 3. Gramática Formal Simplificada

```ebnf
programa        := (funcao | main_funcao)*

funcao          := 'func' IDENTIFICADOR '(' parametros? ')' ':' TIPO bloco
main_funcao     := 'main' '(' ')' bloco

parametros      := parametro (',' parametro)*
parametro       := IDENTIFICADOR ':' TIPO

bloco           := '{' (declaracao | comando)* '}'

declaracao      := 'var' IDENTIFICADOR ':' TIPO ('=' expressao)? ';'

comando         := atribuicao
                 | chamada_funcao
                 | comandoIf
                 | comandoWhile
                 | comandoFor
                 | comandoPrint
                 | comandoInput
                 | comandoReturn

atribuicao      := IDENTIFICADOR '=' expressao ';'
chamada_funcao  := IDENTIFICADOR '(' (expressao (',' expressao)*)? ')'

comandoIf       := 'if' '(' expressao ')' bloco ('else' bloco)?
comandoWhile    := 'while' '(' expressao ')' bloco
comandoFor      := 'for' '(' atribuicao expressao ';' atribuicao ')' bloco

comandoPrint    := 'print' '(' expressao ')' ';'
comandoInput    := 'input' '(' IDENTIFICADOR ')' ';'
comandoReturn   := 'return' expressao ';'

expressao       := termo (('+' | '-') termo)*
termo           := IDENTIFICADOR | NUMERO | chamada_funcao
```

---

## 🌳 4. Árvore Sintática Abstrata (AST)

A AST representa o código fonte em forma de árvore.

**Estrutura de cada nó da AST:**

- **tipo:** Tipo do nó (ex: FUNCAO, DECLARACAO, ATRIBUICAO, SOMA, etc.)
- **valor:** Conteúdo (ex: nome da variável, número, operador)
- **filho:** Ponteiro para o primeiro filho
- **proximo:** Ponteiro para o próximo irmão

### Exemplo de AST:

```c
func soma(a: int, b: int): int {
    return a + b;
}
```

```yaml
FUNCAO
├── IDENTIFICADOR: soma
├── PARAMETROS
│   ├── IDENTIFICADOR: a
│   └── IDENTIFICADOR: b
├── TIPO: int
└── BLOCO
    └── RETURN
        └── SOMA
            ├── IDENTIFICADOR: a
            └── IDENTIFICADOR: b
```

---

## 🗃️ 5. Estruturas de Dados no Parser

- **Variável:** Nome e tipo
- **Escopo:** Pilha de escopos (variáveis locais e globais)
- **Função:** Tabela de funções (nome e tipo de retorno)
- **Token Atual:** Controle do token da análise léxica
- **Árvore:** Estrutura de nós interligados (tipo `NoArvore`)

---

## 🔍 6. Funcionamento do Parser Manual

O parser usa **funções recursivas** para cada regra da gramática.

**Principais funções:**

- `programa()`: Lê funções e main
- `funcao()`: Declaração de função
- `parametros()`: Leitura de parâmetros
- `bloco()`: Bloco `{ }` de comandos e declarações
- `declaracao()`: Declaração de variáveis
- `comando()`: Direciona para o comando correto
- `atribuicao()`, `chamada_funcao()`
- `comandoIf()`, `comandoWhile()`, `comandoFor()`
- `comandoPrint()`, `comandoInput()`, `comandoReturn()`
- `expressao()`, `termo()`: Expressões aritméticas

---

## 🧱 7. Gerenciamento de Escopo e Variáveis

- Pilha de escopos para variáveis locais
- Vetor de variáveis declaradas por escopo
- Tabela global de funções

**Funções para:**

- Abrir/fechar escopos
- Declarar variáveis
- Verificar existência de variáveis
- Gerenciar funções

**Erros gerados:**

- Variáveis ou funções duplicadas
- Uso antes da declaração

---

## 🧪 8. Exemplos de Código para Teste

**Exemplo 1: Função soma**

```c
func soma(a: int, b: int): int {
    return a + b;
}

main() {
    var resultado: int;
    resultado = soma(4, 5);
    print(resultado);
}
```
Saída esperada: **9**

**Exemplo 2: Variáveis e Expressões**

```c
main() {
    var x: int = 10;
    var y: int = 5;
    var z: int;
    z = x - y;
    print(z);
}
```
Saída esperada: **5**

**Exemplo 3: Controle Condicional**

```c
func maior(a: int, b: int): int {
    if (a - b) {
        return a;
    } else {
        return b;
    }
}

main() {
    var m: int;
    m = maior(7, 3);
    print(m);
}
```
Saída esperada: **7**


## 📚 11. Referências e Recursos

- [Documentação Flex](https://westes.github.io/flex/manual/)
- [NASM Manual](https://www.nasm.us/doc/)
- [MinGW - GCC para Windows](https://www.mingw-w64.org/)
- **Livro referência:** "Compilers: Principles, Techniques, and Tools" (Aho, Lam, Sethi, Ullman)
