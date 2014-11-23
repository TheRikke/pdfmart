#include "MergePDF.h"
#include <QFile>
#include  <QtDebug>

namespace {
const char* MERGE_TOOL_NAMES[] = { "pdftk", "pdftk.exe", NULL };
}

MergePDF::MergePDF(QObject *parent)

   : QObject(parent)
   , MergeTool(this) {
   FindPdfTk();
}

void MergePDF::FindPdfTk() {
   int index = 0;
   bool foundPdfTK = false;
   MergeTool.setArguments(QStringList("--help"));
   while(MERGE_TOOL_NAMES[index] != 0) {
      MergeTool.setProgram(MERGE_TOOL_NAMES[index]);
      MergeTool.start();
      MergeTool.waitForFinished(-1);
      if(MergeTool.error() == QProcess::UnknownError) {
         foundPdfTK = true;
         break;
      }
      qDebug() << "pdftk executable not found" << MERGE_TOOL_NAMES[index];
      index++;
   }
   if(!foundPdfTK) {
      qFatal("pdftk executable not found");
   }
}

void MergePDF::Merge(QString inputFile1, QString inputFile2, QString outputFile, bool /*splitByPage*/) {
   QStringList arguments;
   arguments.append(QString("A=%1").arg(inputFile1));
   arguments.append(QString("B=%1").arg(inputFile2));
   arguments.append("cat");
   arguments.append(GetOrder(GetNumberOfPages(inputFile1)));
   arguments.append("output");
   arguments.append(outputFile);
   arguments.append("verbose");
   arguments.append("dont_ask");
   MergeTool.setArguments(arguments);
   MergeTool.start();
   MergeTool.waitForFinished(-1);
}

void MergePDF::Merge(const QStringList& inputFiles, const PageList& pageList, const QString& outputFile) {
   QStringList arguments;

   char fileAlias('A');
   foreach (const QString& file, inputFiles) {
      arguments.append(QString("%1=%2").arg(fileAlias).arg(file));
      ++fileAlias;
   }
   arguments.append("cat");

   fileAlias = 'A';
   foreach (const PageEntry& pageEntry, pageList) {
      arguments.append(QString("%1%2").arg((char)(fileAlias + pageEntry.first)).arg(pageEntry.second+1));
   }
   arguments.append("output");
   arguments.append(outputFile);
   arguments.append("verbose");
   arguments.append("dont_ask");
   MergeTool.setArguments(arguments);
   MergeTool.start();
   MergeTool.waitForFinished(-1);
   qDebug() << "ConvertLog: " << MergeTool.state() << QString(MergeTool.readAll()).replace('\r',"") << MergeTool.arguments().join("' '");
}


int MergePDF::GetNumberOfPages(QString pdfFile) {
   qDebug("Start GetNumberOfPages");
   int numberOfPages = 0;
   QStringList arguments;
   arguments.append(pdfFile);
   arguments.append("dump_data");
   MergeTool.setArguments(arguments);
   MergeTool.start();
   MergeTool.waitForStarted(-1);
   MergeTool.waitForFinished(-1);
   QByteArray processOutput = MergeTool.readAll();
//   qDebug() << "processOutput = " << processOutput.toPercentEncoding() << MergeTool.arguments().join("' '");
   QRegExp rx("NumberOfPages:\\s+(\\d+)");
   if(rx.indexIn(processOutput) > -1)
      numberOfPages = rx.cap(1).toInt();
   qDebug("GetNumberOfPages = %d", numberOfPages);
   MergeTool.close();
   return numberOfPages;
}

QStringList MergePDF::GetOrder(int numberOfPages) {
   QStringList listOfPages;
   listOfPages.append("A1");
   listOfPages.append(QString("B%1").arg(numberOfPages));
   for (int currentPage = 2 ; currentPage <= numberOfPages ; ++currentPage) {
      listOfPages.append(QString("A%1").arg(currentPage));
      listOfPages.append(QString("B%1").arg( numberOfPages - currentPage + 1));
   }
   qDebug("listOfPages = %s", listOfPages.join(" ").toStdString().c_str());
   return listOfPages;
}

