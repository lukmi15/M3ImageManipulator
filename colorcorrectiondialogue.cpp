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

void ColorCorrectionDialogue::on_radioRgb_pressed()
{
	ui->radioYuv->setChecked(false);
	ui->boxRgb->setEnabled(true);
	ui->boxYuv->setEnabled(false);
}

void ColorCorrectionDialogue::on_radioYuv_pressed()
{
	ui->radioRgb->setChecked(false);
	ui->boxYuv->setEnabled(true);
	ui->boxRgb->setEnabled(false);
}

void ColorCorrectionDialogue::on_sliderR_valueChanged(int value)
{
		ui->labelR->setText(QString::number(value));
}

void ColorCorrectionDialogue::on_sliderG_valueChanged(int value)
{
		ui->labelG->setText(QString::number(value));
}

void ColorCorrectionDialogue::on_sliderB_valueChanged(int value)
{
		ui->labelB->setText(QString::number(value));
}

void ColorCorrectionDialogue::on_sliderY_valueChanged(int value)
{
		ui->labelY->setText(QString::number(value));
}

void ColorCorrectionDialogue::on_sliderU_valueChanged(int value)
{
		ui->labelU->setText(QString::number(value));
}

void ColorCorrectionDialogue::on_sliderV_valueChanged(int value)
{
		ui->labelV->setText(QString::number(value));
}


void ColorCorrectionDialogue::on_boxExitButtons_accepted() {}
void ColorCorrectionDialogue::on_boxExitButtons_rejected() {}
void ColorCorrectionDialogue::on_previewButton_toggled(bool checked) {}
void ColorCorrectionDialogue::closeEvent(QCloseEvent *event) {}
