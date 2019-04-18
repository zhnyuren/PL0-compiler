#include "error.h"

Error::Error()
{

}

void Error::append(int n)
{
    string str = "Line " + to_string(row) + /*", Column " + to_string(col + 1) +*/ " : " + error_message[n] + " (#" + to_string(n) + ").";
    error_buf.push_back(str);
    cout << str << endl;
    err_count++;
}
