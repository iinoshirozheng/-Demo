#ifndef ARGUMENT_DIALOG_H
#define ARGUMENT_DIALOG_H

#include <QDialog>
#include <QVector>

namespace Ui {
class Argument_Dialog;
}

class Argument_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Argument_Dialog(QWidget *parent = nullptr);
    ~Argument_Dialog();
    void Slot_GetArgumentData( QVector<double> startVel, QVector<double> maxVel, QVector<double> Tacc, QVector<double> Tdec);
    void Slot_GetServoStatus(int ServoON);
    void UpdateStatus();
    void Slot_IsAxisInit(int isAxisInit);


public:
    QVector<double> m_vStartVel;
    QVector<double> m_vMaxVel;
    QVector<double> m_vTacc;
    QVector<double> m_vTdec;
    int m_bAxisInit;


private slots:
    void on_Finished_clicked();

signals:
    void signal_UpdateData(QVector<double> startVel, QVector<double> maxVel, QVector<double> Tacc, QVector<double> Tdec); // TODO:

private:
    Ui::Argument_Dialog *ui;

};

#endif // ARGUMENT_DIALOG_H
