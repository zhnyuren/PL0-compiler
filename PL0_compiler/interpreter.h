#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <sstream>
#include <iterator>
#include "args.h"

class Interpreter
{
public:
    Interpreter();

    int base(int lev);
    int interpret(string input);

    static const int stacksize = 500; // stack size
    int b; // base register
    int p; // program register
    int t; // topstack register
    int st[stacksize + 7]; // data store
    instruction inst; // instruction register

};

#endif // INTERPRETER_H
