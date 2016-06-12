#include "ToolsTest.h"

#include "Logger.h"
#include "OcrHandler.h"
#include "PMSettings.h"
#include "PopplerTools.h"

#include <QDebug>
#include <QRegularExpression>
#include <QSettings>
#include <QTemporaryDir>

static const int MAX_DISTANCE = 300;

size_t uiLevenshteinDistance(const QString &s1, const QString &s2) {
   const size_t m(s1.size());
   const size_t n(s2.size());

   if( m == 0 ) return n;
   if( n == 0 ) return m;

   size_t* costs = new size_t[n + 1];

   for( size_t k = 0; k <= n; k++ ) {
      costs[k] = k;
   }

   size_t i = 0;
   for ( QString::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i ) {
      costs[0] = i + 1;
      size_t corner = i;

      size_t j = 0;
      for ( QString::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j ) {
         size_t upper = costs[j + 1];
         if( *it1 == *it2 ) {
            costs[j + 1] = corner;
         } else {
            size_t t(upper < corner ? upper : corner);
            costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
         }

         corner = upper;
      }
   }

   size_t result = costs[n];
   delete [] costs;

   return result;
}

static bool checkOCRResult(QFile& ocredPDFFile, PopplerTools& popplerTools) {
   bool ocrResultIsGood = false;
   QRegularExpression regex(" [ ]+");
   QString ocrFileText = popplerTools.ReadText(ocredPDFFile);
   QFile textFile(QString(":/tests/text_%1.txt").arg(QSettings().value("Ocr/language",
                                                                       "deu").toString()));
   textFile.open(QIODevice::ReadOnly);
   QString originalText = textFile.readAll();

   size_t distance = uiLevenshteinDistance(originalText.replace('\n', ' ').replace(regex, " "),
                                            ocrFileText.replace('\n', ' ').replace(regex, " "));

   if(distance > MAX_DISTANCE)
      qCritical() << "tesseract did not produce good results. Distance was " << distance << "and should be less than " << MAX_DISTANCE;
   else {
      ocrResultIsGood = true;
      qDebug() << "Distance: " << distance;
   }
   return ocrResultIsGood;
}

int ToolsTest::Test() {
   Q_ASSERT(uiLevenshteinDistance("rosettacode", "raisethysword") == 8);
   Q_ASSERT(uiLevenshteinDistance("edocattesor", "drowsyhtesiar") == 8);
   Q_ASSERT(uiLevenshteinDistance("kitten", "sitting") == 3);

   Logger::SetLogLevel(Logger::INFO);
   QTemporaryDir tempDir;
   tempDir.setAutoRemove(!PMSettings::IsDebugEnabled());
   PopplerTools popplerTools;
   int exitCode = 0;

   if(tempDir.isValid())
      qDebug() << "Tempdir for test file is " << tempDir.path();
   else {
      qCritical() << "Could not create temp dir";
      exitCode = 1;
   }
   if(!popplerTools.HasFoundPDFImagesExec()) {
      qCritical() << "could not find pdfimages";
      exitCode = 1;
   }

   if(!popplerTools.HasFoundPDFUniteExec()) {
      qCritical() << "could not find pdfunite";
      exitCode = 1;
   }

   if((exitCode == 0) && popplerTools.HasFoundPDFImagesExec() &&
      popplerTools.HasFoundPDFUniteExec()) {
      QFile testFile( QString(":/tests/scan_%1.pdf").arg(QSettings().value("Ocr/language",
                                                                           "deu").toString()));
      QString testFileExtracted = tempDir.path().append("/testFile.pdf");
      if(!testFile.exists()) {
         qDebug() << "Missing resource " << testFile.fileName();
         exitCode = 1;
      } else if(testFile.copy(testFileExtracted)) {
         exitCode = 1;

         QStringList pageImages = popplerTools.WriteToSeparatePages(
            QStringList() << testFileExtracted, PageList() << PageEntry(0,0) << PageEntry(0,
                                                                                          1),
            tempDir.path());
         if(pageImages.size() == 2) {
            OcrHandler ocrHandler;
            QStringList separatedPDFFilesWithText;
            foreach(const QString &file, pageImages) {
               separatedPDFFilesWithText.append(ocrHandler.AddTextToPDF(file, tempDir.path() +
                                                                        QDir::separator() +
                                                                        QFileInfo(file).baseName()));
            }
            if(separatedPDFFilesWithText.size() == pageImages.size()) {
               QString ocredPDFFileName(testFileExtracted + "_ocr.pdf");
               popplerTools.MergePDF(separatedPDFFilesWithText, ocredPDFFileName, MetaDataList());
               QFile ocredPDFFile(ocredPDFFileName);
               if(ocredPDFFile.exists()) {
                  if(popplerTools.HasFoundPDFToTextExec()) {
                     exitCode = checkOCRResult(ocredPDFFile, popplerTools) ? 0 : 1;
                  } else {
                     qDebug() << "pdftotext not found. can't check ocr results.";
                     exitCode = 0;
                  }
               } else
                  qCritical() << "outputFile has not been written";
            } else
               qCritical() << "could not merge pdf pages into one pdf";
         } else {
            qCritical() <<
            "Separating pdf into one image per page resulted in unexpected number of files. Expected: 2"
                        << ", Result: " << pageImages.size();
            qCritical() << pageImages;
         }
      } else {
         qCritical() << "Could not write testfile to temp directory: " << testFileExtracted;
         return 1;
      }
   }
   return exitCode;
}
