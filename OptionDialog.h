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

   typedef QVector<Poppler::Document*> PopplerDocumentList;
   bool eventFilter(QObject *object, QEvent *event);
 signals:

 public slots:
   void OnColumnResized(int logicalIndex, int oldSize, int newSize);

   void OnMergedViewColumnResized(int, int oldSize, int newSize);
   void OnColumnCountChanged(int, int);

 private slots:
   void on_actionEineAktion_triggered();

   void on_duplexButton_clicked();

   void on_addInput_clicked();

   void on_removeInput_clicked();

 private:
   void LoadPDFs();
};

#endif // OPTIONDIALOG_H
