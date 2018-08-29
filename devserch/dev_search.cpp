#include "dev_search.h"
#include <stdint.h>
#include <iostream>
#include "crc/crc.h"
using namespace std;


//std::string get__string(quint8 * p, int size)
//{
//    std::string s_text;

//    char ipbuf[];
//    sprintf(ipbuf, "%d.%d.%d.%d", ip[0],ip[1],ip[2],ip[3]);
//    ipaddr = ipbuf;
//    return ipaddr;
//}

int string_to_hex(QString  source, QString str,  quint8 * buf, quint8 size, int base)
{
    int start = 0;
    int addr = 0;
    int i = 0;
    int mark = 0;

    for(i= 0; i < size; i++)
    {
        addr = source.indexOf(str, start);
        if(addr != -1)
        {
            mark = 1;
            QByteArray midarray = source.mid(start, addr -start).toLatin1();
            buf[i] = midarray.toUShort(NULL, base);
            start = addr+1;

        }
        else
        {
            if(mark == 1 && (start+1) <= source.size())
            {
                QByteArray midarray = source.mid(start).toLatin1();
                buf[i] = midarray.toUShort(NULL, base);
                start = addr+1;
            }
            else
                break;
        }
    }
    return 0;
}

void DEV_DATA_INFO::dev_data_init(QByteArray info)
{
    qDebug("info size %d", sizeof(DEV_DATA_INFO)); //未进行crc校验，感觉没必要
    memcpy(&devtype, info.data() + DATA_INFO_START_ADDR, sizeof(DEV_DATA_INFO));
    devtype = info[DEV_TYPE_ADDR];
}
//DEV_SEARCH::DEV_SEARCH()
//{
//    udpsend =  new QUdpSocket(this);
//    if(udpsend == NULL)
//    {
//        qDebug("udpsend create failed!");
//    }
//    udpreceive =  new QUdpSocket(this);
//    if(udpreceive == NULL)
//    {
//        qDebug("udpreceive create failed!");
//    }
//    sendport = 0xfffd;
//    recvport = 0xfffe;
//    memset(sendbuf, 0x00, DEV_SEARCH_FRAME_LEN);
//    udpreceive->bind(recvport, QUdpSocket::ShareAddress);


//    connect(udpreceive, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
//}

DEV_SEARCH::DEV_SEARCH(quint16 sendp, quint16 recvp)
{
    udpsend =  new QUdpSocket(this);
    if(udpsend == NULL)
    {
        qDebug("udpsend create failed!");
    }
    udpreceive =  new QUdpSocket(this);
    if(udpreceive == NULL)
    {
        qDebug("udpreceive create failed!");
    }
    sendport = sendp;
    recvport = recvp;
    memset(sendbuf, 0x00, DEV_SEARCH_FRAME_LEN);
    udpreceive->bind(recvport, QUdpSocket::ShareAddress);


    connect(udpreceive, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}


void DEV_SEARCH::send_data_init(quint16 cmd, quint8 devtype, char * data, quint16 addr, quint16 size)
{

    uint16_t val = 0;
    memset(sendbuf, 0x00, sizeof(sendbuf));

    sendbuf[0] = 0x7f;
    sendbuf[1] = (cmd>>8);
    sendbuf[2] = cmd;
    sendbuf[3] = 0x00;
    sendbuf[4] = 0xd0;
    sendbuf[5] = devtype;
    sendbuf[6] = 0x06;

    if(data != NULL && size > 0){
        memcpy(&sendbuf[addr], data, size);
    }

    sendbuf[DEV_SEARCH_FRAME_LEN -1] = 0x0d;

    val = CRC_16((uint8_t *)(&sendbuf[0]), DEV_SEARCH_FRAME_LEN);
    qDebug("crc is %d", val);

    sendbuf[CRC_1_ADDR] = val >> 8;
    sendbuf[CRC_2_ADDR] = val;

}

DEV_SEARCH::~DEV_SEARCH()
{
    if(udpsend != NULL)
        delete udpsend;
    if(udpreceive != NULL)
        delete udpreceive;
}

void DEV_SEARCH::dev_send_data(quint16 cmd, quint8 devtype, char * data, quint16 addr, quint16 size)
{
    send_data_init(cmd,devtype, data, addr, size);
    udpsend->writeDatagram(sendbuf, DEV_SEARCH_FRAME_LEN,
                                     QHostAddress::Broadcast, sendport);
}

//void DEV_SEARCH::processPendingDatagrams()
//{

//    DEV_DATA_INFO new_dev;
//    QByteArray datagram;
//    while (udpreceive->hasPendingDatagrams()) {

//        if(udpreceive->pendingDatagramSize() != DEV_SEARCH_FRAME_LEN)
//            continue;
//        datagram.resize(udpreceive->pendingDatagramSize());
//        udpreceive->readDatagram(datagram.data(), datagram.size());

//        new_dev.dev_data_init(datagram);
//        adddevshow(new_dev);
//    }


//}

void DEV_SEARCH::dev_send_data(QByteArray data)
{

    data[CRC_1_ADDR] = 0;
    data[CRC_2_ADDR] = 0;

    uint16_t val = CRC_16((uint8_t *)(data.data()), DEV_SEARCH_FRAME_LEN);
    data[CRC_1_ADDR] = val >> 8;
    data[CRC_2_ADDR] = val;
    udpsend->writeDatagram(data, QHostAddress::Broadcast, sendport);
}

void DEV_SEARCH::processPendingDatagrams()
{

//    DEV_DATA_INFO new_dev;
    QByteArray datagram;
    while (udpreceive->hasPendingDatagrams()) {

        datagram.resize(udpreceive->pendingDatagramSize());
        udpreceive->readDatagram(datagram.data(), datagram.size());
    }
    receive_data_process(datagram);

}

//void DEV_SEARCH::processPendingDatagrams()
//{
//    qDebug("receive data");
//}

int DEV_SEARCH::receive_data_process(QByteArray data)
{
    return 1;
}


