#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "AMONet_Controller.h"
#include "argument_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum AXIS{ AXIS_X = 0, AXIS_Y, AXIS_Z };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    AMONet_Controller* controller;
    Argument_Dialog* ArgDlg;

    void SetArgumentData();




private slots:


    void on_AMONet_Init_clicked();

    void on_Start_Ring_clicked();

    void on_Axis_Initialize_clicked();

    void on_Servo_clicked();

    void on_Set_Argument_clicked();

    void on_X_Left_clicked();

    void on_X_Right_clicked();

    void on_X_Stop_clicked();

    void on_Y_Left_clicked();

    void on_Y_Right_clicked();

    void on_Y_Stop_clicked();

    void on_Z_Left_clicked();

    void on_Z_Right_clicked();

    void on_Z_Stop_clicked();

    void on_ALL_STOP_clicked();

    void on_X_ResetCMD_clicked();

    void on_X_ResetPos_clicked();

    void on_Y_ResetCMD_clicked();

    void on_Y_ResetPos_clicked();

    void on_Z_ResetCMD_clicked();

    void on_Z_ResetPos_clicked();

    void on_ResetAllCommand_clicked();

    void on_Move_All_clicked();

    void on_Test_clicked();

signals:

    void signal_SendArgumentData( QVector<F64> startVel, QVector<F64> maxVel, QVector<F64> Tacc, QVector<F64> Tdec);
    void signal_ServoStatus(BOOL isServoON );
    void signal_IsAxisInit(BOOL isAxisInit);


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
