#include "args.h"

char ch;
symbol sym;
string id;
long long num;
int line_len;
int row;
int col;
int err_count;
int code_index;
string line_code;
instruction code[cxmax + 7];
string word[norw + 7];
symbol wsym[norw + 7];
symbol ssym[512];
set<symbol> declbegsys;
set<symbol> statbegsys;
set<symbol> facbegsys;
string mnemonic[17];
symbol_table TAB[txmax + 7];
string error_message[57];
ifstream source_code_file;
vector<vector<string> > pcode_buf;
vector<string> error_buf;
jmp_buf buf;
string interpret_output;
vector<string> source_code;
