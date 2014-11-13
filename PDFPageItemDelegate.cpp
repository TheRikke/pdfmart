#include "PDFPageItemDelegate.h"

#include "PageList.h"

#include <poppler-qt5.h>
#include <QPainter>

PDFPageItemDelegate::PDFPageItemDelegate(QObject *parent, QVector<Poppler::Document *> documents)
   : QAbstractItemDelegate(parent)
   , documents_(documents)
{
}

void PDFPageItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   int documentNumber = index.row();
   int pageNumber = index.column();
   if(index.internalPointer())
   {
      PageEntry *pageEntry = static_cast<PageEntry*>(index.internalPointer());
      documentNumber = pageEntry->first;
      pageNumber = pageEntry->second;
   }
   Poppler::Document *document = documents_.at(documentNumber);
   if(pageNumber < document->numPages()) {
      Poppler::Page *page = document->page(pageNumber);
      QImage newPage = page->renderToImage();
      delete page;
      painter->drawImage(option.rect, newPage);
   }
}

QSize PDFPageItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   int row = index.isValid() ? index.row() : 0;
   return QSize(documents_.at(row)->page(0)->pageSize());
}
