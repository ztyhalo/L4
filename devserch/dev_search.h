#ifndef DEV_SEARCH_H
#define DEV_SEARCH_H

#include <QtNetwork>
#include <QString>
#include <string>
#include <QByteArray>

#define GET_TEXT(buf, size, form, sign)  { std::string s_text;\
                                            char mbuf[24];    \
                                            memset(mbuf, 0x00, sizeof(mbuf));\
                                            for(int i = 0; i < size -1; i++)\
                                            {\
                                                sprintf(mbuf+strlen(mbuf), #form#sign, buf[i]);\
                                            }\
                                            sprintf(mbuf+strlen(mbuf), #form, buf[size-1]);\
                                            s_text = mbuf; \
                                            qDebug("test test %s",mbuf);\
                                            return s_text;\
                                            }


class DEV_DATA_INFO
{
public:
    quint8 devtype;
    quint8 mac[6];
    quint8 ip[4];
    quint8 netmask[4];
    quint8 gateway[4];
    char   name[21];
    char   devid[21];
public:
    DEV_DATA_INFO()
    {
        memset(this, 0x00, sizeof(DEV_DATA_INFO));
    }
    ~DEV_DATA_INFO()
    {
        ;
    }
    void dev_data_init(QByteArray info);
    std::string get_ip_string(void)
    {
        GET_TEXT(ip,4, %d, .);
    }

    std::string get_gw_string(void)
    {
        GET_TEXT(gateway,4, %d, .);
    }

    std::string get_netmask_string(void)
    {
        GET_TEXT(netmask, 4, %d, .);
    }
    std::string get_mac_string(void)
    {
        GET_TEXT(mac, 6, %02x, :);
    }
    std::string get_dev_name(void)
    {
        return name;
    }
    std::string get_dev_id(void)
    {
        return devid;
    }

    bool mac_is_ok(void)
    {
          quint8 midmac[6];
          memset(midmac, 0x00, sizeof(midmac));
          if(memcmp(midmac, mac, sizeof(midmac)) == 0)
              return false;
          return true;
    }


};

class DEV_SEARCH:public QObject
{
     Q_OBJECT
public:
#define DEV_TYPE_ADDR        5
#define DEV_SEARCH_FRAME_LEN 208
#define CRC_1_ADDR           205
#define CRC_2_ADDR           206
#define DATA_INFO_START_ADDR  71
    enum
    {
        SERVER_DEV_CMD      = 0x1003,
        SEARCH_DEV_CMD_ASK  = 0x1004,
        SET_NET_PARA        = 0x1005,
        SET_NET_PARA_ASK    = 0x1006,
        SET_DEV_NAME        = 0X1007,
        SET_DEV_NAME_ASK    = 0X1008
    };

private:
    quint16         sendport;
    quint16         recvport;
    char            sendbuf[DEV_SEARCH_FRAME_LEN];
public:

    QUdpSocket *    udpsend;
    QUdpSocket *    udpreceive;

public slots:
    void processPendingDatagrams();

signals:
    void adddevsig(DEV_DATA_INFO  devinfo) const;

public:

//    DEV_SEARCH();
    DEV_SEARCH(quint16 sendp = 0xfffd, quint16 recvp = 0xfffe);
    ~DEV_SEARCH();

    void adddevshow(DEV_DATA_INFO  devinfo) const
    {
        emit adddevsig(devinfo);
    }

    void dev_send_data(quint16 cmd, quint8 devtype, char * data = NULL, quint16 addr = 0, quint16 size = 0);
    void send_data_init(quint16 cmd, quint8 devtype, char * data, quint16 addr, quint16 size);



    void dev_send_data(QByteArray data);
    virtual int receive_data_process(QByteArray data);
};


int string_to_hex(QString  source, QString str,  quint8 * buf, quint8 size, int base = 10);

#endif // DEV_SEARCH_H
