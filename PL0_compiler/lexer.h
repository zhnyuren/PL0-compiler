#ifndef LEXER_H
#define LEXER_H

#include "error.h"
#include "args.h"

class Lexer
{
public:
    Error error;

    Lexer();
    void nextch();
    void nextsym();
    void skip(set<symbol> s1, set<symbol> s2, int errn);
};

#endif // LEXER_H
