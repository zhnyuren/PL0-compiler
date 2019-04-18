#include "parser.h"

Parser::Parser()
{
    error = Error();
}

// lev: block level, tx: symbol table index, fsys: FOLLOW set

// <block> ::= [<constant description part>][<variable description part>][<procedure description part>]<statement>
void Parser::block(int lev, int tx, set<symbol> fsys)
{
    int dx; // data allocation index
    int tx0; // initial table index
    int cx0; // initial code index
    dx = 3;
    tx0 = tx;
    TAB[tx].adr = code_index; // start of this block
    gen(jmp, 0, 0); // jump from declaration part to statement part

    if (lev > levmax) // Too many nesting levels
    {
        error.append(32);
    }

    do {
        if (sym == constsym) // constant
        {
            lexer.nextsym();
            do {
                constdeclaration(lev, tx, dx); // <constant description part>
                while (sym == comma)
                {
                    lexer.nextsym();
                    constdeclaration(lev, tx, dx); // {,<constant declaration>}
                }
                if (sym == semicolon)
                {
                    lexer.nextsym();
                }
                else // Missing ',' or ';'"
                {
                    error.append(5);
                }
            } while (sym == ident);
        }
        if (sym == varsym) // variable
        {
            lexer.nextsym();
            do {
                vardeclaration(lev, tx, dx); // <variable description part>
                while (sym == comma)
                {
                    lexer.nextsym();
                    vardeclaration(lev, tx, dx);
                }
                if (sym == semicolon)
                {
                    lexer.nextsym();
                }
                else // Missing ',' or ';'"
                {
                    error.append(5);
                }
            } while (sym == ident);
        }

        while (sym == procsym) // <Process Description Section> ::= <process header> <block>; {<process description section>}
        {
            lexer.nextsym();
            if (sym == ident) // <process header> ::= procedure<identifier>;
            {
                enter(procedure, dx, lev, tx); // enter symbol table
                lexer.nextsym();
            }
            else // Expected identifier after 'const', 'var', 'procedure'
            {
                error.append(4);
            }

            if (sym == semicolon) // ';' after indentifier
            {
                lexer.nextsym();
            }
            else // Missing ',' or ';'"
            {
                error.append(5);
            }

            set<symbol> follow; follow.clear();
            follow.insert(semicolon);
            follow.insert(fsys.begin(), fsys.end());
            block(lev + 1, tx, follow); // <block>

            if (sym == semicolon) // ;
            {
                lexer.nextsym();

                set<symbol> follow1; follow1.clear();
                follow1.insert(statbegsys.begin(), statbegsys.end());
                follow1.insert(ident);
                follow1.insert(procsym);

                lexer.skip(follow1, fsys, 6);
            }
            else // Missing ',' or ';'"
            {
                error.append(5);
            }
        }

        set<symbol> follow; follow.clear();
        follow.insert(statbegsys.begin(), statbegsys.end());
        follow.insert(ident);

        lexer.skip(follow, declbegsys, 7);
    } while (declbegsys.count(sym));

    code[TAB[tx0].adr].addr = code_index; // back enter statement code's tart address
    TAB[tx0].adr = code_index; // code's start address
    cx0 = code_index;
    gen(itn, 0, dx); // topstack point o operation area

    set<symbol> follow; follow.clear();
    follow.insert(semicolon);
    follow.insert(fsys.begin(), fsys.end());
    follow.insert(endsym);

    statement(follow, lev, tx); // <statement>
    gen(opr, 0, 0); // return

    set<symbol> follow1; follow1.clear();

    lexer.skip(fsys, follow1, 8);
}

// <constant definition> ::= <identifier>=<unsigned integer>
void Parser::constdeclaration(int lev, int &tx, int &dx)
{
    if (sym == ident)
    {
        lexer.nextsym();
        if (sym == eql || sym == becomes)
        {
            if (sym == becomes) // Expected '=' instead of ':='"
            {
                error.append(1);
            }

            lexer.nextsym();
            if (sym == number)
            {
                enter(constant, dx, lev, tx); // enter symbol table: constant
                lexer.nextsym();
            }
            else // Expected number after '='
            {
                error.append(2);
            }
        }
        else // Expected '=' after identifier
        {
            error.append(3);
        }
    }
    else // Expected identifier after 'const', 'var', 'procedure'
    {
        error.append(4);
    }
}

