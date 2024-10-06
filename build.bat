@echo off

cls
g++ -Wall -g -W -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-missing-field-initializers ^
    src/*.cpp ^
    src/lexer/*.cpp ^
    src/parser/*.cpp ^
    src/typechecker/*.cpp ^
    src/compiler/*.cpp ^
    -o bin/output.exe || echo boo

