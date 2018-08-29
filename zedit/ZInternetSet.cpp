#include <QDesktopWidget>
#include <QRegExpValidator>
#include <QLabel>
#include <QApplication>
#include "ZInternetSet.h"
#include <QComboBox>

//QRect  GetScreenInfo(void)
//{
//   QDesktopWidget* desktopWidget = QApplication::desktop();
//   //获取可用桌面大小
////       QRect deskRect = desktopWidget->availableGeometry();
//   //获取设备屏幕大小
//   QRect screenRect = desktopWidget->screenGeometry();

//   return screenRect;
//}




ZInternetSet::ZInternetSet(QWidget* pParent /* = 0 */)
: QDialog(pParent)
{

    QGroupBox *ipgroup = new QGroupBox(tr("使用下面的IP地址(S):"));
    QLabel *ipinfo = new QLabel(tr("IP地址(I):"));

   QLabel *mskinfo = new QLabel(tr("子网掩码(U)："));
   QLabel *gtinfo = new QLabel(tr("默认网关(D)："));

   ipframe = new ZGroup;
   maskframe = new ZGroup;
   gtframe   = new ZGroup;

   QLineEdit * ipline = new QLineEdit;

   ipline->setFocusPolicy(Qt::NoFocus);
   ipline->setMaximumHeight(4);
//    echoLineEdit->setFocus();
//! [0]

//! [1]
    QGroupBox * dnsGroup = new QGroupBox(tr("使用下面的DNS服务器地址(E):"));

    QLabel * fdnsinfo = new QLabel(tr("首选DNS服务器(P):"));
    QLabel * sdnsinfo = new QLabel(tr("备用DNS服务器(P):"));

    QLineEdit * dnsline = new QLineEdit;

    dnsline->setFocusPolicy(Qt::NoFocus);
    dnsline->setMaximumHeight(4);


//    QLine     * dnsline = new QLine;

    f_dnsframe = new ZGroup;
    s_dnsframe = new ZGroup;

//! [1]





//! [6]
    QGridLayout *ipLayout = new QGridLayout;
    ipLayout->setVerticalSpacing(5);

    ipLayout->setRowMinimumHeight(0, 24);
    ipLayout->setColumnMinimumWidth(1, 124);
    ipLayout->addWidget(ipinfo, 0, 0);
//    echoLayout->addWidget(echoComboBox, 0, 1);
    ipLayout->addWidget(ipframe, 0, 1);

    ipLayout->setRowMinimumHeight(1, 24);
    ipLayout->addWidget(mskinfo, 1,  0);
    ipLayout->addWidget(maskframe, 1, 1);

    ipLayout->setRowMinimumHeight(2, 24);
    ipLayout->addWidget(gtinfo, 2, 0);
    ipLayout->addWidget(gtframe, 2,1);

    ipLayout->addWidget(ipline, 3, 0, 1, 2);
    ipgroup->setLayout(ipLayout);
//! [6]

//! [7]
    QGridLayout *dnsLayout = new QGridLayout;

    dnsLayout->setVerticalSpacing(5);
    dnsLayout->setRowMinimumHeight(0, 24);
    dnsLayout->setColumnMinimumWidth(1, 124);

    dnsLayout->addWidget(fdnsinfo, 0, 0);
    dnsLayout->addWidget(f_dnsframe, 0, 1);

    dnsLayout->setRowMinimumHeight(1, 24);
    dnsLayout->addWidget(sdnsinfo, 1, 0);
    dnsLayout->addWidget(s_dnsframe, 1, 1);


    dnsLayout->addWidget(dnsline, 2, 0, 1, 2);
    dnsGroup->setLayout(dnsLayout);

//! [8]
    QGridLayout *layout = new QGridLayout;

//    layout->setRowStretch(0, 1);
//    layout->setRowStretch(1, 1);

    //button layout
     QGroupBox  * butGroup = new QGroupBox;
     QGridLayout *buttonLayout = new QGridLayout;

     QPushButton * okbutton    = new QPushButton("确定");
     QPushButton * exbutton    = new QPushButton("取消");

     buttonLayout->setColumnMinimumWidth(0, 80);
//      buttonLayout->setColumnStretch(3, 1);

     buttonLayout->addWidget(okbutton, 0 ,1, 1, 1);
     buttonLayout->addWidget(exbutton, 0 ,3, 1, 1);
     butGroup->setLayout(buttonLayout);


    layout->addWidget(ipgroup,  0, 0);
    layout->addWidget(dnsGroup, 1, 0);
    layout->addWidget(butGroup, 2, 0);

    setLayout(layout);

    setWindowTitle(tr("Internet协议"));

    connect(okbutton, SIGNAL(clicked()), this, SLOT(set_ip()));
    connect(exbutton, SIGNAL(clicked()), this, SLOT(exit_ip_set()));

}
void ZInternetSet::ip_init(QString ip, QString netmask, QString gw)
{
    ipframe->settext(ip);
    maskframe->settext(netmask);
    gtframe->settext(gw);
}

void ZInternetSet::set_ip_result(int val)
{
    if(val == 0) //
    {
        QMessageBox msgBox(QMessageBox::Warning, tr("IP设置"),
                           "IP设置成功！", 0, this);
        msgBox.addButton(tr("确定"), QMessageBox::YesRole);

//          buttonClicked(QAbstractButton *button);
        msgBox.exec();
        this->accept();
    }
}

void ZInternetSet::set_ip(void)
{
    if(f_dnsframe->text_is_empty() && s_dnsframe->text_is_empty())
    {
        if(maskframe->get_text_is_ok() && ipframe->get_text_is_ok()
           && gtframe->get_text_is_ok()
//                && f_dnsframe->get_text_is_ok() && s_dnsframe->get_text_is_ok() )                   //ip设置有效
        )
        {


            TCPText txt(ipframe->text(),maskframe->text(), gtframe->text(),
                        f_dnsframe->text(), s_dnsframe->text());

            emit set_ip_text(txt);


        }
    }
    else
    {
        QMessageBox msgBox(QMessageBox::Warning, tr("TCP/IP"),
                           "无效的IP地址。", 0, this);
        msgBox.addButton(tr("确定"), QMessageBox::YesRole);

        msgBox.exec();
    }
}

void ZInternetSet::exit_ip_set(void)
{
    this->accept();
}

ZInternetSet::~ZInternetSet()
{

}









