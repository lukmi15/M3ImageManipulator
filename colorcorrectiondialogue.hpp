/*Color correction dialogue for ImageManipulator
Author(s)			: Lukas Mirow
Date of creation	: 6/28/2020
*/

#ifndef COLOR_CORRECTION_DIALOGUE_HPP
#define COLOR_CORRECTION_DIALOGUE_HPP

#include "mainwindow.h"
#include "shared.hpp"
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {class ColorCorrectionDialogue;}
QT_END_NAMESPACE

class ColorCorrectionDialogue : public QDialog
{
	Q_OBJECT

	public:
		ColorCorrectionDialogue(QWidget *parent = nullptr);
		~ColorCorrectionDialogue();

	private slots:
		void on_radioRgb_pressed();
		void on_radioYuv_pressed();
		void on_sliderR_valueChanged(int value);
		void on_sliderG_valueChanged(int value);
		void on_sliderB_valueChanged(int value);
		void on_sliderY_valueChanged(int value);
		void on_sliderU_valueChanged(int value);
		void on_sliderV_valueChanged(int value);
		void on_boxExitButtons_accepted();
		void on_boxExitButtons_rejected();
		void on_previewButton_toggled(bool checked);
		void closeEvent(QCloseEvent *event);


	private:
		Ui::ColorCorrectionDialogue *ui;
		void setDarkMode(bool);
		QString readQssFile(const QString& fn);

};

#endif //COLOR_CORRECTION_DIALOGUE_HPP
