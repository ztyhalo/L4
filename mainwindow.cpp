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
    sgin = menuBar()->addMenu(tr("登录"));

    para_skip = menuBar()->addMenu(tr("参数预览"));
    dev_search = menuBar()->addMenu(tr("设备搜索"));
    net_pro = menuBar()->addMenu(tr("远程操作"));

    view = menuBar()->addMenu(tr("视图"));


    add_sgin_action();
    add_para_action();
    add_pro_action();
    add_search_action();

    setWindowOpacity(1);
    QRect win = GetScreenInfo();

    setMinimumSize(win.width()/3,win.height()/3);
    resize(win.width()/2,win.height()/2);

    createDockWindows();

    connect(&devsearch, SIGNAL(adddevsig(DEV_DATA_INFO)), this, SLOT(adddevlist(DEV_DATA_INFO)));

    tim = new QTimer(this);
    connect(tim, SIGNAL(timeout()), this, SLOT(keyconnect()));
    rogtab = NULL;
    vnctab = NULL;
    set_mark = 0;


}

void MainWindow::keyconnect(void)
{
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
    Z_ADD_ACTION(login,sgin, "登录", "登录");
    Z_ADD_ACTION(exit,sgin, "注销", "注销");

    connect(login, SIGNAL(triggered()), this, SLOT(rogindev()));
}

void MainWindow::rogindev(void)
{

    QTreeWidgetItem* curItem = devlist->currentItem();  //获取当前被点击的节点
    if(devlist== NULL)                                   //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    {
        qDebug("rogindev devlist no list!");
        return;
    }


    QString logip = curItem->text(1);
    QString cmd = QString("C:/Program Files/Internet Explorer/iexplore.exe -k"
                          " http://%1").arg(logip);

    QString w_string = QString("http://%1/ - Windows Internet Explorer").arg(logip);

    STARTUPINFO si;


    memset(&si, 0x00, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = true;

    memset(&wpi, 0x00, sizeof(wpi));

    bool bRet = CreateProcess(
        NULL,
        (LPWSTR)cmd.toStdWString().c_str(),
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL, &si, &wpi);

    WId wid = 0;
    int i = 0;
    do {
       Sleep(100);
       wid = (WId)FindWindow(L"IEFrame", w_string.toStdWString().c_str());
       i++;

    } while(wid == 0 && i < 10000);

    if(wid == 0)
    {
        //错误处理
        qDebug("wid 0error!");
        return;
    }

    QWindow *m_window;
    m_window = QWindow::fromWinId(wid);

    m_window->setFlags(m_window->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                       | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC); //

    rogtab = QWidget::createWindowContainer(m_window, centertab);

    rogtab->setFocusPolicy(Qt::WheelFocus);

    zattachWindowThreadInput(wpi.dwThreadId);

    centertab->addTab(rogtab, tr("登录"));
}

void MainWindow::destory_ie(void)
{
    if(rogtab != NULL)
    {
        kill_w_process(wpi);
        delete rogtab;
        rogtab = NULL;
    }
}

void MainWindow::destory_vnc(void)
{
    if(vnctab != NULL)
    {
        kill_w_process(vncpi);
        delete vnctab;
        vnctab = NULL;

    }
    if(tim != NULL)
    {
        tim->stop();
    }
}

DWORD  MainWindow::get_paraid(void)
{

    LONG                        status;
    DWORD                        dwParentPID = (DWORD)-1;
    HANDLE                        hProcess;
    PROCESS_BASIC_INFORMATION    pbi;

    PROCNTQSIP NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
    GetModuleHandle(L"ntdll"), "NtQueryInformationProcess");

    if(NULL == NtQueryInformationProcess)
    {
       return (DWORD)-1;
    }
    // Get process handle
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,  wpi.dwProcessId);
    if (!hProcess)
    {
       return (DWORD)-1;
    }

    // Retrieve information
    status = NtQueryInformationProcess( hProcess,
       ProcessBasicInformation,
       (PVOID)&pbi,
       sizeof(PROCESS_BASIC_INFORMATION),
       NULL
       );

    // Copy parent Id on success
    if  (!status)
    {
       dwParentPID = pbi.InheritedFromUniqueProcessId;
    }

    CloseHandle (hProcess);

    return dwParentPID;


}

//浏览当前设备的参数
void MainWindow::add_para_action(void)
{
    Z_ADD_ACTION(skip, para_skip, "浏览当前", "浏览当前设备");
//    connect(skip,SIGNAL(tirggered()), this, SLOT(skipdev()));
}

void MainWindow::skipdev(void)
{
    ;
}

void MainWindow::add_search_action(void)
{
    Z_ADD_ACTION(search_all, dev_search, "搜索所有", "搜索所有设备");
    connect(search_all, SIGNAL(triggered()), &devsearch, SLOT(start_skip_dev()));
}

