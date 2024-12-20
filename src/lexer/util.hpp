#include "lexer.hpp"

namespace Lexer {

    inline char* newString(char* c, int len) {
        char* str = new char[len+1];
        str[len] = 0;
        while (len--) str[len] = c[len];
        return str;
    }

    bool isAlpha(char c) {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }

    bool isNumber(char c) {
        return (c >= '0' && c <= '9');
    }

    bool isHexNumber(char c) {
        return isNumber(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
    }

    bool bracketChar(char c) {
        return c == '{' || c == '}' || 
                c == '(' || c == ')' || 
                c == '[' || c == ']';
    }

    bool isBreakChar(char c) {
        return isOperatorChar(c) ||
                bracketChar(c) ||
                c == ' ' ||
                c == '\t' ||
                c == '\n' ||
                c == ':' ||
                c == ';' ||
                c == ',' ||
                !c;
    }

    bool isSymbolChar(char c, int pos) {
        return isAlpha(c) || c == '_' || (pos && isNumber(c));
    }

    bool numberLiteralChar(char c) {
        return isHexNumber(c) || c == '_' || c == '.';
    }

    bool isOperatorChar(char c) {
        return c == '+' || c == '-' || c =='*' || c == '/' || c == '%' || 
                c == '>' || c == '<' || 
                c == '=' || 
                c == '^' || c == '&' || c == '|' || c == '~' || 
                c == '!' || 
                c == '.' || 
                c == '[' || c == ']';
    }

    bool isOperator(char* c) {
        char* str = new char[4]{};
        str[0] = *c;

        if (isOperatorChar(c[1])) str[1] = c[1];
        if (isOperatorChar(c[2])) str[2] = c[2];

        if (auto key = operations.find(str); key != operations.end()) {
            return true;
        }
        return false;
    }

    /**
     * Yes this leaks memory because the strings never get deleted,
     * but it shouldn't matter as the user is doing something wrong
     * if its called enough to matter.
     */
    char* toHexByte(char c) {
        const char* hexDigits = "0123456789ABCDEF";
        return new char[3]{hexDigits[c >> 4], hexDigits[c & 0xf], 0};
    }
}



