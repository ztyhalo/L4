﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QtGui>
#include <QDockWidget>
#include <QWebView>
#include <QLabel>
#include <QBoxLayout>

QRect  GetScreenInfo(void)
{
   QDesktopWidget* desktopWidget = QApplication::desktop();
       QRect deskRect = desktopWidget->availableGeometry();
//   QRect screenRect = desktopWidget->screenGeometry();

//   return screenRect;
   return deskRect;
}

void attachWindowThreadInput(WId remoteWindowId)
{
    DWORD currentThreadId = GetCurrentThreadId();
    DWORD remoteThreadId = GetWindowThreadProcessId((HWND)remoteWindowId, NULL);
    qDebug() << __FUNCTION__ << "win" << remoteWindowId << "thr" << remoteThreadId;

    if (currentThreadId != remoteThreadId) {
        int err = AttachThreadInput(remoteThreadId, currentThreadId, true);
        if (!err)
        {
            qErrnoWarning("AttachThreadInput");
            qDebug() << "err " << err;
        }
        else
        {
            qDebug("set successful!");
        }
    }
    else
    {
        qDebug("no set!");
    }
}



void zattachWindowThreadInput(DWORD remoteThreadId)
{
    DWORD currentThreadId = GetCurrentThreadId();

    if (currentThreadId != remoteThreadId) {

        int err = AttachThreadInput(currentThreadId, remoteThreadId, true);

        if (!err)
            qErrnoWarning("AttachThreadInput");
        else
        {
            qDebug("set successful!");
        }
    }
    else
    {
        qDebug("no set!");
    }
}


void reattachWindowThreadInput(DWORD remoteThreadId)
{
    DWORD currentThreadId = GetCurrentThreadId();

    if (currentThreadId != remoteThreadId) {

        int err = AttachThreadInput(remoteThreadId ,currentThreadId, true);
        qDebug() << "err " << err;

        if (!err)
            qErrnoWarning("AttachThreadInput");
        else
        {
            qDebug("set successful!");
        }
    }
    else
    {
        qDebug("no set!");
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const QSize MAIN_SIZE_MAX = QSize(16777215, 16777215);
    this->setMaximumSize(MAIN_SIZE_MAX);
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);

    setWindowIcon(QIcon(":hn.jpg"));
    setWindowTitle(" HNDZ L4");

    sgin       = new ZMenu("登录(R)", this);

//    para_skip  = new ZMenu("参数预览", this);
    dev_search = new ZMenu("设备搜索(S)", this);
//    net_pro    = new ZMenu("远程操作", this);
    view       = new ZMenu("视图(V)", this);

    add_sgin_action();
//    add_para_action();
//    add_pro_action();
    add_search_action();

    setWindowOpacity(1);
    QRect win = GetScreenInfo();

    setMinimumSize(win.width()/3,win.height()/3);
    resize(win.width()/3+ 50,win.height()/2);
    qDebug() << win.width() << win.height();
//    resize(win.width(),win.height());
    createDockWindows();

    connect(&devsearch, SIGNAL(add_dev_sig(DEV_DATA_INFO)), this, SLOT(adddevlist(DEV_DATA_INFO)));
//    connect(devlist, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
//            this, SLOT(changeButton()));

    timsize = 0;
    tim = new QTimer(this);
    connect(tim, SIGNAL(timeout()), this, SLOT(keyconnect()));


}
//开启定时器
void MainWindow::start_timer(int interval)
{
    if(timsize == 0)
    {
        tim->start(interval);
        timsize++;
    }
}
void MainWindow::stop_timer(void)
{
    if(timsize > 0)
    {
        timsize--;
    }
    else
        tim->stop();
}

//鼠标检测事件
void MainWindow::keyconnect(void)
{
    int index = centertab->currentIndex();
    quint8 ty = tabpro[index].type;
    if(ty != REMOTE_PROCESS && ty != PARA_SKIP_PROCESS)
    {
        return;
    }

     QWidget      *  vnctab = centertab->currentWidget();
    if(vnctab != NULL)
    {

        if(vnctab->isActiveWindow())
        {

             SHORT kye = GetAsyncKeyState(VK_LBUTTON);
             if(kye != 0)
            {
                QPoint mousepos;
                mousepos = QCursor::pos();


                QPoint GlobalPoint(vnctab->mapToGlobal(QPoint(0, 0)));//获取控件在窗体中的坐标
                int x = GlobalPoint.x();
                int y = GlobalPoint.y();


                if(mousepos.x() > x && mousepos.y() > y)
                {
                    vnctab->setFocus(Qt::TabFocusReason);
                }
            }
         }
    }
}


