@echo off

cls
g++ -Wall -g -W -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-missing-field-initializers ^
    src/*.cpp ^
    src/lexer/*.cpp ^
    src/parser/*.cpp ^
    -o bin/output.exe || exit 1

