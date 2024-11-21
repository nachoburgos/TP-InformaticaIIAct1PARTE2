#include "portcfg.h"
#include "ui_portcfg.h"

portCfg::portCfg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::portCfg)
{
    ui->setupUi(this);
}

portCfg::~portCfg()
{
    delete ui;
}
