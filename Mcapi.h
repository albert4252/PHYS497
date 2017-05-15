///////////////////////////////////////////////////////////////////////////
//
//  NAME
//    mcapi.h - motion control API function prototypes & macros
//
//  DESCRIPTION
//    Include this header file in your source code to provide prototypes
//    for the motion control API functions.
//
//    #include "mcapi.h"
//
//  RELEASE HISTORY
//    Copyright (c) 1995-2003 by Precision Micro Control Corp. All rights
//    reserved.
//
//    Version 1.0a      15-Jun-95       Programmer: Brian Gaynor
//      - First release
//
//    Version 1.1a      21-Jul-95       Programmer: Brian Gaynor
//      - Update to API for future growth
//
//    Version 1.2a      06-Oct-95       Programmer: Brian Gaynor
//      - Added support for DC2 controller
//      - New error handling functions and constants support added
//      - MCAPI now includes a get version function
//
//    Version 1.2b      06-Oct-95       Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 1.2c      29-Nov-95       Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 1.3a      26-Apr-96       Programmer: Brian Gaynor
//      - Added block mode functions, support (through extended functions)
//        for LabVIEW, Borland C/C++, and Delphi
//      - MCDecodeStatus function provides controller independent access
//        to status bits
//      - Improved error handling and internal memory management (reduces
//        MCAPI footprint). Note that the MCERR_AXIS_TYPE error has been
//        moved to the unsupported error group
//
//    Version 1.3b      23-May-96       Programmer: R. Brian Gaynor
//      - Fixed divide by zero error during init that occured when
//        calculating point storage on a DCX with no (i.e. 0) motor modules
//
//    Version 1.3c      30-Jan-97       Programmer: R. Brian Gaynor
//      - Added additional status word constants for the DC2 and module new
//        type/subtype identifier (MC210)
//
//    Version 2.0a      27-Jun-97       Programmer: R. Brian Gaynor
//      - Support for 32-bit Windows 95, backlash compensation, controller
//        register Get/Set, and enhanced contour path operation
//
//    Version 2.0b      05-Sep-97       Programmer: R. Brian Gaynor
//      - Fixed AT200 auto-detect, spaces in AT200 comments, improved RS232
//        support for all controllers. Added output mode constants for AT200
//        PWM mode
//
//    Version 2.0c      23-Sep-97       Programmer: R. Brian Gaynor
//      - Fixed compatibility problem with MCAPI.DLL on Windows 3.1. Fixed
//        reset command operation with DC2
//
//    Version 2.0d      18-Dec-97       Programmer: R. Brian Gaynor
//      - Version number bumped to match API release
//
//    Version 2.1a      22-Feb-99       Programmer: R. Brian Gaynor
//      - Added MCTranslateEx(), MCGoEx(), and MCGetDigitalIOConfig()
//        functions, new controller constants, and I/O latching constants.
//
//    Version 2.1b       7-May-99       Programmer: R. Brian Gaynor
//      - Added MCERR_WINDOWSERROR and MCERR_CANCEL error codes
//
//    Version 2.1c      25-Jun-99       Programmer: R. Brian Gaynor
//      - No changes to this module
//
//    Version 2.20      17-Jan-00       Programmer: R. Brian Gaynor
//      - Added MCArcEndAnle() and MCArcRadius() contour mode functions
//      - Added MCFindAuxEncIdx(), MCIndexArm(), and MCWaitForIndex()
//        homing functions
//      - Added MCIsAtTarget() and MCIsStopped() functions to simplify
//        "motion complete" program logic.
//      - Added pmccmdex() and pmcrpyex() functions to ease programming
//        of MCCL commnads not directly supported by MCAPI
//      - Added MC_ABSOLUTE and MC_RELATIVE constants (used by any
//        function that accepts an absolute/relative parameter).
//      - Added MC_DATA_TORQUE constant to support data capture on 300
//        series modules
//
//    Version 2.21       6-Mar-00       Programmer: R. Brian Gaynor
//      - No changes to this module
//
//    Version 2.22      31-Mar-00       Programmer: R. Brian Gaynor
//      - No changes to this module
//
//    Version 2.23      16-May-00      Programmer: R. Brian Gaynor
//      - Added MC_RATE_UNKNOWN constant
//
//    Version 3.0.0      1-Mar-01      Programmer: R. Brian Gaynor
//      - Added support for DCX-PCI300 controller
//      - Added constant for MC320 module type
//      - Added MCGetAxisConfiguration() for read-only axis configuration
//      - Added MCGetConfigurationEx() for extended controller
//        configuration information
//
//    Version 3.0.1      1-May-01       Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 3.0.2      6-Jun-01       Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 3.1.0     19-Sep-01       Programmer: Brian Gaynor
//      - Added support for the DCX-PCI100 controller
//      - Added prototypes and constants for Position Capture mode
//        MCEnableCapture(), MCGetCount()
//      - Added prototypes and constants for Position Compare mode
//        MCEnableCompare(), MCConfigureCompare(), MCGetCount()
//      - Added prototypes and constants for Digital Filter mode
//        MCEnableDigitalFilter(), MCGetDigitalFilter(),
//        MCSetDigitalFilter(), MCIsFilter(), MCGetCount()
//
//    Version 3.2.0     11-Jan-02       Programmer: Brian Gaynor
//      - Added additional argument-type constants for pmccmdex()
//      - Added prototypes and  constants for MCGetModuleInputMode() /
//        MCSetModuleInputMode()
//      - Added prototype and constants for MCGetOperatingMode()
//      - Added prototype and new data structure for MCGetMotionConfigEx()
//        and MCSetMotionConfigEx()
//      - Added prototype and new data structure for MCGetFilterConfigEx()
//        and MCSetFilterConfigEx()
//      - Added prototype and new data structure for MCSetCommutation()
//
//    Version 3.2.1     26-Apr-02       Programmer: Brian Gaynor
//      - No changes to this module
//
//    Version 3.3.0		15-Jan-03		Programmer: Brian Gaynor
//      - Added constants for MFX-PCI1000 series controllers
//      - Added prototypes, structures, and constants for MCGetStatusEX(),
//        MCSTATUSEX, and MCDecodeStatusEx()
//      - Added status word lookup constants MC_STAT_PRI_ENC_FAULT, 
//        MC_STAT_AUX_ENC_FAULT, MC_STAT_LOOK_AUX_IDX, MC_STAT_AUX_IDX_FND
//      - Structure tags added to provide better support for source code
//        browsing tools
//      - Added prototype for MCEanableInterrupt()
//      - Added prototype for MCInterruptOnPosition()
//
//    Version 3.4.0		 2-Jun-03		Programmer: Brian Gaynor
//      - Added some conditional typedefs for basic Windows data types so
//        this header can also be used with products like LabWindows/CVI
//      - Added MCEnableEncoderFault() and constants MC_ENC_FAULT_PRI,
//        MC_ENC_FAULT_SEC for encoder fault detection feature of
//        Multiflex series of controllers
//      - Added MCGetAnalogEx/()MCSetAnalogEx() to make it easier for
//        Visual Basic users to handle 16-bit unsigned values
//      - Added pmcgetramex() and pmcputramex() to support larger memory
//        space of the PCI controllers (32-bit address offset and size)
//      - Added new fields to MCAXISCONFIG and MCPARAMEX structures
//
//    Version 3.4.1		31-Oct-03		Programmer: Brian Gaynor
//      - No changes to this module
//
///////////////////////////////////////////////////////////////////////////

