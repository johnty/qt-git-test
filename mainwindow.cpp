#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    git_libgit2_init();
    hasDir = false;
    git_repository* repo = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
    if (repo != NULL)
    {
        git_repository_free(repo);
    }
    git_libgit2_shutdown();
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
        const char* repo_dir = ui->textRepoLocation->text().toStdString().c_str();
        int ret = git_repository_open(&repo, repo_dir);
        printf("repo opened\n");
    }
}

void MainWindow::on_pushButtonInit_clicked()
{
    if (!hasDir)
        return;
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
}

void MainWindow::on_pushButtonCommit_clicked()
{
    if (repo != NULL)
    {
        QString filepath = ui->textRepoLocation->text();
        QFile myfile(filepath+ "/test.txt");
        if (myfile.open(QFile::WriteOnly))
        {
            QTextStream out(&myfile);
            out << ui->textContent->toPlainText();
        }
        git_strarray array = {0};
        git_index* index;
        struct print_payload payload;
        git_repository_index(&index, repo);
        git_index_add_all(index, &array, 0, NULL, &payload);

        git_index_write(index);

        git_index_free(index);
        QMessageBox::information(this, tr("commit"), tr("commit successful"));
    }
}

void MainWindow::on_pushButtonLoad_clicked()
{
    QString filepath = ui->textRepoLocation->text();
    QFile myfile(filepath+ "/test.txt");
    if (myfile.open(QFile::ReadOnly))
    {
        QString data;
        QTextStream s1(&myfile);
        data.append(s1.readAll());
        ui->textContent->setText(data);
    }
}
