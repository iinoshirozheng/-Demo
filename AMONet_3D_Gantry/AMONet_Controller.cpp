#include "AMONet_Controller.h"
#include <QMessageBox>
#include <QEventLoop>


AMONet_Controller::AMONet_Controller( QObject *parent) : QObject(parent)
{

    // 看們狗模式 需要用在打開
    // hEvent = CreateEventA(NULL, FALSE, FALSE, "UserEvent");

    m_MaxAxis = 3;      // 軸的數量
    m_RingNo = 0;       // Ring NO.0
    m_DeviceIP = 0;     // Device IP = 0

    m_AlmLogic = High_Active; // 0
    m_LtcLogic = Low_Active ; // 1
    m_SDLogic  = Low_Active ; // 1

    m_bInit = FALSE;
    m_bStart = FALSE;
    m_bAxisInit = FALSE ;
    m_bServoStatus = FALSE;
    m_bERC   = FALSE;
    m_bRALM  = FALSE;
    m_IsRelative = TRUE;
    m_OpenStatus = FALSE;

    AMONet_status = new QTimer(this);
    AMONet_status->setTimerType(Qt::PreciseTimer);
    AMONet_status->setInterval(50);
    connect(AMONet_status, &QTimer::timeout, this,[=](){
        for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
        {
            _mnet_m4_motion_done(m_RingNo, m_DeviceIP, AxisNumber, m_vGetMotion[AxisNumber]);
            _mnet_m4_get_command(m_RingNo, m_DeviceIP, AxisNumber, m_vGetCommand[AxisNumber]);
            _mnet_m4_get_current_speed(m_RingNo, m_DeviceIP, AxisNumber, m_vGetSpeed[AxisNumber]);
            _mnet_m4_get_position(m_RingNo, m_DeviceIP, AxisNumber, m_vGetPosition[AxisNumber]);
        }
        emit sendStatus( m_vGetMotion, m_vGetCommand, m_vGetSpeed, m_vGetPosition);
    });

}

AMONet_Controller::~AMONet_Controller()
{
    AMONet_status->stop();
    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        // Set servo Driver ON
        // I16 status= _mnet_m4_set_svon( U16 RingNo, U16 DeviceIP, U16 AxisNo, U16 ON_OFF)
        _mnet_m4_sd_stop(m_RingNo, m_DeviceIP, AxisNumber);
        _mnet_m4_set_svon(m_RingNo, m_DeviceIP, AxisNumber, 0);
    }
    _mnet_stop_ring(m_RingNo);
    _mnet_close();
}

BOOL AMONet_Controller::ErrorHandler(I16 statusRet)
{
    if (!ADM_SUCCESS(statusRet))
    {
        QString errorMessage = "Operation failed with error code: " + QString::number(statusRet);
        QMessageBox::critical(NULL, "ERROR : Set RingNo. failed !" , errorMessage);
        return FALSE;
    }
    return TRUE;
}

BOOL AMONet_Controller::CardInitialize()
{
    m_bInit = _mnet_initial();

    if (!m_bInit)
    {
        QMessageBox::critical(NULL, "ERROR : Card Initialize fault !" , "No AMONet device has found.");
        return FALSE;
    }
    return TRUE;
}

