#include <QDesktopWidget>
#include <QRegExpValidator>
#include <QLabel>
#include <QApplication>
#include "ZGroup.h"

ZSquaren::ZSquaren(QWidget *parent):QWidget(parent)
{
    paint=new QPainter;
}

void ZSquaren::setPen(int len)
{
    framlen = len;
}

void ZSquaren::paintEvent(QPaintEvent*)

//paintEvent函数由系统自动调用，用不着我们人为的去调用。
{

     paint->begin(this);
     paint->setPen(QPen(Qt::blue,framlen));
     paint->drawRect(QRect(x(),y(), width()-framlen, height()-framlen));
     qDebug("x %d y %d w %d h %d", x(), y(), width(), height());
     paint->end();
}


void ZGroup::setPen(int len)
{
    framlen = len;
}

void ZGroup::paintEvent(QPaintEvent*)
{

     paint->begin(this);
     paint->setPen(QPen(Qt::blue,framlen));
     paint->drawRect(QRect(0, 0, Z_W - framlen , Z_H - framlen ));
     paint->end();
}

ZGroup::ZGroup(QWidget* pParent /* = 0 */)
: QWidget(pParent)
{
    int i;
    int fram_leng = 1;

    paint=new QPainter;

    int start_x = 0;
    int start_y = 0;
    int swidth = 30;
    int shight = 20;

    setPen(fram_leng);

    Z_W = swidth * 4 + 2 * fram_leng;
    Z_H = shight + 2 * fram_leng;

    resize(Z_W+fram_leng, Z_H+fram_leng);
//    resize(Z_W, Z_H);

    qDebug("start x %d y %d", x(), y());

    start_x += fram_leng;
    start_y += fram_leng;
    for(i = 0; i < 4; i++)
    {

        parabuf[i] = new ZEdit(this);

        parabuf[i]->setGeometry(QRect(start_x, start_y, swidth, shight));
        start_x = start_x + swidth;
        qDebug("start x %d y %d", start_x, start_y);
        connect(parabuf[i], SIGNAL(textChanged(const QString&)), this, SLOT(textchangedslot(const QString&)));
        connect(parabuf[i], SIGNAL(textEdited (const QString&)), this, SLOT(texteditedslot(const QString&)));
    }
    for(i = 0; i < 3; i++)
    {
        parabuf[i]->set_nexttab_edit(parabuf[i+1]);
        QWidget::setTabOrder(parabuf[i], parabuf[i +1]);
    }
    for(i = 3; i > 0; i-- )
    {
         parabuf[i]->set_beforetab_edit(parabuf[i -1]);
    }

     start_x = x() + fram_leng;
     start_y = y() + fram_leng;
    for(int i = 0; i < 3; i++)
    {

        labelbuf[i] = new QLabel(tr(" ."),this);
        start_x = start_x + swidth;
        labelbuf[i]->setGeometry(QRect(start_x-(swidth/10), start_y+1, swidth/5, shight-2));
        labelbuf[i]->setAlignment(Qt::AlignCenter);
    }

}

ZGroup::~ZGroup()
{

}

void ZGroup::textchangedslot(const QString& /*text*/)
{
    QString ipaddr = QString("%1.%2.%3.%4")
            .arg(parabuf[0]->text())
            .arg(parabuf[1]->text())
            .arg(parabuf[2]->text())
            .arg(parabuf[3]->text());

    emit textchanged(ipaddr);
}

bool ZGroup::text_is_empty(void)
{
    for(int i = 0; i <4; i++)
    {
        if(parabuf[i]->text().isEmpty() || parabuf[i]->text().isNull())
            return true;
    }
    return false;
}

void ZGroup::texteditedslot(const QString &/*text*/)
{

    QString ipaddr = QString("%1.%2.%3.%4")
            .arg(parabuf[0]->text())
            .arg(parabuf[1]->text())
            .arg(parabuf[2]->text())
            .arg(parabuf[3]->text());

    emit textedited(ipaddr);
}

bool ZGroup::get_text_is_ok(QString text)
{
     QString qstring_validate = text;

     // IP地址验证
     QRegExp regexp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
     QRegExpValidator regexp_validator(regexp, this);
     int nPos = 0;
     QValidator::State state = regexp_validator.validate(qstring_validate, nPos);

     if(state ==  QValidator::Acceptable)
     {
         return true;
     }
     return false;
}

bool ZGroup::get_text_is_ok(void)
{
    return get_text_is_ok(text());
}

void ZGroup::settext(const QString &text)
{
    QString addrbuf[4];
    QString qstring_validate = text;

    // IP地址验证
    QRegExp regexp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    QRegExpValidator regexp_validator(regexp, this);
    int nPos = 0;
    QValidator::State state = regexp_validator.validate(qstring_validate, nPos);
    // IP合法
    if (state == QValidator::Acceptable)
    {
        QStringList ippartlist = text.split(".");

        int strcount = ippartlist.size();

        for(int i = 0; i < strcount; i++)
        {
            addrbuf[i] = ippartlist.at(i);
            parabuf[i]->setText(addrbuf[i]);
        }

    }
}

QString ZGroup::text()
{
    return QString("%1.%2.%3.%4")
        .arg(parabuf[0]->text())
        .arg(parabuf[1]->text())
        .arg(parabuf[2]->text())
        .arg(parabuf[3]->text());
}

void ZGroup::setStyleSheet(const QString &styleSheet)
{
    for(int i = 0; i < 3; i++)
    {
        parabuf[i]->setStyleSheet(styleSheet);
    }
}
