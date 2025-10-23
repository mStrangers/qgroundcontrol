#pragma once

#include "QGCState.h"
#include "QGCMAVLink.h"

#include <QtCore/QTimer>

#include <cstdint>
#include <functional>

class Vehicle;

class WaitForMavlinkMessageState : public QGCState
{
    Q_OBJECT

public:
    using Predicate = std::function<bool(const mavlink_message_t &message)>;

    WaitForMavlinkMessageState(QState *parent, uint32_t messageId, Predicate predicate = Predicate(), int timeoutMsecs = 0);

signals:
    void timeout();

private slots:
    void _onEntered();
    void _onExited();
    void _messageReceived(const mavlink_message_t &message);
    void _onTimeout();

private:
    uint32_t _messageId = 0U;
    Predicate _predicate;
    Vehicle *_vehicle = nullptr;
    int _timeoutMsecs = 0;
    QTimer _timeoutTimer;
};
