#include <QString>
#include "builddetails.h"

BuildDetails::BuildDetails()
{
}

QString BuildDetails::versionStr(void)
{
    QString s;

    s = QString::number(PHOEBETRIA_VERSION)
        + "." + QString::number(PHOEBETRIA_MAJOR_REVISION)
        + "." + QString::number(PHOEBETRIA_MINOR_REVISION)
        + " " + PHOEBETRIA_STATUS_STR;

    return s;
}

QString BuildDetails::buildDateTimeStr(void)
{
    QString s;

    s = QString(__DATE__) + " " + QString(__TIME__);

    return s;
}

QString BuildDetails::qtVersion(void)
{
    QString s;

    s = QT_VERSION_STR;

    return s;
}

