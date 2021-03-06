#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include "ui_dialog.h"
#include "PageList.h"

namespace Poppler {
class Document;
}

class OptionDialog
   : public QDialog
   , public Ui::Dialog {
   Q_OBJECT
public:
   explicit OptionDialog(QObject *parent = NULL);

   PageList GetPageList() const;

   void AddInputFiles(const QStringList &fileNames);
   void SetDebug(bool);
   typedef QVector<Poppler::Document*> PopplerDocumentList;

public slots:
   void OnColumnResized(int logicalIndex, int oldSize, int newSize);

   void OnMergedViewColumnResized(int, int oldSize, int newSize);
   void OnColumnCountChanged(int, int);
   void OnScaleUp();
   void OnScaleDown();
   void OnDockedChanged(bool);
   void OnAddedPageToMergeView(QModelIndex,int,int);

 private slots:
   void on_duplexButton_clicked();

   void on_addInput_clicked();

   void on_removeInput_clicked();

   void on_writePDFButton_clicked();

   void on_appendAll_clicked();

private:
   void LoadPDFs();
   void ScaleFocusTable(float scale);
   bool eventFilter(QObject *object, QEvent *event);
   bool DebugEnabled;

   void writeNewPDF(const QString &directory, QStringList fileNames, QString saveFileName);

   QList<int> DockedSplitterSizes;

protected:
   void closeEvent(QCloseEvent *);
};

#endif // OPTIONDIALOG_H
