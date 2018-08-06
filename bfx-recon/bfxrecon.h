#ifndef BFXRECON_H
#define BFXRECON_H

#define FC_MAX_CHANNELS 5

#define RECON_RPM_STEPSIZE 100
#define RECON_MAXRPM 65500

typedef enum FanControllerModes
{
    FanControllerMode_Unknown,
    FanControllerMode_Auto,
    FanControllerMode_Manual,
    FanControllerMode_SoftwareAuto
} FanControllerModes;

typedef struct
{
    QString name;
    FanControllerModes controlMode;
} NameAndControlMode;

typedef QList <NameAndControlMode> NameAndControlModeList;

#endif // BFXRECON_H
