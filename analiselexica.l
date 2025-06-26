%{
#include <stdio.h>
#include "tokens.h"

char* yytext;
%}

%option noyywrap
%x COMMENT

DIGIT       [0-9]+
LETTER      [a-zA-Z]
ID          {LETTER}({LETTER}|{DIGIT})*
WHITESPACE  [ \t\r\n]
STRING      \"([^\"\n])*\" 
CHAR        \'([^\'\n])\'

%%

"if"            { return IF; }
"else"          { return ELSE; }
"for"|"while"   { return FOR; }
"switch"        { return SWITCH; }
"case"          { return CASE; }
"default"       { return DEFAULT; }
"break"         { return BREAK; }
"return"        { return RETURN; }
"var"           { return VAR; }
"func"          { return FUNC; }
"main"          { return MAIN; }
"print"         { return PRINT; }
"input"         { return INPUT; }
"int"|"float"|"char"|"bool"|"string" { return TIPO; }

"=="            { return IGUAL; }
"!="            { return DIFERENTE; }
">="            { return MAIOR_IGUAL; }
"<="            { return MENOR_IGUAL; }
"+"             { return SOMA; }
"-"             { return SUBTRACAO; }
"*"             { return MULTIPLICACAO; }
"/"             { return DIVISAO; }
"="             { return ATRIBUICAO; }
">"             { return MAIOR; }
"<"             { return MENOR; }
"&&"            { return E_LOGICO; }
"||"            { return OU_LOGICO; }
"!"             { return NOT; }
"("             { return ABRE_PARENTESE; }
")"             { return FECHA_PARENTESE; }
"{"             { return ABRE_CHAVES; }
"}"             { return FECHA_CHAVES; }
";"             { return PONTO_VIRGULA; }
","             { return VIRGULA; }
":"             { return DOIS_PONTOS; }

{DIGIT}         { return NUMERO; }
{ID}            { return IDENTIFICADOR; }
{STRING}        { return STRING_LITERAL; }
{CHAR}          { return CHAR_LITERAL; }

"/*"            { BEGIN(COMMENT); }
<COMMENT>"*/"   { BEGIN(INITIAL); }
<COMMENT>[ \t\r\n]+ { /* Ignorar espaços e quebras */ }
<COMMENT>.      { /* Ignorar tudo dentro de comentário */ }

{WHITESPACE}    { /* Ignorar espaços */ }
.               { return ERRO; }
%%
