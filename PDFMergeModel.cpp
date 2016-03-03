#include "PDFMergeModel.h"

#include <poppler-qt5.h>
#include <QImage>
#include <QDebug>
#include <QMimeData>
#include <QStandardItem>

namespace {
const char *MIME_TYPE = "application/x-pdfcat-pdfpages";
}
PDFMergeModel::PDFMergeModel(QObject * parent)
   : QAbstractTableModel(parent)
   , documents_()
   , pageList_() {
}

int PDFMergeModel::rowCount(const QModelIndex & /* parent */) const {
   return 1;
}

int PDFMergeModel::columnCount(const QModelIndex & /* parent */) const {
   return pageList_.count();
}

QModelIndex PDFMergeModel::index(int row, int column, const QModelIndex &/*parent*/) const {
   return createIndex(row, column);
}

QModelIndex PDFMergeModel::parent(const QModelIndex &child) const {
   qDebug() << "createIndex for child" << child;
   return QModelIndex();
}

QVariant PDFMergeModel::data(const QModelIndex &index, int role) const {
   QVariant result;
   switch(role) {
   case Qt::UserRole:
      QSize pagePos(pageList_.at(index.column()).first, pageList_.at(index.column()).second);
      result.setValue(pagePos);
      break;
   }
   return result;
}

bool PDFMergeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
   bool result = false;
   switch(role) {
   case Qt::UserRole:
      if(index.isValid() && value.isValid()) {
         QSize position(value.toSize());
         if(pageList_.count() <= index.column()) {
            pageList_.resize(index.column() + 1);
            emit layoutChanged();
         }
         pageList_[index.column()] = PageEntry(position.width(), position.height());
         result = true;
         emit dataChanged(index, index);
      }
      break;
   }
   return result;
}

QVariant PDFMergeModel::headerData(int section, Qt::Orientation orientation, int role) const {
   QVariant result;
   switch(role) {
   case Qt::DisplayRole:
      if(orientation == Qt::Horizontal) {
         result.setValue(QString("Page %1").arg(section + 1));
      } else {
         result.setValue(QString("%1").arg(section + 1));
      }
   }

   return result;
}

bool PDFMergeModel::removeColumns(int column, int count, const QModelIndex &parent) {
   if(count>0) {
      beginRemoveColumns(parent, column, column + count - 1);
      pageList_.remove(column, count);
      endRemoveColumns();
   }
   return true;
}

Qt::ItemFlags PDFMergeModel::flags(const QModelIndex &/*index*/) const {
   return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable;
}

bool PDFMergeModel::dropMimeData(const QMimeData *data, Qt::DropAction /*action*/, int row, int column, const QModelIndex &parent) {
   QStringList formats = data->formats();
   QByteArray encodedData = data->data(formats[0]);
   QDataStream stream(&encodedData, QIODevice::ReadOnly);
   PageList pageList;
   while(!stream.atEnd()) {
      int origin_row, origin_column;
      stream >> origin_row >> origin_column;
      pageList.append(PageEntry(origin_row, origin_column));
   }

   int beginColumn = -1;

   if (column != -1)
      beginColumn = row;
   else if (parent.isValid())
      beginColumn = parent.column();
   else
      beginColumn = columnCount(QModelIndex());

   insertColumns(beginColumn, pageList.count(), QModelIndex());
   foreach(PageEntry entry, pageList) {
      pageList_.insert(beginColumn++, entry);
   }
   emit layoutChanged();
   return true;
}

QStringList PDFMergeModel::mimeTypes() const {
   QStringList types(MIME_TYPE);
   return types;
}
