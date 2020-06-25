#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	setAcceptDrops(true);
	setActionsThatRequireAnImage(false);
	QScroller::grabGesture(ui->scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
	qApp->installEventFilter(this);
	readSettingsFromConfig();
}

void MainWindow::updateStatusBar()
{
	statusBar()->showMessage(QString("Zoom: ")+ QString::number(round(zoomFactor * 100)) + '%');
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
	saveSettingsToConfig();
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

int MainWindow::openFile()
{
	return openFile(QFileDialog::getOpenFileName(this, tr("Open file for edit"), "", FILE_FILTER));
}

void MainWindow::addRecentFile(const QString& fname)
{
	recentFiles.append(fname);
	while (recentFiles.count() > MAX_RECENT_FILES)
		recentFiles.pop_front();
}

int MainWindow::openFile(QString fname)
{
	if(!maybeSave())
		return 1;
	if (!loadPixmap(QImage(fname)))
	{
		setWindowTitleFileName(QUrl(fname).fileName());
		addRecentFile(fname);
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

QPoint MainWindow::calcCanvasPosition()
{
	QPoint ret = ui->centralwidget->pos();
	// ret += ui->gridLayoutOuter->pos();
	ret += ui->scrollArea->pos();
	ret += ui->scrollArea->viewport()->pos();
	return ret;
}

bool MainWindow::isPosOnCanvas(QPoint pos)
{
	QRect canvasRect = QRect(calcCanvasPosition(), ui->scrollArea->viewport()->size());
	return canvasRect.contains(pos);
}

void MainWindow::on_actionPrint_triggered()
{
	// QPrinter printer;
	// QPrintDialog printDialog(&printer, this);
	// printDialog.exec();
	QPrinter printer;
        QPrintDialog *dlg = new QPrintDialog(&printer, this);
        if(dlg->exec() == QDialog::Accepted)
	{
                QPainter painter(&printer);
                painter.drawImage(QPoint(0,0), pixmap.toImage());
                painter.end();
        }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Control)
	{
		ctrlIsPressed = true;
		event->accept();
	}
	else
		event->ignore();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Control)
	{
		ctrlIsPressed = false;
		event->accept();
	}
	else
		event->ignore();
}

void MainWindow::wheelEvent(QWheelEvent *wEvent)
{
	// if (ui->actionZoomIn->isEnabled() and ctrlIsPressed) //FIXME
	// {
		QPoint numPixels = wEvent->pixelDelta();
		QPoint numDegrees = wEvent->angleDelta() / 8;
		if (!numPixels.isNull())
		{
			if (numPixels.y() > 0)
				applyZoom(zoomFactor * 1.1);
			else
				applyZoom(zoomFactor * 0.9);
		}
		else if (!numDegrees.isNull())
		{
			if (numDegrees.y() > 0)
				applyZoom(zoomFactor * 1.1);
			else
				applyZoom(zoomFactor * 0.9);
		}
		wEvent->accept();
	// }
	// else
	// 	wEvent->ignore();
}

//FIXME: Usage of this event filter code messes with the clicking behavior for the rest of the application
// bool MainWindow::eventFilter(QObject *obj, QEvent *event)
// {
// 	QEvent::Type eventType = event->type();
// 	if (eventType == QEvent::MouseMove)
// 	{
// 		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
// 		mouseMoveEvent(mouseEvent);
// 		return true;
// 	}
// 	else if (eventType == QEvent::Wheel)
// 	{
// 		QWheelEvent *wEvent = static_cast<QWheelEvent*>(event);
// 		wheelEvent(wEvent);
// 		return true;
// 	}
// 	else if (eventType == QEvent::MouseButtonPress)
// 	{
// 		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
// 		mousePressEvent(mouseEvent);
// 		return true;
// 	}
// 	else if (eventType == QEvent::MouseButtonRelease)
// 	{
// 		QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
// 		mouseReleaseEvent(mouseEvent);
// 		return true;
// 	}
// 	else
// 		return false;
// }

// void MainWindow::mouseMoveEvent(QMouseEvent *event)
// {
// 	if (isPosOnCanvas(event->pos()))
// 	{
// 		if (event->buttons() == Qt::LeftButton)
// 		{
// 			setCursor(Qt::ClosedHandCursor);
// 			scrollImageAccordingToMouseMovement(event);
// 			lastMouseDragPos = event->pos();
// 		}
// 		else
// 			setCursor(Qt::OpenHandCursor);
// 	}
// 	else
// 		setCursor(Qt::ArrowCursor);
// 	event->accept();
// }

void MainWindow::scrollImageAccordingToMouseMovement(QMouseEvent *event)
{
	QPoint mousePos = event->pos();
	int mouseDragX = mousePos.x() - lastMouseDragPos.x();
	int mouseDragY = mousePos.y() - lastMouseDragPos.y();
	ui->scrollArea->horizontalScrollBar()->setValue(ui->scrollArea->horizontalScrollBar()->value() - mouseDragX);
	ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->value() - mouseDragY);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	if (isPosOnCanvas(event->pos()))
	{
		setCursor(Qt::ClosedHandCursor);
		lastMouseDragPos = event->pos();
		event->accept();
	}
	else
		event->ignore();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
	if (isPosOnCanvas(event->pos()))
	{
		// setCursor(Qt::OpenHandCursor);
		setCursor(Qt::ArrowCursor); //HACK while the event filter method doesn't work
		scrollImageAccordingToMouseMovement(event);
		event->accept();
	}
	else
		event->ignore();
}

void MainWindow::saveSettingsToConfig()
{
	QFile f(CONFIG_FILE_PATH);
	f.open(QIODevice::WriteOnly | QIODevice::Text);
	f.write(("Window Pos X:" + QString::number(x()) + '\n').toStdString().c_str());
	f.write(("Window Pos Y:" + QString::number(y()) + '\n').toStdString().c_str());
	f.write(("Window Width:" + QString::number(width()) + '\n').toStdString().c_str());
	f.write(("Window Height:" + QString::number(height()) + '\n').toStdString().c_str());
	f.write("Recent Files Opened:");
	for (auto it = recentFiles.begin(); it + 1 == recentFiles.end(); it++)
		f.write((*it + ",").toStdString().c_str());
	f.write((recentFiles.end() - 1)->toStdString().c_str());
	//TODO: Language
	f.close();
}

unsigned MainWindow::readUint(QTextStream *in)
{
	bool ok;
	unsigned ret = in->readLine().split(':')[1].toUInt(&ok, 10);
	if (not ok)
		throw runtime_error("Failed to read an uint from the config file");
	return ret;
}

QPoint MainWindow::readPoint(QTextStream *in)
{
	return QPoint(readUint(in), readUint(in));
}

QSize MainWindow::readSize(QTextStream *in)
{
	return QSize(readUint(in), readUint(in));
}

QList<QString> MainWindow::readListOfStrings(QTextStream *in)
{
	QList<QString> ret;
	QString strs = in->readLine().split(':')[1];
	for (QString str : strs.split(','))
		ret.append(str);
	return ret;
}

void MainWindow::readSettingsFromConfig()
{
	if (not QFile::exists(CONFIG_FILE_PATH))
		return;
	try
	{
		QFile f(CONFIG_FILE_PATH);
		f.open(QIODevice::ReadOnly | QIODevice::Text);
		QTextStream in(&f);
		move(readPoint(&in));
		resize(readSize(&in));
		recentFiles = readListOfStrings(&in);
		//TODO: Language
		f.close();
	}
	catch (...)
	{
		QMessageBox::critical(this, APPLICATION_NAME, tr("Failed to read the config file"));
	}
}
