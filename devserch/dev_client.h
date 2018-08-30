#ifndef __ZDEV_CLIENT_H__
#define __ZDEV_CLIENT_H__


#include <QtNetwork>
#include <QString>
#include <string>
#include "devserch/dev_search.h"
#include <QByteArray>
#include "zedit/ZInternetSet.h"



class ZDEV_CLIENT:public DEV_SEARCH
{
    Q_OBJECT
public:

#define DEV_AUTO_DEF_ADDR   71
#define IP_ASK_MRAK_ADDR    77
#define IP_SET_INFO_LEN     12
#define NAME_SET_ADDR       77
#define NAME_SET_INFO_LEN   42
    DEV_DATA_INFO          info;
    QTimer         *       iptime;
    QTimer         *       nametime;

public:
    ZDEV_CLIENT():DEV_SEARCH(0xfffd, 0xfffe)
    {
        iptime = new QTimer(this);
        connect(iptime, SIGNAL(timeout()), this, SLOT(ip_set_fail_process()));
        nametime = new QTimer(this);
        connect(nametime, SIGNAL(timeout()), this, SLOT(name_set_fail_process()));
    }
    ~ZDEV_CLIENT()
    {
        if(iptime != NULL)
        {
            delete iptime;
        }
        if(nametime != NULL)
        {
            delete nametime;
        }
    }
public slots:
    void start_skip_dev(void);
    void ip_set_fail_process(void);
    void name_set_fail_process(void);
signals:
    void add_dev_sig(DEV_DATA_INFO  devinfo) const;
    void ip_set_state(int state) const;
    void name_set_state(int state) const;

public:
    int receive_data_process(QByteArray data);
    static int get_dev_info(void);
    int get_dev_info(QString dev);
    void get_dev_name(QString dir);
    void set_dev_name(DEV_DATA_INFO);
    void set_ip_info(DEV_DATA_INFO data);
};

#endif // __DEV_CLIENT_H__
