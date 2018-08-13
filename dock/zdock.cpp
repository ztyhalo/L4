
#include "zdock.h"


ZDockWidget::ZDockWidget(QMainWindow * win, const QString &title, QWidget *parent, Qt::WindowFlags flags):
    QDockWidget(title,parent,flags)
{
    wins_p = win;
}
ZDockWidget::ZDockWidget(QMainWindow * win, QWidget *parent, Qt::WindowFlags flags ):QDockWidget(parent, flags)
{
    wins_p = win;
}

void ZDockWidget::mouseDoubleClickEvent (QMouseEvent * ev)
{
    qDebug("z dock mouse double 0x%x!", ev->button());
    if(ev->buttons() ==  Qt::RightButton)
    {
        qDebug("z dock double!");
    }
}

bool ZDockWidget::event(QEvent *event)
{
    if(event->type() == QEvent::NonClientAreaMouseButtonDblClick)
    {
        qDebug("z event process!");
        if(wins_p != NULL)
        {
            if(isFloating())
            {
                qDebug("dock float!");
//                wins_p->addDockWidget(Qt::AllDockWidgetAreas, this);
                setFloating(FALSE);
//                wins_p->setCentralWidget(this);
            }

        }
//        return TRUE;

    }
    return QDockWidget::event(event);
}
