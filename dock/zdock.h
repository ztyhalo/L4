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
void kill_w_process(PROCESS_INFORMATION proid);

class ZProcessWidget
{
// Q_OBJECT
private:
     QWidget      *         qwid_p;
     QWindow      *         qwin_p;
     PROCESS_INFORMATION    pi;
     QString         cmd;
     QString         w_name;    //窗口名字
     QString         class_n;    //类名字

public:
    ZProcessWidget(QString pro="", QString w="", QString c=""):
        cmd(pro),w_name(w),class_n(c)
    {
        qwid_p = NULL;
        qwin_p = NULL;
        memset(&pi, 0x00, sizeof(pi));
    }

    ~ZProcessWidget()
    {
        if(qwid_p != NULL)
            delete qwid_p;
        if(qwin_p != NULL)
            delete qwin_p;

        kill_w_process(pi);
    }

    int creat_process(QWidget *parent = 0);

    QWidget * get_widget(void)
    {
        return qwid_p;
    }
};




#endif // __Z_DOCK_H__
