#include "mainwindow.h"
#include <QApplication>
#include  <QTextEncoder>
#include "devserch/dev_search.h"
#include <QDesktopServices>
#include<QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("gbk"));
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
    MainWindow w;


//    w.setMaximumSize(,);
//     QObject::connect(&search, SIGNAL(adddevsig(DEV_DATA_INFO)), &w, SLOT(adddevlist(DEV_DATA_INFO)));
//     QObject::connect((&w)->skip, SIGNAL(tirggered(bool )), &search, SLOT(start_skip_dev(bool)));

    w.show();


//    QDesktopServices::openUrl(QUrl("http://192.168.1.254"));
//    search.dev_send_data(0x1003, 0x12);

//    a.installNativeEventFilter(&w);
    return a.exec();
}
