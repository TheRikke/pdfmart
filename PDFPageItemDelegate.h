#ifndef PDFPAGEITEMDELEGATE_H
#define PDFPAGEITEMDELEGATE_H

#include <QAbstractItemDelegate>

namespace Poppler {
   class Document;
}

class PDFPageItemDelegate : public QAbstractItemDelegate
{
public:
   PDFPageItemDelegate(QObject *parent, QVector<Poppler::Document*> documents);

   void paint(QPainter *painter, const QStyleOptionViewItem &option,
              const QModelIndex &index) const;

   QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
   QVector<Poppler::Document*> documents_;
};

#endif // PDFPAGEITEMDELEGATE_H
