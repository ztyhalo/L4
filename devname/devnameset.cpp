
#include "devnameset.h"


DevNameDialog::DevNameDialog(QString name, QWidget* pParent )
:QDialog(pParent)
{

    setWindowTitle(tr("设备名称设置"));
    oldlabel = new QLabel("设备名字");
    oldname = new QLineEdit(name);

    oldname->setReadOnly(true);

    newlabel = new QLabel("设备新名字");
    newname = new QLineEdit;

    QGridLayout * layout = new QGridLayout;

    buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    buttonbox->button(QDialogButtonBox::Ok)->setText("确定");
    buttonbox->button(QDialogButtonBox::Cancel)->setText("取消");

    connect(buttonbox, SIGNAL(accepted()), this, SLOT(set_name()));

    layout->addWidget(oldlabel, 1, 2);
    layout->addWidget(oldname,  2, 2);
    layout->addWidget(newlabel, 3, 2);
    layout->addWidget(newname,  4, 2);

    layout->addWidget(buttonbox, 6, 2);

    this->setLayout(layout);

    newname->setFocus();

}


void  DevNameDialog::set_name(void)
{
    emit click_set_dev(newname->text());
}

void DevNameDialog::name_set_result(int val)
{
    if(val == 0)
    {
        this->accept();
    }
}

