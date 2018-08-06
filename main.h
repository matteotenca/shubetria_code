#ifndef MAIN_H
#define MAIN_H

#include <QObject>

class CloseHelper : public QObject
{
    Q_OBJECT

public:

    explicit CloseHelper(QObject *parent = 0);

protected slots:

    void onLastWindowClosed(void);

};

#endif // MAIN_H
