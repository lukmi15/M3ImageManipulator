#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), isModified(false), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // connect(ui->scrollArea->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setModified(bool)));
    ui->scrollArea->setBackgroundRole(QPalette::Dark);
    applyZoom(1);
}

void MainWindow::applyZoom(float newZoomFactor)
{
	zoomFactor = newZoomFactor;
	ui->picLabel->resize(zoomFactor * ui->picLabel->pixmap(Qt::ReturnByValue).size());
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

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(this, tr("About " APPLICATION_NAME), tr("Created by Lukas Mirow on TODO"));
}

void MainWindow::on_actionOpen_triggered()
{
    if(!maybeSave())
        return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file for edit") );
    openFile(fileName);

    // if( !file.open(QFile::Text | QFile::ReadOnly ) ) {
    //     QMessageBox::warning( this, tr("ImageManipulator"), tr("File could not be opened") );
    //     return;
    // }

}

void MainWindow::setModified(bool modified)
{
    this->isModified = modified;
}


void MainWindow::on_actionNew_triggered()
{
    if(!maybeSave())
        return;
    // ui->scrollArea->clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(maybeSave())
        event->accept();
    else
        event->ignore();
}

void MainWindow::on_actionPrint_triggered()
{
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Rejected)
        return;
    // ui->scrollArea->print(&printer);
}

void MainWindow::openFile(QString fname)
{
    img = QImage(fname);
    ui->picLabel->setPixmap(QPixmap::fromImage(img));
    applyZoom(1);
};

void MainWindow::dropEvent(QDropEvent *event)
{
	openFile(event->mimeData()->urls()[0].fileName()); //TODO: Test
}

void MainWindow::on_actionZoomIn_triggered()
{
	applyZoom(zoomFactor + 0.1);
}

void MainWindow::on_actionZoomOut_triggered()
{
	applyZoom(zoomFactor - 0.1);
}

void MainWindow::on_actionZoomOriginal_triggered()
{
	applyZoom(1);
}

void MainWindow::on_actionZoomFit_triggered()
{
	QSize imageSize = ui->picLabel->size();
	QSize widgetSize = ui->scrollArea->size();
	float newZoom = min((float)imageSize.width() / (float)widgetSize.width(), (float)imageSize.height() / (float)widgetSize.height());
	qInfo() << "newZoom is " << newZoom << Qt::endl;
	applyZoom(newZoom);
}
/*
   void ImageViewer::fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}
*/