// <variable description part>::= var<identifier>{,<identifier>};
void Parser::vardeclaration(int lev, int &tx, int &dx)
{
    if (sym == ident)
    {
        enter(variable, dx, lev, tx);
        lexer.nextsym();
    }
    else // Expected identifier after 'const', 'var', 'procedure'
    {
        error.append(4);
    }
}

// <statement>
void Parser::statement(set<symbol> fsys, int lev, int &tx)
{
    int i, cx1, cx2;
    if (sym == ident) // assignment
    {
        i = position(id, tx);
        if (i == 0)
        {
            error.append(11);
        }
        else if (TAB[i].kind != variable) // Cannot assign to a const or procedure
        {
            error.append(12);
            i = 0;
        }
        lexer.nextsym();
        if (sym == becomes)
        {
            lexer.nextsym();
        }
        else // Expected ':='
        {
            error.append(13);
        }

        expression(fsys, lev, tx); // <expression>
        if (i != 0)
        {
            gen(sto, lev - TAB[i].level, TAB[i].adr);
        }
    }
    else if (sym == callsym) // procedure
    {
        lexer.nextsym();
        if (sym != ident)
        {
            error.append(14);
        }
        else
        {
            i = position(id, tx);
            if (i == 0)
            {
                error.append(11);
            }
            else
            {
                if (TAB[i].kind == procedure)
                {
                    gen(cal, lev - TAB[i].level, TAB[i].adr);
                }
                else
                {
                    error.append(15);
                }
            }
            lexer.nextsym();
        }
    }
    else if (sym == ifsym)
    {
        lexer.nextsym();

        set<symbol> follow; follow.clear();
        follow.insert(thensym);
        follow.insert(fsys.begin(), fsys.end());
        follow.insert(dosym);

        condition(follow, lev, tx);

        if (sym == thensym)
        {
            lexer.nextsym();
        }
        else
        {
            error.append(16);
        }

        cx1 = code_index;
        gen(jpc, 0, 0);

        set<symbol> follow1; follow1.clear();
        follow1.insert(fsys.begin(), fsys.end());
        follow1.insert(elsesym);

        statement(follow1, lev, tx);

        if (sym == elsesym)
        {
            lexer.nextsym();
            cx2 = code_index;
            gen(jmp, 0, 0);
            code[cx1].addr = code_index;
            statement(fsys, lev, tx);
            code[cx2].addr = code_index;
        }
        else
        {
            code[cx1].addr = code_index;
        }
    }
    else if (sym == beginsym)
    {
        lexer.nextsym();

        set<symbol> follow; follow.clear();
        follow.insert(semicolon);
        follow.insert(endsym);
        follow.insert(fsys.begin(), fsys.end());

        statement(follow, lev, tx);

        set<symbol> follow1; follow1.clear();
        follow1.insert(statbegsys.begin(), statbegsys.end());
        follow1.insert(semicolon);

        while (follow1.count(sym))
        {
            if (sym == semicolon)
            {
                lexer.nextsym();
            }
            else // Missing ';' between statements
            {
                error.append(10);
            }

            statement(follow, lev, tx);
        }

        if (sym == endsym) // end
        {
            lexer.nextsym();
        }
        else // Expected ';' or 'end'
        {
            error.append(17);
        }
    }
    else if (sym == whilesym)
    {
        cx1 = code_index;
        lexer.nextsym();

        set<symbol> follow; follow.clear();
        follow.insert(fsys.begin(), fsys.end());
        follow.insert(dosym);

        condition(follow, lev, tx);

        cx2 = code_index;
        gen(jpc, 0, 0);

        if (sym == dosym)
        {
            lexer.nextsym();
        }
        else
        {
            error.append(18);
        }

        statement(fsys, lev, tx);

        gen(jmp, 0, cx1);
        code[cx2].addr = code_index;
    }
    else if (sym == readsym)
    {
        lexer.nextsym();
        if (sym == lparen)
        {
            do {
                lexer.nextsym();
                if (sym == ident)
                {
                    i = position(id, tx);
                    if (i == 0)
                    {
                        error.append(11);
                    }
                    else
                    {
                        if (TAB[i].kind != variable) // Cannot assign to a const or procedure
                        {
                            i = 0;
                            error.append(12);
                        }
                        else
                        {
                            gen(red, lev - TAB[i].level, TAB[i].adr);
                        }
                    }
                }
                else
                {
                    error.append(28);
                }

                lexer.nextsym();

            } while (sym == comma);
        }
        else
        {
            error.append(40);
        }

        if (sym != rparen)
        {
            error.append(22);
        }

        lexer.nextsym();
    }
    else if (sym == writesym)
    {
        lexer.nextsym();
        if (sym == lparen)
        {
            set<symbol> follow; follow.clear();
            follow.insert(rparen);
            follow.insert(fsys.begin(), fsys.end());
            follow.insert(comma);

            do {
                lexer.nextsym();

                expression(follow, lev, tx);

                gen(wrt, 0, 0);
            } while (sym == comma);

            if (sym != rparen)
            {
                error.append(22);
            }
            lexer.nextsym();
        }
        else
        {
            error.append(40);
        }
    }
    else if (sym == repeatsym)
    {
        cx1 = code_index;
        lexer.nextsym();

        set<symbol> follow; follow.clear();
        follow.insert(semicolon);
        follow.insert(fsys.begin(), fsys.end());
        follow.insert(untilsym);

        statement(follow, lev, tx);

        set<symbol> follow1; follow1.clear();
        follow1.insert(ifsym);
        follow1.insert(beginsym);
        follow1.insert(whilesym);
        follow1.insert(callsym);
        follow1.insert(semicolon);

        while (follow1.count(sym))
        {
            if (sym == semicolon)
            {
                lexer.nextsym();
            }
            else
            {
                error.append(5);
            }

            set<symbol> follow2; follow2.clear();
            follow2.insert(fsys.begin(), fsys.end());
            follow2.insert(semicolon);
            follow2.insert(untilsym);

            statement(follow2, lev, tx);
        }
        if (sym == untilsym)
        {
            lexer.nextsym();
            condition(fsys, lev, tx);
            gen(jpc, 0, cx1);
        }
        else
        {
            error.append(25);
        }
    }

    set<symbol> follow; follow.clear();

    lexer.skip(fsys, follow, 19);
}

