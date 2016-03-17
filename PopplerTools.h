#ifndef POPPLER_TOOLS_H
#define POPPLER_TOOLS_H

#include "IMerge.h"
#include <QProcess>

class PopplerTools
      : public IMerge
{
public:
   PopplerTools();

   void Merge(const QStringList &inputFiles, const PageList &pageList, const QString &outputFile, const MetaDataList &meta_data_list);
   void MergePDF(const QStringList &inputFiles, const QString &outputFile, const MetaDataList &meta_data_list);
   QStringList WriteToPageList(const QStringList &inputFiles, const PageList &pageList, const QString &outputPath);
private:
   QProcess PDFImageProcess;
   QProcess PDFUniteProcess;
};

#endif // POPPLER_TOOLS_H
