#include "argument_dialog.h"
#include "ui_argument_dialog.h"
#include "mainwindow.h"
#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

Argument_Dialog::Argument_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Argument_Dialog)
{
    ui->setupUi(this);
    m_bAxisInit = false;
    QRegularExpression reInt("^[0-9]*$"); // TODO:這邊之後要修改
    QRegularExpression reDouble
            ("^(([0-9]+\.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*\ .[0-9]+)|([0])|([1-9][0-9]*))$");
    QRegularExpressionValidator* velocityValidator = new QRegularExpressionValidator(reInt, this);
    QRegularExpressionValidator* timeValidator = new QRegularExpressionValidator(reDouble, this);

    ui->X_StartSpeed->setValidator(velocityValidator);
    ui->X_MaxSpeed->setValidator(velocityValidator);
    ui->X_Acceleration->setValidator(timeValidator);
    ui->X_Deceleration->setValidator(timeValidator);

    ui->Y_StartSpeed->setValidator(velocityValidator);
    ui->Y_MaxSpeed->setValidator(velocityValidator);
    ui->Y_Acceleration->setValidator(timeValidator);
    ui->Y_Deceleration->setValidator(timeValidator);

    ui->Z_StartSpeed->setValidator(velocityValidator);
    ui->Z_MaxSpeed->setValidator(velocityValidator);
    ui->Z_Acceleration->setValidator(timeValidator);
    ui->Z_Deceleration->setValidator(timeValidator);

}

Argument_Dialog::~Argument_Dialog()
{
    delete ui;
}


void Argument_Dialog::on_Finished_clicked()
{
    UpdateStatus();
    if(m_bAxisInit) emit signal_UpdateData(m_vStartVel, m_vMaxVel, m_vTacc, m_vTdec);
    this->hide();
}

void Argument_Dialog::Slot_GetArgumentData(QVector<double> startVel, QVector<double> maxVel, QVector<double> Tacc, QVector<double> Tdec)
{
    m_vStartVel = startVel;
    m_vMaxVel = maxVel;
    m_vTacc = Tacc;
    m_vTdec = Tdec;
    qDebug() << startVel.size();

    if(m_vStartVel.size() > 0)
    {
        ui->X_StartSpeed->setText(QString::number(startVel[0]));
        ui->X_MaxSpeed->setText(QString::number(maxVel[0]));
        ui->X_Acceleration->setText(QString::number(Tacc[0]));
        ui->X_Deceleration->setText(QString::number(Tdec[0]));
    }
    if(m_vStartVel.size() > 1)
    {
        ui->Y_StartSpeed->setText(QString::number(startVel[1]));
        ui->Y_MaxSpeed->setText(QString::number(maxVel[1]));
        ui->Y_Acceleration->setText(QString::number(Tacc[1]));
        ui->Y_Deceleration->setText(QString::number(Tdec[1]));
    }

    if(m_vStartVel.size() > 2)
    {
        ui->Z_StartSpeed->setText(QString::number(startVel[2]));
        ui->Z_MaxSpeed->setText(QString::number(maxVel[2]));
        ui->Z_Acceleration->setText(QString::number(Tacc[2]));
        ui->Z_Deceleration->setText(QString::number(Tdec[2]));
    }
}

void Argument_Dialog::Slot_GetServoStatus(int ServoON)
{
    if(ServoON) ui->Servo->setText("Servo ON");
    else ui->Servo->setText("Servo OFF");
}

void Argument_Dialog::UpdateStatus()
{
    if(!m_bAxisInit) return;

    if(m_vStartVel.size() > 0)
    {
        m_vStartVel[0] = ui->X_StartSpeed->text().toDouble();
        m_vMaxVel[0] = ui->X_MaxSpeed->text().toDouble();
        m_vTacc[0] = ui->X_Acceleration->text().toDouble();
        m_vTdec[0] = ui->X_Deceleration->text().toDouble();
    }

    if(m_vStartVel.size() > 1)
    {
        m_vStartVel[1] = ui->Y_StartSpeed->text().toDouble();
        m_vMaxVel[1] = ui->Y_MaxSpeed->text().toDouble();
        m_vTacc[1] = ui->Y_Acceleration->text().toDouble();
        m_vTdec[1] = ui->Y_Deceleration->text().toDouble();
    }

    if(m_vStartVel.size() > 2)
    {
        m_vStartVel[2] = ui->Z_StartSpeed->text().toDouble();
        m_vMaxVel[2] = ui->Z_MaxSpeed->text().toDouble();
        m_vTacc[2] = ui->Z_Acceleration->text().toDouble();
        m_vTdec[2] = ui->Z_Deceleration->text().toDouble();
    }

}

void Argument_Dialog::Slot_IsAxisInit(int isAxisInit)
{
    m_bAxisInit = isAxisInit;
}

