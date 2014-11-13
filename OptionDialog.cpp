#include "OptionDialog.h"
#include <poppler-qt5.h>
#include <QObject>
#include <QListWidgetItem>
#include "PDFPagesModel.h"
#include "PDFMergeModel.h"
#include "PDFPageItemDelegate.h"
#include <QDebug>

OptionDialog::OptionDialog(QObject *parent) :
	Ui::Dialog()
{
   setupUi(this);
   connect (pdfPages->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(OnColumnResized(int, int, int)));
   connect (mergedView->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(OnMergedViewColumnResized(int, int, int)));
   connect (mergedView->horizontalHeader(), SIGNAL(sectionCountChanged(int,int)), SLOT(OnColumnCountChanged(int,int)));
}

PageList OptionDialog::GetPageList() const
{
   const QAbstractItemModel* model = mergedView->model();
   int columnCount = model->columnCount();
   PageList resultList;
   for(int columnIndex = 0;columnIndex < columnCount;++columnIndex)
   {
      const QSize pagePos = model->data(model->index(0, columnIndex), Qt::UserRole).toSize();
      resultList.append(PageEntry(pagePos.width(), pagePos.height()));
   }
   return resultList;
}

void OptionDialog::MergPDFs()
{
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
   PDFPagesModel* pagesModel = new PDFPagesModel(this);
   pagesModel->setPDFs(documents);
   pdfPages->setItemDelegate(new PDFPageItemDelegate(this, documents));
   pdfPages->setModel(pagesModel);
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

void OptionDialog::OnMergedViewColumnResized(int /*logicalIndex*/, int oldSize, int newSize)
{
   const int columnCount = mergedView->model()->columnCount();
   const int rowCount = mergedView->model()->rowCount();

   QModelIndex firstRowModelIndex = mergedView->rootIndex();
   QSize pageSize = mergedView->itemDelegate()->sizeHint(QStyleOptionViewItem(), firstRowModelIndex);
   int newHeight = (newSize * pageSize.height()) / pageSize.width();
   for(int columnIndex = 0; columnIndex < columnCount; ++columnIndex)
   {
      mergedView->setColumnWidth(columnIndex, newSize);
   }

   for(int rowIndex = 0; rowIndex < rowCount; ++rowIndex)
   {
      mergedView->setRowHeight(rowIndex, newHeight);
   }
}

void OptionDialog::on_actionEineAktion_triggered()
{
//   MergPDFs();
}

void OptionDialog::on_duplexButton_clicked()
{
//   const int documentCount = pdfPages->model()->rowCount();
   PDFPagesModel* pagesModel = qobject_cast<PDFPagesModel*>(pdfPages->model());
   const int maxPageCount = pdfPages->model()->columnCount();

   PageList pageList;
   for (int currentPage = 0 ; currentPage < maxPageCount ; ++currentPage) {
      pageList.append(PageEntry(0, currentPage));
      pageList.append(PageEntry(1, (maxPageCount - 1) - currentPage));
   }

   PDFMergeModel* mergeModel = new PDFMergeModel(this);
   mergeModel->setPDF(pagesModel->GetPDFs(), pageList);
   mergedView->setItemDelegate(new PDFPageItemDelegate(this, pagesModel->GetPDFs()));
   mergedView->setModel(mergeModel);
   mergedView->resizeRowsToContents();
   mergedView->resizeColumnsToContents();

   //Poppler::Document* document = Poppler::Document::loadFromData()
}

void OptionDialog::OnColumnCountChanged(int oldSize, int newSize)
{
   if(oldSize > 0 && newSize > 0) {
      int columnSize = mergedView->columnWidth(0);
      OnMergedViewColumnResized(0, columnSize, columnSize);
   }
}