BOOL AMONet_Controller::SetStartRing()
{
    if (!m_bInit)
    {
        QMessageBox::critical(NULL, "ERROR : Card Initialize fault !" ,
                              "AMONet device has not been completed\n    Please perform \"AMONet Init\" first.");
        return FALSE;
    }

     // Set MNET Baudrate
     // Baudrate = 0:  2.5 MBps
     // Baudrate = 1:  5   MBps
     // Baudrate = 2:  10  MBps
     // Baudrate = 3:  20  MBps
     // Set the baudrate of ring communication
     // Format : I16 status=_mnet_set_ring_config(U16 RingNo, U16 BaudRate)
     if(!ErrorHandler( _mnet_set_ring_config(m_RingNo, 3) )) return FALSE;


     // Soft reset ring to the initial status
     // Format : I16 status =_mnet_reset_ring (U16 RingNo)
     if(!ErrorHandler( _mnet_reset_ring(m_RingNo) )) return FALSE;


     // Start ring communication
     // Format : I16 status =_mnet_start_ring(U16 RingNo)
     if(!ErrorHandler( _mnet_start_ring(m_RingNo) )) return FALSE;


     // Set the ring continue error and error rate
     // Format : I16 status =_mnet_set_ring_quality_param(U16 RingNo, U16 ContinueErr, U16 ErrRate)
     // ContinueErr U16 : Range 1~0xffff
     //  ErrRate U16 : Error counter per 1k rounds
     if(!ErrorHandler( _mnet_set_ring_quality_param( m_RingNo, 100, 800 ) )) return FALSE;


     // Enable watchdog to monitor MNET communication status
     // Format : I16 status = _mnet_enable_soft_watchdog(U16 RingNo, HANDLE *User_hEvent)
     // if(!ErrorHandler( _mnet_enable_soft_watchdog( m_RingNo, &hEvent ) )) return FALSE;
     // 看們狗模式 需要用在打開
     m_bStart = TRUE; // judge this function is successfully set Ring or not.
     return TRUE;
}

BOOL AMONet_Controller::AxisInitialize( U16 maxAxis )
{
    m_MaxAxis = maxAxis ;

    if( !(1 <= m_MaxAxis && m_MaxAxis <= 4) )
    {
        QMessageBox::critical(NULL, "ERROR : Wrong Axis information !" ,"Variable maxAxis must be 1 ~ 4 ");
        return FALSE;
    }

    if(!m_bStart)
    {
        QMessageBox::critical(NULL, "ERROR : Set Start Ring fault !" ,
                              "Setting Start Ring has not been completed\n    Please perform \"Start Ring\" first.");
        return FALSE;
    }

    // Initial axis motion module
    if(!ErrorHandler( _mnet_m4_initial( m_RingNo, m_DeviceIP ) )) return FALSE;

    //
    // Set Motion module configuration
    //


    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        if(!m_bAxisInit)
        {
            m_vStartVel.append(3000);  // Start velocity
            m_vMaxVel.append(15000);   // Max velocity
            m_vTacc.append(0.5);       // 0.5 sec from 0 to Max velocity
            m_vTdec.append(0.5);       // 0.5 sec from Max to 0 velocity

            m_vGetCommand.append(0);
            m_vGetPosition.append(0);
            m_vGetSpeed.append(0);
            m_vGetMotion.append(0);
        }
        else
        {
            m_vStartVel[AxisNumber] = 3000;  // Start velocity
            m_vMaxVel[AxisNumber] = 15000;  // Max velocity
            m_vTacc[AxisNumber] = 0.5;     // 0.5 sec from 0 to Max velocity
            m_vTdec[AxisNumber] = 0.5;       // 0.5 sec from Max to 0 velocity

            m_vGetCommand[AxisNumber] = 0;
            m_vGetPosition[AxisNumber] = 0;
            m_vGetSpeed[AxisNumber] = 0;
            m_vGetMotion[AxisNumber] = 0;
        }



        // setting of encoderfeedback pulse input mode
        // 設定Ecoder輸入模式(0:1P mode; 2:2P mode ,  0:正轉; 1:反轉)
        if(!ErrorHandler( _mnet_m4_set_pls_iptmode( m_RingNo, m_DeviceIP, AxisNumber, 2, 0 ) )) return FALSE;

        // Set pulse command output mode
        //設定pluse命命輸出模式(0:1P mode; 7:2P mode)
        if(!ErrorHandler( _mnet_m4_set_pls_outmode( m_RingNo, m_DeviceIP, AxisNumber, 7) )) return FALSE;

        // Set the counters input source
        // Counter source ( 0:External Feedback; 1:Command pulse )
        if(!ErrorHandler( _mnet_m4_set_feedback_src( m_RingNo, m_DeviceIP, AxisNumber, 0 ) )) return FALSE;

        // Set ERC logic and timing
        // U16 status=_mnet_m4_set_erc( U16 RingNo, U16 DeviceIP, U16 AxisNo, U16 erc_logic,U16 erc_on_time, U16 erc_off_time)
        if(!ErrorHandler( _mnet_m4_set_erc( m_RingNo, m_DeviceIP, AxisNumber, 1, 6, 1 ) )) return FALSE;

        // Set a trapezoidal Velocity profile
        //設定梯形速率表(起始速度，最大速度，加速度，減速度)
        // TODO: update and new the variable start_val max_val
        if(!ErrorHandler( _mnet_m4_set_tmove_speed( m_RingNo, m_DeviceIP, AxisNumber
                                                    , m_vStartVel[AxisNumber], m_vMaxVel[AxisNumber],
                                                    m_vTacc[AxisNumber], m_vTdec[AxisNumber] ) )) return FALSE;

        // Reset The Command counter Value To Zero.
        // 重要! 之後可能會用到 重製command
        if(!ErrorHandler( _mnet_m4_reset_command( m_RingNo, m_DeviceIP, AxisNumber ) )) return FALSE;

        // Reset The Feedback counter Value To Zero.
        // 重要! 之後可能會用到 重製position
        if(!ErrorHandler( _mnet_m4_reset_position( m_RingNo, m_DeviceIP, AxisNumber ) )) return FALSE;

        // ////////////////////////////////////////////////////////////////////////////////////////////////// //
        // 此部分為邏輯電路 如需更改或增加可另外寫函式更改，可參考以下函數

        // Set latch counter logic, High active
        // I16 status=_mnet_m4_set_ltc_logic( U16 RingNo, U16 DeviceIP , U16 AxisNo,  U16 ltc_logic)
        // Set Latch Logic, when the Latch is triggered, the command counter, feedback counter and error counter will be latched.
        // Call _mnet_m4_get_latch_data to get the latched data.
        if(!ErrorHandler( _mnet_m4_set_ltc_logic( m_RingNo, m_DeviceIP, AxisNumber, m_LtcLogic ) )) return FALSE;


        // Set SD Logic And Operating Mode
        // I16 status=_mnet_m4_set_sd( U16 RingNo, U16 DeviceIP, U16 AxisNo, I16 enable,I16 sd_logic, I16 sd_latch, I16 sd_mode)
        if(!ErrorHandler( _mnet_m4_set_sd( m_RingNo, m_DeviceIP, AxisNumber, 0, m_SDLogic, 0, 0 ) )) return FALSE;


        // Set alarm logic and operating mode
        // I16 status=_mnet_m4_set_alm( U16 RingNo, U16 DeviceIP, U16 AxisNo, U16 alm_logic, U16 alm_mode)
        //設定alm logic和運作模式(第四格:高態(0)，低態(1)，若用伺服馬達選0，步進馬達選1) // 這邊要改
        if(!ErrorHandler( _mnet_m4_set_alm( m_RingNo, m_DeviceIP, AxisNumber, m_AlmLogic, 0 ) )) return FALSE;

    }

    if(!AMONet_status->isActive()) AMONet_status->start();
    m_bAxisInit = TRUE;
    return TRUE;
}

