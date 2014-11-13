#ifndef MERGEPDF_H
#define MERGEPDF_H

#include "PageList.h"
#include <QObject>
#include <QProcess>


class MergePDF : public QObject
{
   Q_OBJECT
public:
   explicit MergePDF(QObject *parent = 0);

   void FindPdfTk();
signals:

public slots:
   void Merge(QString, QString, QString, bool);
   void Merge(const QStringList& inputFiles, const PageList& pageList, const QString& outputFile);

private:
   QProcess MergeTool;
   QStringList GetOrder(int numberOfPages);
   int GetNumberOfPages(QString pdfFile);
};

#endif // MERGEPDF_H
