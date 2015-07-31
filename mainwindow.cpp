#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    git_libgit2_init();
    hasDir = false;
    git_repository* repo = NULL;

    ui->pushButtonInit->setDisabled(true); //don't allow init on start
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


        if (git_repository_open_ext(
                    NULL, repo_dir, GIT_REPOSITORY_OPEN_NO_SEARCH, NULL) == 0)
        {

            //!!!! so apparently the repo_dir value gets changed
            //! when you call open above to check things: .git gets appended to it, so ...
            //! this is ugly sandbox code anyway...

            repo_dir = ui->textRepoLocation->text().toStdString().c_str();
            int ret = git_repository_open(&repo, repo_dir);

            qDebug() << "opening existing repo at " << ui->textRepoLocation->text() << "ret = " << ret;

            ui->pushButtonInit->setDisabled(true);

            //load em tags

            ui->listRevisions->clear();
            git_strarray tags = {0};
            int error = git_tag_list(&tags, repo);
            qDebug() <<"found numTags =" <<tags.count;
            if (error == 0) {
                QString text_str;
                for (int i=0; i<tags.count; i++) {
                    text_str+=tags.strings[i];
                    text_str+="\n";
                    ui->listRevisions->addItem(tags.strings[i]);
                }
            }

        }
        else {
            ui->pushButtonInit->setEnabled(true);
        }

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

        //add to index
        git_strarray array = {0};
        git_index* index;
        struct print_payload payload;
        git_repository_index(&index, repo);
        git_index_add_all(index, &array, 0, NULL, &payload);

        git_index_write(index);

        git_index_free(index);

        //do commit


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

void MainWindow::on_pushButtonSave_clicked()
{

}

void MainWindow::on_listRevisions_clicked(const QModelIndex &index)
{
    qDebug() << "Selected tag " << ui->listRevisions->currentItem()->text();
}

void MainWindow::on_pushButtonTag_clicked()
{
    //make a tag!

    if (repo == NULL)
        return;

    qDebug() << "making a tag...";
    git_oid oid;
    git_object *target = NULL;
    git_signature *tagger = NULL;

    int err = git_revparse_single(&target, repo, "HEAD^{commit}");
    qDebug() << "reverseparse: "<<err;
    err = git_signature_now(&tagger, "johnty", "info@johnty.ca");
    qDebug() << "get signature: "<<err;

    //probably not the best text widgets to use?
    const char* tag_name = ui->textTagName->toPlainText().toStdString().c_str();
    const char* tag_msg = ui->textTagAnno->toPlainText().toStdString().c_str();

    err = git_tag_create(
          &oid,               /* new object id */
          repo,               /* repo */
          tag_name,           /* name */
          target,             /* target */
          tagger,             /* name/email/timestamp */
          tag_msg, /* message */
          false);             /* force? */
    qDebug() << "create tag: "<<err;

    git_object_free(target);
    git_signature_free(tagger);
}

void MainWindow::on_pushButtonLoadRev_clicked()
{
    if (ui->listRevisions->currentRow() >= 0)
    {
        QString tag_name_str = "refs/tags/"+ui->listRevisions->currentItem()->text();
        const char* tag_name = tag_name_str.toStdString().c_str();
        git_reference *ref;
        if (git_reference_lookup(&ref, repo, tag_name) >= 0)
        {
            git_tag *tag;
            git_reference_peel((git_object**)&tag, ref, GIT_OBJ_TAG);

            qDebug() << "selected tag message = " << git_tag_message(tag);
            git_tag_free(tag);
            git_reference_free(ref);
        }
    }
}
