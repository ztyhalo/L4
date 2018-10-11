
#include "zdock.h"


ZDockWidget::ZDockWidget(QMainWindow * win, const QString &title, QWidget *parent, Qt::WindowFlags flags):
    QDockWidget(title,parent,flags)
{
    wins_p = win;
}
ZDockWidget::ZDockWidget(QMainWindow * win, QWidget *parent, Qt::WindowFlags flags ):QDockWidget(parent, flags)
{
    wins_p = win;
}

void ZDockWidget::mouseDoubleClickEvent (QMouseEvent * ev)
{
    qDebug("z dock mouse double 0x%x!", ev->button());
    if(ev->buttons() ==  Qt::RightButton)
    {
        qDebug("z dock double!");
    }
}

bool ZDockWidget::event(QEvent *event)
{
    if(event->type() == QEvent::NonClientAreaMouseButtonDblClick)
    {
        qDebug("z event process!");
        if(wins_p != NULL)
        {
            if(isFloating())
            {
                qDebug("dock float!");
//                wins_p->addDockWidget(Qt::AllDockWidgetAreas, this);
                setFloating(FALSE);
//                wins_p->setCentralWidget(this);
            }

        }
//        return TRUE;

    }
    return QDockWidget::event(event);
}

void DoublieKeyWidget::mouseDoubleClickEvent (QMouseEvent * ev)
{
    qDebug("DoublieKeyWidget mouse double 0x%x!", ev->button());
    if(ev->buttons() ==  Qt::RightButton)
    {
        qDebug("DoublieKeyWidget double!");
    }
}

void kill_w_process(PROCESS_INFORMATION proid)
{
    if (proid.hProcess && proid.hThread )
    {
        qDebug("kill process!!!");
        DWORD dwEC = 0;

        BOOL b = GetExitCodeProcess(proid.hProcess,   &dwEC);

        if (b)
        {
            qDebug("termin process!!!");
            TerminateProcess( proid.hProcess, dwEC );
        }

        // Close process and thread handles.
        CloseHandle( proid.hProcess );
        CloseHandle( proid.hThread );
    }
}

int ZProcessWidget::creat_process(QWidget *parent)
{

    if(cmd.isEmpty())
    {
        qDebug("cmd is empty!");
        return 0;
    }
    STARTUPINFO si;

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
    if(bRet == 0)
    {
        qDebug("vnc fail!");
        return 0;
    }

     wid = 0;
    int i = 0;

//    do
//    {
//       Sleep(100);
//       wid = (WId)FindWindow(class_n.toStdWString().c_str(),  w_name.toStdWString().c_str());
//       i++;
//    }
//    while(wid == 0 && i < 10000);


    while(wid == 0 && i < 100)
    {
        QEventLoop eventloop;

        QTimer::singleShot(100, &eventloop, SLOT(quit()));
        eventloop.exec();
        wid = (WId)FindWindow( class_n.toStdWString().c_str(),  w_name.toStdWString().c_str());
    }

    if(wid == 0)
    {
        qDebug("wid 0 error!");
        return 0;
    }
    qwin_p = QWindow::fromWinId(wid);

    qwin_p->setFlags(qwin_p->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                       | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC
                       | Qt::BypassWindowManagerHint);
    qwid_p = QWidget::createWindowContainer(qwin_p, parent);

    qwid_p->setFocusPolicy(Qt::WheelFocus);
//    qwid_p->activateWindow();

    return 1;
}
int ZProcessWidget::restart_process(QString w, QString c, QWidget *parent)
{
    WId wid = 0;
//    int i = 0;

    while(wid == 0)
    {
        QEventLoop eventloop;

        QTimer::singleShot(100, &eventloop, SLOT(quit()));
        eventloop.exec();
        wid = (WId)FindWindow( c.toStdWString().c_str(), w.toStdWString().c_str());
    }

    if(wid == 0)
    {
        qDebug("wid 0 error!");
        return 0;
    }
    else
        qDebug() << "find wid!";

    delete qwin_p;
    delete qwid_p;


    qwin_p = QWindow::fromWinId(wid);

    qwin_p->setFlags(qwin_p->flags() | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint
                       | Qt::WindowType_Mask | Qt::MSWindowsFixedSizeDialogHint | Qt::MSWindowsOwnDC
                       | Qt::BypassWindowManagerHint);
    qwid_p = QWidget::createWindowContainer(qwin_p, parent);

    qwid_p->setFocusPolicy(Qt::WheelFocus);
    return 1;
}

int ZMenu::add_action(QString n, QString text, bool en)
{
    ZAction ac(n);

    ac.act = new QAction(n, parent);
    if(text.isEmpty())
        ac.act->setStatusTip(n);
    else
        ac.act->setStatusTip(text);
    menu_p->addAction(ac.act);
    action.append(ac);
    ac.act->setEnabled(en);
    return 1;
}



QAction * ZMenu::get_action(QString n)
{
    QVector<ZAction>::iterator iter;
    for(iter = action.begin(); iter != action.end(); iter++)
    {
       if(iter->na == n)
           return iter->act;
    }
    return NULL;
}

void ZMenu::action_enable(bool en)
{
    QVector<ZAction>::iterator iter;
    for(iter = action.begin(); iter != action.end(); iter++)
    {
        iter->act->setEnabled(en);
    }

}
