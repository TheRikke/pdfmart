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
   , pageList_()
{
}


void PDFMergeModel::setPDF(QVector<Poppler::Document*> documents, PageList pageList)
{
   documents_ = documents;
   pageList_ = pageList;
}

int PDFMergeModel::rowCount(const QModelIndex & /* parent */) const
{
   return 1;
}

int PDFMergeModel::columnCount(const QModelIndex & /* parent */) const
{
   return pageList_.count();
}

QModelIndex PDFMergeModel::index(int row, int column, const QModelIndex &parent) const
{
   qDebug() << "createIndex for" << row << column << parent;
//   return createIndex(pageList_.at(column).first, pageList_.at(column).second);
   return createIndex(row, column, (void*)&pageList_.at(column));
}

QModelIndex PDFMergeModel::parent(const QModelIndex &child) const
{
   qDebug() << "createIndex for child" << child;
   return QModelIndex();
}

QVariant PDFMergeModel::data(const QModelIndex &index, int role) const
{
   QVariant result;
   switch(role)
   {
   case Qt::DisplayRole:
      result = QString::number(index.column());
      break;
   case Qt::UserRole:
      QSize pagePos(pageList_.at(index.column()).first, pageList_.at(index.column()).second);
      result.setValue(pagePos);
      break;
   }
   return result;
}

QVariant PDFMergeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   return QVariant();
}

Qt::ItemFlags PDFMergeModel::flags(const QModelIndex &/*index*/) const
{
   return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEditable;
}

//bool PDFMergeModel::insertColumn(int acolumn, const QModelIndex &aparent)
//{
////   beginInsertColumns(aparent, acolumn, acolumn);
////   endInsertColumns();
//   return true;
//}

bool PDFMergeModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
   QStringList formats = data->formats();
   QByteArray encodedData = data->data(formats[0]);
   QDataStream stream(&encodedData, QIODevice::ReadOnly);
   PageList pageList;
   while(!stream.atEnd())
   {
      long long origin_row, origin_column;
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
   foreach(PageEntry entry, pageList)
   {
      pageList_.insert(beginColumn++, entry);
   }
   emit layoutChanged();
//   qDebug() << "row: " << row << " column:" << column << "origin" << origin_row << origin_column << parent;
   return true;
}

QStringList PDFMergeModel::mimeTypes() const
{
   QStringList types(MIME_TYPE);
   return types;
}