// <expression>
void Parser::expression(set<symbol> fsys, int lev, int &tx)
{
    symbol addop;

    set<symbol> follow; follow.clear();
    follow.insert(pluss);
    follow.insert(fsys.begin(), fsys.end());
    follow.insert(minuss);

    if (sym == pluss || sym == minuss)
    {
        addop = sym;
        lexer.nextsym();

        term(follow, lev, tx);

        if (addop == minuss)
        {
            gen(opr, 0, 1); // -
        }
    }
    else
    {
        term(follow, lev, tx);
    }

    while (sym == pluss || sym == minuss)
    {
        addop = sym;
        lexer.nextsym();

        term(follow, lev, tx);

        if (addop == pluss)
        {
            gen(opr, 0, 2);
        }
        else
        {
            gen(opr, 0, 3);
        }
    }
}

// <term>
void Parser::term(set<symbol> fsys, int lev, int &tx)
{
    symbol mulop;

    set<symbol> follow; follow.clear();
    follow.insert(times);
    follow.insert(fsys.begin(), fsys.end());
    follow.insert(slash);

    factor(follow, lev, tx);

    while (sym == times || sym == slash)
    {
        mulop = sym;
        lexer.nextsym();

        factor(follow, lev, tx);

        if (mulop == times)
        {
            gen(opr, 0, 4);
        }
        else
        {
            gen(opr, 0, 5);
        }
    }
}

// <factor>
void Parser::factor(set<symbol> fsys, int lev, int &tx)
{
    int i;

    lexer.skip(facbegsys, fsys, 24);

    while (facbegsys.count(sym))
    {
        if (sym == ident)
        {
            i = position(id, tx);
            if (i == 0) // Undeclared identifier
            {
                error.append(11);
            }
            else
            {
                if (TAB[i].kind == constant)
                {
                    gen(lit, 0, TAB[i].val);
                }
                else if (TAB[i].kind == variable)
                {
                    gen(lod, lev - TAB[i].level, TAB[i].adr);
                }
                else if (TAB[i].kind == procedure) // Unexpected procedure indentifier in expression
                {
                    error.append(21);
                }
                else
                {
                    cout << "symbol table error" << endl;
                }
            }
            lexer.nextsym();
        }
        else if (sym == number)
        {
            if (num > nmax) // too large
            {
                error.append(30);
                num = 0;
            }
            gen(lit, 0, static_cast<int>(num));
            lexer.nextsym();
        }
        else if (sym == lparen)
        {
            lexer.nextsym();

            set<symbol> follow; follow.clear();
            follow.insert(rparen);
            follow.insert(fsys.begin(), fsys.end());

            expression(follow, lev, tx);

            if (sym == rparen)
            {
                lexer.nextsym();
            }
            else // Missing ')'
            {
                error.append(22);
            }
        }
        set<symbol> follow; follow.clear();
        follow.insert(lparen);

        lexer.skip(fsys, follow, 23);
    }
}