void MainWindow::changeButton()
{
//    QTreeWidgetItem* curItem = devlist->currentItem();  //获取当前被点击的节点

    quint8 ty = get_dev_type(devlist->get_current_row());

    switch (ty)
    {
        case LX_DEV:
            para_skip->action_enable(true);
            net_pro->action_enable(true);
        break;
        case SWITCH_DEV:
            para_skip->action_enable(false);
            net_pro->action_enable(false);
        break;
    }
}

//登录设备与注销设备

void MainWindow::add_sgin_action(void)
{
    sgin->add_action("登录");
    sgin->add_action("注销");
    sgin->get_action("登录")->setIcon(QIcon(":rogin.jpg"));
    sgin->get_action("注销")->setIcon(QIcon(":exit.jpg"));

    roginBar = addToolBar(tr("登录"));
    roginBar->addAction(sgin->get_action("登录"));
    roginBar->addAction(sgin->get_action("注销"));
    roginBar->setMovable(false);

    connect(sgin->get_action("登录"), SIGNAL(triggered()), this, SLOT(rogindev()));
}



void MainWindow::rogindev(void)
{
 ;
}
//ie 登录配置三旺服务器
void MainWindow::ieconfig(void)
{

    QString logip;

    if(tab_pro_init( logip, IE_PROCESS) == false)
    {
        qDebug() << "skip init fail!";
        return;
    }
    QString cmd = QString("C:/Program Files/Internet Explorer/iexplore.exe -k"
                          " http://%1").arg(logip);
    QString w_string = QString("http://%1/ - Windows Internet Explorer").arg(logip);

    ZProcessWidget * nwebpro = new ZProcessWidget(cmd, w_string, "IEFrame");

    nwebpro->creat_process(centertab);

    zattachWindowThreadInput(nwebpro->get_pid_info().dwThreadId);
    add_process(nwebpro, IE_PROCESS, logip);
    QString name = "ie设置 "+ logip;
    centertab->addTab(nwebpro->get_widget(), name);//tr("登录"));
    centertab->setCurrentWidget(nwebpro->get_widget());

    this->resize(this->width()+1, this->height()+1);
}

//void MainWindow::destory_ie(void)
//{
//    if(webpro != NULL)
//    {
//        delete webpro;
//        webpro = NULL;
//    }
//}

void MainWindow::destory_vnc(void)
{
//    if(tim != NULL)
//    {
//        tim->stop();
//    }
//    vnctab = NULL;
}

//void MainWindow::destory_skip(void)
//{
//    if(skippro != NULL)
//    {
//        delete skippro;
//        skippro = NULL;
//    }
//}


//浏览当前设备的参数
void MainWindow::add_para_action(void)
{
    para_skip->add_action("浏览当前", "浏览当前设备");
    connect(para_skip->get_action("浏览当前"), SIGNAL(triggered()), this, SLOT(remote_skip_dev()));

}

void MainWindow::skipdev(void)
{
    ;
}

void MainWindow::restart_dev_search(void)
{
    devlist->clear();
    devinfolist.clear();
    devsearch.start_skip_dev();
}


void MainWindow::add_search_action(void)
{
    dev_search->add_action( "搜索所有", "搜索所有设备", true);

    dev_search->get_action("搜索所有")->setIcon(QIcon(":find.png"));
    skipBar = addToolBar(tr("搜索所有"));
    skipBar->addAction(dev_search->get_action("搜索所有"));
    skipBar->setMovable(false);
    connect(dev_search->get_action("搜索所有"), SIGNAL(triggered()), this, SLOT(restart_dev_search()));
}

void MainWindow::add_pro_action(void)
{
     net_pro->add_action("远程目前", "远程当前设备");
     connect(net_pro->get_action("远程目前"), SIGNAL(triggered()), this, SLOT(remotedev()));
}

void MainWindow::toolBarFloat(bool topLevel)
{
    qDebug("tool %d",topLevel);
    if(topLevel == 0)
    {
        setCentralWidget(centerzdock);
    }
}

