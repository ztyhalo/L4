#ifndef __DEVNAMESET_H__
#define __DEVNAMESET_H__


#include <QtWidgets>
#include <QDockWidget>
#include <QEvent>
#include <QMainWindow>



class DevNameDialog: public QDialog
{
    Q_OBJECT
public:
    DevNameDialog(QString name = "", QWidget* pParent = 0);
public:
    QLabel           *      oldlabel;
    QLineEdit        *      oldname;
    QLabel           *      newlabel;
    QLineEdit        *      newname;
    QDialogButtonBox *      buttonbox;

signals:
    void click_set_dev(QString n);

public slots:
    void set_name(void);
    void name_set_result(int val);

};







#endif // __DEVNAMESET_H__
