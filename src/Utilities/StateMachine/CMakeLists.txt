# ============================================================================
# State Machine Subsystem
# ============================================================================

target_sources(${CMAKE_PROJECT_NAME}
    PRIVATE
        QGCState.cc
        QGCState.h
        QGCStateMachine.cc
        QGCStateMachine.h
        DelayState.cc
        DelayState.h
        ErrorTransition.h
        FunctionState.cc
        FunctionState.h
        SayState.h
        SendMavlinkCommandState.cc
        SendMavlinkCommandState.h
        SendMavlinkMessageState.cc
        SendMavlinkMessageState.h
    WaitForMavlinkMessageState.cc
    WaitForMavlinkMessageState.h
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