void MainWindow::add_pro_action(void)
{
     Z_ADD_ACTION(pro_now, net_pro, "&远程目前", "远程当前设备");
     connect(pro_now, SIGNAL(triggered()), this, SLOT(tightremotedev()));
}

void MainWindow::tightremotedev(void)
{

    QTreeWidgetItem* curItem = devlist->currentItem();  //获取当前被点击的节点
    if(devlist== NULL)                                   //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    {
        qDebug("rogindev devlist no list!");
        return;
    }

    if(get_dev_type(devlist->get_current_row()) != 0x00)
    {
        qDebug("dev type is LX!");
        return ;
    }

    QString logip = curItem->text(1);

    QString cmd = QString("C:/Program Files/TightVNC/tvnviewer.exe %1").arg(logip);

    STARTUPINFO si;

    memset(&si, 0x00, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = true;

    memset(&vncpi, 0x00, sizeof(vncpi));


    bool bRet = CreateProcess(
        NULL,
        (LPWSTR)cmd.toStdWString().c_str(),
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL, &si, &vncpi);
    if(bRet == 0)
    {
        qDebug("vnc fail!");
    }

    WId wid = 0;
    int i = 0;

    do {
       Sleep(100);
       vnc_hand = FindWindow(L"TvnWindowClass", L"Qt for Embedded Linux VNC Server - TightVNC Viewer");
       wid = (WId)vnc_hand;
//       wid = (WId)FindWindow(NULL, L"New TightVNC Connection");
         qDebug("proid 0x%x", wid);
       i++;
    }while(wid == 0 && i < 10000);

    if(wid == 0)
    {
        qDebug("wid 0error!");
        return;
    }

     tim->start(1000);

     QWindow *m_window;
     m_window = QWindow::fromWinId(wid);

    m_window->setFlags(m_window->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                       | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC
                       | Qt::BypassWindowManagerHint);
    vnctab = QWidget::createWindowContainer(m_window, centertab);

    vnctab->setFocusPolicy(Qt::WheelFocus);

    centertab->addTab(vnctab, tr("连接"));


    qDebug("currentIndex is %d!", centertab->currentIndex());
//

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
        centertab->removeTab(index);

         qDebug("father id is 0x%x", get_paraid());
        destory_ie();
        destory_vnc();
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
    view->addAction(dock->toggleViewAction());
    connect(devlist,SIGNAL(itemClicked(QTreeWidgetItem*,int)),
              this,SLOT(devlistclick(QTreeWidgetItem* ,int)));
    devlist->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(devlist,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(popMenu(const QPoint&)));//检测鼠标右键

    qDebug("dev list dock is 0x%x!",dock->focusPolicy());

    devlist->setFocusPolicy(Qt::ClickFocus);

    qDebug("devlist is 0x%x!",devlist->focusPolicy());


//    QRect screenRect = centralWidget()->screenGeometry();
//    ui->centralWidget->getContentsMargins();
//    centerdock =  new QDockWidget(tr("窗体"), this);

//    setCentralWidget(centerdock);

//    connect(centerdock,SIGNAL(topLevelChanged(bool)),this,SLOT(toolBarFloat(bool)));

    centerzdock = new ZDockWidget(this, tr("窗体"));

//    centerzdock = new QDockWidget(tr("窗体"), this);
    setCentralWidget(centerzdock);
    connect(centerzdock,SIGNAL(topLevelChanged(bool)),this,SLOT(toolBarFloat(bool)));
//    centerdock->setAllowedAreas(Qt::LeftDockWidgetArea| Qt::RightDockWidgetArea);

    qDebug("windows is 0x%x!",centerzdock->focusPolicy());
    centertab = new QTabWidget(centerzdock);
//    QLabel * filename = new QLabel(tr("File name:"));
//    centertab->addTab(filename, tr("General"));

    centertab->setTabsClosable(TRUE);

//    centertab->tabCloseRequested.connect(centertab->removeTab);
    connect(centertab, SIGNAL(tabCloseRequested(int)), this, SLOT(removeMtab(int)));
    centerzdock->setWidget(centertab);

    centertab->setFocusPolicy(Qt::ClickFocus);

    qDebug("centertab is 0x%x!",centertab->focusPolicy());
    view->addAction(centerzdock->toggleViewAction());


    infodock =  new QDockWidget(tr("提示信息"), this);
    infodock->setAllowedAreas(Qt::BottomDockWidgetArea);

//    centertab = new QTabWidget(centerdock);
//    QLabel * filename = new QLabel(tr("File name:"));
//    centertab->addTab(filename, tr("General"));

//    centerdock->setWidget(centertab);
    addDockWidget(Qt::BottomDockWidgetArea, infodock);
    view->addAction(infodock->toggleViewAction());

//    QHBoxLayout * cenlayout = new QHBoxLayout;
//    cenlayout->addWidget(dock);
//    cenlayout->addWidget(centerdock);


}
//在设备列表上点击鼠标左键
void MainWindow::devlistclick(QTreeWidgetItem* item,int val)
{
    qDebug("click val %d", val);
    set_mark = 0;

}

