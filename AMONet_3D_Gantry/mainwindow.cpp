#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    // AMONet_Controller creat in heap, delete when this widget is close
    controller = new AMONet_Controller(this);
    ArgDlg = new Argument_Dialog(this);
    QRegularExpression re("^[0-9]*$"); // TODO:這邊之後要修改
    QRegularExpressionValidator* moveCommandValidator = new QRegularExpressionValidator(re, this);
    ui->X_MoveCommand->setValidator(moveCommandValidator);
    ui->Y_MoveCommand->setValidator(moveCommandValidator);
    ui->Z_MoveCommand->setValidator(moveCommandValidator);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_AMONet_Init_clicked()
{
    if(!controller->CardInitialize()) return;
}


void MainWindow::on_Start_Ring_clicked()
{
    if(!controller->SetStartRing()) return;
}


void MainWindow::on_Axis_Initialize_clicked()
{
    if(!controller->AxisInitialize(3)) return;
    connect(this, &MainWindow::signal_IsAxisInit, ArgDlg, &Argument_Dialog::Slot_IsAxisInit);
    emit signal_IsAxisInit(controller->m_bAxisInit);

    connect(controller, &AMONet_Controller::sendStatus, this, [=]
            (QVector<U16> m_vMotionStatus, QVector<I32> m_vGetCommand, QVector<F64> m_vGetSpeed, QVector<I32> m_vGetPosition)
    {
        if( controller->m_MaxAxis > 0 )
        {
            ui->X_Motion->setText(QString::number(m_vMotionStatus[0]));
            ui->X_Command->setText(QString::number(m_vGetCommand[0]));
            ui->X_Position->setText(QString::number(m_vGetPosition[0]));
            ui->X_Speed->setText(QString::number(m_vGetSpeed[0]));
        }

        if(controller->m_MaxAxis > 1)
        {
            ui->Y_Motion->setText(QString::number(m_vMotionStatus[1]));
            ui->Y_Command->setText(QString::number(m_vGetCommand[1]));
            ui->Y_Position->setText(QString::number(m_vGetPosition[1]));
            ui->Y_Speed->setText(QString::number(m_vGetSpeed[1]));
        }

        if(controller->m_MaxAxis > 2)
        {
            ui->Z_Motion->setText(QString::number(m_vMotionStatus[2]));
            ui->Z_Command->setText(QString::number(m_vGetCommand[2]));
            ui->Z_Position->setText(QString::number(m_vGetPosition[2]));
            ui->Z_Speed->setText(QString::number(m_vGetSpeed[2]));
        }
    } );
}


void MainWindow::on_Servo_clicked()
{
    controller->ServoSwitch();
    if(controller->m_bServoStatus) return ui->Servo->setText("Servo ON");
    else return ui->Servo->setText("Servo OFF");
}

void MainWindow::SetArgumentData()
{
    if(!controller->m_bAxisInit) return;
    controller->SetSpeed(controller->m_vStartVel, controller->m_vMaxVel, controller->m_vTacc, controller->m_vTdec);
    emit signal_SendArgumentData(controller->m_vStartVel, controller->m_vMaxVel, controller->m_vTacc, controller->m_vTdec);

}

void MainWindow::on_Set_Argument_clicked()
{

    connect(this, &MainWindow::signal_ServoStatus, ArgDlg, &Argument_Dialog::Slot_GetServoStatus);
    connect(this, &MainWindow::signal_SendArgumentData, ArgDlg, &Argument_Dialog::Slot_GetArgumentData);
    connect(ArgDlg, &Argument_Dialog::signal_UpdateData, this, [=]
            (QVector<double> startVel, QVector<double> maxVel, QVector<double> Tacc, QVector<double> Tdec)
    {
        controller->m_vStartVel = startVel;
        controller->m_vMaxVel = maxVel;
        controller->m_vTacc = Tacc;
        controller->m_vTdec = Tdec;
        controller->SetSpeed(startVel, maxVel, Tacc, Tdec);
    });

    emit signal_ServoStatus(controller->m_bServoStatus);
    SetArgumentData();
    ArgDlg->setWindowModality(Qt::ApplicationModal); // 專注視窗
    ArgDlg->show();
    ArgDlg->exec();
}

