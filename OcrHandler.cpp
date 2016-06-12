#include "OcrHandler.h"
#include "Logger.h"

#include <QDebug>
#include <QSettings>
#include <QFile>

namespace {
   const char* TESSERACT_NAMES[] = { "tesseract", "tesseract.exe", "C:/Program Files (x86)/Tesseract-OCR/tesseract.exe", NULL };
}


bool FindTesseract(QProcess &tesseractProcess) {
   int index = 0;
   bool foundTesseract = false;
   tesseractProcess.setArguments(QStringList("--help"));
   while(TESSERACT_NAMES[index] != 0) {
      QString command(TESSERACT_NAMES[index]);
      tesseractProcess.setProgram(command);
      tesseractProcess.start(command, QStringList("--help"));
      tesseractProcess.waitForFinished(-1);
      QByteArray procOutput = tesseractProcess.readAllStandardError();
      Logger::Log(tesseractProcess, procOutput);
      if(tesseractProcess.error() == QProcess::UnknownError && procOutput.size() > 0) {
         foundTesseract = true;
         qDebug() << "Found " << TESSERACT_NAMES[index];
         break;
      }
      qDebug() << "Tesseract executable not found yet: " << TESSERACT_NAMES[index];
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

   Logger::Log(TesseractProcess);

   return pdfOutputName + ".pdf";
}

bool OcrHandler::IsReady() const
{
   return IsTesseractReady;
}

