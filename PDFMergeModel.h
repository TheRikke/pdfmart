#ifndef PDF_MERGE_MODEL_H
#define PDF_MERGE_MODEL_H

#include "PageList.h"

#include <QAbstractTableModel>

namespace Poppler {
class Document;
}

class PDFMergeModel : public QAbstractTableModel {
   Q_OBJECT
 public:
   PDFMergeModel(QObject *parent = 0);

   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;
   virtual QModelIndex index(int row, int column,
                             const QModelIndex &parent = QModelIndex()) const;
   virtual QModelIndex parent(const QModelIndex &child) const;

   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
   bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
   QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
   bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex());

   virtual Qt::ItemFlags flags(const QModelIndex &index) const;

   bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                     int row, int column, const QModelIndex &parent);

   virtual QStringList mimeTypes() const;

 private:
   QVector<Poppler::Document*> documents_;
   PageList pageList_;
};

#endif // PDF_MERGE_MODEL_H
