#include "OptionDialog.h"
#include <poppler-qt5.h>
#include <QObject>
#include <QListWidgetItem>

OptionDialog::OptionDialog(QObject */*parent*/) :
	Ui::Dialog()
{

//   QObject::connect(actionEineAktion, SIGNAL(triggered()), this, SLOT(MergPDFs()));
}

void OptionDialog::MergPDFs()
{
   connect (pdfPages->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(OnColumnResized(int, int, int)));
   QVector<Poppler::Document*> documents;

   for(int itemRow = 0; itemRow < InputList->count(); ++itemRow)
   {
      const QString inputFile = InputList->item(itemRow)->text();
      Poppler::Document* document = Poppler::Document::load(inputFile);
      if (!document || document->isLocked()) {
         qDebug("Error loading pdf: '%s'", inputFile.toStdString().c_str());
      }
      else
      {
         documents.append(document);
      }
   }
   pagesModel_.setPDF(documents);
   pdfPages->setItemDelegate(new PDFPageItemDelegate(this, documents));
   pdfPages->setModel(&pagesModel_);
   pdfPages->resizeRowsToContents();
   pdfPages->resizeColumnsToContents();
}

void OptionDialog::OnColumnResized(int /*logicalIndex*/, int oldSize, int newSize)
{
   const int columnCount = pdfPages->model()->columnCount();
   const int rowCount = pdfPages->model()->rowCount();

   QModelIndex firstRowModelIndex = pdfPages->rootIndex();
   QSize pageSize = pdfPages->itemDelegate()->sizeHint(QStyleOptionViewItem(), firstRowModelIndex);
   int newHeight = (newSize * pageSize.height()) / pageSize.width();
   for(int columnIndex = 0; columnIndex < columnCount; ++columnIndex)
   {
      pdfPages->setColumnWidth(columnIndex, newSize);
   }

   for(int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      pdfPages->setRowHeight(rowIndex, newHeight);
   }
}

void OptionDialog::on_actionEineAktion_triggered()
{
    MergPDFs();
}
