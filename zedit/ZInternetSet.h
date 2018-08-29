#ifndef __Z_INTERNET_SET_H__
#define __Z_INTERNET_SET_H__

#include <QWidget>
#include <QPainter>
#include <QGroupBox>
#include <QGridLayout>
//#include "ipedit/MyIpPartLineEdit.h"
#include "ZGroup.h"
#include <QDialog>
#include <QPushButton>
#include <QMessageBox>

class QLineEdit;
class QLabel;
//class MyIpPartLineEdit;



class TCPText
{
public:
    QString ip;
    QString mask;
    QString gw;
    QString dns;
    QString sdns;
public:
    TCPText(QString i, QString mk,QString g,QString d,QString sd):
        ip(i),mask(mk),gw(g),dns(d),sdns(sd)
    {
        ;
    }

};






class ZInternetSet : public QDialog
{
    Q_OBJECT
public:
    ZInternetSet(QWidget* pParent = 0);
    ~ZInternetSet();


public:
    ZGroup *  maskframe;
    ZGroup *  ipframe;
    ZGroup *  gtframe;

    ZGroup *  f_dnsframe;
    ZGroup *  s_dnsframe;

public slots:
    void set_ip(void);
    void exit_ip_set(void);
    void set_ip_result(int val);

signals:
    void set_ip_text(TCPText text);
public:
    void ip_init(QString ip, QString netmask, QString gw);

};

#endif /*__Z_INTERNET_SET_H__*/
