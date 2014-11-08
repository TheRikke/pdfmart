#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include "ui_dialog.h"
#include "PDFPagesModel.h"
#include "PDFPageItemDelegate.h"

class OptionDialog
      : public QObject
      , public Ui::Dialog
{
   Q_OBJECT
public:
   explicit OptionDialog(QObject *parent = 0);

signals:

public slots:
   void MergPDFs();
   void OnColumnResized(int logicalIndex, int oldSize, int newSize);

private slots:
   void on_actionEineAktion_triggered();

private:
   PDFPagesModel pagesModel_;
};

#endif // OPTIONDIALOG_H
