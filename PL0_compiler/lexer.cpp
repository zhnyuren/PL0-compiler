#include "lexer.h"

Lexer::Lexer()
{
    error = Error();
}

void Lexer::nextch()
{
    if (col == line_len) // get character to end of line
    {
        line_len = 0;
        col = 0;
        line_code = source_code[static_cast<unsigned int>(row)] + " ";
        line_len = static_cast<int>(line_code.length());
        row++;
        cout << "test complete. row: " << row << endl;
        if (row > static_cast<int>(source_code.size()))
        {
            cout << "program incomplete" << endl;
            longjmp(buf, 1); // longjmp tp setjump(buf)
        }
    }
    if (col < line_len)
    {
        ch = line_code[static_cast<unsigned int>(col)];
        col++;
    }
}

void Lexer::nextsym()
{
    while (ch == ' ' || ch == '\n' || ch == '\t') {nextch();cout << "tt" << endl;}

    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) // keywords ot indentifier
    {
        int k = 0;
        string str = "";
        do {
            if (k < al)
            {
                str += ch;
            }
            nextch();
        } while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'));

        k = static_cast<int>(str.length());
        id = str;
        int l = 1;
        int r = norw; // binary search reserved word table
        do {
            k = (l + r) / 2;
            if (id <= word[k])
            {
                r = k - 1;
            }
            if (id >= word[k])
            {
                l = k + 1;
            }
        } while (l <= r);

        if (l - 1 > r) // keywords
        {
            sym = wsym[k];
            cout << "end?" << endl;
        }
        else // identifier
        {
            sym = ident;
        }
    }
    else if (ch >= '0' && ch <= '9') // number
    {
        long long k = 0;  // caution: int cannot be compared with INT32_MAX
        num = 0;
        sym = number;
        do {
            num = 10 * num + (ch - '0');
            k++;
            nextch();
        } while (ch >= '0' && ch <= '9');

        if (k > nmax) // This number is so large
        {
            error.append(30);
        }
    }
    else if (ch == ':')
    {
        nextch();
        if (ch == '=') // :=
        {
            sym = becomes;
            nextch();
        }
        else // :
        {
            sym = nul;
        }
    }
    else if (ch == '<') // < & <= & <>
    {
        nextch();
        if (ch == '=') // <=
        {
            sym = leq;
            nextch();
        }
        else if (ch == '>') // <>
        {
            sym = neq;
            nextch();
        }
        else // <
        {
            sym = lss;
        }
    }
    else if (ch == '>') // > & >=
    {
        nextch();
        if (ch == '=')
        {
            sym = geq;
            nextch();
        }
        else
        {
            sym = gtr;
        }
    }
    else // single character
    {
        sym = ssym[static_cast<unsigned int>(ch)];
        nextch();
        cout << "gaga" << endl;
    }
}


// testify to skip
// s1: follow
// s2: stop
// errn: error number
void Lexer::skip(set<symbol> s1, set<symbol> s2, int errn)
{
    if (!s1.count(sym))
    {
        error.append(errn);
        while (!s1.count(sym) && !s2.count(sym))
        {
            cout << "text" << endl;
            nextsym();
            cout << "bext" << endl;
        }
    }
}
