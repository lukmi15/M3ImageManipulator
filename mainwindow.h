#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define APPLICATION_NAME "Image Manipulator"

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtPrintSupport>

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
		void on_actionE_xit_triggered();
		void on_action_Open_triggered();
		void setModified(bool modified);
		void on_action_New_triggered();
		void on_action_Print_triggered();

	private:
		bool isModified;
		Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
