#ifndef __Z_GROUP_H__
#define __Z_GROUP_H__

#include <QWidget>
#include <QPainter>
#include <QGroupBox>
#include <QGridLayout>
#include "ZEdit.h"
#include <QLabel>

class ZSquaren:public QWidget
{
 Q_OBJECT
public:
 ZSquaren(QWidget *parent = 0);
 void setPen(int len);
 private:
 void paintEvent(QPaintEvent*);
 QPainter *paint;
 int framlen;
};

class ZGroup : public QWidget
{
    Q_OBJECT
public:
    ZGroup(QWidget* pParent = 0);
    ~ZGroup();
     void setPen(int len);
    void settext(const QString &text);
    QString text();
    void setStyleSheet(const QString &styleSheet);
    bool get_text_is_ok(QString text);
    bool get_text_is_ok(void);
    bool text_is_empty(void);

signals:
    void textchanged(const QString& text);
    void textedited(const QString &text);

private slots:
    void textchangedslot(const QString& text);
    void texteditedslot(const QString &text);
private:
void paintEvent(QPaintEvent*);


private:
    int      Z_W;
    int      Z_H;
    QPainter *paint;
    int framlen;
    ZEdit  * parabuf[4];
    QLabel * labelbuf[3];
};

#endif /*__Z_GROUP_H__*/