void MainWindow::removeMtab(int index)
{
    if(centertab != NULL)
    {
        qDebug() << "index " <<index;
        if(index >= tabpro.size())
        {
            qDebug() << "removetab index err!" << index;
        }

        quint8 ty = tabpro[index].type;
        if(ty == REMOTE_PROCESS || ty == PARA_SKIP_PROCESS)
        {
            stop_timer();
        }
        centertab->removeTab(index);
        tabpro[index].destory_tabprocess();
        tabpro.removeAt(index);

    }

}

void MainWindow::createDockWindows(void)
{
    QDockWidget *dock = new QDockWidget(tr("设备列表"), this);

    devlist = new ZQTreeWidget(dock);

    dock->setAllowedAreas(Qt::LeftDockWidgetArea| Qt::RightDockWidgetArea);
    devlist->setEnabled(false);
    devlist->setRootIsDecorated(false);
    devlist->setHeaderLabels(QStringList() << tr("设备名称") << tr("IP地址") << tr("MAC地址"));
    devlist->header()->setStretchLastSection(false);
    dock->setWidget(devlist);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    view->get_menu()->addAction(dock->toggleViewAction());
//    connect(devlist,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
//              this,SLOT(devlistclick(QTreeWidgetItem* ,int)));
    devlist->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(devlist,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(popMenu(const QPoint&)));//检测鼠标右键


    devlist->setFocusPolicy(Qt::ClickFocus);


    centerzdock = new ZDockWidget(this, tr("操作窗口"));

    setCentralWidget(centerzdock);
    connect(centerzdock,SIGNAL(topLevelChanged(bool)),this,SLOT(toolBarFloat(bool)));

    centertab = new QTabWidget(centerzdock);

    centertab->setTabsClosable(TRUE);

    connect(centertab, SIGNAL(tabCloseRequested(int)), this, SLOT(removeMtab(int)));
    connect(centertab, SIGNAL(currentChanged(int)), this, SLOT(tabchange(int)));
    connect(centertab, SIGNAL(tabBarClicked(int)), this, SLOT(tabchange(int)));

    centerzdock->setWidget(centertab);

    centertab->setFocusPolicy(Qt::ClickFocus);

    qDebug("centertab is 0x%x!",centertab->focusPolicy());
//    view->addAction(centerzdock->toggleViewAction());
    view->get_menu()->addAction(centerzdock->toggleViewAction());


    infodock =  new QDockWidget(tr("提示信息"), this);
    infodock->setAllowedAreas(Qt::BottomDockWidgetArea);


    addDockWidget(Qt::BottomDockWidgetArea, infodock);
    view->get_menu()->addAction(infodock->toggleViewAction());

}
//在设备列表上点击鼠标左键
void MainWindow::devlistclick(QTreeWidgetItem* item,  int val)
{   
    quint8 ty = get_dev_type( devlist->get_current_row());
    if(ty == LX_DEV)            //L  系列
    {
        net_pro->action_enable(true);
        qDebug("action enable!!!!");
    }
    else
    {
         net_pro->action_enable(false);
    }

}

 void MainWindow::tabchange(int index)
 {
     if(index >= 0)
     {
        centertab->widget(index)->setFocus(Qt::TabFocusReason);
     }
 }

void MainWindow::remotedev(void)
{
    QString logip;

    if(tab_pro_init( logip, REMOTE_PROCESS) == false)
    {
        qDebug() << "skip init fail!";
        return;
    }
//    QString cmd = QString("C:/Program Files/TightVNC/tvnviewer.exe %1").arg(logip);
    QString cmd = QString("tvnviewer.exe %1").arg(logip);
    QString na = QString("VNC-%1").arg(logip);

    ZProcessWidget * nvncpro = new ZProcessWidget(cmd, na, "TvnWindowClass");
//    ZProcessWidget * nvncpro = new ZProcessWidget(cmd, "Qt for Embedded Linux VNC Server - TightVNC Viewer",
//                                                  "TvnWindowClass");

    nvncpro->creat_process(centertab);

    start_timer();

    add_process(nvncpro, REMOTE_PROCESS, logip);

    zattachWindowThreadInput(nvncpro->get_pid_info().dwThreadId);
    centertab->addTab(nvncpro->get_widget(), tr("远程")+logip);

    centertab->setCurrentWidget(nvncpro->get_widget());

    qDebug("vnc tab index is %d", centertab->currentIndex());

    this->resize(this->width()+1, this->height()+1);

}


