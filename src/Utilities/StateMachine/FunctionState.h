#pragma once

#include "QGCState.h"

#include <functional>

class FunctionState : public QGCState
{
    Q_OBJECT

public:
    FunctionState(const QString& stateName, QState* parentState, std::function<void()>);

signals:
    void functionCompleted();

private:
    std::function<void()> _function;
};
