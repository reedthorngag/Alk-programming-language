#include <fstream>
#include <stdio.h>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

void pad(Lexer::Token token, int* line, int* col) {
    while (*line < token.line) {
        printf("\n");
        (*line)++;
        *col = 1;
    }
    while (*col < token.column) {
        printf(" ");
        (*col)++;
    }
}

void printVal(Lexer::Token token, int* col) {
    char* ptr = token.value;
    while (*ptr) {
        printf("%c",*(ptr++));
        (*col)++;
    }
}

int main(int argc, char** argv) {

    if (argc < 2) {
        printf("No input file provided.\n");
        return 1;
    }

    char* inputFile = argv[1];

    std::ifstream file(inputFile, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        printf("Failed to open %s\n",inputFile);
        exit(1);
    }

    int size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buf = new char[size+1];
    buf[size] = 0;
    if (!file.read(buf,size)) {
        printf("Failed to read from %s\n",inputFile);
        exit(1);
    }

    file.close();

    std::vector<Lexer::Token>* tokens = Lexer::lexerParse(inputFile, buf);
    if (!tokens) return 1;

    const char* TokenTypeMap[]{
        "ENDLINE",
        "COMMA",
        "SCOPE_START",
        "SCOPE_END",
        "GROUPING_START",
        "GROUPING_END",
        "KEYWORD",
        "SYMBOL",
        "TYPE",
        "OPERATOR",
        "LITERAL",
        "EOF"
    };

    const char* SymbolTypeMap[]{
        "FUNC",
        "VAR",
        "CONST",
        "IF",
        "ELSE",
        "WHILE",
        "GLOBAL"
    };

    printf("Input file: %s\n%s\n",inputFile,buf);

    printf("Tokens length: %d\n",(int)tokens->size());

    int line = 0;
    int col = 1;
    for (int i = 0; i < (int)tokens->size(); i++) {
        Lexer::Token token = tokens->at(i);
        printf("token: %s %s\n",TokenTypeMap[token.type], 
                token.type == Lexer::TokenType::SYMBOL ? token.value : 
                token.type == Lexer::TokenType::KEYWORD ? SymbolTypeMap[token.keyword] : 
                token.type == Lexer::TokenType::LITERAL ? token.value : 
                token.type == Lexer::TokenType::TYPE ? token.value : 
                token.type == Lexer::TokenType::OPERATOR ? token.value : ""
            );
    }

    for (int i = 0; i < (int)tokens->size(); i++) {
        Lexer::Token token = tokens->at(i);

        switch (token.type) {
            case Lexer::TokenType::KEYWORD:
                pad(token,&line,&col);
                token.value = (char*)SymbolTypeMap[token.keyword];
                printVal(token,&col);
                if (tokens->at(i+1).type != Lexer::TokenType::ENDLINE) {
                    printf(" ");
                    col++;
                }
                break;
            case Lexer::TokenType::SYMBOL:
                pad(token,&line,&col);
                printVal(token,&col);
                if (tokens->at(i+1).type != Lexer::TokenType::ENDLINE && 
                    tokens->at(i+1).type != Lexer::TokenType::GROUPING_START &&
                    tokens->at(i+1).type != Lexer::TokenType::GROUPING_END &&
                    tokens->at(i+1).type != Lexer::TokenType::TYPE) {
                    printf(" ");
                    col++;
                }
                break;
            case Lexer::TokenType::LITERAL:
                pad(token,&line,&col);
                printVal(token,&col);
                if (tokens->at(i+1).type != Lexer::TokenType::ENDLINE &&
                    tokens->at(i+1).type != Lexer::TokenType::GROUPING_END) {
                    printf(" ");
                    col++;
                }
                break;
            case Lexer::TokenType::TYPE:
                printf(":");
                col++;
                pad(token,&line,&col);
                printVal(token,&col);
                if (tokens->at(i+1).type != Lexer::TokenType::ENDLINE &&
                    tokens->at(i+1).type != Lexer::TokenType::GROUPING_END) {
                    printf(" ");
                    col++;
                }
                break;
            case Lexer::TokenType::OPERATOR:
                pad(token,&line,&col);
                printVal(token,&col);
                printf(" ");
                col++;
                break;
            case Lexer::TokenType::GROUPING_START:
                pad(token,&line,&col);
                printf("(");
                col++;
                break;
            case Lexer::TokenType::GROUPING_END:
                pad(token,&line,&col);
                printf(")");
                col++;
                break;
            case Lexer::TokenType::SCOPE_START:
                pad(token,&line,&col);
                printf("{");
                col++;
                break;
            case Lexer::TokenType::SCOPE_END:
                pad(token,&line,&col);
                printf("}\n");
                col = 1;
                line++;
                break;
            case Lexer::TokenType::ENDLINE:
                pad(token,&line,&col);
                printf(";");
                col++;
                break;
            case Lexer::TokenType::COMMA:
                pad(token,&line,&col);
                printf(",");
                col++;
                break;
            case Lexer::TokenType::FILE_END:
                break;
        }

    }
    fflush(stdout);

    std::unordered_map<std::string, Parser::Node*>* tree = Parser::parseTokens(tokens);

    if (!tree) return 1;


    return 0;
}

