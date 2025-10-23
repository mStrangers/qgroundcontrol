#pragma once

#include "FunctionState.h"
#include "QGCState.h"

#include <QStateMachine>
#include <QFinalState>
#include <QString>

#include <functional>

class Vehicle;
class GuidedModeCancelledTransition;
class FunctioState;
class SayState;
class SetFlightModeState;

class QGCStateMachine : public QStateMachine
{
    Q_OBJECT
public:
    QGCStateMachine(const QString& machineName, QObject* parent = nullptr);

    void setError(const QString& errorString);

signals:
    void error();

public slots:
    void displayError();

private:
    void _init();

    Vehicle*    _vehicle = nullptr;
    QString     _errorString;

    friend class QGCState;
};

