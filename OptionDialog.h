#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include "ui_dialog.h"

class OptionDialog : public Ui::Dialog
{
public:
	explicit OptionDialog(QObject *parent = 0);

signals:

public slots:
	void MergPDFs();
};

#endif // OPTIONDIALOG_H
