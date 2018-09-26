#include "mainwindow.h"
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
//       QRect deskRect = desktopWidget->availableGeometry();
   QRect screenRect = desktopWidget->screenGeometry();

   return screenRect;
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

    setWindowTitle("HNDZ L4");
    sgin       = new ZMenu("登录", this);

    para_skip  = new ZMenu("参数预览", this);
    dev_search = new ZMenu("设备搜索", this);
    net_pro    = new ZMenu("远程操作", this);
    view       = new ZMenu("视图", this);

    add_sgin_action();
    add_para_action();
    add_pro_action();
    add_search_action();

    setWindowOpacity(1);
    QRect win = GetScreenInfo();

    setMinimumSize(win.width()/3,win.height()/3);
    resize(win.width()/2,win.height()/2);

    createDockWindows();

    connect(&devsearch, SIGNAL(add_dev_sig(DEV_DATA_INFO)), this, SLOT(adddevlist(DEV_DATA_INFO)));

    tim = new QTimer(this);
    connect(tim, SIGNAL(timeout()), this, SLOT(vnckeyconnect()));


    vncpro = NULL;
    webpro = NULL;
    skippro = NULL;

}

void MainWindow::vnckeyconnect(void)
{
   QWidget      *  vnctab = vncpro->get_widget();
  if(vnctab != NULL)
  {
    if(vnctab->isActiveWindow())
    {
        if((GetAsyncKeyState(VK_LBUTTON)) > 0)
        {
            QPoint mousepos;
            mousepos = QCursor::pos();


            QPoint GlobalPoint(vnctab->mapToGlobal(QPoint(0, 0)));//获取控件在窗体中的坐标
            int x = GlobalPoint.x();
            int y = GlobalPoint.y();

            qDebug("x %d y %d mouse x %d mouse y %d", x, y, mousepos.x(), mousepos.y());

            if(mousepos.x() > x && mousepos.y() > y)
            {
                qDebug("is active window!");
                vnctab->setFocus(Qt::TabFocusReason);
            }
        }
     }
    else
    {
        qDebug("no active windwo!");
    }
  }
}

//登录设备与注销设备

void MainWindow::add_sgin_action(void)
{
    sgin->add_action("登录");
    sgin->add_action("注销");

    connect(sgin->get_action("登录"), SIGNAL(triggered()), this, SLOT(rogindev()));
}

void MainWindow::rogindev(void)
{

    if(webpro != NULL)
    {
        qDebug("Have dev!");
        return ;
    }

    if(devlist== NULL)                                   //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    {
        qDebug("rogindev devlist no list!");
        return;
    }
    QTreeWidgetItem* curItem = devlist->currentItem();  //获取当前被点击的节点

    quint8 ty = get_dev_type(devlist->get_current_row());

    if(ty == 0x00 || ty == DEV_MAX_TYPE)
    {
        qDebug("dev type is LX!");
        return ;
    }

    QString logip = curItem->text(1);
    QString cmd = QString("C:/Program Files/Internet Explorer/iexplore.exe -k"
                          " http://%1").arg(logip);
    QString w_string = QString("http://%1/ - Windows Internet Explorer").arg(logip);

    webpro = new ZProcessWidget(cmd, w_string, "IEFrame");

    webpro->creat_process(centertab);

    zattachWindowThreadInput(webpro->get_pid_info().dwThreadId);
    centertab->addTab(webpro->get_widget(), tr("登录"));
    centertab->setCurrentWidget(webpro->get_widget());

    tab[centertab->currentIndex()]  = SWITCH_DEV;
    this->resize(this->width()+1, this->height()+1);


}

void MainWindow::destory_ie(void)
{
    if(webpro != NULL)
    {
        delete webpro;
        webpro = NULL;
    }
}

void MainWindow::destory_vnc(void)
{
    if(vncpro != NULL)
    {
        delete vncpro;
        vncpro = NULL;
    }
    if(tim != NULL)
    {
        tim->stop();
    }
}

void MainWindow::destory_skip(void)
{
    if(skippro != NULL)
    {
        delete skippro;
        skippro = NULL;
    }
}


