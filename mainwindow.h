#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define APPLICATION_NAME "ImageManipulator"

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtPrintSupport>
#include <algorithm>

QT_BEGIN_NAMESPACE
namespace Ui {class MainWindow;}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{

	Q_OBJECT

	public:
		MainWindow(QWidget *parent = nullptr);
		~MainWindow();
		void save();
		bool maybeSave();
		void closeEvent(QCloseEvent *event);

	private slots:
		void on_actionExit_triggered();
		void on_actionOpen_triggered();
		void setModified(bool modified);
		void on_actionNew_triggered();
		void on_actionPrint_triggered();
		void on_actionAbout_triggered();
		void on_actionZoomIn_triggered();
		void on_actionZoomOut_triggered();
		void on_actionZoomOriginal_triggered();
		void on_actionZoomFit_triggered();

	private:
		bool isModified;
		Ui::MainWindow *ui;
		void openFile(QString);
		void dropEvent(QDropEvent*);
		float zoomFactor;
		QImage img;
		void applyZoom(float);
		void applyZoom();
		QPixmap pixmap;
		float calcFittingZoom();

};

#endif // MAINWINDOW_H
