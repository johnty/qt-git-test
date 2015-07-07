#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include "git2.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    git_libgit2_init();
    hasDir = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonDir_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    int res = dialog.exec();
    if (res)
    {
        QString mypath = dialog.selectedFiles()[0];
        ui->textRepoLocation->setText(mypath);
        hasDir = true;
    }
}

void MainWindow::on_pushButtonInit_clicked()
{
    git_repository* repo = NULL;
    const char* repo_dir = ui->textRepoLocation->text().toStdString().c_str();
    int res = git_repository_init(&repo, repo_dir,0);
    if (res == 0)
    {
        QMessageBox::information(this, tr("info"), tr("init successful"));
    }
    else
    {
        QMessageBox::information(this, tr("info"), tr("something went wrong"));
    }
    if (repo != NULL)
    {
        delete repo;
    }
}
