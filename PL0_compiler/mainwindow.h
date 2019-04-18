#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "args.h"
#include "parser.h"
#include "interpreter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    QString str2qstr(const string s);
    string qstr2str(const QString qs);
    void args_init();
    void displayErrorLog();
    void displayPcode();
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QString open_file_name;
    string file_path;
    bool compiled;
};

#endif // MAINWINDOW_H
