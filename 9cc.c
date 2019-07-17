#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

// types of token
typedef enum{
    TK_RESERVED,    // operator
    TK_NUM,         // integer token
    TK_EOF,         // token which represents the end of input
} TokenKind;

typedef struct Token Token;

// toke type
struct Token{
    TokenKind kind; // types of token
    Token *next;    // next input token
    int val;        // value of TK_NUM if kind is TK_NUM
    char *str;      // token string 
};


// current token
Token *token;

// error reporting function
// takes same inputs as printf
void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// input program
char *user_input;

// reports location of error
void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");    // outputs pos pieces of spaces
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// push forward token and returns True if next token is equal to an expected operator,
// returns False else
bool consume(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token = token->next;
    return true;
}

// push forward token if next token is equal to an expected operator,
// reports error else
void expect(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op)
        error_at(token->str, "it is not '%c'", op);
    token = token->next;
}

// push forward token and returns its value if next token is a number,
// reports error else
int expect_number(){
    if (token->kind != TK_NUM)
        error_at(token->str, "this is not a number");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

// create new token and connects to cur
Token *new_token(TokenKind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    cur->next = tok;
    return tok;
}

// tokenize the input string p and return the token
Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p){
        // skip null srting 
        if (isspace(*p)) {
            p++;
            continue; 
        }
    
        if (*p == '+' || *p == '-'){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }
    
        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "can not tokenize");
    
    }

    new_token(TK_EOF, cur, p);
    return head.next;

}




int main(int argc, char **argv){
    if (argc != 2){
        error("wrong number of args");
        return 1;
    }

    user_input = argv[1];

    // tokenize
    token = tokenize(argv[1]);

    // print head part of assembly
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // check whether the beginning of the input is a number and
    // print mov instruction
    printf("    mov rax, %d\n", expect_number());

    // consume `+ $number` or `- $number` and
    // print assembly
    while(!at_eof()){
        if(consume('+')){
            printf("    add rax, %d\n", expect_number());
            continue; 
        } 
    
        expect('-');
        printf("    sub rax, %d\n", expect_number());
    }

    printf("    ret\n");
    return 0;


}
