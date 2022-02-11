#ifndef REFERENCE_DIALOG_H
#define REFERENCE_DIALOG_H

#include <QDialog>
#include "AMONet_Controller.h"

namespace Ui {
class Reference_Dialog;
}

class Reference_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Reference_Dialog(QWidget *parent = nullptr);
    ~Reference_Dialog();

private:
    Ui::Reference_Dialog *ui;
};

#endif // REFERENCE_DIALOG_H
