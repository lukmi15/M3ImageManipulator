/*Color correction dialogue for ImageManipulator
Author(s)		: Lukas Mirow
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

	private:
		Ui::ColorCorrectionDialogue *ui;
		void setDarkMode(bool);
		QString readQssFile(const QString& fn);

};

#endif //COLOR_CORRECTION_DIALOGUE_HPP
