#include "PopplerTools.h"
#include "PMSettings.h"

#include <QTemporaryDir>
#include <QDebug>

namespace {
   const char* PDFIMAGES_NAMES[] = { "pdfimages", "pdfimages.exe", NULL };
   const char* PDFUNITE_NAMES[] = { "pdfunite", "pdfunite.exe", NULL };
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
   FindPDFImages(PDFImageProcess, PDFIMAGES_NAMES);
   FindPDFImages(PDFUniteProcess, PDFUNITE_NAMES);
}

void PopplerTools::Merge(const QStringList &/*inputFiles*/, const PageList &/*pageList*/, const QString &/*outputFile*/, const MetaDataList &/*meta_data_list*/)
{
}

void PopplerTools::MergePDF(const QStringList &inputFiles, const QString &outputFile, const MetaDataList &meta_data_list)
{
   QStringList arguments;
   arguments.append(inputFiles);
   arguments.append(outputFile);
   PDFUniteProcess.setArguments(arguments);
   PDFUniteProcess.start();
   PDFUniteProcess.waitForFinished();

   qDebug() << "output: " << PDFUniteProcess.readAllStandardOutput() << PDFUniteProcess.errorString() << PDFUniteProcess.readAllStandardError();
 }

QStringList PopplerTools::WriteToPageList(const QStringList &inputFiles, const PageList &pageList, const QString &outputPath)
{
   int pageIndex = 0;
   QStringList outputFiles;
   QString destinationPath = outputPath;
   QTemporaryDir tempDir;
   tempDir.setAutoRemove(!PMSettings::IsDebugEnabled());
   if (tempDir.isValid()) {
      destinationPath = tempDir.path();
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

     // correct the filename. pdfimages does not tell us, how the file will be called
     QFileInfo fileTofind(outputFile);
     if (!fileTofind.exists()) {
        QStringList foundFiles = dir.entryList(QStringList() << (fileTofind.baseName() + "*" ));
        if(foundFiles.size() == 1) {
           outputFile = destinationPath + QDir::separator() + foundFiles.first();
        }
        else {
           qWarning() << "Could not find correct output file of pdfimages " << foundFiles;
        }
     }

     outputFiles.append(outputFile);
   }

   return outputFiles;
}
