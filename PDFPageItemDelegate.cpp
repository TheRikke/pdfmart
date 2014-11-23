#include "PDFPageItemDelegate.h"

#include "PageList.h"

#include <poppler-qt5.h>
#include <QPainter>
#include <QCache>

Q_DECLARE_METATYPE(Poppler::Document*)

namespace {
const int MAX_CACHED_IMAGES = 500;
}

PDFPageItemDelegate::PDFPageItemDelegate(QObject *parent)
   : QAbstractItemDelegate(parent) {
}

void PDFPageItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
   static QCache< ImageKey, QImage > imageCache(MAX_CACHED_IMAGES);
   int documentNumber = index.row();
   int pageNumber = index.column();
   const QAbstractItemModel *itemModel = index.model();
   QVariant data = itemModel->data(index, Qt::UserRole);
   if(data.isValid()) {
      QSize position(data.toSize());
      documentNumber = position.height();
      pageNumber = position.width();
   }
   QVector<Poppler::Document*> documents = itemModel->property("SourceDocuments").value< QVector<Poppler::Document*> >();

   qDebug("Paint doc %d page %d into %d, %d (row, column)", documentNumber, pageNumber, index.row(), index.column());
   Poppler::Document *document = documents.at(documentNumber);

   if(pageNumber < document->numPages()) {
      ImageKey key(document, pageNumber);
      QImage *newPage = imageCache.object(key);
      if(!newPage) {
         qDebug("Cache miss for page %d in doc %p", pageNumber, document);
         Poppler::Page *page = document->page(pageNumber);
         newPage = new QImage(page->renderToImage());
         delete page;

         imageCache.insert(key, newPage);
      }
      painter->drawImage(option.rect, *newPage);
   }
}

QSize PDFPageItemDelegate::sizeHint(const QStyleOptionViewItem &/*option*/, const QModelIndex &index) const {
   QSize result;
   if(index.isValid()) {
      QVector<Poppler::Document*> documents = index.model()->property("SourceDocuments").value< QVector<Poppler::Document*> >();
      result = documents.at(index.row())->page(0)->pageSize();
   }
   return result;
}
