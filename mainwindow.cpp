#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	setAcceptDrops(true);
}

void MainWindow::applyZoom()
{
	ui->picLabel->setPixmap(pixmap.scaled(pixmap.size() * zoomFactor, Qt::KeepAspectRatio));
}

void MainWindow::applyZoom(float newZoomFactor)
{
	if (newZoomFactor > 4)
		zoomFactor = 4;
	else if (newZoomFactor < .25)
		zoomFactor = .25;
	else
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
	openFile();
}

void MainWindow::on_actionNew_triggered()
{
    loadPixmap(QPixmap(0, 0));
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

void MainWindow::openFile()
{
	openFile(QFileDialog::getOpenFileName(this, tr("Open file for edit"), "", FILE_FILTER));
}

void MainWindow::openFile(QString fname)
{
	if(!maybeSave())
		return;
	loadPixmap(QImage(fname));
	// if( !file.open(QFile::Text | QFile::ReadOnly ) ) {
	//     QMessageBox::warning( this, tr("ImageManipulator"), tr("File could not be opened") );
	//     return;
	// }
};

void MainWindow::signalLoadError(const QString& reason)
{
	QString msg("The content could not be loaded because ");
	msg.append(reason);
	QMessageBox::critical(this, APPLICATION_NAME, tr(msg.toStdString().c_str()));
}

void MainWindow::dropEvent(QDropEvent *event)
{
	const QMimeData *md = event->mimeData();
	if (md->hasImage())
	{
		QImage img = qvariant_cast<QImage>(md->imageData());
		loadPixmap(img);
	}
	else
		openFile(md->urls()[0].toLocalFile());
	event->acceptProposedAction();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasImage() or event->mimeData()->hasUrls())
		event->acceptProposedAction();
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
	QSize imageSize = ui->picLabel->size() / zoomFactor;
	QSize widgetSize = ui->scrollArea->size();
	float newZoom = min((float)widgetSize.width() / (float)imageSize.width(), (float)widgetSize.height() / (float)imageSize.height());
	/*FIXME: This calculation doesn't work if the image is smaller than the widget (tested with the Corona picture)
	qDebug() << "Widget size: " << widgetSize.width() << 'x' << widgetSize.height() << Qt::endl;
	qDebug() << "Image size: " << imageSize.width() << 'x' << imageSize.height() << Qt::endl;
	qDebug() << "New zoom is " << newZoom << Qt::endl;*/
	return newZoom;// - 0.02; //HACK: There seems to be a padding in `scrollArea`, TODO
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
		loadPixmap(cPixmap);
	}
}

void MainWindow::on_actionCopy_triggered()
{
	QGuiApplication::clipboard()->setPixmap(pixmap);
}

void MainWindow::loadPixmap(const QImage& img)
{
	loadPixmap(QPixmap::fromImage(img));
}

void MainWindow::loadPixmap(const QPixmap& pm)
{
	if (pm.isNull())
		signalLoadError("it does not appear to contain an image");
	else
	{
		pixmap = pm;
		applyZoom(1);
	}
}

void MainWindow::on_actionCut_triggered()
{
	on_actionCopy_triggered();
	on_actionNew_triggered();
}
