#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QHash>
#include <QTreeWidget>
#include <devserch/dev_search.h>
#include <QtWidgets/QWidget>
#include <QLineEdit>
#include "dock/zdock.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QDialog  * dialg;
    QLineEdit  * qwiget;
public:
    QMenu *sgin;
    QMenu *para_skip;
    QMenu *dev_search;
    QMenu *net_pro;
    QMenu * view;

    QTreeWidget * devlist;


    QAction * login;
    QAction * exit;

    QAction * skip;

    QAction * search_all;

    QAction * pro_now;
    QWidget * proshow;


//    QDockWidget  * centerdock;
    QTabWidget   * centertab;

    ZDockWidget  * centerzdock;
    QWidget      *  rogtab;

    QDockWidget                 * infodock;
    DEV_SEARCH                    devsearch;

    QVector<DEV_DATA_INFO>      devinfolist;   //存储已经添加的设备信息列表
public slots:
    void adddevlist(DEV_DATA_INFO  devinfo);
    void plaint(void);
    void devlistclick(QTreeWidgetItem* item,int val);
    void rogindev(void);
    void popMenu(const QPoint& point);
    void skipdev(void);
    void remotedev(void);
    void tightremotedev(void);

public slots:
    void toolBarFloat(bool topLevel);

public:
    BOOL dev_is_have(DEV_DATA_INFO dev);
    void add_devinfo_list(DEV_DATA_INFO dev);
    void add_sgin_action(void);
    void add_para_action(void);
    void add_search_action(void);
    void add_pro_action(void);
    void createDockWindows(void);
};

#define Z_ADD_ACTION(act, menu, info, addinfo) {act = new QAction(tr(info), this);\
                                            act->setStatusTip(tr(addinfo));\
                                            menu->addAction(act);}

#endif // MAINWINDOW_H
