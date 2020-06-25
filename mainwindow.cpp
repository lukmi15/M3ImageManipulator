#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	setAcceptDrops(true);
	setActionsThatRequireAnImage(false);
}

void MainWindow::updateStatusBar()
{
	statusBar()->showMessage(QString("Zoom: ")+ QString::number(zoomFactor * 100) + '%');
}

void MainWindow::applyZoom()
{
	ui->picLabel->setPixmap(pixmap.scaled(pixmap.size() * zoomFactor, Qt::KeepAspectRatio));
	updateStatusBar();
}

void MainWindow::setActionsThatRequireAnImage(bool setTo)
{
	ui->actionSave->setEnabled(setTo);
	ui->actionSaveAs->setEnabled(setTo);
	ui->actionCopy->setEnabled(setTo);
	ui->actionCut->setEnabled(setTo);
	ui->actionZoomIn->setEnabled(setTo);
	ui->actionZoomOut->setEnabled(setTo);
	ui->actionZoomOriginal->setEnabled(setTo);
	ui->actionZoomFit->setEnabled(setTo);
	ui->actionPrint->setEnabled(setTo);
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
	pixmap = QPixmap(0, 0);
	setActionsThatRequireAnImage(false);
	ui->picLabel->setPixmap(pixmap);
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

int MainWindow::openFile()
{
	return openFile(QFileDialog::getOpenFileName(this, tr("Open file for edit"), "", FILE_FILTER));
}

int MainWindow::openFile(QString fname)
{
	if(!maybeSave())
		return 1;
	if (!loadPixmap(QImage(fname)))
	{
		setWindowTitleFileName(QUrl(fname).fileName());
		return 0;
	}
	else
		return 1;
};

void MainWindow::signalLoadError(const QString& reason)
{
	QString msg = QString("The content could not be loaded because ") + reason;
	QMessageBox::critical(this, APPLICATION_NAME, tr(msg.toStdString().c_str()));
}

void MainWindow::setWindowTitleFileName(QString fn)
{
	setWindowTitle(fn + " - " + APPLICATION_NAME);
}

void MainWindow::dropEvent(QDropEvent *event)
{
	const QMimeData *md = event->mimeData();
	if (md->hasImage())
	{
		QImage img = qvariant_cast<QImage>(md->imageData());
		if (!loadPixmap(img))
			setWindowTitleFileName("<Untitled>");
		else
			return;
	}
	else
	{
		QUrl url = md->urls()[0];
		if (openFile(url.toLocalFile()))
			return;
	}
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
	if (!loadPixmap(cPixmap))
		setWindowTitleFileName("<Untitled>");
}

void MainWindow::on_actionCopy_triggered()
{
	QGuiApplication::clipboard()->setPixmap(pixmap);
}

int MainWindow::loadPixmap(const QImage& img)
{
	return loadPixmap(QPixmap::fromImage(img));
}

int MainWindow::loadPixmap(const QPixmap& pm)
{
	if (pm.isNull())
	{
		signalLoadError("it does not appear to contain an image");
		return 1;
	}
	else
	{
		pixmap = pm;
		applyZoom(1);
		setActionsThatRequireAnImage(true);
		return 0;
	}
}

void MainWindow::on_actionCut_triggered()
{
	on_actionCopy_triggered();
	on_actionNew_triggered();
}
