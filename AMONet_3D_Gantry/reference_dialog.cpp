#include "reference_dialog.h"
#include "ui_reference_dialog.h"

Reference_Dialog::Reference_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Reference_Dialog)
{
    ui->setupUi(this);
}

Reference_Dialog::~Reference_Dialog()
{
    delete ui;
}
