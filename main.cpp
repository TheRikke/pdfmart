#include <QApplication>
#include <QCommandLineParser>

#include "OptionDialog.h"
#include "MergePDF.h"
#include "QObject"

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   QCoreApplication::setApplicationName("pdfmart");
   QCoreApplication::setApplicationVersion("1.0");

   QCommandLineParser parser;
   parser.setApplicationDescription("Merge and reorganize PDF pages");
   parser.addHelpOption();
   parser.addVersionOption();
   parser.addPositionalArgument("input", QCoreApplication::translate("main", "Source files to merge."), "[input pdf's ...]");

   QCommandLineOption outputFileName(QStringList() << "o" << "output", "Write generated data into <file>.", "file", "merged.pdf");
   parser.addOption(outputFileName);
   parser.process(app);

   OptionDialog dialog(&app);

   QStringList args = parser.positionalArguments();
   dialog.AddInputFiles(args);
   dialog.setWindowFlags(Qt::Window);

   dialog.setModal(true);
   dialog.setVisible(true);
   return app.exec();
}
