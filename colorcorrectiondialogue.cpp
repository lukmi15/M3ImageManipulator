/*Implementation of color correction dialgoue for ImageManipulator
Author(s)		: Lukas Mirow
Date of creation	: 6/28/2020
*/

#include "colorcorrectiondialogue.hpp"
#include "ui_colorcorrectiondialogue.h"

// using namespace Ui;

ColorCorrectionDialogue::ColorCorrectionDialogue(QWidget *parent) : QDialog(parent), ui(new Ui::ColorCorrectionDialogue)
{
	ui->setupUi(this);
	setDarkMode(((MainWindow*)parent)->isDarkMode());
}

ColorCorrectionDialogue::~ColorCorrectionDialogue() {delete ui;}

void ColorCorrectionDialogue::setDarkMode(bool to)
{
	if (to)
		setStyleSheet(readQssFile(DARKMODE_QSS_FILE_PATH));
	else
		setStyleSheet(readQssFile(BRIGHTMODE_QSS_FILE_PATH));
}

QString ColorCorrectionDialogue::readQssFile(const QString& fn)
{
	QFile f(fn);
	if (not f.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::critical(this, APPLICATION_NAME, tr("Failed to read QSS file"));
		return QString();
	}
	QTextStream in(&f);
	return in.readAll();
}

