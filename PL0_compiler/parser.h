#ifndef PARSER_H
#define PARSER_H

#include "args.h"
#include "lexer.h"
#include "error.h"

class Parser
{
public:
    Parser();

    Error error;
    Lexer lexer;

    void block(int lev, int tx, set<symbol> fsys);
    void constdeclaration(int lev, int &tx, int &dx);
    void vardeclaration(int lev, int &tx, int &dx);
    void statement(set<symbol> fsys, int lev, int &tx);
    void expression(set<symbol> fsys, int lev, int &tx);
    void term(set<symbol> fsys, int lev, int &tx);
    void factor(set<symbol> fsys, int lev, int &tx);
    void condition(set<symbol> fsys, int lev, int &tx);
    void gen(opcode x, int y, int z);
    void append_pcode(int ii);
    void enter(symbol_table_type k, int &dx, int lev, int &tx);
    int position(string id, int tx);
};

#endif // PARSER_H
