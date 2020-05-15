#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), isModified(false), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->textEdit->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setModified(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::maybeSave()
{
    if (this->isModified)
    {
        auto ret = QMessageBox::warning(this, APPLICATION_NAME, "The file was modified\nDo you want to save it?", QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret==QMessageBox::Save)
            save();
        else if (ret==QMessageBox::Discard)
            return true;
	else // QMessageBox::Cancel
            return false;
    }
    return true;
}

void MainWindow::save()
{
    // TODO
}

void MainWindow::on_actionE_xit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_action_Open_triggered()
{
    if(!maybeSave())
        return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file for edit") );
    QFile file(fileName);

    if( !file.open(QFile::Text | QFile::ReadOnly ) ) {
        QMessageBox::warning( this, tr("ImageManipulator"), tr("File could not be opened") );
        return;
    }

    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->setText(text);
}

void MainWindow::setModified(bool modified)
{
    this->isModified = modified;
}


void MainWindow::on_action_New_triggered()
{
    if(!maybeSave())
        return;
    ui->textEdit->clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave())
        event->accept();
    else
        event->ignore();
}

void MainWindow::on_action_Print_triggered()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Rejected)
        return;
    ui->textEdit->print(&printer);
}
