#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    args_init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::str2qstr(const string str)
{
    return QString::fromLocal8Bit(str.data());
}

string MainWindow::qstr2str(const QString qstr)
{
    QByteArray cdata = qstr.toLocal8Bit();
    return string(cdata);
}

void MainWindow::args_init()
{
    line_len = 0;
    err_count = 0;
    code_index = 0;
    row = 0;
    col = 0;
    ch = ' ';

    // error
    error_message[1] = "Expected '=' instead of ':='";
    error_message[2] = "Expected number after '='";
    error_message[3] = "Expected '=' after identifier";
    error_message[4] = "Expected identifier after 'const', 'var', 'procedure'";
    error_message[5] = "Missing ',' or ';'";
    error_message[6] = "Incorrect Symbol after procedure description";
    error_message[7] = "Expected statement";
    error_message[8] = "Incorrect Symbol after program statement part";
    error_message[9] = "Expected '.'";
    error_message[10] = "Missing ';' between statements";
    error_message[11] = "Undeclared identifier";
    error_message[12] = "Cannot assign to a const or procedure";
    error_message[13] = "Expected ':='";
    error_message[14] = "Expected identifier after 'call'";
    error_message[15] = "Const or variable uncallable";
    error_message[16] = "Expected then";
    error_message[17] = "Expected ';' or 'end'";
    error_message[18] = "Expected 'do'";
    error_message[19] = "Incorrect symbol fter statement";
    error_message[20] = "Expected relation operator";
    error_message[21] = "Unexpected procedure indentifier in expression";
    error_message[22] = "Missing ')'";
    error_message[23] = "Unexpected symbol after factor";
    error_message[24] = "Cannot start expression with this symbol";
    error_message[25] = "Runtime Error. Address overbound";
    error_message[26] = "Program too long";
    error_message[30] = "This number is so large";
    error_message[32] = "Too many nesting levels";
    error_message[40] = "Expected '('";

    // keywords
    word[1] = "begin";		                           word[2] = "call";
    word[3] = "const";		                           word[4] = "do";
    word[5] = "else";		                           word[6] = "end";
    word[7] = "if";			                           word[8] = "odd";
    word[9] = "procedure";	                           word[10] = "read";
    word[11] = "repeat";	                           word[12] = "then";
    word[13] = "until";		                           word[14] = "var";
    word[15] = "while";		                           word[16] = "write";

    // keywords symbol
    wsym[1] = beginsym;                                wsym[2] = callsym;
    wsym[3] = constsym;                                wsym[4] = dosym;
    wsym[5] = elsesym;                                 wsym[6] = endsym;
    wsym[7] = ifsym;                                   wsym[8] = oddsym;
    wsym[9] = procsym;                                 wsym[10] = readsym;
    wsym[11] = repeatsym;	                           wsym[12] = thensym;
    wsym[13] = untilsym;                               wsym[14] = varsym;
    wsym[15] = whilesym;                               wsym[16] = writesym;

    // operator & delimiter symbol
    memset(ssym, nul, sizeof(ssym));
    ssym[static_cast<unsigned int>('+')] = pluss;      ssym[static_cast<unsigned int>('-')] = minuss;
    ssym[static_cast<unsigned int>('*')] = times;      ssym[static_cast<unsigned int>('/')] = slash;
    ssym[static_cast<unsigned int>('(')] = lparen;     ssym[static_cast<unsigned int>(')')] = rparen;
    ssym[static_cast<unsigned int>('=')] = eql;        ssym[static_cast<unsigned int>(',')] = comma;
    ssym[static_cast<unsigned int>('.')] = period;     ssym[static_cast<unsigned int>('<')] = lss;
    ssym[static_cast<unsigned int>('>')] = gtr;        ssym[static_cast<unsigned int>(';')] = semicolon;

    // instruction operation mnemonic
    mnemonic[lit] = "LIT";                             mnemonic[opr] = "OPR";
    mnemonic[lod] = "LOD";                             mnemonic[sto] = "STO";
    mnemonic[cal] = "CAL";                             mnemonic[itn] = "INT";
    mnemonic[jmp] = "JMP";                             mnemonic[jpc] = "JPC";
    mnemonic[red] = "RED";                             mnemonic[wrt] = "WRT";

    // FIRST(<declaration>)
    declbegsys.clear();
    declbegsys.insert(constsym);
    declbegsys.insert(varsym);
    declbegsys.insert(procsym);

    // FIRST(<statement>)
    statbegsys.clear();
    statbegsys.insert(beginsym);
    statbegsys.insert(callsym);
    statbegsys.insert(ifsym);
    statbegsys.insert(whilesym);
    statbegsys.insert(repeatsym);

    // FIRST(<factor>)
    facbegsys.clear();
    facbegsys.insert(ident);
    facbegsys.insert(number);
    facbegsys.insert(lparen);

    // empty buffer
    for (int i = static_cast<int>(pcode_buf.size()) - 1; i >= 0; i--)
    {
        vector<string>().swap(pcode_buf[static_cast<unsigned int>(i)]);
    }
    vector<vector<string> >().swap(pcode_buf);
    vector<string>().swap(error_buf);

    // empty source code buffer
    vector<string>().swap(source_code);
}

