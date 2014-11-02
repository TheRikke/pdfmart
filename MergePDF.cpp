#include "MergePDF.h"
#include  <QtDebug>

namespace {
   const char* MERGE_TOOL_NAME = "h:/sourcecode/pdfcat-build/pdftk.exe";
}
MergePDF::MergePDF(QObject *parent)
   : QObject(parent)
   , MergeTool() {
   MergeTool.setProgram(MERGE_TOOL_NAME);
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

   qDebug() << "ConvertLog: " << MergeTool.state() << MergeTool.readAll().toPercentEncoding() << MergeTool.arguments().join("' '");
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
   qDebug() << "processOutput = " << processOutput.toPercentEncoding();
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

