#include "dev_search.h"
#include <stdint.h>
#include <iostream>
#include "crc/crc.h"
using namespace std;

void DEV_DATA_INFO::dev_data_init(QByteArray info)
{
    qDebug("info size %d", sizeof(DEV_DATA_INFO)); //未进行crc校验，感觉没必要
    memcpy(&devtype, info.data() + DATA_INFO_START_ADDR, sizeof(DEV_DATA_INFO));
    devtype = info[DEV_TYPE_ADDR];
}
DEV_SEARCH::DEV_SEARCH()
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
    sendport = 0xfffd;
    recvport = 0xfffe;
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

void DEV_SEARCH::processPendingDatagrams()
{

    DEV_DATA_INFO new_dev;
    QByteArray datagram;
    while (udpreceive->hasPendingDatagrams()) {

        if(udpreceive->pendingDatagramSize() != DEV_SEARCH_FRAME_LEN)
            continue;
        datagram.resize(udpreceive->pendingDatagramSize());
        udpreceive->readDatagram(datagram.data(), datagram.size());

        new_dev.dev_data_init(datagram);
        adddevshow(new_dev);
    }


}

void DEV_SEARCH::dev_send_data(QByteArray data)
{

    uint16_t val = CRC_16((uint8_t *)(data.data()), DEV_SEARCH_FRAME_LEN);
    data[CRC_1_ADDR] = val >> 8;
    data[CRC_2_ADDR] = val;
    udpsend->writeDatagram(sendbuf, DEV_SEARCH_FRAME_LEN,
                                     QHostAddress::Broadcast, sendport);
}

void DEV_SEARCH::start_skip_dev(void)
{
    QWaitCondition waitdelay;
    QMutex         mutex;
    uint           dlytime[2] = {100, 40};

    mutex.lock();
    for(int i = 0; i < 2; i++)
    {
        dev_send_data(0x1003, 0x12);
//        send_skip_rate(i*3+1);
        waitdelay.wait(&mutex, dlytime[i]);
        dev_send_data(0x1003, 0x11);
//        send_skip_rate(i*3+2);
        waitdelay.wait(&mutex, dlytime[i]);
        dev_send_data(0x1003, 0x0);
//        send_skip_rate(i*3+3);
        if(i == 0)
            waitdelay.wait(&mutex, dlytime[i+1]);
    }
    mutex.unlock();
}
