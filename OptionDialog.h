#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include "ui_dialog.h"
#include "PageList.h"

class OptionDialog
      : public QDialog
      , public Ui::Dialog
{
   Q_OBJECT
public:
   explicit OptionDialog(QObject *parent = NULL);

   PageList GetPageList() const;

signals:

public slots:
   void MergPDFs();
   void OnColumnResized(int logicalIndex, int oldSize, int newSize);

   void OnMergedViewColumnResized(int, int oldSize, int newSize);
   void OnColumnCountChanged(int, int);

private slots:
   void on_actionEineAktion_triggered();

   void on_duplexButton_clicked();

private:
};

#endif // OPTIONDIALOG_H