BOOL AMONet_Controller::ServoSwitch()
{
    if(!m_bAxisInit)
    {
        QMessageBox::critical(NULL, "ERROR : Axis Initialize fault !" ,
                              "Axis Initialize has not been completed\n    Please perform \"Axis Initialize\" first.");
        return FALSE;
    }

    m_bServoStatus = !m_bServoStatus;
    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        // Set servo Driver ON
        // I16 status= _mnet_m4_set_svon( U16 RingNo, U16 DeviceIP, U16 AxisNo, U16 ON_OFF)
        if(!ErrorHandler( _mnet_m4_set_svon(m_RingNo, m_DeviceIP, AxisNumber, m_bServoStatus) )) return FALSE;
    }
    return m_bServoStatus;
}

BOOL AMONet_Controller::AlarmSwitch()
{
    if(!m_bServoStatus)
    {
        QMessageBox::critical(NULL, "ERROR : Servo is not open !" ,
                              "Servo has not been opened\n    Please perform \"Servo On\" first.");
        return FALSE;
    }

    m_bRALM = !m_bRALM;
    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        // Set servo Driver ON
        // I16 status= _mnet_m4_set_svon( U16 RingNo, U16 DeviceIP, U16 AxisNo, U16 ON_OFF)
        if(!ErrorHandler( _mnet_m4_set_ralm(m_RingNo, m_DeviceIP, AxisNumber, m_bRALM))) return FALSE;
    }
    return m_bRALM;
}

