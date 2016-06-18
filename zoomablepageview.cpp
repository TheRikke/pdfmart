#include "zoomablepageview.h"

#include <QWheelEvent>

ZoomablePageView::ZoomablePageView(QWidget *parent)
   : QTableView(parent)
{
}

void ZoomablePageView::wheelEvent(QWheelEvent *event)
{
   if(event->modifiers() == Qt::CTRL) {
      if(event->angleDelta().y() > 0) {
         emit ZoomIn();
      } else {
         emit ZoomOut();
      }
      event->accept();
   } else {
      QTableView::wheelEvent(event);
   }
}


void ZoomablePageView::keyPressEvent(QKeyEvent *event)
{
   if(event->modifiers().testFlag(Qt::ControlModifier)) {
      if(event->key() == Qt::Key_Plus) {
         emit ZoomIn();
         event->accept();
      } else if(event->key() == Qt::Key_Minus) {
         emit ZoomOut();
         event->accept();
      }
   }
   if(!event->isAccepted()) {
      QTableView::keyPressEvent(event);
   }
}
