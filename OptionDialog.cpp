#include "OptionDialog.h"
#include "PDFPagesModel.h"
#include "PDFMergeModel.h"
#include "PDFPageItemDelegate.h"
#include "MergePDF.h"
#include <poppler-qt5.h>
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QObject>
#include <QSettings>
Q_DECLARE_METATYPE(Poppler::Document*)

OptionDialog::OptionDialog(QObject */*parent*/) :
   Ui::Dialog() {
   setupUi(this);

   QSettings settings;
   restoreGeometry(settings.value("Dialog/geometry").toByteArray());

   mergedView->installEventFilter(this);

   connect (pdfPages->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(OnColumnResized(int, int, int)));
   connect (mergedView->horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(OnMergedViewColumnResized(int, int, int)));
   connect (mergedView->horizontalHeader(), SIGNAL(sectionCountChanged(int,int)), SLOT(OnColumnCountChanged(int,int)));

   mergedView->setItemDelegate(new PDFPageItemDelegate(this));
   mergedView->setModel(new PDFMergeModel(this));

   pdfPages->setItemDelegate(new PDFPageItemDelegate(this));
   pdfPages->setModel(new PDFPagesModel(this));
}

PageList OptionDialog::GetPageList() const {
   const QAbstractItemModel* model = mergedView->model();
   int columnCount = model->columnCount();
   PageList resultList;
   for(int columnIndex = 0; columnIndex < columnCount; ++columnIndex) {
      const QSize pagePos = model->data(model->index(0, columnIndex), Qt::UserRole).toSize();
      resultList.append(PageEntry(pagePos.width(), pagePos.height()));
   }
   return resultList;
}

void OptionDialog::LoadPDFs() {
   QVector<Poppler::Document*> documents;

   for(int itemRow = 0; itemRow < InputList->count(); ++itemRow) {
      const QString inputFile = InputList->item(itemRow)->text();
      Poppler::Document* document = Poppler::Document::load(inputFile);
      if (!document || document->isLocked()) {
         qDebug("Error loading pdf: '%s'", inputFile.toStdString().c_str());
      } else {
         documents.append(document);
      }
   }

   QVariant docs;
   docs.setValue<PopplerDocumentList>(documents);
   pdfPages->model()->setProperty("SourceDocuments", docs);
   mergedView->model()->setProperty("SourceDocuments", docs);
   pdfPages->resizeRowsToContents();
   pdfPages->resizeColumnsToContents();
}

void OptionDialog::OnColumnResized(int /*logicalIndex*/, int /*oldSize*/, int newSize) {
   const int columnCount = pdfPages->model()->columnCount();
   const int rowCount = pdfPages->model()->rowCount();

   QModelIndex firstRowModelIndex = pdfPages->rootIndex();
   QSize pageSize = pdfPages->itemDelegate()->sizeHint(QStyleOptionViewItem(), firstRowModelIndex);
   int newHeight = (newSize * pageSize.height()) / pageSize.width();
   for(int columnIndex = 0; columnIndex < columnCount; ++columnIndex) {
      pdfPages->setColumnWidth(columnIndex, newSize);
   }

   for(int rowIndex = 0; rowIndex < rowCount; ++rowIndex) {
      pdfPages->setRowHeight(rowIndex, newHeight);
   }
}

void OptionDialog::OnMergedViewColumnResized(int /*logicalIndex*/, int /*oldSize*/, int newSize) {
   const int columnCount = mergedView->model()->columnCount();
   const int rowCount = mergedView->model()->rowCount();

   QModelIndex firstRowModelIndex = mergedView->rootIndex();
   QSize pageSize = mergedView->itemDelegate()->sizeHint(QStyleOptionViewItem(), firstRowModelIndex);
   int newHeight = (newSize * pageSize.height()) / pageSize.width();
   for(int columnIndex = 0; columnIndex < columnCount; ++columnIndex) {
      mergedView->setColumnWidth(columnIndex, newSize);
   }

   for(int rowIndex = 0; rowIndex < rowCount; ++rowIndex) {
      mergedView->setRowHeight(rowIndex, newHeight);
   }
}