bool  MainWindow::tab_pro_init(QString &ipaddr, quint8 ty)
{
    if(devlist== NULL)
    {
        qDebug("rogindev devlist no list!");
        return false;
    }
    QTreeWidgetItem* curItem = devlist->currentItem();  //获取当前被点击的节点


    ipaddr = curItem->text(1);
    int tab = tab_is_enable(ty, ipaddr);
    if(tab != -1)
    {
        qDebug() << ipaddr <<"have init!";
        centertab->setCurrentIndex(tab);
        return false;
    }
    return true;
}

void MainWindow::remote_skip_dev(void)
{

    QString logip;


    if(tab_pro_init( logip, PARA_SKIP_PROCESS) == false)
    {
        qDebug() << "skip init fail!";
        return;
    }

//    QString cmd = QString("D:/L4/qh/lx_hebing/Lx/bin/debug/win/L3Config.exe");
    QString cmd = QString("L3Config.exe --workmode=client --server=%1").arg(logip);;
    QString na = QString("L3Ftp-%1").arg(logip);
    ZProcessWidget * skippro = new ZProcessWidget(cmd, na, "Qt5QWindowIcon");

    skippro->creat_process(centertab);

    zattachWindowThreadInput(skippro->get_pid_info().dwThreadId);
    start_timer();
    add_process(skippro, PARA_SKIP_PROCESS, logip);
    centertab->addTab(skippro->get_widget(), QString("浏览%1初始化").arg(logip));
    centertab->setCurrentWidget(skippro->get_widget());
    int index = centertab->currentIndex();

    this->resize(this->width()+1, this->height()+1);


    skippro->restart_process(QString("Dialog-%1").arg(logip), "Qt5QWindowIcon", centertab);

//     zattachWindowThreadInput(skippro->get_pid_info().dwThreadId);
//     tim->start(100);
     centertab->removeTab(index);
     centertab->addTab(skippro->get_widget(), QString("浏览%1密码").arg(logip));
     centertab->setCurrentWidget(skippro->get_widget());

     this->resize(this->width()+1, this->height()+1);


     skippro->restart_process(QString("L3Config-%1").arg(logip), "Qt5QWindowIcon", centertab);

     centertab->removeTab(index);

     centertab->addTab(skippro->get_widget(), QString("浏览%1参数").arg(logip));
     centertab->setCurrentWidget(skippro->get_widget());

     this->resize(this->width()+1, this->height()+1);



}
void MainWindow::ftp_dev(void)
{

    QString logip;

    if(tab_pro_init( logip, FTP_PROCESS) == false)
    {
        qDebug() << "skip init fail!";
        return;
    }

//  QString cmd = QString("D:/zty/ftp/build-ftp-Desktop_Qt_5_4_1_MinGW_32bit-Debug/debug/ftp.exe");
    QString cmd = QString("ftp.exe %1").arg(logip);

    ZProcessWidget * ftppro = new ZProcessWidget(cmd, QString("FTP-%1").arg(logip), "Qt5QWindowIcon");

    ftppro->creat_process(centertab);

//    zattachWindowThreadInput(webpro->get_pid_info().dwThreadId);
    add_process(ftppro, FTP_PROCESS, logip);
    centertab->addTab(ftppro->get_widget(), QString("ftp-%1").arg(logip));
    centertab->setCurrentWidget(ftppro->get_widget());
//    this->resize(this->width()+10, this->height()+50);
    ftppro->get_widget()->hide();
    ftppro->get_widget()->show();


}

void MainWindow::setip(void)
{


    DEV_DATA_INFO dev = devinfolist[devlist->get_current_row()];

    QPoint GlobalPoint(centerzdock->mapToGlobal(QPoint(0, 0)));//获取控件在窗体中的坐标

    qDebug("centertab xw %d h %d!", centerzdock->width(), centerzdock->height());

//    QDialog      * ipdia = new QDialog();
    ZInternetSet * ipAddr = new ZInternetSet();

    ipAddr->ip_init(QString::fromStdString(dev.get_ip_string()),
                    QString::fromStdString(dev.get_netmask_string()),
                    QString::fromStdString(dev.get_gw_string()));

//    centertab->addTab(ipAddr, tr("ip"));
    ipAddr->move(GlobalPoint.x() +centerzdock->width()/3, GlobalPoint.y() +centerzdock->height()/5);
//    ipAddr->show();
    connect(ipAddr, SIGNAL(set_ip_text(TCPText)), this, SLOT(set_ip_addr(TCPText)));
    connect(&devsearch, SIGNAL(ip_set_state(int)), ipAddr, SLOT(set_ip_result(int)));

    ipAddr->exec();

    delete ipAddr;

}

