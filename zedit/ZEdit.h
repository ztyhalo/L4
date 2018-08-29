#ifndef __Z_EDIT_H__
#define __Z_EDIT_H__

#include <QLineEdit>

class QWidget;
class QFocusEvent;
class QKeyEvent;

class ZEdit : public QLineEdit
{
    Q_OBJECT
public:
    ZEdit(QWidget *parent = 0, int size = 1);
    ~ZEdit(void);


    void set_nexttab_edit(QLineEdit *nexttab) { next_tab_ = nexttab; }
    void set_beforetab_edit(QLineEdit *nexttab) { before_tab_ = nexttab; }

protected:
    virtual    void focusInEvent(QFocusEvent *e);
    virtual    void keyPressEvent(QKeyEvent *event);

private slots:
    void text_edited(const QString& text);

private:
    QLineEdit *before_tab_;
    QLineEdit *next_tab_;
};

#endif /*__Z_EDIT_H__*/
