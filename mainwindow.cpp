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
   //获取可用桌面大小
//       QRect deskRect = desktopWidget->availableGeometry();
   //获取设备屏幕大小
   QRect screenRect = desktopWidget->screenGeometry();

   return screenRect;
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

}

void MainWindow::skipdev(void)
{
    ;
}


void MainWindow::add_sgin_action(void)
{
    Z_ADD_ACTION(login,sgin, "登录", "登录");
    Z_ADD_ACTION(exit,sgin, "注销", "注销");

    connect(login, SIGNAL(triggered()), this, SLOT(rogindev()));
}

void MainWindow::add_para_action(void)
{
    Z_ADD_ACTION(skip, para_skip, "&浏览当前", "浏览当前设备");
//    connect(skip,SIGNAL(tirggered()), this, SLOT(skipdev()));
}

void MainWindow::add_search_action(void)
{
    Z_ADD_ACTION(search_all, dev_search, "&搜索所有", "搜索所有设备");
    connect(search_all, SIGNAL(triggered()), &devsearch, SLOT(start_skip_dev()));
}

void MainWindow::add_pro_action(void)
{
     Z_ADD_ACTION(pro_now, net_pro, "&远程目前", "远程当前设备");
     connect(pro_now, SIGNAL(triggered()), this, SLOT(tightremotedev()));
}


void MainWindow::toolBarFloat(bool topLevel)
{
    qDebug("tool %d",topLevel);
    if(topLevel == 0)
    {
        setCentralWidget(centerzdock);
    }
}

