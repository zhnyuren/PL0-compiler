#include "interpreter.h"

Interpreter::Interpreter()
{

}

// return the base address through static chain
int Interpreter::base(int lev)
{
    int rt = b;
    while (lev > 0)
    {
        rt = st[rt];
        lev--;
    }
    return rt;
}

// function interpret
int Interpreter::interpret(string input)
{
    int input_buffer[stacksize + 7];
    int ibx = 0;
    int buffer_size = 0;

    // split with space
    istringstream iss(input);
    vector<string> str_vec { istream_iterator<string>(iss), istream_iterator<string>() } ;

    // string to int & store into input buffer
    for (unsigned int i = 0; i < str_vec.size(); i++)
    {
        int tmp = 0;
        try {
            tmp = stoi(str_vec[i]);
        } catch (...) {
            return -2;
        }
        input_buffer[ibx++] = tmp;
    }

    buffer_size = ibx;
    ibx = 0;

    t = p = 0;
    b = 1;
    st[1] = st[2] = st[3] = 0;

    do {
        inst = code[p];
        p++;
        switch (inst.opc)
        {
        case lit:
            t++;
            st[t] = inst.addr;
            break;
        case opr:
            switch (inst.addr)
            {
            case 0: // return
                t = b - 1;
                p = st[t + 3];
                b = st[t + 2];
                break;
            case 1: // minus
                st[t] = -st[t];
                break;
            case 2: // add
                t--;
                st[t] += st[t + 1];
                break;
            case 3: // sub
                t--;
                st[t] -= st[t + 1];
                break;
            case 4: // mul
                t--;
                st[t] *= st[t + 1];
                break;
            case 5: // div
                t--;
                st[t] /= st[t + 1];
                break;
            case 6: // parity
                st[t] %= 2;
                break;
            case 8: // equal
                t--;
                st[t] = (st[t] == st[t + 1]);
                break;
            case 9: // not equal
                t--;
                st[t] = (st[t] != st[t + 1]);
                break;
            case 10: // <
                t--;
                st[t] = (st[t] < st[t + 1]);
                break;
            case 11: // >=
                t--;
                st[t] = (st[t] >= st[t + 1]);
                break;
            case 12: // >
                t--;
                st[t] = (st[t] > st[t + 1]);
                break;
            case 13: // <=
                t--;
                st[t] = (st[t] <= st[t + 1]);
                break;
            default:
                break;
            }
            break;
        case lod:
            t++;
            st[t] = st[base(inst.lev) + inst.addr];
            break;
        case sto:
            st[base(inst.lev) + inst.addr] = st[t];
            t--;
            break;
        case cal:
            st[t + 1] = base(inst.lev);
            st[t + 2] = b;
            st[t + 3] = p;
            b = t + 1;
            p = inst.addr;
            break;
        case itn:
            t += inst.addr;
            break;
        case jmp:
            p = inst.addr;
            break;
        case jpc:
            if (st[t] == 0)
            {
                p = inst.addr;
            }
            t--;
            break;
        case red:
            if (ibx == buffer_size) // Too few input parameters
            {
                return -1;
            }
            st[base(inst.lev) + inst.addr] = input_buffer[ibx];
            ibx++;
            break;
        case wrt:
            char tmp[50];
            sprintf(tmp, "%d", st[t]);
            interpret_output += (string(tmp) + "\n");
            t++;
            break;
//        default:
//            break;
        }
    } while (p != 0);

    if (ibx < buffer_size) // Too many input parameters
    {
        return -3;
    }

    return 0;
}
