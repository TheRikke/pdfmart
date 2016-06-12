#include "OptionDialog.h"
#include "MergePDF.h"
#include "PopplerTools.h"
#include "ToolsTest.h"
#include "PMSettings.h"
#include "OcrHandler.h"
#include "Logger.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QObject>

void myMessageHandler(QtMsgType type, const QMessageLogContext &/*context*/, const QString &msg)
{
   QString txt;
   switch (type) {
   case QtInfoMsg:
      txt = QString("Info: %1").arg(msg);
      break;
   case QtDebugMsg:
      txt = QString("Debug: %1").arg(msg);
      break;
   case QtWarningMsg:
      txt = QString("Warning: %1").arg(msg);
      break;
   case QtCriticalMsg:
      txt = QString("Critical: %1").arg(msg);
      break;
   case QtFatalMsg:
      txt = QString("Fatal: %1").arg(msg);
      abort();
   }
   QFile outFile("log");
   outFile.open(QIODevice::WriteOnly | QIODevice::Append);
   QTextStream ts(&outFile);
   ts << txt << endl;
}

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   QCoreApplication::setOrganizationName("rikky@web.de");
   QCoreApplication::setApplicationName("pdfmart");
   QCoreApplication::setApplicationVersion("1.0");

   QCommandLineParser parser;
   parser.setApplicationDescription("Merge and reorganize PDF pages");
   parser.addHelpOption();
   parser.addVersionOption();
   parser.addPositionalArgument("input", QCoreApplication::translate("main", "Source files to merge."), "[input pdf's ...]");

   QCommandLineOption outputFileName(QStringList() << "o" << "output", "Write generated data into <file>.", "file", "merged.pdf");
   parser.addOption(outputFileName);
   QCommandLineOption debugFlag(QStringList() << "d" << "debug", "Debug output and keep temporary files.");
   parser.addOption(debugFlag);
   QCommandLineOption logFileFlag(QStringList() << "l" << "logfile", "Output infos and logs to logfile.");
   parser.addOption(logFileFlag);
   QCommandLineOption noconversionFlag(QStringList() << "n" << "no-conversion", "Disable workaround for tesseract bug and skip some image conversion.");
   parser.addOption(noconversionFlag);
   QCommandLineOption testFlag(QStringList() << "t" << "test-external-tools", "Validate external tools can be found and behave as expected");
   parser.addOption(testFlag);
   parser.process(app);

   Logger::SetLogLevel(parser.isSet("debug") ? Logger::INFO : Logger::WARNING);
   Logger::Log(Logger::INFO, QString("Settings read from ") + QSettings().fileName());
   if(parser.isSet("logfile")) {
      qInstallMessageHandler(myMessageHandler);
   }

   PMSettings settings(parser.isSet("debug"), parser.isSet("no-conversion"));
   if(parser.isSet("test-external-tools")) {
      ToolsTest test;
      exit(test.Test());
   }

   OptionDialog dialog(&app);

   QStringList args = parser.positionalArguments();
   dialog.AddInputFiles(args);
   dialog.setWindowFlags(Qt::Window);
   dialog.setModal(true);
   dialog.setVisible(true);
   return app.exec();
}
