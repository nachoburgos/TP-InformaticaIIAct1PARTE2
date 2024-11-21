#ifndef PORTCFG_H
#define PORTCFG_H

#include <QDialog>

namespace Ui {
class portCfg;
}

class portCfg : public QDialog
{
    Q_OBJECT

public:
    explicit portCfg(QWidget *parent = 0);
    ~portCfg();
    Ui::portCfg *ui;

private:
};

#endif // PORTCFG_H