BOOL AMONet_Controller::ErcSwitch()
{
    if(!m_bServoStatus)
    {
        QMessageBox::critical(NULL, "ERROR : Servo is not open !" ,
                              "Servo has not been opened\n    Please perform \"Servo On\" first.");
        return FALSE;
    }

    m_bERC = !m_bERC;
    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        // Set servo Driver ON
        // I16 status= _mnet_m4_set_svon( U16 RingNo, U16 DeviceIP, U16 AxisNo, U16 ON_OFF)
        if(!ErrorHandler( _mnet_m4_set_erc_on(m_RingNo, m_DeviceIP, AxisNumber, m_bERC))) return FALSE;
    }
    return m_bERC;
}

void AMONet_Controller::ResetPositionAndCmd()
{
    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        // Set servo Driver ON
        // I16 status= _mnet_m4_set_svon( U16 RingNo, U16 DeviceIP, U16 AxisNo, U16 ON_OFF)
        if(!ErrorHandler( _mnet_m4_reset_command(m_RingNo, m_DeviceIP, AxisNumber))) return ;
        if(!ErrorHandler( _mnet_m4_reset_position(m_RingNo, m_DeviceIP, AxisNumber))) return ;
    }
}

BOOL AMONet_Controller::DriverClose()
{
    if(!ErrorHandler( _mnet_stop_ring(m_RingNo))) return FALSE;
    if(!ErrorHandler( _mnet_close())) return FALSE;
    return TRUE;
}

void AMONet_Controller::MotionStop()
{
    if(IsAllMotionStop()) return ;
    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        // Set servo Driver ON
        // I16 status= _mnet_m4_set_svon( U16 RingNo, U16 DeviceIP, U16 AxisNo, U16 ON_OFF)
        if(!ErrorHandler( _mnet_m4_sd_stop(m_RingNo, m_DeviceIP, AxisNumber))) return ;
    }

}

void AMONet_Controller::MotionStop(U16 AxisNumber)
{
    if(IsMotionStop(AxisNumber)) return ;
    // Set servo Driver ON
    // I16 status= _mnet_m4_set_svon( U16 RingNo, U16 DeviceIP, U16 AxisNo, U16 ON_OFF)
    if(!ErrorHandler( _mnet_m4_sd_stop(m_RingNo, m_DeviceIP, AxisNumber))) return ;
}

void AMONet_Controller::MotionMove(U16 AxisNumber ,I32 Distance)
{
    if(IsMotionStop(AxisNumber))
    {
        if(m_IsRelative)
        {
            if(!ErrorHandler( _mnet_m4_start_r_move(m_RingNo, m_DeviceIP, AxisNumber, Distance))) return ;
        }

        else
        {
            if(!ErrorHandler( _mnet_m4_start_a_move(m_RingNo, m_DeviceIP, AxisNumber, Distance))) return ;
        }
    }
}

void AMONet_Controller::MotionMove(I32 Distance)
{
    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        if(IsMotionStop(AxisNumber)) MotionMove(AxisNumber, Distance);
    }
}

void AMONet_Controller::MotionMoveAndWait(U16 AxisNumber, I32 Distance)
{
    if(!IsMotionStop(AxisNumber)) return;    
    MotionMove(AxisNumber, Distance);
    WaitMotionStop(AxisNumber);   
}


void AMONet_Controller::SetSpeed(QVector<F64> startVel, QVector<F64> maxVel, QVector<F64> Tacc, QVector<F64> Tdec)
{
    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        if( startVel[AxisNumber] < 0 || maxVel[AxisNumber] < 0 )
        {
            QMessageBox::critical(NULL, tr("ERROR : Setting speed error !") ,
                                  tr("Invalid speed setting!"));
            return ;
        }
        if(!ErrorHandler( _mnet_m4_set_tmove_speed(m_RingNo, m_DeviceIP, AxisNumber,
                                                   startVel[AxisNumber], maxVel[AxisNumber],
                                                   Tacc[AxisNumber], Tdec[AxisNumber]))) return ;
    }
}

