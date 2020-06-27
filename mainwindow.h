#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define APPLICATION_NAME "ImageManipulator"
#define MODIFICATION_WARNING "The image was modified, do you want to save it?"
#define ABOUT_TEXT "Created by Lukas Mirow on TODO<br>Created using Qt (see Help -> About Qt)"
#define FILE_FILTER "Images (*.png *.bmp *.jpg, *.jpeg)"
#define CONFIG_FILE_PATH QDir::homePath() + "/.ImageManipulatorConfig"
#define MAX_RECENT_FILES 10
#define SETTINGS_GROUP_NAME "Settings"
#define SETTINGS_IMAGE_PATHS_NAME "recents"
#define SETTINGS_IMAGE_PATH_NAME "recent"
#define SETTINGS_GEOMETRY_NAME "geometry"

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtPrintSupport>
#include <algorithm>
#include <QClipboard>
#include <cmath>
#include <stdexcept>

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
		void on_actionNew_triggered();
		void on_actionPrint_triggered();
		void on_actionAbout_triggered();
		void on_actionAboutQt_triggered();
		void on_actionZoomIn_triggered();
		void on_actionZoomOut_triggered();
		void on_actionZoomOriginal_triggered();
		void on_actionZoomFit_triggered();
		void on_actionCopy_triggered();
		void on_actionPaste_triggered();
		void on_actionCut_triggered();

	private:
		bool modified = false;
		Ui::MainWindow *ui;
		int openFile(QString);
		int openFile();
		void dropEvent(QDropEvent*);
		void dragEnterEvent(QDragEnterEvent*);
		float zoomFactor = 1;
		void applyZoom(float);
		void applyZoom();
		QPixmap pixmap;
		float calcFittingZoom();
		int loadPixmap(const QPixmap&);
		int loadPixmap(const QImage&);
		void signalLoadError(const QString& reason);
		void setActionsThatRequireAnImage(bool);
		void setWindowTitleFileName(QString fn);
		void updateStatusBar();
		QPoint calcCanvasPosition();
		bool isPosOnCanvas(QPoint pos);
		void scrollImageAccordingToMouseMovement(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		QPoint lastMouseDragPos;
		void wheelEvent(QWheelEvent *wEvent);
		QStringList recentFiles;
		void addRecentFile(const QString& fname);
		unsigned readUint(QTextStream *in);
		QPoint readPoint(QTextStream *in);
		QSize readSize(QTextStream *in);
		void saveSettingsToConfig();
		void readSettingsFromConfig();
		QString fname;

};

#endif // MAINWINDOW_H
