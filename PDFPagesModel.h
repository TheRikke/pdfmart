#ifndef PDFPAGESMODEL_H
#define PDFPAGESMODEL_H

#include <QAbstractTableModel>

namespace Poppler {
   class Document;
}

class PDFPagesModel : public QAbstractTableModel
{
   Q_OBJECT
public:
   PDFPagesModel(QObject *parent = 0);

   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;
   virtual QModelIndex index(int row, int column,
                             const QModelIndex &parent = QModelIndex()) const;
   virtual QModelIndex parent(const QModelIndex &child) const;

   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
   QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

   virtual Qt::ItemFlags flags(const QModelIndex &index) const;
   virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

   void setProperty(const char *name, const QVariant &value);
   bool event(QEvent *event);
private:
   QVector<Poppler::Document*> documents_;
   int columnCount_;
};

#endif // PDFPAGESMODEL_H