void AMONet_Controller::SetSpeed(U16 AxisNumber, F64 startVel, F64 maxVel, F64 Tacc, F64 Tdec)
{
    if( startVel < 0 || maxVel < 0 )
    {
        QMessageBox::critical(NULL, tr("ERROR : Setting speed error !") ,
                              tr("Invalid speed setting!"));
        return ;
    }
    if(!ErrorHandler( _mnet_m4_set_tmove_speed(m_RingNo, m_DeviceIP, AxisNumber, startVel, maxVel, Tacc, Tdec))) return ;
}

void AMONet_Controller::ResetPosition(U16 AxisNumber)
{
    if(!ErrorHandler( _mnet_m4_reset_position(m_RingNo, m_DeviceIP, AxisNumber))) return ;
}

void AMONet_Controller::ResetCommand(U16 AxisNumber)
{
    if(!ErrorHandler( _mnet_m4_reset_command(m_RingNo, m_DeviceIP, AxisNumber))) return ;
}

void AMONet_Controller::ResetCommand()
{
    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        // Set servo Driver ON
        // I16 status= _mnet_m4_set_svon( U16 RingNo, U16 DeviceIP, U16 AxisNo, U16 ON_OFF)
        if(!ErrorHandler( _mnet_m4_reset_command(m_RingNo, m_DeviceIP, AxisNumber))) return ;
    }
}

void AMONet_Controller::ResetPosition()
{
    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        // Set servo Driver ON
        // I16 status= _mnet_m4_set_svon( U16 RingNo, U16 DeviceIP, U16 AxisNo, U16 ON_OFF)
        if(!ErrorHandler( _mnet_m4_reset_position(m_RingNo, m_DeviceIP, AxisNumber))) return ;
    }
}

void AMONet_Controller::WaitMotionStop(U16 AxisNumber)
{

    if( IsMotionStop(AxisNumber) == FALSE )
    {
        QTimer wait;
        wait.setTimerType(Qt::PreciseTimer);

        QEventLoop loop;

        connect(this, &AMONet_Controller::signal_IsStop, &loop, &QEventLoop::quit);
        connect(this, &AMONet_Controller::signal_IsStop, &wait, &QTimer::stop);
        connect(&wait, &QTimer::timeout,this,[=](){
            WaittingToStop(AxisNumber);
        });
        wait.start(1);

        loop.exec();
    }
    return ;
}

void AMONet_Controller::WaitAllMotionStop()
{

    if( IsAllMotionStop() == FALSE )
    {
        QTimer waitAll;
        waitAll.setTimerType(Qt::PreciseTimer);

        QEventLoop loop;

        connect(this, &AMONet_Controller::signal_IsAllStop, &loop, &QEventLoop::quit);
        connect(this, &AMONet_Controller::signal_IsAllStop, &waitAll, &QTimer::stop);
        connect(&waitAll, &QTimer::timeout,this,[=](){
            WaittingAllStop();
        });
        waitAll.start(1);

        loop.exec();
    }
    return ;
}

BOOL AMONet_Controller::IsMotionStop(U16 AxisNumber)
{
    if(!m_bServoStatus)
    {
        QMessageBox::critical(NULL, tr("ERROR : Servo ON error !") ,
                              tr("Please open servo first !"));
        return FALSE;
    }
    U16 motion;
    _mnet_m4_motion_done(m_RingNo, m_DeviceIP, AxisNumber, motion);
    return !motion;
}

BOOL AMONet_Controller::IsAllMotionStop()
{
    if(!m_bServoStatus)
    {
        QMessageBox::critical(NULL, tr("ERROR : Servo ON error !") ,
                              tr("Please open servo first !"));
        return FALSE;
    }
    U16 motion;
    for(U16 AxisNumber = 0 ; AxisNumber < m_MaxAxis ; ++AxisNumber )
    {
        _mnet_m4_motion_done(m_RingNo, m_DeviceIP, AxisNumber, motion);
        if(motion != 0) return FALSE;
    }
    return TRUE;
}

// SLOTS //

void AMONet_Controller::WaittingToStop(U16 AxisNumber)
{
    if(IsMotionStop(AxisNumber)) emit signal_IsStop();
}

void AMONet_Controller::WaittingAllStop()
{
    if(IsAllMotionStop()) emit signal_IsAllStop();
}


