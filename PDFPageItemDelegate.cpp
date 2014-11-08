#include "PDFPageItemDelegate.h"

#include <poppler-qt5.h>
#include <QPainter>

PDFPageItemDelegate::PDFPageItemDelegate(QObject *parent, QVector<Poppler::Document *> documents)
   : QAbstractItemDelegate(parent)
   , documents_(documents)
{
}

void PDFPageItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   Poppler::Page *page = documents_.at(index.row())->page(index.column());
   QImage newPage = page->renderToImage();
   delete page;
   painter->drawImage(option.rect, newPage);
}

QSize PDFPageItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   int row = index.isValid() ? index.row() : 0;
   return QSize(documents_.at(row)->page(0)->pageSize());
}