//浏览当前设备的参数
void MainWindow::add_para_action(void)
{
    para_skip->add_action("浏览当前", "浏览当前设备",true);
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
//    connect(dev_search->get_action("搜索所有"), SIGNAL(triggered()), &devsearch, SLOT(start_skip_dev()));
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
//        if(tab[index] == LX_DEV)
//        {
//            destory_vnc();
//        }
//        else
//        {
//            destory_ie();
//        }
        if(index >= tabprotype.size())
        {
            qDebug() << "removetab index err!" << index;
        }
        switch (tabprotype.at(index))
        {
            case REMOTE_PROCESS:
                destory_vnc();
            break;
            case IE_PROCESS:
                destory_ie();
            break;
            case FTP_PROCESS:
            break;
            case PARA_SKIP_PROCESS:
            break;
        default:
            break;
        }

        centertab->removeTab(index);
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
    connect(devlist,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
              this,SLOT(devlistclick(QTreeWidgetItem* ,int)));
    devlist->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(devlist,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(popMenu(const QPoint&)));//检测鼠标右键


    devlist->setFocusPolicy(Qt::ClickFocus);

    qDebug("devlist is 0x%x!",devlist->focusPolicy());


    centerzdock = new ZDockWidget(this, tr("窗体"));

    setCentralWidget(centerzdock);
    connect(centerzdock,SIGNAL(topLevelChanged(bool)),this,SLOT(toolBarFloat(bool)));

    qDebug("windows is 0x%x!",centerzdock->focusPolicy());
    centertab = new QTabWidget(centerzdock);
//    QLabel * filename = new QLabel(tr("File name:"));
//    centertab->addTab(filename, tr("General"));

    centertab->setTabsClosable(TRUE);

    connect(centertab, SIGNAL(tabCloseRequested(int)), this, SLOT(removeMtab(int)));
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

void MainWindow::remotedev(void)
{
    if(vncpro != NULL)
    {
        qDebug("Have dev!");
        return ;
    }

    if(devlist== NULL)                                   //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    {
        qDebug("rogindev devlist no list!");
        return;
    }
    QTreeWidgetItem* curItem = devlist->currentItem();  //获取当前被点击的节点


    if(get_dev_type(devlist->get_current_row()) != 0x00)
    {
        qDebug("dev type is LX!");
        return ;
    }

    QString logip = curItem->text(1);

    QString cmd = QString("C:/Program Files/TightVNC/tvnviewer.exe %1").arg(logip);

    vncpro = new ZProcessWidget(cmd, "Qt for Embedded Linux VNC Server - TightVNC Viewer", "TvnWindowClass");

    vncpro->creat_process(centertab);

    tim->start(1000);

    centertab->addTab(vncpro->get_widget(), tr("连接"));
    centertab->setCurrentWidget(vncpro->get_widget());
    qDebug("vnc tab index is %d", centertab->currentIndex());
     tab[centertab->currentIndex()]  = LX_DEV;
    this->resize(this->width()+1, this->height()+1);

//    centertab->setTabEnabled(, );

}


