#include <fstream>
#include <stdio.h>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "typechecker/typechecker.hpp"
#include "compiler/compiler.hpp"

void pad(Lexer::Token token, int* line, int* col) {
    while (*line < token.file.line) {
        printf("\n");
        (*line)++;
        *col = 1;
    }
    while (*col < token.file.col) {
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

void printNode(Parser::Node* node, int depth) {
    int d = depth;
    while (d--) printf(" | ");
    printf("%s",Parser::NodeTypeMap[(int)node->type]);
    switch (node->type) {
        case Parser::NodeType::SYMBOL:
            printf(": %s: %s\n",node->symbol->name,Parser::TypeMap[node->symbol->t]);
            break;
        case Parser::NodeType::FUNCTION:
            printf(": %s\n",node->symbol->name);
            break;
        case Parser::NodeType::LITERAL:
            printf(": %s\n",node->literal.value);
            break;
        case Parser::NodeType::OPERATION:
            printf(": %s %s\n",node->op.value, Parser::OpTypeMap[node->op.type]);
            break;
        case Parser::NodeType::INVOCATION:
            printf(": %s %lld\n",node->symbol->name,(long long)node->symbol->func);
            break;
        default:
            printf("\n");
            break;
    }
    Parser::Node* child = node->firstChild;
    while (child) {
        printNode(child,depth+1);
        child = child->nextSibling;
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

    std::vector<Lexer::Token>* tokens = Lexer::parse(inputFile, buf);
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
        "EOF",
        "ARRAY_START",
        "ARRAY_END"
    };

    printf("Input file: %s\n%s\n",inputFile,buf);

    printf("Tokens length: %d\n",(int)tokens->size());

    int line = 0;
    int col = 1;
    for (int i = 0; i < (int)tokens->size(); i++) {
        Lexer::Token token = tokens->at(i);
        printf("token: %s %s\n",TokenTypeMap[token.type], 
                token.type == Lexer::TokenType::SYMBOL ? token.value : 
                token.type == Lexer::TokenType::KEYWORD ? Lexer::KeywordTypeMap[token.keyword] : 
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
                token.value = (char*)Lexer::KeywordTypeMap[token.keyword];
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
            case Lexer::TokenType::ARRAY_START:
                pad(token,&line,&col);
                printf("[");
                col++;
                break;
            case Lexer::TokenType::ARRAY_END:
                pad(token,&line,&col);
                printf("]");
                col++;
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

    printf("syntax tree:\n");
    for (auto& pair : *tree) {
        printNode(pair.second,0);
        printf("\n");
    }

    printf("Parser completed successfully!\n");

    if (!TypeChecker::process(tree)) {
        printf("Type check failed!\n");
        return 1;
    } else {
        printf("Type check passed!\n");
    }

    if (auto key = tree->find("main");key != tree->end()) {
        switch (key->second->symbol->func->returnType) {
                case Parser::Type::i8:
                case Parser::Type::i16:
                case Parser::Type::i32:
                case Parser::Type::i64:
                case Parser::Type::u8:
                case Parser::Type::u16:
                case Parser::Type::u32:
                case Parser::Type::u64:
                case Parser::Type::null:
                case Parser::Type::boolean:
                    break;
                default:
                    printf("ERROR: Main function return type invalid!\n");
                    return 1;
            }
    } else {
        printf("ERROR: No main function!\n");
        return 1;
    }

    int len = 0;
    for (; inputFile[len++];);
    for (;inputFile[--len] != '.';);
    len++;

    char* outFileName = new char[len+4];
    outFileName[len] = 'a';
    outFileName[len+1] = 's';
    outFileName[len+2] = 'm';
    outFileName[len+3] = 0;
    
    for (;len--;) outFileName[len] = inputFile[len];

    printf("Out file: %s\n",outFileName);

    std::ofstream outFile(outFileName, std::ios::binary | std::ios::trunc);
    if (!outFile.is_open()) {
        printf("Failed to open %s\n",outFileName);
        exit(1);
    }
    
    if (!Compiler::compile(tree, &outFile)) {
        printf("Compile failed!\n");
        return 1;
    } else {
        printf("Compile succeeded!\n");
    }

    outFile.close();

    return 0;
}

