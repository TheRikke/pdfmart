#ifndef IMERGE_H
#define IMERGE_H

#include "PageList.h"

#include <QHash>

typedef QHash<QString, QString> MetaDataList;

class IMerge {
public:
   virtual ~IMerge() {}
   virtual void Merge(const QStringList& inputFiles, const PageList& pageList, const QString& outputFile, const MetaDataList &meta_data_list = MetaDataList()) = 0;
   virtual QStringList WriteToPageList(const QStringList& inputFiles, const PageList& pageList, const QString &outputPath) = 0;
};

#endif // IMERGE_H
