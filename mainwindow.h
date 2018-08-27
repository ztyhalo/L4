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
#include <wtypes.h>
#include <QTime>

namespace Ui {
class MainWindow;
}


#define ProcessBasicInformation 0

 typedef struct
 {
     DWORD ExitStatus;
     DWORD PebBaseAddress;
     DWORD AffinityMask;
     DWORD BasePriority;
     ULONG UniqueProcessId;
     ULONG InheritedFromUniqueProcessId;
 }   PROCESS_BASIC_INFORMATION;


 typedef LONG (__stdcall *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);


enum {
    LX_DEV = 0x00,
    SWITCH_DEV = 0x11,
    DEV_MAX_TYPE = 0xff
};

class TabType
{
public:
    int    index;
    quint8 type;
public:
    TabType(int dex = 0, quint8 t = 0)
    {
        index = dex;
        type =t;
    }

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *  ui;
    QDialog  *        dialg;
    QLineEdit  *      qwiget;


public:
    ZMenu *             sgin;
    ZMenu *             para_skip;
    ZMenu *             dev_search;
    ZMenu *             net_pro;
    ZMenu *             view;

    ZQTreeWidget *              devlist;
    ZProcessWidget *            vncpro;
    ZProcessWidget *            webpro;
    QVector<DEV_DATA_INFO>      devinfolist;   //存储已经添加的设备信息列表
    quint8                      tab[2];




    QWidget * proshow;
    QWindow *m_window;

    QTabWidget   * centertab;

   ZDockWidget  * centerzdock;



    QDockWidget                 * infodock;
    DEV_SEARCH                    devsearch;


    QTimer               * tim;
public slots:
    void adddevlist(DEV_DATA_INFO  devinfo);
//    void plaint(void);
    void devlistclick(QTreeWidgetItem* item,int val);
    void rogindev(void);
    void popMenu(const QPoint& point);
    void skipdev(void);
    void remotedev(void);

    void removeMtab(int index);

public slots:
    void toolBarFloat(bool topLevel);
    void foucschange(QObject *object);
    void vnckeyconnect();



public:
    BOOL dev_is_have(DEV_DATA_INFO dev);
    void add_devinfo_list(DEV_DATA_INFO dev);
    void add_sgin_action(void);
    void add_para_action(void);
    void add_search_action(void);
    void add_pro_action(void);
    void createDockWindows(void);
    void destory_ie(void);
    void destory_vnc(void);
    DWORD  get_paraid(void);
    quint8 get_dev_type(int dev);

     void keyPressEvent(QKeyEvent *);
     void keyReleaseEvent(QKeyEvent *);
//     void mousePressEvent(QMouseEvent *mouse);
     bool eventFilter(QObject *, QEvent *);
};


#endif // MAINWINDOW_H