void MainWindow::on_X_Left_clicked()
{
    ui->X_MoveCommand->setFocus(Qt::FocusReason::MouseFocusReason);
    controller->MotionMoveAndWait(AXIS_X, ui->X_MoveCommand->text().toLong() * (-1));
}

void MainWindow::on_X_Right_clicked()
{
    ui->X_MoveCommand->setFocus(Qt::FocusReason::MouseFocusReason);
    controller->MotionMoveAndWait(AXIS_X, ui->X_MoveCommand->text().toLong());
}

void MainWindow::on_X_Stop_clicked()
{
    ui->X_MoveCommand->setFocus(Qt::FocusReason::MouseFocusReason);
    controller->MotionStop(AXIS_X);
}

void MainWindow::on_Y_Left_clicked()
{
    ui->Y_MoveCommand->setFocus(Qt::FocusReason::MouseFocusReason);
    controller->MotionMoveAndWait(AXIS_Y, ui->Y_MoveCommand->text().toLong() * (-1)) ;
}

void MainWindow::on_Y_Right_clicked()
{
    ui->Y_MoveCommand->setFocus(Qt::FocusReason::MouseFocusReason);
    controller->MotionMoveAndWait(AXIS_Y, ui->Y_MoveCommand->text().toLong());
}

void MainWindow::on_Y_Stop_clicked()
{
    ui->Y_MoveCommand->setFocus(Qt::FocusReason::MouseFocusReason);
    controller->MotionStop(AXIS_Y);
}

void MainWindow::on_Z_Left_clicked()
{
    ui->Z_MoveCommand->setFocus(Qt::FocusReason::MouseFocusReason);
    controller->MotionMoveAndWait(AXIS_Z, ui->Z_MoveCommand->text().toLong() * (-1)) ;
}

void MainWindow::on_Z_Right_clicked()
{
    ui->Z_MoveCommand->setFocus(Qt::FocusReason::MouseFocusReason);
    controller->MotionMoveAndWait(AXIS_Z, ui->Z_MoveCommand->text().toLong());
}

void MainWindow::on_Z_Stop_clicked()
{
    ui->Z_MoveCommand->setFocus(Qt::FocusReason::MouseFocusReason);
    controller->MotionStop(AXIS_Z);
}

void MainWindow::on_ALL_STOP_clicked()
{
    controller->MotionStop();
}

void MainWindow::on_X_ResetCMD_clicked()
{
    controller->ResetCommand(AXIS_X);
}

void MainWindow::on_X_ResetPos_clicked()
{
    controller->ResetPosition(AXIS_X);
}

void MainWindow::on_Y_ResetCMD_clicked()
{
    controller->ResetCommand(AXIS_Y);
}

void MainWindow::on_Y_ResetPos_clicked()
{
    controller->ResetPosition(AXIS_Y);
}

void MainWindow::on_Z_ResetCMD_clicked()
{
    controller->ResetCommand(AXIS_Z);
}

void MainWindow::on_Z_ResetPos_clicked()
{
    controller->ResetPosition(AXIS_Z);
}

void MainWindow::on_ResetAllCommand_clicked()
{
    controller->ResetPositionAndCmd();
}

void MainWindow::on_Move_All_clicked()
{
    if(!controller->IsAllMotionStop()) return;
    controller->MotionMove(AXIS_X, ui->X_MoveAllCommand->text().toLong()) ;
    controller->MotionMove(AXIS_Y, ui->Y_MoveAllCommand->text().toLong()) ;
    controller->MotionMove(AXIS_Z, ui->Z_MoveAllCommand->text().toLong()) ;
    controller->WaitAllMotionStop() ;

}


void MainWindow::on_Test_clicked()
{
    if(!controller->IsAllMotionStop()) return;
    double Distance = ui->Test_EditLine->text().toDouble();
    controller->MotionMoveAndWait(AXIS_X, Distance);
    controller->MotionMoveAndWait(AXIS_Y, Distance);
    controller->MotionMoveAndWait(AXIS_Z, Distance);
}

