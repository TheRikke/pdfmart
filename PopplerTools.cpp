#include "PopplerTools.h"
#include "PMSettings.h"
#include "Logger.h"

#include <QDebug>

namespace {
   const char* PDFIMAGES_NAMES[] = { "pdfimages", "pdfimages.exe", NULL };
   const char* PDFUNITE_NAMES[] = { "pdfunite", "pdfunite.exe", NULL };
   const char* PDFTOTEXT_NAMES[] = { "pdftotext", "pdftotext.exe", NULL };
}

bool FindPDFImages(QProcess &pdfimagesProcess, const char* imageNames[]) {
   int index = 0;
   bool foundPdfImages = false;
   pdfimagesProcess.setArguments(QStringList("--help"));
   while(imageNames[index] != 0) {
      pdfimagesProcess.setProgram(imageNames[index]);
      pdfimagesProcess.start();
      pdfimagesProcess.waitForFinished(-1);
      if(pdfimagesProcess.error() == QProcess::UnknownError) {
         foundPdfImages = true;
         break;
      }
      qDebug() << "pdfimage executable not found" << imageNames[index];
      index++;
   }
   if(!foundPdfImages) {
      qWarning("pdfimage executable not found");
   }

   return foundPdfImages;
}

PopplerTools::PopplerTools()
{
   TempDir.setAutoRemove(!PMSettings::IsDebugEnabled());
   FoundPDFImagesExec = FindPDFImages(PDFImageProcess, PDFIMAGES_NAMES);
   FoundPDFUniteExec = FindPDFImages(PDFUniteProcess, PDFUNITE_NAMES);
   FoundPDFToTextExec = FindPDFImages(PDFToTextProcess, PDFTOTEXT_NAMES);
}

void PopplerTools::Merge(const QStringList &/*inputFiles*/, const PageList &/*pageList*/, const QString &/*outputFile*/, const MetaDataList &/*meta_data_list*/)
{
   abort(); // not implemented
}

void PopplerTools::MergePDF(const QStringList &inputFiles, const QString &outputFile, const MetaDataList &/*meta_data_list*/)
{
   QStringList arguments;
   arguments.append(inputFiles);
   arguments.append(outputFile);
   PDFUniteProcess.setArguments(arguments);
   PDFUniteProcess.start();
   PDFUniteProcess.waitForFinished();
   Logger::Log(PDFUniteProcess);
 }

QStringList PopplerTools::WriteToSeparatePages(const QStringList &inputFiles, const PageList &pageList, const QString &outputPath)
{
   int pageIndex = 0;
   QStringList outputFiles;
   QString destinationPath = outputPath;

   if (TempDir.isValid()) {
      destinationPath = TempDir.path();
   }
   qDebug() << "Tempdir: " << destinationPath;

   QDir dir(destinationPath);

   foreach (const PageEntry& page, pageList) {
      QString outputFile = destinationPath + QDir::separator() + QString("temp_doc_%1.tmp").arg(++pageIndex);
      QStringList arguments;
      const int pageNumber = page.second + 1;
      arguments.append("-all");
      arguments.append("-l");
      arguments.append(QString::number(pageNumber));
      arguments.append("-f");
      arguments.append(QString::number(pageNumber));
      arguments.append(inputFiles.at(page.first));
      arguments.append(outputFile);
      PDFImageProcess.setArguments(arguments);
      PDFImageProcess.start();
      PDFImageProcess.waitForFinished();
      Logger::Log(PDFImageProcess);
      // correct the filename. pdfimages does not tell us how the file will be called
      QFileInfo fileTofind(outputFile);
      if (!fileTofind.exists()) {
         QStringList foundFiles = dir.entryList(QStringList() << (fileTofind.baseName() + "*" ));
         if(foundFiles.size() == 1) {
            outputFile = destinationPath + QDir::separator() + foundFiles.first();
         }
         else {
            if(foundFiles.size() > 1) {
               qWarning() << "Found to many images for this page: " << fileTofind.baseName() << ". Maybe not from scanned source?";
            } else {
               qWarning() << "Could not find correct output for file " << fileTofind.baseName() << ". Found these pdfimages " << foundFiles;
            }
            outputFile = "";
         }
      }

      if(QFileInfo(outputFile).exists()) {
         outputFiles.append(outputFile);
      }
   }

   return outputFiles;
}

QString PopplerTools::ReadText(QFile &file)
{
   QTemporaryDir tempDir;
   QFile outputFile(tempDir.path() + "/text_file.txt");
   QString resultText;
   if(FoundPDFToTextExec) {
      PDFToTextProcess.setArguments( QStringList() << file.fileName() << outputFile.fileName());
      PDFToTextProcess.start();
      PDFToTextProcess.waitForFinished();
      Logger::Log(PDFToTextProcess);
      if(outputFile.exists()) {
         outputFile.open(QIODevice::ReadOnly);
         resultText = QString(outputFile.readAll());
      } else {
         Logger::Log(Logger::ERROR, "could not read textfile: " + outputFile.fileName() );
      }

   } else {
      Logger::Log(Logger::ERROR, "pdftotext not found. No text extraction from PDF possible");
   }
   return resultText;
}

bool PopplerTools::HasFoundPDFImagesExec() const
{
   return FoundPDFImagesExec;
}

bool PopplerTools::HasFoundPDFUniteExec() const
{
   return FoundPDFUniteExec;
}

bool PopplerTools::HasFoundPDFToTextExec() const
{
   return FoundPDFToTextExec;
}
