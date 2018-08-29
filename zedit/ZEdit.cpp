
#include <QIntValidator>
#include <QKeyEvent>
#include "ZEdit.h"

ZEdit::ZEdit(QWidget *parent/* = 0*/, int size)
: QLineEdit(parent)
{
    before_tab_ = NULL;
    next_tab_ = NULL;

    this->setMaxLength(3);
    this->setFrame(false);
    this->setAlignment(Qt::AlignCenter);

    QValidator *validator = new QIntValidator(0, 255, this);
    this->setValidator(validator);

    connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(text_edited(const QString&)));
}

ZEdit::~ZEdit(void)
{
}

void ZEdit::focusInEvent(QFocusEvent *e)
{
    this->selectAll();
    QLineEdit::focusInEvent(e);
}

void ZEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Period)
    {
        if (next_tab_)
        {
            next_tab_->setFocus();
            next_tab_->selectAll();
        }
    }
    if(event->key() == Qt::Key_Left)
    {
        if(this->cursorPosition() == 0 &&  before_tab_ != NULL)
        {
            before_tab_->setFocus();
            before_tab_->selectAll();
        }
    }
    if(event->key() == Qt::Key_Right)
    {
        if(this->text().size() == this->cursorPosition()&& next_tab_ != NULL)
        {
            next_tab_->setFocus();
            next_tab_->selectAll();
        }

    }
    QLineEdit::keyPressEvent(event);
}

void ZEdit::text_edited(const QString& text)
{
    QIntValidator v(0, 255, this);
    QString ipaddr = text;
    int pos = 0;
    QValidator::State state = v.validate(ipaddr, pos);
    if (state == QValidator::Acceptable)
    {
        if (ipaddr.size() > 1)
        {
            if (ipaddr.size() == 2)
            {
                int ipnum = ipaddr.toInt();

                if (ipnum > 25)
                {
                    if (next_tab_)
                    {
                        next_tab_->setFocus();
                        next_tab_->selectAll();
                    }
                }
            }
            else
            {
                if (next_tab_)
                {
                    next_tab_->setFocus();
                    next_tab_->selectAll();
                }
            }
        }
    }
}
