#ifndef MERGEPDF_H
#define MERGEPDF_H

#include "PageList.h"
#include <IMerge.h>
#include <QObject>
#include <QProcess>


typedef QHash<QString, QString> MetaDataList;

class MergePDF : public IMerge {
 public:
   explicit MergePDF();

   void Merge(QString, QString, QString, bool);
   void Merge(const QStringList& inputFiles, const PageList& pageList, const QString& outputFile, const MetaDataList &meta_data_list = MetaDataList());

   QStringList WriteToPageList(const QStringList &inputFiles, const PageList &pageList, const QString &outputPath);
 private:
   void FindPdfTk();
   QProcess MergeTool;
   QStringList GetOrder(int numberOfPages);
   int GetNumberOfPages(QString pdfFile);

};

#endif // MERGEPDF_H
