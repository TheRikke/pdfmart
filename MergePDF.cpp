#include "MergePDF.h"
#include <QFile>
#include  <QtDebug>
#include <QTemporaryFile>

namespace {
   const char* MERGE_TOOL_NAMES[] = { "pdftk", "pdftk.exe", NULL };
}

MergePDF::MergePDF()
   :  MergeTool() {
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

void WriteMetaDataFile(QTemporaryFile& meta_data_file, const MetaDataList &meta_data_list)
{
   if(meta_data_file.open())
   {
      QTextStream in(&meta_data_file);
      MetaDataList::const_iterator it = meta_data_list.constBegin();
      while (it != meta_data_list.constEnd()) {
         in << "InfoKey: " << it.key() << "\nInfoValue: " << it.value() << "\n";
         ++it;
      }
      meta_data_file.close();
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

void MergePDF::Merge(const QStringList& inputFiles, const PageList& pageList, const QString& outputFile, const MetaDataList &meta_data_list) {
   QString current_output_file = meta_data_list.empty() ? outputFile : outputFile + ".tmp";
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
   arguments.append(current_output_file);
   arguments.append("verbose");
   arguments.append("dont_ask");

   MergeTool.setArguments(arguments);
   MergeTool.start();
   MergeTool.waitForFinished(-1);
   qDebug() << "ConvertLog: " << MergeTool.state() << QString(MergeTool.readAll()).replace('\r',"") << MergeTool.arguments().join("' '");


   if(!meta_data_list.empty()) {
      QTemporaryFile meta_data_file;
      meta_data_file.setAutoRemove(false);
      WriteMetaDataFile(meta_data_file, meta_data_list);
      qDebug() << meta_data_file.fileName();
      QStringList meta_data_arguments;
      meta_data_arguments.append(current_output_file);

      meta_data_arguments.append("update_info");
      meta_data_arguments.append(meta_data_file.fileName());

      meta_data_arguments.append("output");
      meta_data_arguments.append(outputFile);
      meta_data_arguments.append("verbose");
      meta_data_arguments.append("dont_ask");

      MergeTool.setArguments(meta_data_arguments);
      MergeTool.start();
      MergeTool.waitForFinished(-1);
      qDebug() << "ConvertLog: " << MergeTool.state() << QString(MergeTool.readAll()).replace('\r',"") << MergeTool.arguments().join("' '");
   }
}

QStringList MergePDF::WriteToPageList(const QStringList &/*inputFiles*/, const PageList &/*pageList*/, const QString &/*outputPath*/)
{
   qFatal("Not implemented");
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

