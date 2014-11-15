#include "PDFPageItemDelegate.h"

#include "PageList.h"

#include <poppler-qt5.h>
#include <QPainter>

Q_DECLARE_METATYPE(Poppler::Document*)

PDFPageItemDelegate::PDFPageItemDelegate(QObject *parent)
   : QAbstractItemDelegate(parent)
{
}

void PDFPageItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   int documentNumber = index.row();
   int pageNumber = index.column();
   const QAbstractItemModel *itemModel = index.model();
   QVariant data = itemModel->data(index, Qt::UserRole);
   if(data.isValid())
   {
      QSize position(data.toSize());
      documentNumber = position.height();
      pageNumber = position.width();
   }
   QVector<Poppler::Document*> documents = itemModel->property("SourceDocuments").value< QVector<Poppler::Document*> >();

   Poppler::Document *document = documents.at(documentNumber);
   if(pageNumber < document->numPages()) {
      Poppler::Page *page = document->page(pageNumber);
      QImage newPage = page->renderToImage();
      delete page;
      painter->drawImage(option.rect, newPage);
   }
}

QSize PDFPageItemDelegate::sizeHint(const QStyleOptionViewItem &/*option*/, const QModelIndex &index) const
{
   QSize result;
   if(index.isValid())
   {
      QVector<Poppler::Document*> documents = index.model()->property("SourceDocuments").value< QVector<Poppler::Document*> >();
      result = documents.at(index.row())->page(0)->pageSize();
   }
   return result;
}
