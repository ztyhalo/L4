#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QHash>
#include <QTreeWidget>
#include <QAbstractNativeEventFilter>
#include <devserch/dev_client.h>
#include <QtWidgets/QWidget>
#include <QLineEdit>
#include "dock/zdock.h"
#include <wtypes.h>
#include <QTime>
#include "zedit/ZInternetSet.h"
#include "zedit/ZGroup.h"
#include "devname/devnameset.h"


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

enum {
    NO_PROCESS = 0x00,
    REMOTE_PROCESS = 0x01,
    IE_PROCESS ,
    FTP_PROCESS,
    PARA_SKIP_PROCESS
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

class TabProcess
{
public:
    ZProcessWidget * pro;
    quint8           type;
    quint8           id;
    QString          ip;
public:
    TabProcess(ZProcessWidget * p = NULL, quint8 ty = NO_PROCESS, QString ipaddr = "")
    {
        pro = p;
        type = ty;
        id = 0;
        ip = ipaddr;
    }
    ~TabProcess()
    {
        qDebug() << "Destory TabProcess!";
//        if(pro != NULL)
//        {

//            delete pro;
//            pro = NULL;
//        }
//        type = NO_PROCESS;
    }
    void tabprocess_init(ZProcessWidget * p = NULL, quint8 ty = NO_PROCESS)
    {
        pro = p;
        type = ty;
    }
    void destory_tabprocess(void)
    {
        if(pro != NULL)
        {

            delete pro;
            pro = NULL;
        }
        type = NO_PROCESS;
    }
};

class MainWindow : public QMainWindow //,public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *  ui;

public:
    ZMenu *             sgin;
    ZMenu *             para_skip;
    ZMenu *             dev_search;
    ZMenu *             net_pro;
    ZMenu *             view;
    QToolBar *          roginBar;
    QToolBar *          skipBar;

    ZQTreeWidget *              devlist;


    QList<TabProcess>           tabpro;
//    QWidget      *              vnctab;
    QVector<DEV_DATA_INFO>      devinfolist;   //存储已经添加的设备信息列表
    quint8                      tab[4];
    QList<quint8>               tabprotype;

    QTabWidget   *              centertab;
    ZDockWidget  *              centerzdock;
    QDockWidget  *              infodock;
    ZDEV_CLIENT                 devsearch;
    QTimer       *              tim;
    int                         timsize;

public slots:
    void adddevlist(DEV_DATA_INFO  devinfo);
//    void plaint(void);
    void devlistclick(QTreeWidgetItem* item,int val);
    void rogindev(void);
    void ieconfig(void);
    void popMenu(const QPoint& point);
    void skipdev(void);
    void remotedev(void);

    void removeMtab(int index);
    void tabchange(int index);
    void setip(void);
    void set_ip_addr(TCPText text);
    void set_name_text(QString n);
    void set_dev_name(void);

    void restart_dev_search(void);
    void remote_skip_dev(void);
    void ftp_dev(void);



public slots:
    void toolBarFloat(bool topLevel);
//    void foucschange(QObject *object);
    void keyconnect();
    void changeButton();


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
    void destory_skip(void);
    void destory_ftp(void);
    void set_process_type(quint8 type);
    void add_process(ZProcessWidget * pro, quint8 type, QString ipaddr);
    DWORD  get_paraid(void);
    quint8 get_dev_type(int dev);
    int tab_is_enable(quint8 ty, QString ipaddr);
    bool  tab_pro_init(QString &ipaddr, quint8 ty);
    quint8 current_dev_type(void);
    void start_timer(int interval = 100);
    void stop_timer(void);


//     void keyPressEvent(QKeyEvent *);
//     void keyReleaseEvent(QKeyEvent *);
//     void mousePressEvent(QMouseEvent *mouse);
//     bool eventFilter(QObject *, QEvent *);
//    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
//    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE
//    {
//        if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
//        {        MSG * pMsg = reinterpret_cast<MSG *>(message);
//            if (pMsg->message == WM_LBUTTONDOWN)
//            {            //获取到系统鼠标移动，可以做像qq一样的忙碌检测
//                qDebug() << pMsg->message;
//            }
//        }
//        return false;
//    }
};


#endif // MAINWINDOW_H
