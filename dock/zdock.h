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


#endif // __Z_DOCK_H__
