#ifndef POPPLER_TOOLS_H
#define POPPLER_TOOLS_H

#include "IMerge.h"
#include <QProcess>
#include <QTemporaryDir>

class PopplerTools
      : public IMerge
{
public:
   PopplerTools();

   void Merge(const QStringList &inputFiles, const PageList &pageList, const QString &outputFile, const MetaDataList &meta_data_list);
   void MergePDF(const QStringList &inputFiles, const QString &outputFile, const MetaDataList &meta_data_list);
   QStringList WriteToSeparatePages(const QStringList &inputFiles, const PageList &pageList, const QString &outputPath);
   QString ReadText(QFile& file);
   bool HasFoundPDFImagesExec() const;
   bool HasFoundPDFUniteExec() const;
   bool HasFoundPDFToTextExec() const;

private:
   QProcess PDFImageProcess;
   QProcess PDFUniteProcess;
   QProcess PDFToTextProcess;
   QTemporaryDir TempDir;
   bool FoundPDFImagesExec;
   bool FoundPDFUniteExec;
   bool FoundPDFToTextExec;
};

#endif // POPPLER_TOOLS_H
