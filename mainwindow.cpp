#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->scrollArea->setBackgroundRole(QPalette::Dark);
}

void MainWindow::applyZoom()
{
	ui->picLabel->setPixmap(pixmap.scaled(pixmap.size() * zoomFactor, Qt::KeepAspectRatio));
}

void MainWindow::applyZoom(float newZoomFactor)
{
	zoomFactor = newZoomFactor;
	applyZoom();
}

MainWindow::~MainWindow()
{
	delete ui;
}

bool MainWindow::maybeSave()
{
	if (this->modified)
	{
		auto ret = QMessageBox::warning(this, APPLICATION_NAME, MODIFICATION_WARNING, QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		if (ret == QMessageBox::Save)
			save();
		else if (ret == QMessageBox::Discard)
			return true;
		else
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

void MainWindow::on_actionAboutQt_triggered()
{
	QMessageBox::aboutQt(this, tr("About Qt"));
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::about(this, tr("About " APPLICATION_NAME), tr(ABOUT_TEXT));
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

void MainWindow::on_actionNew_triggered()
{
    if(!maybeSave())
        return;
    pixmap = QPixmap(0, 0);
    applyZoom(1);
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
	pixmap = QPixmap::fromImage(img);
	applyZoom(1);
};

void MainWindow::dropEvent(QDropEvent *event)
{
	openFile(event->mimeData()->urls()[0].fileName()); //TODO: Test
}

void MainWindow::on_actionZoomIn_triggered()
{
	applyZoom(zoomFactor * 1.1);
}

void MainWindow::on_actionZoomOut_triggered()
{
	applyZoom(zoomFactor * 0.9);
}

void MainWindow::on_actionZoomOriginal_triggered()
{
	applyZoom(1);
}

float MainWindow::calcFittingZoom()
{
	QSize imageSize = ui->picLabel->size();
	QSize widgetSize = ui->scrollArea->size();
	float newZoom = min((float)widgetSize.width() / (float)imageSize.width(), (float)widgetSize.height() / (float)imageSize.height());
	return newZoom - 0.02; //HACK: There seems to be a padding in `scrollArea`, TODO
}

void MainWindow::on_actionZoomFit_triggered()
{
	applyZoom(calcFittingZoom());
}

void MainWindow::on_actionPaste_triggered()
{
	QPixmap cPixmap = QGuiApplication::clipboard()->pixmap();
	if (!cPixmap.isNull())
	{
		pixmap = cPixmap;
		applyZoom(1);
	}
}

void MainWindow::on_actionCopy_triggered()
{
	QGuiApplication::clipboard()->setPixmap(pixmap);
}

void MainWindow::on_actionCut_triggered()
{
	on_actionCopy_triggered();
	on_actionNew_triggered();
}
