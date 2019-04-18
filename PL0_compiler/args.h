#pragma once

#ifndef ARGS_H
#define ARGS_H

#include <cstring>
#include <fstream>
#include <setjmp.h>
#include <set>
#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
using namespace std;

// system constant
const int norw = 16; // number of reserved words
const int txmax = 100; // length of identifier table
const int nmax = INT32_MAX; // maxmium number
const int al = 10; // length of identifiers
const int amax = 2047; // maximum address
const int levmax = 3; // maxmiun depth of block nesting
const int cxmax = 200; // size of code array

enum symbol // reserved words symbol
{
    nul, ident, number, pluss, minuss, times, slash,
    oddsym, eql, neq, lss, leq, gtr, geq, lparen,
    rparen, comma, semicolon, period, becomes,beginsym,
    endsym, ifsym, thensym,whilesym, dosym, callsym,
    constsym, varsym, procsym, readsym, writesym,
    elsesym, repeatsym, untilsym
};

enum symbol_table_type // symbol table type
{
    constant, variable, procedure
};

enum opcode // P-code operation
{
    lit, opr, lod, sto, cal, itn, jmp, jpc, red, wrt
};

struct instruction // instruction
{
    opcode opc; // P-code operation
    int lev; // level
    int	addr; // address
};

struct symbol_table // symbol table
{
    string name;
    symbol_table_type kind;
    int val;
    int level;
    int adr;
};

extern char ch; // last character read
extern symbol sym; // last symbol read
extern string id; // last identifier read
extern long long num; // last number read
extern int line_len; // line length
extern int err_count; // error count
extern int code_index; // code allocation index
extern int row; // line count & row
extern int col; // character count & column
extern string line_code; // a row of source code
extern instruction code[cxmax + 7]; // Pcode instruction array
extern string word[norw + 7]; // keywords table
extern symbol wsym[norw + 7]; // reserved words symbol table
extern symbol ssym[512]; // operator & delimiter symbol table
extern set<symbol> declbegsys; // FIRST(<declaration>)
extern set<symbol> statbegsys; // FIRST(<statement>)
extern set<symbol> facbegsys; // FIRST(<factor>)
extern string mnemonic[17]; // Pcode instruction operation mnemonic
extern symbol_table TAB[txmax + 7];
extern string error_message[57]; // error message
extern ifstream source_code_file; // source code file
extern vector<vector<string> > pcode_buf; // pcode buffer
extern vector<string> error_buf; // error log buffer
extern jmp_buf buf; // local label
extern string interpret_output;
extern vector<string> source_code;

#endif // ARGS_H
