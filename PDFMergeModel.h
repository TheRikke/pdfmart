#ifndef PDF_MERGE_MODEL_H
#define PDF_MERGE_MODEL_H

#include "PageList.h"

#include <QAbstractTableModel>

namespace Poppler {
   class Document;
}

class PDFMergeModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   PDFMergeModel(QObject *parent = 0);

   void setPDF(QVector<Poppler::Document*> documents, PageList pageList);

   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;
   virtual QModelIndex index(int row, int column,
                             const QModelIndex &parent = QModelIndex()) const;
   virtual QModelIndex parent(const QModelIndex &child) const;

   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
   QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

   virtual Qt::ItemFlags flags(const QModelIndex &index) const;
//   inline bool insertColumn(int acolumn, const QModelIndex &aparent);

   bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                     int row, int column, const QModelIndex &parent);

   virtual QStringList mimeTypes() const;

private:
   QVector<Poppler::Document*> documents_;
   PageList pageList_;
 };

#endif // PDF_MERGE_MODEL_H
