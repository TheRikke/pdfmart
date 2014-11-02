#ifndef MERGEPDF_H
#define MERGEPDF_H

#include <QObject>
#include <QProcess>

class MergePDF : public QObject
{
	Q_OBJECT
public:
	explicit MergePDF(QObject *parent = 0);

signals:

public slots:
	void Merge(QString, QString, QString, bool);

private:
	QProcess MergeTool;
	QStringList GetOrder(int numberOfPages);
	int GetNumberOfPages(QString pdfFile);
};

#endif // MERGEPDF_H
