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


 // ntdll!NtQueryInformationProcess (NT specific!)
 //
 // The function copies the process information of the
 // specified type into a buffer
 //
 // NTSYSAPI
 // NTSTATUS
 // NTAPI
 // NtQueryInformationProcess(
 //    IN HANDLE ProcessHandle,              // handle to process
 //    IN PROCESSINFOCLASS InformationClass, // information type
 //    OUT PVOID ProcessInformation,         // pointer to buffer
 //    IN ULONG ProcessInformationLength,    // buffer size in bytes
 //    OUT PULONG ReturnLength OPTIONAL      // pointer to a 32-bit
 //                                          // variable that receives
 //                                          // the number of bytes
 //                                          // written to the buffer
 // );
 typedef LONG (__stdcall *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);

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



    PROCESS_INFORMATION wpi;
    PROCESS_INFORMATION vncpi;

public:
    QMenu *sgin;
    QMenu *para_skip;
    QMenu *dev_search;
    QMenu *net_pro;
    QMenu * view;

    ZQTreeWidget * devlist;


    QAction * login;
    QAction * exit;

    QAction * skip;

    QAction * search_all;

    QAction * pro_now;
    QWidget * proshow;
    QWindow *m_window;

//    QDockWidget  * centerdock;
    QTabWidget   * centertab;

   ZDockWidget  * centerzdock;
//    QDockWidget  * centerzdock;
    QWidget      *  rogtab;
    QWidget      *  vnctab;

    QDockWidget                 * infodock;
    DEV_SEARCH                    devsearch;

    QVector<DEV_DATA_INFO>      devinfolist;   //存储已经添加的设备信息列表
    QTimer               * tim;
public slots:
    void adddevlist(DEV_DATA_INFO  devinfo);
    void plaint(void);
    void devlistclick(QTreeWidgetItem* item,int val);
    void rogindev(void);
    void popMenu(const QPoint& point);
    void skipdev(void);
    void remotedev(void);
    void tightremotedev(void);
    void removeMtab(int index);

public slots:
    void toolBarFloat(bool topLevel);
    void foucschange(QObject *object);
    void keyconnect();



public:
    BOOL dev_is_have(DEV_DATA_INFO dev);
    void add_devinfo_list(DEV_DATA_INFO dev);
    void add_sgin_action(void);
    void add_para_action(void);
    void add_search_action(void);
    void add_pro_action(void);
    void createDockWindows(void);
    void destory_ie(void);
    DWORD  get_paraid(void);
    quint8 get_dev_type(int dev);

     void keyPressEvent(QKeyEvent *);
     void keyReleaseEvent(QKeyEvent *);
};

#define Z_ADD_ACTION(act, menu, info, addinfo) {act = new QAction(tr(info), this);\
                                            act->setStatusTip(tr(addinfo));\
                                            menu->addAction(act);}

#endif // MAINWINDOW_H