// <condition>
void Parser::condition(set<symbol> fsys, int lev, int &tx)
{
    symbol relop;

    if (sym == oddsym)
    {
        lexer.nextsym();

        expression(fsys, lev, tx);

        gen(opr, 0, 6);
    }
    else
    {
        set<symbol> follow; follow.clear();
        follow.insert(neq);
        follow.insert(fsys.begin(), fsys.end());
        follow.insert(eql);
        follow.insert(gtr);
        follow.insert(leq);
        follow.insert(lss);
        follow.insert(geq);

        expression(follow, lev, tx);

        set<symbol> follow1; follow1.clear();
        follow1.insert(eql);
        follow1.insert(gtr);
        follow1.insert(neq);
        follow1.insert(lss);
        follow1.insert(leq);
        follow1.insert(geq);

        if (follow1.count(sym) == 0) // Expected relation operator
        {
            error.append(20);
        }
        else
        {
            relop = sym;
            lexer.nextsym();

            expression(fsys, lev, tx);

            switch (relop)
            {
            case eql:
                gen(opr, 0, 8);
                break;
            case neq:
                gen(opr, 0, 9);
                break;
            case lss:
                gen(opr, 0, 10);
                break;
            case geq:
                gen(opr, 0, 11);
                break;
            case gtr:
                gen(opr, 0, 12);
                break;
            case leq:
                gen(opr, 0, 13);
                break;
            default:
                break;
            }
        }
    }
}


// generate pcode instruction stored in 'code'
// fop: mnemonic, lev: level, addr: address
void Parser::gen(opcode fop, int lev, int addr)
{
    if (addr > amax)
    {
        error.append(25);
        longjmp(buf, 1);
    }

    if (code_index > cxmax)
    {
        error.append(26);
        longjmp(buf, 1);
    }

    code[code_index].opc = fop;
    code[code_index].lev = lev;
    code[code_index].addr = addr;

    code_index++;
}

// append pcode from array 'code' to pcode buffer
void Parser::append_pcode(int start)
{
    for (int i = start; i < code_index; i++) {
        const char* str = mnemonic[code[i].opc].data();

        char i_s[100];
        char str_s[100];
        char lev_s[100];
        char add_s[100];

        sprintf(i_s, "%d", i);
        sprintf(str_s, "%s", str);
        sprintf(lev_s, "%d", code[i].lev);
        sprintf(add_s, "%d", code[i].addr);

        vector<string> tmp;
        tmp.push_back(string(i_s));
        tmp.push_back(string(str_s));
        tmp.push_back(string(lev_s));
        tmp.push_back(string(add_s));

        pcode_buf.push_back(tmp);
    }
}


// enter symbol table
// k: const/var/procedure, dx: relative address
// lev: level, tx: symbol table index
void Parser::enter(symbol_table_type k, int &dx, int lev, int &tx)
{
    tx++;

    TAB[tx].name = id;
    TAB[tx].kind = k;

    if (k == constant)
    {
        if (num > nmax) // Too large
        {
            error.append(30);
            num = 0;
        }
        TAB[tx].val = static_cast<int>(num);
    }
    else if (k == variable)
    {
        TAB[tx].level = lev;
        TAB[tx].adr = dx;
        dx++;
    }
    else if (k == procedure)
    {
        TAB[tx].level = lev;
    }
    else
    {
        cout << "symbol table error" << endl;
    }
}

// reverse search 'id' in symbol table from 'tx' to 0
int Parser::position(string id, int tx)
{
    int j = tx;
    TAB[0].name = id;
    while (TAB[j].name != id)
    {
        j--;
    }
    return j;
}