void MainWindow::on_pushButton_clicked()
{
    // set empty
    ui->textBrowser_2->clear();
    ui->tableWidget->clear();
    ui->textBrowser_3->clear();
    ui->textBrowser_4->clear();
    ui->textEdit->clear();

    // open file
    open_file_name = QFileDialog::getOpenFileName(this, tr("Open txt file"), "./", tr("Text files(*.txt);;All files (*.*)"));

    // read from file
    file_path = qstr2str(open_file_name);
    open_file_name = str2qstr(file_path);
    qDebug() << "open_file_name: " << open_file_name << endl;

    QString displayString;
    QFile file_to_text(open_file_name);
    if(!file_to_text.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << 1 << endl;
        qDebug() << "File open failed." << endl;
    }
    while(!file_to_text.atEnd())
    {
        QByteArray line = file_to_text.readLine();
        QString str(line);
        displayString.append(str);
    }

    // display path and source code
    ui->textBrowser->clear();
    ui->textBrowser->setPlainText(open_file_name);
    ui->textBrowser_2->clear();
    ui->textBrowser_2->setPlainText(displayString);

    compiled = false;
}

void MainWindow::on_pushButton_2_clicked()
{
    // set empty
    ui->textEdit->clear();
    ui->tableWidget->clear();
    ui->textBrowser_3->clear();
    ui->textBrowser_4->clear();


    if (file_path == "")
    {
        QMessageBox mesg;
        mesg.warning(this,"Alert","File not open.");
        return;
    }

    // argument initializier
    args_init();

    // open file
    source_code_file.open(file_path.c_str());

    // handle the source code
    char str_tmp[512];
    while (source_code_file.getline(str_tmp, 512))
    {
        cout <<  "Read from file: "  << str_tmp << endl;
        source_code.push_back(string(str_tmp));
    }

    // Lexer
    Lexer lexer = Lexer();

    // Error
    Error error = Error();

    // Parser
    Parser parser = Parser();

    // get the first symbol
    lexer.nextsym();

    set<symbol> start;
    start.clear();
    start.insert(period);
    start.insert(declbegsys.begin(), declbegsys.end());
    start.insert(statbegsys.begin(), statbegsys.end());

    if (!setjmp(buf))
    {
        parser.block(0, 0, start);
    }

    cout << "***here***" << endl;

    if (sym != period)
    {
        error.append(9);
    }

    // display error log
    displayErrorLog();

    // append pcode from array 'code' to pcode buffer
    parser.append_pcode(0);

    source_code_file.close();

    // display pcode
    displayPcode();

    compiled = true;
}

void MainWindow::displayErrorLog()
{
    string displayError = "";
    for (unsigned int i = 0; i < error_buf.size(); i++)
    {
        displayError += (error_buf[i] + "\n");
    }

    if (err_count > 0) // compiled unsuccessfully
    {
        displayError +=  "-----------------\n";
        displayError +=  (to_string(err_count) + " Errors!\n");
    }
    else if (err_count == 0) // compiled successfully
    {
        displayError =  "-----------------\n";
        displayError += "Compiled Successfully!";
    }

    // display error log
    ui->textBrowser_3->clear();
    ui->textBrowser_3->setPlainText(str2qstr(displayError));
    cout << displayError << endl;
}

void MainWindow::displayPcode()
{
    // display pcode
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->verticalHeader()->setVisible(true);
    ui->tableWidget->horizontalHeader()->setVisible(true);

    // create headers
    QStringList h_header;
    h_header << "Operation" << "Level" << "Address";
    ui->tableWidget->setHorizontalHeaderLabels(h_header);

    // set font
    QFont font = ui->tableWidget->horizontalHeader()->font();
    font.setBold(true);
    ui->tableWidget->horizontalHeader()->setFont(font);
    ui->tableWidget->verticalHeader()->setFont(font);

    // set headers bgcolor
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background:#B0C4DE;}");
    ui->tableWidget->verticalHeader()->setStyleSheet("QHeaderView::section{background:#B0C4DE;}");
    ui->tableWidget->horizontalHeader()->setDefaultSectionSize(177);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(75);

    // uneditable
    ui->tableWidget->setEditTriggers(nullptr);

    // set data
    for (int i = 0; i < static_cast<int>(pcode_buf.size()); i++)
    {
        ui->tableWidget->insertRow(i);

        // set Operation
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(pcode_buf[static_cast<unsigned int>(i)][1])));
        ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        // set Level
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(pcode_buf[static_cast<unsigned int>(i)][2])));
        ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        // set Address
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(pcode_buf[static_cast<unsigned int>(i)][3])));
        ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    // set empty
    ui->textBrowser_4->clear();

    if (file_path == "")
    {
        QMessageBox mesg;
        mesg.warning(this,"Alert","File not open.");
        return;
    }

    if (compiled && err_count == 0)
    {
        interpret_output = "";
        Interpreter interpreter = Interpreter();

        string input = qstr2str(ui->textEdit->toPlainText());

        int rt = interpreter.interpret(input);

        if (rt == 0)
        {
            ui->textBrowser_4->clear();
            ui->textBrowser_4->setPlainText(str2qstr(interpret_output));
        }
        else if (rt == -1)
        {
            QMessageBox mesg;
            mesg.warning(this,"Alert","Interpret Error. Too few input parameters.");
            return;
        }
        else if (rt == -2)
        {
            QMessageBox mesg;
            mesg.warning(this,"Alert","Input Error. Please input integers.");
            return;
        }
        else if (rt == -3)
        {
            QMessageBox mesg;
            mesg.warning(this,"Alert","Interpret Error. Too many input parameters.");
            return;
        }
        else
        {
            cout << "error" << endl;
        }
    }
    else if (!compiled)
    {
        QMessageBox mesg;
        mesg.warning(this,"Alert","Please compile before interpret.");
        return;
    }
    else if (err_count > 0)
    {
        QMessageBox mesg;
        mesg.warning(this,"Alert","Compiled Error. Please debug before interpret.");
        return;
    }
}
