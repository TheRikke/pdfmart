#include <QApplication>

#include "OptionDialog.h"
#include "MergePDF.h"
#include "QObject"

int main(int argc, char *argv[])
{
	MergePDF merger;
	QApplication app(argc, argv);

	qDebug("Setup UI");
	QDialog* dialogMama = new QDialog();
	Ui_Dialog dialog;
	dialog.setupUi(dialogMama);
//	QObject::connect(dialogMama, SIGNAL(accepted()), &merger, SLOT(Merge()));
	dialogMama->setModal(true);
	dialogMama->setVisible(true);
	dialogMama->exec();
	qDebug("Start Merge %s, %s, %s, %d"
		   , dialog.inputFile1->text().toStdString().c_str()
		   , dialog.inputFile2->text().toStdString().c_str()
		   , dialog.outputFile->text().toStdString().c_str()
		   , dialog.splitPerPage->isChecked());
	merger.Merge(dialog.inputFile1->text(), dialog.inputFile2->text(), dialog.outputFile->text(), dialog.splitPerPage->isChecked());
	return 0; //app.exec();
}

