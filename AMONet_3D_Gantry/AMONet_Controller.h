#ifndef AMONET_CONTROLLER_H
#define AMONET_CONTROLLER_H

#include <QObject>
#include <QTimer>

// include from AMONet //
#include <windows.h>
#include <AMONet.h>
#include <AMONetError.h>
// include from AMONet //

 // #include "thread_pool.h"

#define High_Active 0
#define Low_Active  1


class AMONet_Controller : public QObject
{
    Q_OBJECT
public:
    explicit AMONet_Controller(QObject *parent = nullptr);
    ~AMONet_Controller();



public:
    U16    m_RingNo;       // AMONet Driver Ring NO.
    U16    m_DeviceIP;     // Device IP
    U16    m_MaxAxis;      // Maximun Axis

    QTimer* AMONet_status;

    QVector<F64>    m_vStartVel;     // Set start speed
    QVector<F64>    m_vMaxVel;       // Set maximum velocity
    QVector<F64>    m_vTacc;         // Specified acceleration time in unit of second ( 0 ~ 速度Max 所花時間 )
    QVector<F64>    m_vTdec;         // Specified deceleration time in unit of second ( 速度Max ~ 0 所花時間)

    QVector<I32>    m_vGetCommand;
    QVector<I32>    m_vGetPosition;
    QVector<F64>    m_vGetSpeed;
    QVector<U16>    m_vGetMotion;


    // I32    m_NewPosition;  // Set New Position


    BOOL    m_AlmLogic;     // Alarm logic
    BOOL    m_LtcLogic;     // LCT logic
    BOOL    m_SDLogic;      // SD Logic

    // HANDLE  hEvent;     // MNET communication status // 要使用看門口模式再使用

    BOOL    m_bInit;              // PCI-1202U Card is Initialize or not.
    BOOL    m_bStart;             // Judge Start Ring setting ON or not.
    BOOL    m_bAxisInit;          // AxisInitialize Status
    BOOL    m_bServoStatus;       // Servo state
    BOOL    m_bERC;               // ERC state
    BOOL    m_bRALM;              // servo Driver Alarm state
    BOOL    m_IsRelative;         // is Relative or absolutely
    BOOL    m_OpenStatus;

public:
    BOOL CardInitialize();
    BOOL SetStartRing();
    BOOL AxisInitialize(U16 maxAxis = 3);
    BOOL ServoSwitch();

    // 目前用不到 //
    BOOL AlarmSwitch();
    BOOL ErcSwitch();
    BOOL DriverClose();    
    // 目前用不到 //

    void MotionStop();
    void MotionStop(U16 AxisNUmber);
    void MotionMoveAndWait(U16 AxisNumber ,I32 Distance);
    void WaitMotionStop(U16 AxisNumber);
    void WaitAllMotionStop();
    void SetSpeed( QVector<F64> startVel, QVector<F64> maxVel, QVector<F64> Tacc, QVector<F64> Tdec);
    void SetSpeed( U16 AxisNumber, F64 startVel, F64 maxVel, F64 Tacc, F64 Tdec);
    void ResetPosition(U16 AxisNumber);
    void ResetCommand(U16 AxisNumber);
    void ResetPosition();
    void ResetCommand();
    void ResetPositionAndCmd();
    void MotionMove(U16 AxisNumber ,I32 Distance);
    void MotionMove(I32 Distance);



    BOOL IsMotionStop(U16 AxisNumber);
    BOOL IsAllMotionStop();
    void WaittingToStop(U16 AxisNumber);
    void WaittingAllStop();







private:
    BOOL ErrorHandler(I16 statusRet);


signals:
    void signal_IsStop();
    void signal_IsAllStop();
    void signal_Status_Start(U16 m_RingNo, U16 m_DeviceIP, U16 m_MaxAxis);
    void sendStatus( QVector<U16> m_vMotionStatus, QVector<I32> m_vGetCommand, QVector<F64> m_vGetSpeed, QVector<I32> m_vGetPosition);

};

#endif // AMONET_CONTROLLER_H
