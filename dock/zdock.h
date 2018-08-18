#ifndef __Z_DOCK_H__
#define __Z_DOCK_H__


#include <QtWidgets>
#include <QDockWidget>
#include <QEvent>
#include <QMainWindow>



class ZDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    QMainWindow * wins_p;
public:
    explicit ZDockWidget(QMainWindow * win, const QString &title, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    explicit ZDockWidget(QMainWindow * win = 0, QWidget *parent = 0, Qt::WindowFlags flags = 0);

    bool event(QEvent *event);
protected:
     void mouseDoubleClickEvent ( QMouseEvent * ev);

};


class DoublieKeyWidget : public QDockWidget
{
    Q_OBJECT

protected:
     void mouseDoubleClickEvent ( QMouseEvent * ev);

};

class ZQTreeWidget: public QTreeWidget
{
     Q_OBJECT
public:
    ZQTreeWidget(QWidget *parent = 0):QTreeWidget(parent)
    {;
    }

    QModelIndex zindexFromItem(QTreeWidgetItem *item, int column = 0)
    {
         return indexFromItem( item,  column );
    }


    int get_current_row(void)
    {
        if(currentItem() != NULL)
        {
            return indexFromItem(currentItem()).row();
        }
        else
            return -1;
    }
};

class ZQTabWidget: public QTabWidget
{
    Q_OBJECT
public:
   ZQTabWidget(QWidget *parent = 0):QTabWidget(parent)
   {;
   }

   void mousePressEvent(QMouseEvent *)
   {
       qDebug("mouse press !!!");
   }
};


#endif // __Z_DOCK_H__