void MainWindow::createDockWindows(void)
{
    QDockWidget *dock = new QDockWidget(tr("设备列表"), this);

    devlist = new QTreeWidget(dock);

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


//    QRect screenRect = centralWidget()->screenGeometry();
//    ui->centralWidget->getContentsMargins();
//    centerdock =  new QDockWidget(tr("窗体"), this);

//    setCentralWidget(centerdock);

//    connect(centerdock,SIGNAL(topLevelChanged(bool)),this,SLOT(toolBarFloat(bool)));

    centerzdock = new ZDockWidget(this, tr("窗体"));
    setCentralWidget(centerzdock);
    connect(centerzdock,SIGNAL(topLevelChanged(bool)),this,SLOT(toolBarFloat(bool)));
//    centerdock->setAllowedAreas(Qt::LeftDockWidgetArea| Qt::RightDockWidgetArea);


    centertab = new QTabWidget(centerzdock);
//    QLabel * filename = new QLabel(tr("File name:"));
//    centertab->addTab(filename, tr("General"));

    centerzdock->setWidget(centertab);
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

void MainWindow::devlistclick(QTreeWidgetItem* item,int val)
{
    qDebug("click val %d", val);


}

void MainWindow::tightremotedev(void)
{

    QString cmd = "C:/Program Files/TightVNC/tvnviewer.exe "
                  " 192.168.1.112";

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
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
       wid = (WId)FindWindow(L"TvnWindowClass", L"Qt for Embedded Linux VNC Server - TightVNC Viewer");
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

    qDebug("wind flag 0x%x!", m_window->flags());
    m_window->setFlags(m_window->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                       | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC
                       | Qt::BypassWindowManagerHint);
    rogtab = QWidget::createWindowContainer(m_window, centertab);
    centertab->addTab(rogtab, tr("连接"));

}
void MainWindow::remotedev(void)
{

    QString cmd = "C:/Program Files/RealVNC/VNC Viewer/vncviewer.exe "
                  " 192.168.1.112";

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
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
       wid = (WId)FindWindow(L"vwr::CDesktopWin", L"192.168.1.112 (Qt for Embedded Linux VNC Server) - VNC Viewer");
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

    qDebug("wind flag 0x%x!", m_window->flags());
    m_window->setFlags(m_window->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                       | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC
                       | Qt::BypassWindowManagerHint);
    rogtab = QWidget::createWindowContainer(m_window, centertab);
    centertab->addTab(rogtab, tr("连接"));

    do {
       Sleep(10);
       m_window->setFlags(m_window->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                          | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC
                          | Qt::BypassWindowManagerHint);
       i++;
    }while(wid == 0 && i < 10000);

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


void MainWindow::rogindev(void)
{
//    QProcess *pro = new QProcess(this);
//    pro->start("/usr/bin/firefox -new-window http://192.168.1.254");
//    QDesktopServices::openUrl(QUrl("http://192.168.1.254"));

//    QWebView *view = new QWebView(this);

//    view->setGeometry(100, 100, 600, 400);

//    view->load(QUrl("http://www.baidu.com/"));
//    view->show();


//    QLabel * filename = new QLabel(tr("File name:"));
//    rogtab = new QWidget(centertab);
//    centertab->addTab(rogtab, tr("登录"));

//    QString cmd = "C:/Windows/system32/calc.exe";

//    QString cmd = "C:/Program Files/Mozilla Firefox/firefox.exe -new-window"
//                  " http://192.168.1.254";
    QString cmd = "C:/Program Files/Internet Explorer/iexplore.exe -k"
                  " http://192.168.1.254";

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
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

//    Sleep(1000);

//    WId wid = (WId)FindWindow(L"CalcFrame", L"计算器");
//    HWND proid = 0;
    WId wid = 0;
    int i = 0;
    do {
       Sleep(100);
//       wid = (WId)FindWindow(L"MozillaWindowClass", L"Mozilla Firefox");
//       wid = (WId)FindWindow(L"Frame Tab",NULL);
       wid = (WId)FindWindow(L"IEFrame", L"http://192.168.1.254/ - Windows Internet Explorer");
//       proid = FindWindow(L"IEFrame", L"http://192.168.1.254/ - Windows Internet Explorer");
       qDebug("proid 0x%x", wid);
//       if(proid != 0)
//            wid = (WId)FindWindowEx(proid, 0, L"Frame Tab",NULL);
       i++;
    }while(wid == 0 && i < 100);
//    if(wid == 0)
//    {
//        do {
//           Sleep(100);
//           wid = (WId)FindWindow(L"MozillaWindowClass", L"MangedSwitch - Mozilla Firefox");
//           i++;
//        }while(wid == 0 && i < 10);
//    }
    if(wid == 0)
    {
        //错误处理
        qDebug("wid 0error!");
        return;
    }

    //WId wid = (WId)FindWindow(L"Rgui Workspace",NULL);
    QWindow *m_window;
    m_window = QWindow::fromWinId(wid);

    qDebug("wind flag 0x%x!", m_window->flags());
    m_window->setFlags(m_window->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                       | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC); //

//     m_window->setFlags((Qt::WindowType)0xffffffff);
//    QWidget *m_widget;
    rogtab = QWidget::createWindowContainer(m_window, centertab);
//    rogtab->setMinimumSize(400, 300);
    centertab->addTab(rogtab, tr("登录"));

//        proshow = new QWidget();
//        proshow->setWindowFlags( Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
//        this->hide();
//      proshow->show();
//    dialg = new QDialog(this->ui->centralWidget);
//    dialg->setGeometry(100,100,400,300);
//    dialg->show();
//    qwiget = new QLineEdit(this->ui->centralWidget);
//    qwiget->setWindowFlags( Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
//    qwiget->setGeometry(100,100,400,300);
//    qwiget->show();
//    proshow = new QWidget(this->ui->centralWidget);
//    proshow->setGeometry(100,100,400,300);
//    proshow->setStyleSheet("background-color:black;");
//    proshow->show();
//    QDesktopServices::openUrl(QUrl("http://192.168.1.254"));

}

void MainWindow::popMenu(const QPoint& point)
{
    QTreeWidgetItem* curItem=devlist->currentItem();  //获取当前被点击的节点
    if(devlist==NULL)return;           //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
    QString wellName = curItem->text(0);
    if(wellName != "wells")
    {
        QAction deleteWell(tr("&登录该设备"),this);//删除井
        QAction reNameWell(tr("&设置ip"),this);//重命名井
        //在界面上删除该item
       connect(&deleteWell, SIGNAL(triggered()), this, SLOT(rogindev()));
//        connect(&reNameWell,SIGNAL(triggered()),this,SLOT(renameWell()));

        QPoint pos;
        QMenu menu(this->devlist);
        menu.addAction(&deleteWell);
        menu.addAction(&reNameWell);
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

void MainWindow::plaint(void)
{


    qDebug("show start\n");
  //   qDebug("rec inter %d\n", (retv.tv_sec - tv.tv_sec)*1000 +(retv.tv_usec - tv.tv_usec)/1000);

}

MainWindow::~MainWindow()
{
    delete ui;
}
