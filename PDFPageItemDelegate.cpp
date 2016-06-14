#include "PDFPageItemDelegate.h"

#include "PageList.h"

#include <poppler-qt5.h>
#include <QPainter>
#include <QCache>
#include <QScreen>
#include <QApplication>

#include <QDebug>

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
      documentNumber = position.width();
      pageNumber = position.height();
   }
   QVector<Poppler::Document*> documents = itemModel->property("SourceDocuments").value< QVector<Poppler::Document*> >();

   qDebug("Paint doc %d page %d into %d, %d (row, column)", documentNumber, pageNumber, index.row(), index.column());
   if(documentNumber < documents.size()) {
      Poppler::Document *document = documents.at(documentNumber);

      if(pageNumber < document->numPages()) {
         ImageKey key(document, pageNumber);
         QImage *newPage = imageCache.object(key);
         if(!newPage) {
            QScreen* screen = QApplication::screens().first();
            Poppler::Page *page = document->page(pageNumber);
            newPage = new QImage(page->renderToImage(static_cast<const int>(screen->logicalDotsPerInchX()), static_cast<const int>(screen->logicalDotsPerInchY())));
            qDebug("Cache miss for page %d in doc %p (DPI %f, W: %d, H: %d)", pageNumber, document, screen->logicalDotsPerInch(), page->pageSize().width(),  page->pageSize().height());
            delete page;

            imageCache.insert(key, newPage);
         }
         painter->drawImage(option.rect, *newPage);
      }
   }
}

QSize PDFPageItemDelegate::sizeHint(const QStyleOptionViewItem &/*option*/, const QModelIndex &index) const {
   QSize result;
   if(index.isValid()) {
      result = index.model()->property("SizeHint").value< QSize>();
      qDebug() << "Size hint " << result;
   } else {
      qDebug() << "Invalid index";
   }
   return result;
}