void MainWindow::remotedev(void)
{


    QTreeWidgetItem* curItem = devlist->currentItem();  //获取当前被点击的节点
    if(devlist== NULL)                                   //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    {
        qDebug("rogindev devlist no list!");
        return;
    }

    if(get_dev_type(devlist->get_current_row()) != 0x00)
    {
        qDebug("dev type is LX!");
        return ;
    }

    QString logip = curItem->text(1);

    QString cmd = QString("C:/Program Files/RealVNC/VNC Viewer/vncviewer.exe %1").arg(logip);


    QString w_string = QString("%1 (Qt for Embedded Linux VNC Server) - VNC Viewer").arg(logip);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset(&si, 0x00, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = true;



    bool bRet = CreateProcess(
        NULL,
        (LPWSTR)cmd.toStdWString().c_str(),
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL, &si, &pi);

    WId wid = 0;
    int i = 0;

    do {
       Sleep(10);
//       wid = (WId)FindWindow(L"ui::Window::Dialog", L"192.168.1.112 - VNC Viewer");
       wid = (WId)FindWindow(L"vwr::CDesktopWin",w_string.toStdWString().c_str());
       qDebug("proid 0x%x", wid);
       i++;
    }while(wid == 0 && i < 10000);

    if(wid == 0)
    {
        //错误处理
        qDebug("wid 0error!");
        return;
    }


    QWindow *m_window;
    m_window = QWindow::fromWinId(wid);
//    attachWindowThreadInput(wid);


    qDebug("wind flag 0x%x!", m_window->flags());
    m_window->setFlags(m_window->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                       | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC
                       | Qt::BypassWindowManagerHint);
    vnctab = QWidget::createWindowContainer(m_window, centertab);

//    vnctab->installEventFilter(this);
    centertab->addTab(vnctab, tr("连接"));




/*
    i = 0;
    do {
       Sleep(10);
       wid = (WId)FindWindow(L"vwr::CDesktopWin", L"192.168.1.112 (Qt for Embedded Linux VNC Server) - VNC Viewer");

       qDebug("proid 0x%x", wid);
       i++;
    }while(wid == 0 && i < 1000);
     qDebug("wid %d!", wid);

     QWindow *w_window;
     w_window = QWindow::fromWinId(wid);

     qDebug("wind flag 0x%x!", w_window->flags());
     w_window->setFlags(w_window->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                        | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC
                        | Qt::BypassWindowManagerHint); //

   //  m_window->setFlags((Qt::WindowType)0xffffffff);
     QWidget* drogtab = QWidget::createWindowContainer(w_window, centertab);
 //    rogtab->setMinimumSize(400, 300);
     centertab->addTab(drogtab, tr("远程"));
     */
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
        //在界面上删除该item
       connect(&logindev, SIGNAL(triggered()), this, SLOT(rogindev()));
//        connect(&setip,SIGNAL(triggered()),this,SLOT(setip()));

        QPoint pos;
        QMenu menu(this->devlist);
        menu.addAction(&logindev);
        menu.addAction(&setip);
        menu.exec(QCursor::pos());  //在当前鼠标位置显示

    }
}
//是否已经添加了dev设备。
BOOL MainWindow::dev_is_have(DEV_DATA_INFO dev)
{
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

void MainWindow::foucschange(QObject *object)
{
    qDebug("foucs change !");
}

//void MainWindow::plaint(void)
//{

//    qDebug("show start\n");

//}

void MainWindow::keyPressEvent(QKeyEvent * key)
{
//     qDebug("keyPressEvent start\n");
//     zattachWindowThreadInput(vncpi.dwThreadId);
//     attact_key(vnc_hand);
}

void MainWindow::keyReleaseEvent(QKeyEvent * key)
{
//     qDebug("keyReleaseEvent start\n");
//    zattachWindowThreadInput(vncpi.dwThreadId);
}

//void MainWindow::mousePressEvent(QMouseEvent *mouse)
//{
//     qDebug("mouse press\n");
//}

bool MainWindow::eventFilter(QObject * obj, QEvent * event)
{
    if(obj != NULL && obj == vnctab)
    {
//         if(event->type() == QEvent::MouseButtonPress)
         {
             qDebug("event filter mouse!");

         }
    }
    return false;
}

MainWindow::~MainWindow()
{
    delete ui;
}
