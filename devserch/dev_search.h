#ifndef DEV_SEARCH_H
#define DEV_SEARCH_H

#include <QtNetwork>
#include <QString>
#include <string>
#include <QByteArray>

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
        std::string ipaddr;

        char ipbuf[16];
        sprintf(ipbuf, "%d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);
        ipaddr = ipbuf;
        return ipaddr;
    }

    std::string get_netmask_string(void)
    {
        std::string mask;

        char maskbuf[16];
        sprintf(maskbuf, "%d.%d.%d.%d", netmask[0],netmask[1],netmask[2],netmask[3]);
        mask = maskbuf;
        return mask;
    }
    std::string get_mac_string(void)
    {
        std::string macaddr;
        char macbuf[24];
        sprintf(macbuf, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
        macaddr = macbuf;
        qDebug("mac %s\n", macbuf);
        return macaddr;
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
private:
    quint16         sendport;
    quint16         recvport;
    char            sendbuf[DEV_SEARCH_FRAME_LEN];
public:

    QUdpSocket *    udpsend;
    QUdpSocket *    udpreceive;

public slots:
    void processPendingDatagrams();
    void start_skip_dev(void);
signals:
    void adddevsig(DEV_DATA_INFO  devinfo) const;

signals:
    void image_show(void) const;
public:

    DEV_SEARCH();
    ~DEV_SEARCH();

    void adddevshow(DEV_DATA_INFO  devinfo) const
    {
        emit adddevsig(devinfo);
    }
    void udp_send() const
    {
        emit image_show();
    }
    void dev_send_data(quint16 cmd, quint8 devtype, char * data = NULL, quint16 addr = 0, quint16 size = 0);
    void send_data_init(quint16 cmd, quint8 devtype, char * data, quint16 addr, quint16 size);



    void dev_send_data(QByteArray data);
    int receive_data_process(QByteArray data);
};


int string_to_hex(QString  source, QString str,  quint8 * buf, quint8 size, int base = 10);

#endif // DEV_SEARCH_H