void MainWindow::set_dev_name(void)
{
    DEV_DATA_INFO dev = devinfolist[devlist->get_current_row()];
    DevNameDialog * nam = new DevNameDialog(QString::fromStdString(dev.get_dev_name()));
    connect(nam->buttonbox, SIGNAL(rejected()), nam, SLOT(reject()));
    connect(nam, SIGNAL(click_set_dev(QString)), this, SLOT(set_name_text(QString)));
    connect(&devsearch, SIGNAL(name_set_state(int)), nam, SLOT(name_set_result(int)));
    nam->exec();

    delete nam;
}

void MainWindow::set_ip_addr(TCPText text)
{
    DEV_DATA_INFO dev = devinfolist[devlist->get_current_row()];

    string_to_hex(text.ip,   ".",  dev.ip,      sizeof(dev.ip));
    string_to_hex(text.mask, ".",  dev.netmask, sizeof(dev.netmask));
    string_to_hex(text.gw,   ".",  dev.gateway, sizeof(dev.gateway));

    qDebug()<<"ip !" << text.ip;
    dev.get_mac_string();
    devsearch.set_ip_info(dev);
}


void MainWindow::set_name_text(QString n)
{
    qDebug() << "dev name " << n;
    DEV_DATA_INFO dev = devinfolist[devlist->get_current_row()];


    dev.set__name(n);
    qDebug("nnnnnn %s", dev.name);
    devsearch.set_dev_name(dev);
}

//在设备列表上点击鼠标右键
void MainWindow::popMenu(const QPoint& point)
{
    if(devlist == NULL)return;                            //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    QTreeWidgetItem* curItem = devlist->currentItem();  //获取当前被点击的节点

    QString wellName = curItem->text(2);

    if(wellName != "00:00:00:00:00:00")
    {
        quint8 ty = current_dev_type();
        if(ty == LX_DEV)
        {
            QAction logindev(tr("远程该设备"),this);//登录该设备
            QAction skipdev(tr("预览该设备"),this);//登录该设备
            QAction ftpdev(tr("文件传输"),this);//登录该设备
            QAction setip(tr("设置ip"),this);    //设置该设备的Ip
            QAction setname(tr("设置设备名称"),this);   //设置该设备的Ip
            //在界面上删除该item
           connect(&logindev, SIGNAL(triggered()), this, SLOT(remotedev()));
           connect(&skipdev, SIGNAL(triggered()), this, SLOT(remote_skip_dev()));
           connect(&ftpdev, SIGNAL(triggered()), this, SLOT(ftp_dev()));
           connect(&setip,    SIGNAL(triggered()), this, SLOT(setip()));
           connect(&setname,  SIGNAL(triggered()), this, SLOT(set_dev_name()));

            QPoint pos;
            QMenu menu(this->devlist);
            menu.addAction(&logindev);
            menu.addAction(&skipdev);
            menu.addAction(&ftpdev);
            menu.addAction(&setip);
            menu.addAction(&setname);
            menu.exec(QCursor::pos());  //在当前鼠标位置显示
        }
        else if( ty != DEV_MAX_TYPE)
        {
            QAction logindev(tr("配置该设备"),this);//登录该设备
            QAction setip(tr("设置ip"),this);   //设置该设备的Ip
            QAction setname(tr("设置设备名称"),this);   //设置该设备的Ip
            //在界面上删除该item
           connect(&logindev, SIGNAL(triggered()), this, SLOT(ieconfig()));
           connect(&setip,    SIGNAL(triggered()), this, SLOT(setip()));
           connect(&setname,  SIGNAL(triggered()), this, SLOT(set_dev_name()));

            QPoint pos;
            QMenu menu(this->devlist);
            menu.addAction(&logindev);
            menu.addAction(&setip);
            menu.addAction(&setname);
            menu.exec(QCursor::pos());  //在当前鼠标位置显示
        }

    }
}
//是否已经添加了dev设备。
BOOL MainWindow::dev_is_have(DEV_DATA_INFO dev)
{

    if(dev.mac_is_ok() == false)
        return FALSE;

    QVector<DEV_DATA_INFO>::iterator iter;
    for(iter = devinfolist.begin(); iter != devinfolist.end(); iter++)
    {
        if(iter->get_mac_string() == dev.get_mac_string())
        {
            return TRUE;
        }
    }
    return FALSE;
}

