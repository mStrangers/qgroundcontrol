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

signals:
    void error();

private:
    Vehicle *_vehicle = nullptr;
};