void MainWindow::remote_skip_dev(void)
{
    if(skippro != NULL)
    {
        qDebug("Have dev!");
        return ;
    }

//    if(devlist== NULL)                                   //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
//    {
//        qDebug("rogindev devlist no list!");
//        return;
//    }
//    QTreeWidgetItem* curItem = devlist->currentItem();  //获取当前被点击的节点


//    if(get_dev_type(devlist->get_current_row()) != 0x00)
//    {
//        qDebug("dev type is LX!");
//        return ;
//    }

//    QString logip = curItem->text(1);

    QString cmd = QString("D:/L4/qh/lx_hebing/Lx/bin/debug/win/L3Config.exe");

    skippro = new ZProcessWidget(cmd, "L3Config", "Qt5QWindowIcon");

    skippro->creat_process(centertab);

//    zattachWindowThreadInput(webpro->get_pid_info().dwThreadId);
    centertab->addTab(skippro->get_widget(), tr("浏览当前设备"));
    centertab->setCurrentWidget(skippro->get_widget());
    this->resize(this->width()+1, this->height()+1);

    WId wid = 0;
    int i = 0;

    while(wid == 0)
    {
        QEventLoop eventloop;

        QTimer::singleShot(100, &eventloop, SLOT(quit()));
        eventloop.exec();
        wid = (WId)FindWindow( L"Qt5QWindowIcon", L"Dialog");
    }


    if(wid == 0)
    {
        qDebug("wid 0 error!");
        return ;
    }
    else
        qDebug() << "find wid!";
    QWindow * qwin_p = QWindow::fromWinId(wid);

    qwin_p->setFlags(qwin_p->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                       | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC
                       | Qt::BypassWindowManagerHint);
    QWidget * qwid_p = QWidget::createWindowContainer(qwin_p, centertab);

    qwid_p->setFocusPolicy(Qt::WheelFocus);


    qDebug() << "current index " << centertab->currentIndex();

    centertab->removeTab(0);
    centertab->addTab(qwid_p, tr("浏览当前设备"));
    centertab->setCurrentWidget(qwid_p);
     qDebug() << "current index " << centertab->currentIndex();

     this->resize(this->width()+1, this->height()+1);

     wid = 0;
     while(wid == 0)
     {
         QEventLoop eventloop;

         QTimer::singleShot(100, &eventloop, SLOT(quit()));
         eventloop.exec();
         wid = (WId)FindWindow( L"Qt5QWindowIcon", L"MainWindow");
     }


     if(wid == 0)
     {
         qDebug("wid 0 error!");
         return ;
     }
     else
         qDebug() << "find wid!";
     QWindow * nqwin_p = QWindow::fromWinId(wid);

     nqwin_p->setFlags(qwin_p->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                        | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC
                        | Qt::BypassWindowManagerHint);
     QWidget * nqwid_p = QWidget::createWindowContainer(nqwin_p, centertab);

     nqwid_p->setFocusPolicy(Qt::WheelFocus);


     qDebug() << "current index " << centertab->currentIndex();

     centertab->removeTab(0);
     centertab->addTab(nqwid_p, tr("浏览当前设备"));
     centertab->setCurrentWidget(nqwid_p);
      qDebug() << "current index " << centertab->currentIndex();

      this->resize(this->width()+1, this->height()+1);



}
void MainWindow::ftp_dev(void)
{
    if(skippro != NULL)
    {
        qDebug("Have dev!");
        return ;
    }

//    if(devlist== NULL)                                   //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
//    {
//        qDebug("rogindev devlist no list!");
//        return;
//    }
//    QTreeWidgetItem* curItem = devlist->currentItem();  //获取当前被点击的节点


//    if(get_dev_type(devlist->get_current_row()) != 0x00)
//    {
//        qDebug("dev type is LX!");
//        return ;
//    }

//    QString logip = curItem->text(1);

    QString cmd = QString("D:/zty/ftp/build-ftp-Desktop_Qt_5_4_1_MinGW_32bit-Debug/debug/ftp.exe");

    skippro = new ZProcessWidget(cmd, "FTP", "Qt5QWindowIcon");

    skippro->creat_process(centertab);

//    zattachWindowThreadInput(webpro->get_pid_info().dwThreadId);
    centertab->addTab(skippro->get_widget(), tr("浏览当前设备"));
    centertab->setCurrentWidget(skippro->get_widget());


//    centertab->setTabEnabled(, );

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

    if(devlist->hasFocus())
        qDebug("devlist has fouces!");
    else
       qDebug("devlist no has fouces!");
    QTreeWidgetItem* curItem = devlist->currentItem();  //获取当前被点击的节点
    QModelIndex index = devlist->zindexFromItem(curItem);
    qDebug("row is %d!", devlist->get_current_row());
    qDebug("dev type is 0x%x!", get_dev_type(devlist->get_current_row()));
    if(devlist==NULL)return;                            //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    QString wellName = curItem->text(2);

    if(wellName != "00:00:00:00:00:00")
    {
        QAction logindev(tr("登录该设备"),this);//登录该设备
        QAction setip(tr("设置ip"),this);   //设置该设备的Ip
        QAction setname(tr("设置设备名称"),this);   //设置该设备的Ip
        //在界面上删除该item
       connect(&logindev, SIGNAL(triggered()), this, SLOT(rogindev()));
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

void MainWindow::set_process_type(quint8 type)
{
       tabprotype.append(type);
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
