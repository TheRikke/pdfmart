#include "OcrHandler.h"

#include <QDebug>
#include <QSettings>
#include <QFile>

namespace {
   const char* TESSERACT_NAMES[] = { "tesseract", "tesseract.exe", NULL };
}


bool FindTesseract(QProcess &tesseractProcess) {
   int index = 0;
   bool foundTesseract = false;
   tesseractProcess.setArguments(QStringList("--help"));
   while(TESSERACT_NAMES[index] != 0) {
      QString command("C:/Program Files (x86)/Tesseract-OCR/tesseract.exe");
      if(!QFile::exists(command))
      {
         qWarning() << "Damn, not fouund " << command;
      }
//      tesseractProcess.execute(command, QStringList("--help"));
//      tesseractProcess.setWorkingDirectory("C:/Program Files (x86)/Tesseract-OCR/");
      tesseractProcess.setProgram(command);
      tesseractProcess.start(command, QStringList("--help"));
      tesseractProcess.waitForFinished(-1);
      QByteArray procOutput = tesseractProcess.readAllStandardError();
      qDebug() << "output: " << procOutput << tesseractProcess.errorString();// << tesseractProcess.readAllStandardError();
      if(tesseractProcess.error() == QProcess::UnknownError && procOutput.size() > 0) {
         foundTesseract = true;
         qDebug() << "Found " << TESSERACT_NAMES[index];
         break;
      }
      qDebug() << "Tesseract executable not found" << TESSERACT_NAMES[index];
      index++;
   }
   if(!foundTesseract) {
      qWarning("No working Tesseract executable found");
   }

   return foundTesseract;
}

OcrHandler::OcrHandler()
{
   IsTesseractReady = FindTesseract(TesseractProcess);
}

QString OcrHandler::AddTextToPDF(const QString &pdfInputName, const QString &pdfOutputName)
{
   QSettings settings;
   QStringList arguments;
   arguments.append("-l");
   arguments.append(settings.value("Ocr/language", "deu").toString());
   arguments.append(pdfInputName);
   arguments.append(pdfOutputName);
   arguments.append("pdf");
   TesseractProcess.setArguments(arguments);
   TesseractProcess.start();
   TesseractProcess.waitForFinished(-1);

   if(TesseractProcess.exitStatus() != 0) {
      qWarning() << "Tesseract error: " << QString(TesseractProcess.readAll());
   }
   qDebug() << "TesseractLog: " << TesseractProcess.state() << QString(TesseractProcess.readAllStandardOutput()).replace('\r',"") << QString(TesseractProcess.readAllStandardError()).replace('\r',"") << QString("'").append(TesseractProcess.arguments().join("' '")).append("'");

   return pdfOutputName + ".pdf";
}

bool OcrHandler::IsReady() const
{
   return IsTesseractReady;
}

