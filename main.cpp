#include <QApplication>
#include <QCommandLineParser>

#include "OptionDialog.h"
#include "MergePDF.h"
#include "QObject"

int main(int argc, char *argv[])
{
   MergePDF merger;
   QApplication app(argc, argv);
   QCoreApplication::setApplicationName("pdfcat");
   QCoreApplication::setApplicationVersion("1.0");

   QCommandLineParser parser;
   parser.setApplicationDescription("Merge and reorganize PDF pages");
   parser.addHelpOption();
   parser.addVersionOption();
   parser.addPositionalArgument("input", QCoreApplication::translate("main", "Source files to merge."), "[input pdf's ...]");

   QCommandLineOption outputFileName(QStringList() << "o" << "output", "Write generated data into <file>.", "file", "merged.pdf");
   parser.addOption(outputFileName);
   parser.process(app);

   qDebug("Setup UI");
   QDialog* dialogMama = new QDialog();
   OptionDialog dialog;
   dialog.setupUi(dialogMama);

   QStringList args = parser.positionalArguments();
   dialog.InputList->addItems(args);
   dialog.outputFile->setText(parser.value(outputFileName));

   dialog.MergPDFs();

   //	QObject::connect(dialogMama, SIGNAL(accepted()), &merger, SLOT(Merge()));
   dialogMama->setModal(true);
   dialogMama->setVisible(true);
   if(dialogMama->exec() == QDialog::Accepted)
   {
//      qDebug("Start Merge %s, %s, %s, %d"
//             , dialog.inputFile1->text().toStdString().c_str()
//             , dialog.inputFile2->text().toStdString().c_str()
//             , dialog.outputFile->text().toStdString().c_str()
//             , dialog.splitPerPage->isChecked());
      merger.Merge(dialog.InputList->item(0)->text(), dialog.InputList->item(1)->text(), dialog.outputFile->text(), false);
   }
   return 0; //app.exec();
}