void OptionDialog::on_duplexButton_clicked() {
   const int maxPageCount = pdfPages->model()->columnCount();

   PageList pageList;
   for (int currentPage = 0 ; currentPage < maxPageCount ; ++currentPage) {
      pageList.append(PageEntry(0, currentPage));
      pageList.append(PageEntry(1, (maxPageCount - 1) - currentPage));
   }

   QAbstractItemModel *model = mergedView->model();
   model->removeColumns(0, model->columnCount());
   model->insertColumns(0, pageList.count());

   int column = 0;
   foreach(PageEntry entry, pageList) {
      QSize pagePos(entry.first, entry.second);
      model->setData(model->index(0, column), pagePos, Qt::UserRole);
      column++;
   }
   mergedView->resizeRowsToContents();
   mergedView->resizeColumnsToContents();
}

void OptionDialog::OnColumnCountChanged(int oldSize, int newSize) {
   if(oldSize > 0 && newSize > 0) {
      int columnSize = mergedView->columnWidth(0);
      OnMergedViewColumnResized(0, columnSize, columnSize);
   }
}

void OptionDialog::on_addInput_clicked() {
   QFileDialog fileDialog(this, "Choose PDF files");
   fileDialog.setDefaultSuffix(".pdf");
   fileDialog.setNameFilter("PDFs (*.pdf)");
   fileDialog.setFileMode(QFileDialog::ExistingFiles);
   QSettings settings;
   fileDialog.setDirectory(settings.value("Dialog/LastDirectory").toString());
   QStringList fileNames;
   if (fileDialog.exec())
      fileNames = fileDialog.selectedFiles();
   AddInputFiles(fileNames);
}

void OptionDialog::AddInputFiles(const QStringList &fileNames) {
   InputList->addItems(fileNames);
   const QAbstractItemModel* model = mergedView->model();

   QVariant docs = model->property("SourceDocuments");
   PopplerDocumentList oldDocuments = docs.value<PopplerDocumentList>();
   LoadPDFs();
   qDeleteAll(oldDocuments);
}

void OptionDialog::on_removeInput_clicked() {
   QList<QListWidgetItem *> selectedFiles = InputList->selectedItems();
   foreach(QListWidgetItem *item, selectedFiles) {
      delete item;
   }
   LoadPDFs();
}

bool OptionDialog::eventFilter(QObject* object, QEvent* event) {
   if (event->type()==QEvent::KeyPress) {
      QKeyEvent* pKeyEvent=static_cast<QKeyEvent*>(event);
      if (pKeyEvent->key() == Qt::Key_Delete) {
         if (mergedView->hasFocus()) {
            QModelIndexList selectedPages = mergedView->selectionModel()->selectedIndexes();
            QAbstractItemModel* model = mergedView->model();
            foreach(QModelIndex index, selectedPages) {
               model->removeColumn(index.column());
            }

            qDebug() << "Event filter: Focus yes, Delete key pressed";
         } else {
            qDebug() << "Event filter: Focus NO, Delete key pressed";
         }
         return true;
      }
      qDebug() << "Event filter: other key pressed";
   }
   return QWidget::eventFilter(object, event);
}

void OptionDialog::closeEvent(QCloseEvent *event)
{
   QSettings settings;
   settings.setValue("Dialog/geometry", saveGeometry());
   QDialog::closeEvent(event);
}

void OptionDialog::on_writePDFButton_clicked()
{
   QFileDialog fileDialog(this,
                          "Save PDF",
                          QSettings().value("Dialog/LastOutputDirectory").toString(),
                          "PDFs (*.pdf)");
   fileDialog.setDefaultSuffix(".pdf");
   fileDialog.setFileMode(QFileDialog::AnyFile);
   fileDialog.setAcceptMode(QFileDialog::AcceptSave);

   if (fileDialog.exec()) {
      QString saveFileName = fileDialog.selectedFiles().first();

      if(!fileDialog.selectedFiles().isEmpty() && QFile::exists(saveFileName)) {
         if(QMessageBox::question(this,
                               tr("Warning"),
                               tr("File already exists. Overwrite?"),
                               QMessageBox::Ok | QMessageBox::Cancel,
                               QMessageBox::Ok) == QMessageBox::Cancel)
            return;
      }

      MergePDF merger;
      QStringList fileNames;
      int fileNamesCount = InputList->count();
      for(int i = 0; i < fileNamesCount; i++)
      {
         fileNames << InputList->item(i)->text();
      }
      merger.Merge(fileNames, GetPageList(), saveFileName);
   }
}
