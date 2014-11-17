#include "PDFPagesModel.h"

#include <poppler-qt5.h>
#include <QImage>
#include <QMimeData>
#include <QEvent>

Q_DECLARE_METATYPE(Poppler::Document*)

PDFPagesModel::PDFPagesModel(QObject * parent)
   : QAbstractTableModel(parent)
   , documents_()
   , columnCount_(0)
{
}

int PDFPagesModel::rowCount(const QModelIndex & /* parent */) const
{
   return documents_.count();
}

int PDFPagesModel::columnCount(const QModelIndex & /* parent */) const
{
   return columnCount_;
}

QModelIndex PDFPagesModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
   return createIndex(row, column);
}

QModelIndex PDFPagesModel::parent(const QModelIndex &/*child*/) const
{
   return QModelIndex();
}

QVariant PDFPagesModel::data(const QModelIndex &index, int role) const
{
   QVariant result;
   switch(role)
   {
   case Qt::DisplayRole:
      result = QString::number(index.column());
      break;
   }
   return result;
}

QVariant PDFPagesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   QVariant result;
   switch(role)
   {
   case Qt::DisplayRole:
      if(orientation == Qt::Horizontal) {
         result.setValue(QString("Page %1").arg(section + 1));
      } else {
         result.setValue(QString("%1").arg(section + 1));
      }
   }

   return result;
}

Qt::ItemFlags PDFPagesModel::flags(const QModelIndex &/*index*/) const
{
   return Qt::ItemIsSelectable | Qt::ItemIsEnabled |Qt::ItemIsDragEnabled;
}

QMimeData *PDFPagesModel::mimeData(const QModelIndexList &indexes) const
{
   QByteArray encoded;
   QDataStream stream(&encoded, QIODevice::WriteOnly);
   for (int i = 0; i < indexes.count(); ++i) {
      const QModelIndex& modelIndex = indexes.at(i);
       stream << modelIndex.row() << modelIndex.column();
   }

   QMimeData *pdfMimeData = new QMimeData();
   pdfMimeData->setData("application/x-pdfcat-pdfpages", encoded);
   return pdfMimeData;
}

bool PDFPagesModel::event(QEvent *qEvent)
{
   if (qEvent->type() == QEvent::DynamicPropertyChange)
   {
//      QDynamicPropertyChangeEvent* propertyEvent = static_cast<QDynamicPropertyChangeEvent*>(qEvent);
      documents_ = property("SourceDocuments").value< QVector<Poppler::Document*> >();
      columnCount_ = 0;
      foreach(Poppler::Document *document, documents_)
      {
         const int pageCount = document->numPages();
         if (pageCount > columnCount_)
         {
            columnCount_ = pageCount;
         }
      }

//      for(int column = 0; column < columnCount_; ++column) {
//         setHeaderData(column, Qt::Vertical, tr("Page"));
//         setHeaderData(column, Qt::Horizontal, tr("Page"));
//      }
      emit layoutChanged();

      return true;
   }
   return false;
}