void MainWindow::add_devinfo_list(DEV_DATA_INFO dev)
{
    devinfolist.append(dev);
}

quint8 MainWindow::get_dev_type(int dev)
{
    if(dev < devinfolist.size())
        return devinfolist[dev].devtype;
    else
        return 0xff;
}
quint8 MainWindow::current_dev_type(void)
{
    return get_dev_type(devlist->get_current_row());
}


void MainWindow::adddevlist(DEV_DATA_INFO  devinfo)
{
    if(dev_is_have(devinfo))
        return;

    add_devinfo_list(devinfo);

    QTreeWidgetItem *item = new QTreeWidgetItem;

    item->setText(0, QString::fromStdString(devinfo.get_dev_name()));
    item->setText(1, QString::fromStdString(devinfo.get_ip_string()));
    item->setText(2, QString::fromStdString(devinfo.get_mac_string()));

    devlist->addTopLevelItem(item);
    if (!devlist->currentItem()) {
        devlist->setCurrentItem(devlist->topLevelItem(0));
        devlist->setEnabled(true);
    }

}

void MainWindow::add_process(ZProcessWidget * pro, quint8 type, QString ipaddr)
{
    TabProcess mid(pro, type, ipaddr);
    tabpro.append(mid);

}

void MainWindow::set_process_type(quint8 type)
{
       tabprotype.append(type);
}

int MainWindow::tab_is_enable(quint8 ty, QString ipaddr)
{
    for(int i = 0; i < tabpro.size(); i++)
    {
        if(tabpro.at(i).type == ty && tabpro.at(i).ip == ipaddr)
            return i;
    }
    return -1;
}

//void MainWindow::foucschange(QObject *object)
//{
//    qDebug("foucs change !");
//}

//void MainWindow::plaint(void)
//{

//    qDebug("show start\n");

//}

//void MainWindow::keyPressEvent(QKeyEvent * key)
//{
//     qDebug("keyPressEvent start\n");
//     zattachWindowThreadInput(vncpi.dwThreadId);
//     attact_key(vnc_hand);
//}

//void MainWindow::keyReleaseEvent(QKeyEvent * key)
//{
//     qDebug("keyReleaseEvent start\n");
//    zattachWindowThreadInput(vncpi.dwThreadId);
//}

//void MainWindow::mousePressEvent(QMouseEvent *mouse)
//{
//     qDebug("mouse press\n");
//}

//bool MainWindow::eventFilter(QObject * obj, QEvent * event)
//{
//    return false;
//}

//bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
//{
//    Q_UNUSED(eventType);

//   MSG* msg = reinterpret_cast<MSG*>(message);
//   if(msg->message == WM_LBUTTONDOWN)
//        qDebug() << "msg " << msg->message;
//   return false;
//}


MainWindow::~MainWindow()
{
    delete ui;
}

//DWORD  MainWindow::get_paraid(void)
//{

//    LONG                        status;
//    DWORD                        dwParentPID = (DWORD)-1;
//    HANDLE                        hProcess;
//    PROCESS_BASIC_INFORMATION    pbi;

//    PROCNTQSIP NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
//    GetModuleHandle(L"ntdll"), "NtQueryInformationProcess");

//    if(NULL == NtQueryInformationProcess)
//    {
//       return (DWORD)-1;
//    }
//    // Get process handle
//    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,  wpi.dwProcessId);
//    if (!hProcess)
//    {
//       return (DWORD)-1;
//    }

//    // Retrieve information
//    status = NtQueryInformationProcess( hProcess,
//       ProcessBasicInformation,
//       (PVOID)&pbi,
//       sizeof(PROCESS_BASIC_INFORMATION),
//       NULL
//       );

//    // Copy parent Id on success
//    if  (!status)
//    {
//       dwParentPID = pbi.InheritedFromUniqueProcessId;
//    }

//    CloseHandle (hProcess);

//    return dwParentPID;


//}
