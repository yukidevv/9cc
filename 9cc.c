#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>

//Kind of Token
typedef enum{
    TK_RESERVED,//記号
    TK_NUM,//数字
    TK_EOF//入力の終わり
} TokenKind;

typedef struct Token Token;

//Token
struct Token{
    TokenKind kind;//Token型
    Token *next; //次の入力トークン
    int val; //数値
    char *str;//トークン文字列
};

char *user_input;

Token *token;

void error_at(char *loc,char *fmt,...){
    va_list ap;
    va_start(ap,fmt);

    int pos = loc-user_input;
    fprintf(stderr,"%s\n",user_input);
    fprintf(stderr,"%*s",pos," ");
    fprintf(stderr,"^ ");
    vfprintf(stderr,fmt,ap);
    exit(1);
}

Token *new_token(TokenKind kind,Token *cur,char *str){
    Token *tok = calloc(1,sizeof(Token));
    tok->kind=kind;
    tok->str=str;
    cur->next=tok;
    return tok;
}

Token *tokenize(){
    char *p = user_input;
    Token head;
    head.next=NULL;
    Token *cur =&head;
    //sleep(30);
    while(*p){
        if(isspace(*p)){//スペースなら次
            p++;
            continue;
        }
        if(*p=='+'||*p=='-'){//'+'or'-'ならトークン作る
            cur=new_token(TK_RESERVED,cur,p++);
            continue;
        }
        if(isdigit(*p)){
            cur=new_token(TK_NUM,cur,p);
            cur->val=strtol(p,&p,10);
            continue;
        }
        error_at(p,"cant tokenize");
    }
    new_token(TK_EOF,cur,p);
    return head.next;
}

int expect_number(){
    if(token->kind!=TK_NUM){
        error_at(token->str,"not digit");
    }
    int val=token->val;
    //printf("%d\n",val);
    token=token->next;
    return val;
}

bool at_eof(){
    return token->kind==TK_EOF;
}

bool consume(char op){
    if(token->kind!=TK_RESERVED||token->str[0]!=op){//数値または'+'以外の時falseを返却
        return false;
    }
    token=token->next;
    return true;
}

void expect(char op){
    if(token->kind!=TK_RESERVED|| token->str[0]!=op){
        error_at(token->str,"Not %c",op);
    }
    token=token->next;
}

int main(int argc,char **argv){
    if(argc !=2){
        fprintf(stderr,"引数の個数がだめ");
        return 1;
    }

    user_input = argv[1];
    token=tokenize();

    //base(for intel arch)
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("  mov rax, %d\n",expect_number());
    while(!at_eof()){//生成したトークンの数だけ繰り返す "5+20 - 4"->'5','+',"20",'-','4'
        if(consume('+')){
            printf("  add rax, %d\n",expect_number());
            continue;
        }
        expect('-');
        printf("  sub rax, %d\n",expect_number());
    }
    printf("  ret\n ");
    return 0;
}
