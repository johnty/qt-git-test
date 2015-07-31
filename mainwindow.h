#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "git2.h"


namespace Ui {
class MainWindow;
}



enum print_options {
    SKIP = 1,
    VERBOSE = 2,
    UPDATE = 4,
};


struct print_payload {
    enum print_options options;
    git_repository *repo;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonDir_clicked();

    void on_pushButtonInit_clicked();

    void on_pushButtonCommit_clicked();

    void on_pushButtonLoad_clicked();

    void on_pushButtonSave_clicked();

    void on_listRevisions_clicked(const QModelIndex &index);

    void on_pushButtonTag_clicked();

    void on_pushButtonLoadRev_clicked();

private:
    Ui::MainWindow *ui;
    git_repository* repo;
    git_index* index;
    bool hasDir;
};

#endif // MAINWINDOW_H
