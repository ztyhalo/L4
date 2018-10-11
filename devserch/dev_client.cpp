
#include <stdint.h>
#include <iostream>
#include "crc/crc.h"
#include "devserch/dev_client.h"

using namespace std;



//DEV_CLIENT():
//    DEV_SEARCH(0xfffd, 0xfffe)
//{
//    ;
//}


int ZDEV_CLIENT::get_dev_info(void)
{


    QString detail="";
    QList<QNetworkInterface> list=QNetworkInterface::allInterfaces();
    for(int i=0;i<list.count();i++)
    {
        QNetworkInterface interface=list.at(i);
        detail=detail+tr("设备：")+interface.name()+"\n";
        detail=detail+tr("硬件地址：")+interface.hardwareAddress()+"\n";
        QList<QNetworkAddressEntry> entryList=interface.addressEntries();
        for(int j=0;j<entryList.count();j++)
        {
            QNetworkAddressEntry entry=entryList.at(j);
            detail=detail+"\t"+tr("IP 地址：")+entry.ip().toString()+"\n";
            detail=detail+"\t"+tr("子网掩码：")+entry.netmask().toString()+"\n";
            detail=detail+"\t"+tr("广播地址：")+entry.broadcast().toString()+"\n";
        }
    }

    cout << detail.toStdString() <<endl;
    return 1;


}

int ZDEV_CLIENT::get_dev_info(QString dev)
{
    QList<QNetworkInterface> list=QNetworkInterface::allInterfaces();
    for(int i=0;i<list.count();i++)
    {
        QNetworkInterface interface=list.at(i);
        if(interface.name() == dev)
        {
            string_to_hex(interface.hardwareAddress(), ":",  info.mac, sizeof(info.mac), 16);

            QList<QNetworkAddressEntry> entryList=interface.addressEntries();
//            for(int j=0;j<entryList.count();j++)
            if(entryList.count() > 0)
            {

                QNetworkAddressEntry entry=entryList.at(0);
                string_to_hex(entry.ip().toString(), ".",  info.ip, sizeof(info.ip));
                string_to_hex(entry.netmask().toString(), ".",  info.netmask, sizeof(info.netmask));
            }
        }
    }
    return 0;
}

void ZDEV_CLIENT::get_dev_name(QString dir)
{
    QFile file(dir);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug("open file failed!");
    }
    else
    {
        QByteArray devname = file.readAll();
        int size = sizeof(info.name) > devname.size() ? devname.size() : sizeof(info.name);
        memcpy(info.name, devname.data(), size);
        qDebug("name: %s", info.name);
    }
}

void ZDEV_CLIENT::set_dev_name(DEV_DATA_INFO data)
{
    char name[27];
    memset(name, 0x00, sizeof(name));
    memcpy(name, data.mac, sizeof(data.mac));
    memcpy(name+sizeof(data.mac), data.name, sizeof(data.name));
    qDebug() << "dev tyep " << data.devtype;
    dev_send_data(SET_DEV_NAME, data.devtype, name, DEV_AUTO_DEF_ADDR, 27);

}

void ZDEV_CLIENT::set_ip_info(DEV_DATA_INFO data)
{
//    char ipset[];

//    dev_send_data(data);
    dev_send_data(SET_NET_PARA, data.devtype, (char *)data.mac, DEV_AUTO_DEF_ADDR, 18);

}

int ZDEV_CLIENT::receive_data_process(QByteArray data)
{
    if(data.size() != DEV_SEARCH_FRAME_LEN)
    {
        qDebug("Receive data err!");
        return 1;
    }
    uint16_t crcval = (uint8_t)data[CRC_1_ADDR] ;
    crcval = (crcval << 8) | (uint8_t)data[CRC_2_ADDR];

    qDebug("crc1 %d crc2 %d", (uint8_t)data[CRC_1_ADDR], (uint8_t)data[CRC_2_ADDR]);

    data[CRC_1_ADDR] = 0;
    data[CRC_2_ADDR] = 0;

    uint16_t val = CRC_16((uint8_t *)(data.data()), DEV_SEARCH_FRAME_LEN);
    if(val != crcval)
    {
        qDebug("Receive data crc err %d  %d!", val, crcval);
        return 2;
    }

    uint16_t cmd = data[1];
    cmd = cmd << 8 | data[2];
    switch (cmd)
    {
        case SEARCH_DEV_CMD_ASK:
        {
           qDebug("dev type is 0x%x", (uint8_t)data[5]);
           DEV_DATA_INFO new_dev;
           new_dev.dev_data_init(data);
           emit add_dev_sig(new_dev);
        }
        break;
        case SET_NET_PARA_ASK:
//            set_ip_info(data);
            qDebug("set ip ask!");
            if((uint8_t)data[IP_ASK_MRAK_ADDR] == 0)
            {
                emit ip_set_state(0);
            }
            else
            {
                emit ip_set_state(1);
            }
        break;
        case SET_DEV_NAME_ASK:
            qDebug("name set ok!");
            if((uint8_t)data[IP_ASK_MRAK_ADDR] == 0)
            {
                emit name_set_state(0);
            }
            else
            {
                emit name_set_state(1);
            }
        break;
    default:
        break;

    }
    return 0;

}

void ZDEV_CLIENT::start_skip_dev(void)
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

void ZDEV_CLIENT::ip_set_fail_process(void)
{
    ;
}

void ZDEV_CLIENT::name_set_fail_process(void)
{
    ;
}
