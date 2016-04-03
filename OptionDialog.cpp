#include "OptionDialog.h"
#include "OcrHandler.h"
#include "PDFPagesModel.h"
#include "PDFMergeModel.h"
#include "PDFPageItemDelegate.h"
#include "MergePDF.h"
#include "PopplerTools.h"
#include "PMSettings.h"

#include <poppler-qt5.h>
#include <QDebug>
#include <QFileDialog>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QObject>
#include <QSettings>
#include <QTemporaryDir>

Q_DECLARE_METATYPE(Poppler::Document*)

OptionDialog::OptionDialog(QObject */*parent*/)
   : Ui::Dialog()
   , DebugEnabled(false)
{
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
   QVector<Poppler::Document*> documents = pdfPages->model()->property("SourceDocuments").value< QVector<Poppler::Document*> >();
   QSize pageSize = documents.at(0)->page(0)->pageSize();

   qDebug() << "OnColumnResized size " << pageSize;
   int newHeight =  (newSize * pageSize.height()) / pageSize.width();
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
   if (fileDialog.exec()) {
      fileNames = fileDialog.selectedFiles();
      settings.setValue("Dialog/LastDirectory", fileDialog.directory().absolutePath());
   }
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

void OptionDialog::SetDebug(const bool debug)
{
   DebugEnabled = debug;
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
            for (int indexOfSelectedIndex = selectedPages.count() - 1; indexOfSelectedIndex >= 0; indexOfSelectedIndex--)
            {
                model->removeColumn(selectedPages.at(indexOfSelectedIndex).column());
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

MetaDataList GetMetaData(QTableWidget* table)
{
   MetaDataList meta_data_list ;
   for (int row_index = 0; row_index < table->rowCount(); ++row_index) {
      qDebug() << table->item(row_index, 0)->text();
      QTableWidgetItem* item = table->item(row_index, 1);
      if(item && !item->text().isEmpty()) {
         meta_data_list[table->item(row_index, 0)->text()] = item->text();
      }
   }
   return meta_data_list;
}

void OptionDialog::on_writePDFButton_clicked()
{
   QSettings settings;
   QFileDialog fileDialog(this,
                          "Save PDF",
                          settings.value("Dialog/LastOutputDirectory").toString(),
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
      settings.setValue("Dialog/LastOutputDirectory", fileDialog.directory().absolutePath());
      QStringList fileNames;
      const int fileNamesCount = InputList->count();
      for(int i = 0; i < fileNamesCount; i++)
      {
         fileNames << InputList->item(i)->text();
      }

      //separate pages
      PopplerTools tool;
      QStringList files = tool.WriteToSeparatePages(fileNames, GetPageList(), fileDialog.directory().absolutePath());

      //ocr
      OcrHandler ocrHandler;
      QTemporaryDir tempDir;
      tempDir.setAutoRemove(!PMSettings::IsDebugEnabled());
      QStringList separatedPDFFiles;
      if (tempDir.isValid()) {
         qDebug() << tempDir.path();
         foreach(const QString &file, files) {
            separatedPDFFiles.append(ocrHandler.AddTextToPDF(file, tempDir.path() + QDir::separator() + QFileInfo(file).baseName()));
         }
      }

      //unite the pages
      tool.MergePDF(separatedPDFFiles, saveFileName, GetMetaData(tagList));
      qDebug() << files;
   }
}

void OptionDialog::on_appendAll_clicked()
{
   QVector<Poppler::Document*> documents = pdfPages->model()->property("SourceDocuments").value< QVector<Poppler::Document*> >();
   for(int docIndex = 0; docIndex < documents.size(); ++docIndex) {
      for(int pageIndex = 0; pageIndex < documents.at(docIndex)->numPages(); ++pageIndex) {
         QSize pagePos(docIndex, pageIndex);
         mergedView->model()->setData(mergedView->model()->index(0, pageIndex), pagePos, Qt::UserRole);
      }
   }
   mergedView->resizeRowsToContents();
   mergedView->resizeColumnsToContents();
}
