#include "SendMavlinkCommandState.h"
#include "MultiVehicleManager.h"
#include "MissionCommandTree.h"
#include "Vehicle.h"

SendMavlinkCommandState::SendMavlinkCommandState(QState* parentState, MAV_CMD command, double param1, double param2, double param3, double param4, double param5, double param6, double param7)
    : QGCState("SendMavlinkCommandState", parentState)
{
    setup(command, param1, param2, param3, param4, param5, param6, param7);
}

SendMavlinkCommandState::SendMavlinkCommandState(QState* parentState)
    : QGCState("SendMavlinkCommandState", parentState)
{

}

void SendMavlinkCommandState::setup(MAV_CMD command, double param1, double param2, double param3, double param4, double param5, double param6, double param7)
{
    _vehicle = MultiVehicleManager::instance()->activeVehicle();
    _command = command;
    _param1 = param1;
    _param2 = param2;
    _param3 = param3;
    _param4 = param4;
    _param5 = param5;
    _param6 = param6;
    _param7 = param7;

    connect(this, &QState::entered, this, [this] () 
        { 
            qCDebug(QGCStateMachineLog) << QStringLiteral("Sending %1 command").arg(MissionCommandTree::instance()->friendlyName(_command)) << " - " << Q_FUNC_INFO;
            _sendMavlinkCommand();
        });

    connect(this, &QState::exited, this, &SendMavlinkCommandState::_disconnectAll);
}

void SendMavlinkCommandState::_sendMavlinkCommand()
{
    connect(_vehicle, &Vehicle::mavCommandResult, this, &SendMavlinkCommandState::_mavCommandResult);
    _vehicle->sendMavCommand(MAV_COMP_ID_AUTOPILOT1,
                                _command,
                                false /* showError */,
                                static_cast<float>(_param1),
                                static_cast<float>(_param2),
                                static_cast<float>(_param3),
                                static_cast<float>(_param4),
                                static_cast<float>(_param5),
                                static_cast<float>(_param6),
                                static_cast<float>(_param7));
}

void SendMavlinkCommandState::_mavCommandResult(int vehicleId, int targetComponent, int command, int ackResult, int failureCode)
{
    Q_UNUSED(vehicleId);
    Q_UNUSED(targetComponent);

    Vehicle* vehicle = dynamic_cast<Vehicle*>(sender());
    if (!vehicle) {
        qWarning() << "Vehicle dynamic cast on sender() failed!" << " - " << Q_FUNC_INFO;
        return;
    }
    if (vehicle != _vehicle) {
        qCWarning(QGCStateMachineLog) << "Received mavCommandResult from unexpected vehicle" << " - " << Q_FUNC_INFO;
        return;
    }
    if (command != _command) {
        qCWarning(QGCStateMachineLog) << "Received mavCommandResult for unexpected command - expected:actual" << _command << command << " - " << Q_FUNC_INFO;
        return;
    }

    _disconnectAll();

    QString commandName = MissionCommandTree::instance()->friendlyName(_command);

    if (failureCode == Vehicle::MavCmdResultFailureNoResponseToCommand) {
        qCDebug(QGCStateMachineLog) << QStringLiteral("%1 Command - No response from vehicle").arg(commandName) << " - " << Q_FUNC_INFO;
        QString message = QStringLiteral("%1 command failed").arg(commandName);
        setError(message);
    } else if (failureCode == Vehicle::MavCmdResultFailureDuplicateCommand) {
        qCWarning(QGCStateMachineLog) << QStringLiteral("%1 Command - Duplicate command pending").arg(commandName) << " - " << Q_FUNC_INFO;
        QString message = QStringLiteral("%1 command failed due to duplicate command pending").arg(commandName);
        setError(message);
    } else if (ackResult != MAV_RESULT_ACCEPTED) {
        qCWarning(QGCStateMachineLog) << QStringLiteral("%1 Command failed = ack.result: %2").arg(commandName).arg(ackResult) << " - " << Q_FUNC_INFO;
        QString message = QStringLiteral("%1 command failed").arg(commandName);
        setError(message);
    } else {
        // MAV_RESULT_ACCEPTED
        qCDebug(QGCStateMachineLog) << QStringLiteral("%1 Command succeeded").arg(commandName) << " - " << Q_FUNC_INFO;
        emit advance();
    }
}

void SendMavlinkCommandState::_disconnectAll()
{
    disconnect(_vehicle, &Vehicle::mavCommandResult, this, &SendMavlinkCommandState::_mavCommandResult);
}