#ifndef _INC_MCAPI      // include only once
#define _INC_MCAPI

#ifdef __cplusplus      // avoid c++ name mangling
extern "C" {
#endif 

//
//  Motion Control API manifest constants
//
#define MC_ALL_AXES             0

#define MC_ABSOLUTE             0
#define MC_RELATIVE             1

#define MC_BLOCK_COMPOUND       0
#define MC_BLOCK_TASK           1
#define MC_BLOCK_MACRO          2
#define MC_BLOCK_RESETM         3
#define MC_BLOCK_CANCEL         4
#define MC_BLOCK_CONTR_USER     5
#define MC_BLOCK_CONTR_LIN      6
#define MC_BLOCK_CONTR_CW       7
#define MC_BLOCK_CONTR_CCW      8

#define MC_CAPTURE_ACTUAL      16
#define MC_CAPTURE_ERROR       32
#define MC_CAPTURE_OPTIMAL     64
#define MC_CAPTURE_TORQUE     128
#define MC_CAPTURE_ADVANCED   256
#define MC_CAPTURE_AUXILIARY  512
#define MC_CAPTURE_STATUS    1024

#define MC_CENTER_ABS           MC_ABSOLUTE     // use MC_ABSOLUTE in new code
#define MC_CENTER_REL           MC_RELATIVE     // use MC_RELATIVE in new code

#define MC_COMPARE_DISABLE      0
#define MC_COMPARE_ENABLE       1
#define MC_COMPARE_STATIC       1
#define MC_COMPARE_TOGGLE       2
#define MC_COMPARE_ONESHOT      3
#define MC_COMPARE_INVERT  0x0080

#define MC_COUNT_CAPTURE        1
#define MC_COUNT_COMPARE        2
#define MC_COUNT_CONTOUR        4
#define MC_COUNT_FILTER         8
#define MC_COUNT_FILTERMAX     16

#define MC_CURRENT_FULL         1
#define MC_CURRENT_HALF         2

#define MC_DATA_ACTUAL          0               // use MC_CAPTURE_ACTUAL in new code
#define MC_DATA_OPTIMAL         1               // use MC_CAPTURE_OPTIMAL in new code
#define MC_DATA_ERROR           2               // use MC_CAPTURE_ERROR in new code
#define MC_DATA_TORQUE          3               // use MC_CAPTURE_TORQUE in new code

#define MC_DIO_FIXED          256
#define MC_DIO_INPUT            1
#define MC_DIO_OUTPUT           2
#define MC_DIO_HIGH             4
#define MC_DIO_LOW              8
#define MC_DIO_LATCH           16
#define MC_DIO_LATCHABLE      512
#define MC_DIO_STEPPER       1024

#define MC_DIR_POSITIVE         1
#define MC_DIR_NEGATIVE         2

#define MC_ENC_FAULT_PRI        1
#define MC_ENC_FAULT_AUX        2

#define MC_INT_NORMAL           0
#define MC_INT_FREEZE           1
#define MC_INT_ZERO             2

#define MC_IM_OPENLOOP          0
#define MC_IM_CLOSEDLOOP        1

#define MC_LIMIT_ABRUPT         4
#define MC_LIMIT_BOTH           3
#define MC_LIMIT_INVERT    0x0080
#define MC_LIMIT_MINUS          2
#define MC_LIMIT_OFF            0
#define MC_LIMIT_PLUS           1
#define MC_LIMIT_SMOOTH         8
#define MC_LIMIT_HIGH           MC_LIMIT_PLUS   // use MC_LIMIT_PLUS in new code
#define MC_LIMIT_LOW            MC_LIMIT_MINUS  // use MC_LIMIT_MINUS in new code

#define MC_LRN_POSITION         1
#define MC_LRN_TARGET           2

#define MC_MAX_ID              15

#define MC_MODE_CONTOUR         0
#define MC_MODE_GAIN            1
#define MC_MODE_POSITION        2
#define MC_MODE_TORQUE          3
#define MC_MODE_VELOCITY        4
#define MC_MODE_UNKNOWN         5

#define MC_MODULE_TYPE     0x000f
#define MC_MODULE_SUBTYPE  0x00f0

#define MC_OM_BIPOLAR           0
#define MC_OM_UNIPOLAR          1
#define MC_OM_PULSE_DIR         0
#define MC_OM_CW_CCW            1
#define MC_OM_BI_PWM            2
#define MC_OM_UNI_PWM           3

#define MC_OPEN_ASCII           1
#define MC_OPEN_BINARY          2
#define MC_OPEN_EXCLUSIVE  0x8000

#define MC_PHASE_STD            0
#define MC_PHASE_REV            1

#define MC_PROF_UNKNOWN         0
#define MC_PROF_TRAPEZOID       1
#define MC_PROF_SCURVE          2
#define MC_PROF_PARABOLIC       4

#define MC_RATE_UNKNOWN         0
#define MC_RATE_LOW             1
#define MC_RATE_MEDIUM          2
#define MC_RATE_HIGH            4

#define MC_STATUS_NOTIFY      "MCStatusNotify"  // message identifier
#define MC_STAT_BUSY            0
#define MC_STAT_MTR_ENABLE      1
#define MC_STAT_MODE_VEL        2
#define MC_STAT_TRAJ            3
#define MC_STAT_DIR             4
#define MC_STAT_JOG_ENAB        5
#define MC_STAT_HOMED           6
#define MC_STAT_ERROR           7
#define MC_STAT_LOOK_INDEX      8
#define MC_STAT_LOOK_EDGE       9
#define MC_STAT_BREAKPOINT     10
#define MC_STAT_FOLLOWING      11
#define MC_STAT_AMP_ENABLE     12
#define MC_STAT_AMP_FAULT      13
#define MC_STAT_PLIM_ENAB      14
#define MC_STAT_PLIM_TRIP      15
#define MC_STAT_MLIM_ENAB      16
#define MC_STAT_MLIM_TRIP      17
#define MC_STAT_PSOFT_ENAB     18
#define MC_STAT_PSOFT_TRIP     19
#define MC_STAT_MSOFT_ENAB     20
#define MC_STAT_MSOFT_TRIP     21
#define MC_STAT_INP_INDEX      22
#define MC_STAT_INP_HOME       23
#define MC_STAT_INP_AMP        24
#define MC_STAT_INP_AUX        25
#define MC_STAT_INP_PLIM       26
#define MC_STAT_INP_MLIM       27
#define MC_STAT_INP_USER1      28
#define MC_STAT_INP_USER2      29
#define MC_STAT_PHASE          30
#define MC_STAT_FULL_STEP      31
#define MC_STAT_HALF_STEP      32
#define MC_STAT_JOGGING        33
#define MC_STAT_PJOG_ENAB      34
#define MC_STAT_PJOG_ON        35
#define MC_STAT_MJOG_ENAB      36
#define MC_STAT_MJOG_ON        37
#define MC_STAT_INP_PJOG       38
#define MC_STAT_INP_MJOG       39
#define MC_STAT_STOPPING       40
#define MC_STAT_PROG_DIR       41
#define MC_STAT_AT_TARGET      42       // changed in v2.0d from MC_STAT_PWM_ENAB
#define MC_STAT_ACCEL          43  
#define MC_STAT_MODE_POS       44
#define MC_STAT_MODE_TRQE      45
#define MC_STAT_MODE_ARC       46
#define MC_STAT_MODE_CNTR      47
#define MC_STAT_MODE_SLAVE     48
#define MC_STAT_LMT_ABORT      49
#define MC_STAT_LMT_STOP       50
#define MC_STAT_CAPTURE        51
#define MC_STAT_RECORD         52
#define MC_STAT_SYNC           53
#define MC_STAT_MODE_LIN       54
#define MC_STAT_INDEX_FOUND    55
#define MC_STAT_POS_CAPT       56
#define MC_STAT_NULL           57
#define MC_STAT_EDGE_FOUND     58
#define MC_STAT_PRI_ENC_FAULT  59
#define MC_STAT_AUX_ENC_FAULT  60
#define MC_STAT_LOOK_AUX_IDX   61
#define MC_STAT_AUX_IDX_FND    62

#define MC_STEP_FULL            1
#define MC_STEP_HALF            2

#define MC_THREAD           0x100

#define MC_TYPE_NONE            0
#define MC_TYPE_REG             1
#define MC_TYPE_LONG            2
#define MC_TYPE_FLOAT           3
#define MC_TYPE_DOUBLE          4
#define MC_TYPE_STRING          5

#define MC_TYPE_SERVO           1
#define MC_TYPE_STEPPER         2

#define MC_LINEAR               1
#define MC_CIRC_CW              2
#define MC_CIRC_CCW             3

//
//  Controller specific manifest constants
//                     
#define NO_CONTROLLER           0
#define NONE                    NO_CONTROLLER    // old constant, use NO_CONTROLLER instead
#define DCXPC100                1
#define DCXAT100                2
#define DCXAT200                3
#define DC2PC100                4
#define DC2STN                  5
#define DCXAT300                6
#define DCXPCI300               7
#define DCXPCI100               8
#define MFXPCI1000              9

#define MC100                   5
#define MC110                   4
#define MC150                   6
#define MC160                   7
#define MC200                   0
#define MC210                  16
#define MC260                   1
#define MC300                   2
#define MC302                  22
#define MC320                 162
#define MC360                   3
#define MC362                  23
#define MC400                   8
#define MC500                  12
#define MF300                  10
#define MF310                   9
#define NO_MODULE              15
#define MFXSERVO              252
#define MFXSTEPPER            253
#define DC2SERVO              254
#define DC2STEPPER            255

//
//  Error code group masks
//
#define MCERRMASK_UNSUPPORTED 0x00000001L   // function not supported
#define MCERRMASK_HANDLE      0x00000002L   // bad handle
#define MCERRMASK_AXIS        0x00000004L   // bad axis number
#define MCERRMASK_PARAMETER   0x00000008L   // bad parameter
#define MCERRMASK_IO          0x00000010L   // I/O problem
#define MCERRMASK_SYSTEM      0x00000020L   // system level errors

#define MCERRMASK_STANDARD    0xFFFFFFFEL   // most common MCErrorNotify settings
                                            // includes all errors except UNSUPPORTED

#define MC_ERROR_NOTIFY       "MCErrorNotify"   // message identifier

//
//  Individual error codes
//

#define MCERR_NOERROR           0    // no error

//
//  MCERRMASK_SYSTEM group errors
//
#define MCERR_NO_CONTROLLER     1   // no controller assigned for this ID
#define MCERR_OUT_OF_HANDLES    2   // driver out of handles
#define MCERR_OPEN_EXCLUSIVE    3   // cannot open - exclusive
#define MCERR_MODE_UNAVAIL      4   // controller already open in different mode
#define MCERR_UNSUPPORTED_MODE  5   // controller doesn't support this mode
#define MCERR_INIT_DRIVER       6   // couldn't initialize the device driver
#define MCERR_NOT_PRESENT       7   // controller hardware not present
#define MCERR_ALLOC_MEM         8   // memory allocation error
#define MCERR_WINDOWSERROR      9   // Windows function reported an error

//
//  MCERRMASK_UNSUPPORTED group errors
//
#define MCERR_NOTSUPPORTED     11   // controller doesn't support this feature
#define MCERR_OBSOLETE         12   // function is obsolete
#define MCERR_AXIS_TYPE        16   // axis type doesn't support this feature

//
//  MCERRMASK_HANDLE group errors
//
#define MCERR_CONTROLLER       13   // invalid controller handle 
#define MCERR_WINDOW           14   // invalid window handle

//
//  MCERRMASK_AXIS group errors
//
#define MCERR_AXIS_NUMBER      15   // axis number out of range
#define MCERR_ALL_AXES         17   // cannot select "ALL AXES" for function

//
//  MCERRMASK_PARAMETER group errors
//
#define MCERR_RANGE            18   // parameter was out of range
#define MCERR_CONSTANT         19   // constant value inappropriate
#define MCERR_NOT_INITIALIZED  29   // feature not initialized

//
//  MCERRMASK_IO group errors
//                                
#define MCERR_UNKNOWN_REPLY    20   // unexpected or unknown reply 
#define MCERR_NO_REPLY         21   // controller failed to reply 
#define MCERR_REPLY_SIZE       22   // reply size incorrect
#define MCERR_REPLY_AXIS       23   // wrong axis for reply 
#define MCERR_REPLY_COMMAND    24   // reply is for different command
#define MCERR_TIMEOUT          25   // controller failed to respond
#define MCERR_BLOCK_MODE       26   // block mode error
#define MCERR_COMM_PORT        27   // communications port (RS232) error
#define MCERR_CANCEL           28   // operation was canceled
#define MCERR_NOT_FOUND        29   // restore operation could not find data

//
//  Types. We define some windows types if they aren't already defined so that
//  this header file may be used with environments like LabWindows/CVI that 
//  don't include the standard windows headers
//
typedef short int HCTRLR;

#ifndef WORD
  typedef unsigned short WORD;
#endif

#ifndef WINAPI
  #define WINAPI __stdcall
#endif

#ifndef CHAR
  typedef char CHAR;
  typedef CHAR *LPSTR;
  typedef const CHAR *LPCSTR;
#endif

//
//  Data structures (set packing for 32-bit compilers)
//
#pragma pack(4)

//
//  Axis configuration structure
//
typedef struct _MCAXISCONFIG {
	int       cbSize;
	int       ModuleType;
	int       ModuleLocation;
	int       MotorType;
	int       CaptureModes;
	int       CapturePoints;
	int       CaptureAndCompare;
	double    HighRate;
	double    MediumRate;
	double    LowRate;
	double    HighStepMin;
	double    HighStepMax;
	double    MediumStepMin;
	double    MediumStepMax;
	double    LowStepMin;
	double    LowStepMax;
	int       AuxEncoder;			// added v3.4, axis has auxiliary encoder input
} MCAXISCONFIG;


//
//  Commutation parameters structure
//
typedef struct _MCCOMMUTATION {
	int    cbSize;
	double PhaseA;
	double PhaseB;
	int    Divisor;
	int    PreScale;
	int    Repeat;
} MCCOMMUTATION;


//
//  Contouring parameters structure
//
typedef struct _MCCONTOUR {
	double VectorAccel;
	double VectorDecel;
	double VectorVelocity;
	double VelocityOverride;
} MCCONTOUR;


//
//  PID Filter parameters structure (extended)
//
typedef struct _MCFILTEREX {
	int    cbSize;
	double Gain;
	double IntegralGain;
	double IntegrationLimit;
	int    IntegralOption;
	double DerivativeGain;
	double DerSamplePeriod;
	double FollowingError;
	double VelocityGain;
	double AccelGain;
	double DecelGain;
	double EncoderScaling;
	int    UpdateRate;
} MCFILTEREX;


//
//  PID Filter parameters structure (new programs should use MCFILTEREX)
//
typedef struct _MCFILTER {
	double DerivativeGain;
	double DerSamplePeriod;
	double IntegralGain;
	double IntegrationLimit;
	double VelocityGain;
	double AccelGain;
	double DecelGain;
	double FollowingError;
} MCFILTER;


//
//  Jog control parameters structure
//
typedef struct _MCJOG {
	double Acceleration;
	double MinVelocity;
	double Deadband;
	double Gain;
	double Offset;
} MCJOG;


//
//  Motion parameters structure (extended)
//
typedef struct _MCMOTIONEX {
	int       cbSize;
	double    Acceleration;
	double    Deceleration;
	double    Velocity;
	double    MinVelocity;
	int       Direction;
	double    Torque;
	double    Deadband;
	double    DeadbandDelay;
	int       StepSize;
	int       Current;
	int       HardLimitMode;
	int       SoftLimitMode;
	double    SoftLimitLow;
	double    SoftLimitHigh;
	int       EnableAmpFault;
} MCMOTIONEX;


//
//  Motion parameters structure (new programs should use MCMOTIONEX)
//
typedef struct _MCMOTION {
	double    Acceleration;
	double    Deceleration;
	double    Velocity;
	double    MinVelocity;
	short int Direction;
	double    Gain;
	double    Torque;
	double    Deadband;
	double    DeadbandDelay;
	short int StepSize;
	short int Current;
	short int HardLimitMode;
	short int SoftLimitMode;
	double    SoftLimitLow;
	double    SoftLimitHigh;
	short int EnableAmpFault;
	short int Rate;
} MCMOTION;


//
//  Controller configuration structure (extended)
//
typedef struct _MCPARAMEX {
	int       cbSize;
	int       ID;
	int       ControllerType;
	int       NumberAxes;
	int       MaximumAxes;
	int       MaximumModules;
	int       Precision;
	int       DigitalIO;
	int       AnalogInput;
	int       AnalogOutput;
	int       PointStorage;
	int       CanDoScaling;
	int       CanDoContouring;
	int       CanChangeProfile;
	int       CanChangeRates;
	int       SoftLimits;
	int       MultiTasking;
	int       AmpFault;
	double    AnalogInpMin;			// added v3.4, motherboard analog inp min voltage	
	double    AnalogInpMax;			// added v3.4, motherboard analog inp max voltage	
	int       AnalogInpRes;			// added v3.4, motherboard analog inp resolution (bits)
	double    AnalogOutMin;			// added v3.4, motherboard analog out min voltage	
	double    AnalogOutMax;			// added v3.4, motherboard analog out max voltage	
	int       AnalogOutRes;			// added v3.4, motherboard analog out resolution (bits)
} MCPARAMEX;


//
//  Controller configuration structure (new programs should use MCPARAMEX)
//
typedef struct _MCPARAM {
    short int ID;
    short int ControllerType;
    short int NumberAxes;
    short int DigitalIO;
    short int AnalogInput;
    short int AnalogOutput;
    short int AxisType[8];
    short int PointStorage;
    short int CanDoScaling;
    short int CanDoContouring;
    short int CanChangeProfile;
    short int CanChangeRates;
    short int SoftLimits;
    short int MultiTasking;
    short int AmpFault;
} MCPARAM;


//
//  Scaling factors data structure
//
typedef struct _MCSCALE {
    double Constant;
    double Offset;
    double Rate;
    double Scale;
    double Zero;
    double Time;
} MCSCALE;


//
//  Status word data structure
//
typedef struct _MCSTATUSEX {
	int   cbSize;
	DWORD Status;
	DWORD AuxStatus;
	DWORD ProfileStatus;
	DWORD ModeStatus;
} MCSTATUSEX;


//
//  Restore default packing
//
#pragma pack()


//
//  Interrupt callback function typedef
//
typedef void (CALLBACK* MCINTERRUPTPROC)(HWND, HCTRLR, WORD, DWORD);


//
//  API Function Prototypes
//
extern void WINAPI      MCAbort(HCTRLR hCtlr, WORD wAxis);
extern long int WINAPI  MCArcCenter(HCTRLR hCtlr, WORD wAxis, short int nType, double Position);
extern long int WINAPI  MCArcEndAngle(HCTRLR hCtlr, WORD wAxis, short int nType, double Angle);
extern long int WINAPI  MCArcRadius(HCTRLR hCtlr, WORD wAxis, double Radius);
extern long int WINAPI  MCBlockBegin(HCTRLR hCtlr, long int lMode, long int lNum);
extern long int WINAPI  MCBlockEnd(HCTRLR hCtlr, long int* lTaskID);
extern long int WINAPI  MCCancelTask(HCTRLR hCtlr, long int lTaskID);
extern long int WINAPI  MCCaptureData(HCTRLR hCtlr, WORD wAxis, long int lPoints, double Period, double Delay);
extern short int WINAPI MCClose(HCTRLR hCtlr);
extern long int WINAPI  MCConfigureCompare(HCTRLR hCtlr, WORD wAxis, double* values, long int num, double inc, long int mode, double period);
extern short int WINAPI MCConfigureDigitalIO(HCTRLR hCtlr, WORD wChannel, WORD wMode);
extern long int WINAPI  MCContourDistance(HCTRLR hCtlr, WORD wAxis, double Distance);
extern long int WINAPI  MCContourPath(HCTRLR hCtlr, WORD wAxis, WORD wMode, char* lpBuffer);
extern long int WINAPI  MCDecodeStatus(HCTRLR hCtlr, DWORD dwStatus, long int lBit);
extern long int WINAPI  MCDecodeStatusEx(HCTRLR hCtlr, MCSTATUSEX* pStatus, long int lBit);
extern void WINAPI      MCDirection(HCTRLR hCtlr, WORD wAxis, WORD wDir);
extern long int WINAPI  MCEdgeArm(HCTRLR hCtlr, WORD wAxis, double Position);
extern void WINAPI      MCEnableAxis(HCTRLR hCtlr, WORD wAxis, short int bState);
extern long int WINAPI  MCEnableBacklash(HCTRLR hCtlr, WORD wAxis, double Backlash, short int bState);
extern long int WINAPI  MCEnableCapture(HCTRLR hCtlr, WORD wAxis, long int count);
extern long int WINAPI  MCEnableCompare(HCTRLR hCtlr, WORD wAxis, long int flag);
extern long int WINAPI  MCEnableDigitalFilter(HCTRLR hCtlr, WORD wAxis, long int state);
extern void WINAPI      MCEnableDigitalIO(HCTRLR hCtlr, WORD wChannel, short int bState);
extern long int WINAPI  MCEnableEncoderFault(HCTRLR hCtlr, WORD wAxis, long int flag);
extern void WINAPI      MCEnableGearing(HCTRLR hCtlr, WORD wAxis, WORD wMaster, double ratio, short int bState);
extern long int WINAPI  MCEnableInterrupt(HWND hWnd, HCTRLR hCtlr, WORD wAxis, DWORD mask, MCINTERRUPTPROC lpIntFunc);
extern void WINAPI      MCEnableJog(HCTRLR hCtlr, WORD wAxis, short int bState);
extern void WINAPI      MCEnableSync(HCTRLR hCtlr, WORD wAxis, short int bState);
extern void WINAPI      MCErrorNotify(HWND hWnd, HCTRLR hCtlr, DWORD ErrorMask);
extern long int WINAPI  MCFindAuxEncIdx(HCTRLR hCtlr, WORD wAxis, double Position);
extern long int WINAPI  MCFindEdge(HCTRLR hCtlr, WORD wAxis, double Position);
extern long int WINAPI  MCFindIndex(HCTRLR hCtlr, WORD wAxis, double Position);
extern long int WINAPI  MCGetAccelerationEx(HCTRLR hCtlr, WORD wAxis, double* Acceleration);
extern WORD WINAPI      MCGetAnalog(HCTRLR hCtlr, WORD wChannel);
extern long int WINAPI  MCGetAnalogEx(HCTRLR hCtlr, long int Channel, DWORD* Value);
extern long int WINAPI  MCGetAuxEncIdxEx(HCTRLR hCtlr, WORD wAxis, double* Index);
extern long int WINAPI  MCGetAuxEncPosEx(HCTRLR hCtlr, WORD wAxis, double* Position);
extern long int WINAPI  MCGetAxisConfiguration(HCTRLR hCtlr, WORD wAxis, MCAXISCONFIG* pAxisCfg);
extern long int WINAPI  MCGetBreakpointEx(HCTRLR hCtlr, WORD wAxis, double* Breakpoint);
extern long int WINAPI  MCGetCaptureData(HCTRLR hCtlr, WORD wAxis, long int lType, long int lStart,
                                         long int lPoints, double* lpData);
extern void WINAPI      MCGetConfiguration(HCTRLR hCtlr, MCPARAM* lpParam);
extern long int WINAPI  MCGetConfigurationEx(HCTRLR hCtlr, MCPARAMEX* lpParam);
extern short int WINAPI MCGetContourConfig(HCTRLR hCtlr, WORD wAxis, MCCONTOUR* lpContour);
extern long int WINAPI  MCGetContouringCount(HCTRLR hCtlr, WORD wAxis);
extern long int WINAPI  MCGetCount(HCTRLR hCtlr, WORD wAxis, long int type, long int* count);
extern long int WINAPI  MCGetDecelerationEx(HCTRLR hCtlr, WORD wAxis, double* Deceleration);
extern long int WINAPI  MCGetDigitalFilter(HCTRLR hCtlr, WORD wAxis, double* coeff, long int num, long int* actual);
extern short int WINAPI MCGetDigitalIO(HCTRLR hCtlr, WORD wChannel);
extern long int WINAPI  MCGetDigitalIOConfig(HCTRLR hCtlr, WORD wChannel, WORD* wMode);
extern short int WINAPI MCGetError(HCTRLR hCtlr);
extern short int WINAPI MCGetFilterConfig(HCTRLR hCtlr, WORD wAxis, MCFILTER* pFilter);
extern long int WINAPI  MCGetFilterConfigEx(HCTRLR hCtlr, WORD wAxis, MCFILTEREX* pFilter);
extern long int WINAPI  MCGetFollowingError(HCTRLR hCtlr, WORD wAxis, double* Error);
extern long int WINAPI  MCGetGain(HCTRLR hCtlr, WORD wAxis, double* Gain);
extern long int WINAPI  MCGetIndexEx(HCTRLR hCtlr, WORD wAxis, double* Index);
extern long int WINAPI  MCGetInstalledModules(HCTRLR hCtlr, int Modules[], int Size);
extern short int WINAPI MCGetJogConfig(HCTRLR hCtlr, WORD wAxis, MCJOG* lpJog);
extern long int WINAPI  MCGetLimits(HCTRLR hCtlr, WORD wAxis, short int* HardLimitMode, short int* SoftLimitMode,
                                    double* SoftLimitMinus, double* SoftLimitPlus);
extern long int WINAPI  MCGetModuleInputMode(HCTRLR hCtlr, WORD wAxis, long int* Mode);
extern short int WINAPI MCGetMotionConfig(HCTRLR hCtlr, WORD wAxis, MCMOTION* pMotion);
extern long int WINAPI  MCGetMotionConfigEx(HCTRLR hCtlr, WORD wAxis, MCMOTIONEX* pMotion);
extern long int WINAPI  MCGetOperatingMode(HCTRLR hCtlr, WORD wAxis, long int* Mode);
extern long int WINAPI  MCGetOptimalEx(HCTRLR hCtlr, WORD wAxis, double* Optimal);
extern long int WINAPI  MCGetPositionEx(HCTRLR hCtlr, WORD wAxis, double* Position);
extern long int WINAPI  MCGetProfile(HCTRLR hCtlr, WORD wAxis, WORD* wProfile);
extern long int WINAPI  MCGetRegister(HCTRLR hCtlr, long int nRegister, void* Value, long int nType);
extern short int WINAPI MCGetScale(HCTRLR hCtlr, WORD wAxis, MCSCALE* pScale);
extern long int WINAPI  MCGetServoOutputPhase(HCTRLR hCtlr, WORD wAxis, WORD* wPhase);
extern DWORD WINAPI     MCGetStatus(HCTRLR hCtlr, WORD wAxis);
extern long int WINAPI  MCGetStatusEx(HCTRLR hCtlr, WORD wAxis, MCSTATUSEX* pStatus);
extern long int WINAPI  MCGetTargetEx(HCTRLR hCtlr, WORD wAxis, double* Target);
extern long int WINAPI  MCGetTorque(HCTRLR hCtlr, WORD wAxis, double* Torque);
extern long int WINAPI  MCGetVectorVelocity(HCTRLR hCtlr, WORD wAxis, double* Velocity);
extern long int WINAPI  MCGetVelocityActual(HCTRLR hCtlr, WORD wAxis, double* Velocity);
extern long int WINAPI  MCGetVelocityEx(HCTRLR hCtlr, WORD wAxis, double* Velocity);
extern DWORD WINAPI     MCGetVersion(HCTRLR hCtlr);
extern void WINAPI      MCGo(HCTRLR hCtlr, WORD wAxis);
extern long int WINAPI  MCGoEx(HCTRLR hCtlr, WORD wAxis, double Param);
extern void WINAPI      MCGoHome(HCTRLR hCtlr, WORD wAxis);
extern long int WINAPI  MCIndexArm(HCTRLR hCtlr, WORD wAxis, double Position);
extern long int WINAPI  MCInterruptOnPosition(HCTRLR hCtlr, WORD wAxis, long int mode, double position);
extern long int WINAPI  MCIsAtTarget(HCTRLR hCtlr, WORD wAxis, double TimeOut);
extern long int WINAPI  MCIsDigitalFilter(HCTRLR hCtlr, WORD wAxis);
extern long int WINAPI  MCIsEdgeFound(HCTRLR hCtlr, WORD wAxis, double TimeOut);
extern long int WINAPI  MCIsIndexFound(HCTRLR hCtlr, WORD wAxis, double TimeOut);
extern long int WINAPI  MCIsStopped(HCTRLR hCtlr, WORD wAxis, double TimeOut);
extern long int WINAPI  MCLearnPoint(HCTRLR hCtlr, WORD wAxis, long int lIndex, WORD wMode);
extern void WINAPI      MCMacroCall(HCTRLR hCtlr, WORD wMacro);
extern void WINAPI      MCMoveAbsolute(HCTRLR hCtlr, WORD wAxis, double Position);
extern void WINAPI      MCMoveRelative(HCTRLR hCtlr, WORD wAxis, double Distance);
extern long int WINAPI  MCMoveToPoint(HCTRLR hCtlr, WORD wAxis, long int lIndex);
extern HCTRLR WINAPI    MCOpen(short int nID, WORD wMode, LPCSTR lpszName);
extern long int WINAPI  MCReopen(HCTRLR hCtlr, WORD wNewMode);
extern long int WINAPI  MCRepeat(HCTRLR hCtlr, long int lCount);
extern void WINAPI      MCReset(HCTRLR hCtlr, WORD wAxis);
extern void WINAPI      MCSetAcceleration(HCTRLR hCtlr, WORD wAxis, double Rate);
extern void WINAPI      MCSetAnalog(HCTRLR hCtlr, WORD channel, WORD value);
extern long int WINAPI  MCSetAnalogEx(HCTRLR hCtlr, long int channel, DWORD value);
extern void WINAPI      MCSetAuxEncPos(HCTRLR hCtlr, WORD wAxis, double Position);
extern long int WINAPI  MCSetCommutation(HCTRLR hCtlr, WORD wAxis, MCCOMMUTATION* pCommutation);
extern short int WINAPI MCSetContourConfig(HCTRLR hCtlr, WORD wAxis, MCCONTOUR* pContour);
extern void WINAPI      MCSetDeceleration(HCTRLR hCtlr, WORD wAxis, double Rate);
extern long int WINAPI  MCSetDigitalFilter(HCTRLR hCtlr, WORD wAxis, double* coeff, long int num);
extern short int WINAPI MCSetFilterConfig(HCTRLR hCtlr, WORD wAxis, MCFILTER* pFilter);
extern long int WINAPI  MCSetFilterConfigEx(HCTRLR hCtlr, WORD wAxis, MCFILTEREX* pFilter);
extern long int WINAPI  MCSetGain(HCTRLR hCtlr, WORD wAxis, double Gain);
extern short int WINAPI MCSetJogConfig(HCTRLR hCtlr, WORD wAxis, MCJOG* pJog);
extern long int WINAPI  MCSetLimits(HCTRLR hCtlr, WORD wAxis, short int HardLimitMode, short int SoftLimitMode,
                                    double SoftLimitMinus, double SoftLimitPlus);
extern long int WINAPI  MCSetModuleInputMode(HCTRLR hCtlr, WORD wAxis, long int Mode);
extern void WINAPI      MCSetModuleOutputMode(HCTRLR hCtlr, WORD wAxis, WORD wMode);
extern short int WINAPI MCSetMotionConfig(HCTRLR hCtlr, WORD wAxis, MCMOTION* pMotion);
extern long int WINAPI  MCSetMotionConfigEx(HCTRLR hCtlr, WORD wAxis, MCMOTIONEX* pMotion);
extern void WINAPI      MCSetOperatingMode(HCTRLR hCtlr, WORD wAxis, WORD caxis, WORD mode);
extern void WINAPI      MCSetPosition(HCTRLR hCtlr, WORD wAxis, double Position);
extern void WINAPI      MCSetProfile(HCTRLR hCtlr, WORD wAxis, WORD wMode);
extern long int WINAPI  MCSetRegister(HCTRLR hCtlr, long int nRegister, void* Value, long int nType);
extern short int WINAPI MCSetScale(HCTRLR hCtlr, WORD wAxis, MCSCALE* pScale);
extern void WINAPI      MCSetServoOutputPhase(HCTRLR hCtlr, WORD wAxis, WORD wPhase);
extern long int WINAPI  MCSetTimeoutEx(HCTRLR hCtlr, double TimeOut, double* OldTimeOut);
extern long int WINAPI  MCSetTorque(HCTRLR hCtlr, WORD wAxis, double Torque);
extern long int WINAPI  MCSetVectorVelocity(HCTRLR hCtlr, WORD wAxis, double Velocity);
extern void WINAPI      MCSetVelocity(HCTRLR hCtlr, WORD wAxis, double Rate);
extern void WINAPI      MCStop(HCTRLR hCtlr, WORD wAxis);
extern long int WINAPI  MCTranslateErrorEx(short int nError, LPSTR szBuffer, long int nLength);
extern void WINAPI      MCWait(HCTRLR hCtlr, double Period);
extern void WINAPI      MCWaitForDigitalIO(HCTRLR hCtlr, WORD wChannel, short int bState);
extern long int WINAPI  MCWaitForEdge(HCTRLR hCtlr, WORD wAxis, short int bState);
extern long int WINAPI  MCWaitForIndex(HCTRLR hCtlr, WORD wAxis);
extern void WINAPI      MCWaitForPosition(HCTRLR hCtlr, WORD wAxis, double Position);
extern void WINAPI      MCWaitForRelative(HCTRLR hCtlr, WORD wAxis, double Distance);
extern void WINAPI      MCWaitForStop(HCTRLR hCtlr, WORD wAxis, double Period);
extern void WINAPI      MCWaitForTarget(HCTRLR hCtlr, WORD wAxis, double Period);

//
//  Low level controller access commands
//
extern short int WINAPI pmccmd(HCTRLR hCtlr, short int nBytes, void* lpBuffer);
extern long int WINAPI  pmccmdex(HCTRLR hCtlr, WORD wAxis, WORD wCmd, void* Argument, long int nType);
extern short int WINAPI pmcgetc(HCTRLR hCtlr);
extern void WINAPI      pmcgetram(HCTRLR hCtlr, WORD wOffset, void* lpBuffer, short int nBytes);
extern long int WINAPI  pmcgetramex(HCTRLR hCtlr, DWORD offset, void* buffer, DWORD size);
extern short int WINAPI pmcgets(HCTRLR hCtlr, char* lpszBuffer, short int nBytes);
extern short int WINAPI pmcputc(HCTRLR hCtlr, short int nChar);
extern void WINAPI      pmcputram(HCTRLR hCtlr, WORD wOffset, void* lpBuffer, short int nBytes);
extern long int WINAPI  pmcputramex(HCTRLR hCtlr, DWORD offset, void* buffer, DWORD size);
extern short int WINAPI pmcputs(HCTRLR hCtlr, char* lpszBuffer);
extern short int WINAPI pmcrdy(HCTRLR hCtlr);
extern short int WINAPI pmcrpy(HCTRLR hCtlr, short int nBytes, char* lpBuffer);
extern long int  WINAPI pmcrpyex(HCTRLR hCtlr, void* Reply, long int nType);

#ifdef __cplusplus
}
#endif 

#endif

