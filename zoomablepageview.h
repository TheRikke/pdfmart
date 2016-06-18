#ifndef ZOOMABLEPAGEVIEW_H
#define ZOOMABLEPAGEVIEW_H

#include <QTableView>

class ZoomablePageView : public QTableView
{
   Q_OBJECT
public:
   ZoomablePageView(QWidget*);

signals:
   void ZoomIn();
   void ZoomOut();
protected:
   void wheelEvent(QWheelEvent *);

   // QWidget interface
protected:
   void keyPressEvent(QKeyEvent *);
};

#endif // ZOOMABLEPAGEVIEW_H
