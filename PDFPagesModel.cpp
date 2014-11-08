#include "PDFPagesModel.h"

#include <poppler-qt5.h>
#include <QImage>

PDFPagesModel::PDFPagesModel(QObject * parent)
   : QAbstractTableModel(parent)
   , documents_()
   , columnCount_(0)
{
}


void PDFPagesModel::setPDF(QVector<Poppler::Document*> documents)
{
   documents_ = documents;
   columnCount_ = 0;
   foreach(Poppler::Document *document, documents_)
   {
      const int pageCount = document->numPages();
      if (pageCount > columnCount_)
      {
         columnCount_ = pageCount;
      }
   }

//   Poppler::Page* pdfPage = document->page(0); // Document starts at page 0
//   if (pdfPage == 0) {
//   // ... error message ...
//   return;
//   }
//   // Generate a QImage of the rendered page
//   QImage image = pdfPage->renderToImage(/*72.0, 72.0, -1, -1, pdfPageImage->size().width(), pdfPageImage->size().height(), x, y, width, height*/);
//   if (image.isNull()) {
//   // ... error message ...
//   return;
//   }

//   //pdfPageImage->setPixmap(QPixmap::fromImage(image.scaledToHeight(pdfPageImage->size().height())));
//   QListWidgetItem* item = new QListWidgetItem("Page1");
//   pageList->addItem(item);
//   // ... use image ...
//   // after the usage, the page must be deleted
//   delete pdfPage;
}

int PDFPagesModel::rowCount(const QModelIndex & /* parent */) const
{
   return documents_.count();
}

int PDFPagesModel::columnCount(const QModelIndex & /* parent */) const
{
   return columnCount_;
}

QModelIndex PDFPagesModel::index(int row, int column, const QModelIndex &parent) const
{
   return createIndex(row, column);
}

QModelIndex PDFPagesModel::parent(const QModelIndex &child) const
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
   return QVariant();
}

Qt::ItemFlags PDFPagesModel::flags(const QModelIndex &/*index*/) const
{
   return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

