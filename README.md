
```mermaid
stateDiagram-v2
    Idle --> Idle: Reset
    Standby --> Idle: Reset
    Igniter_On --> Idle: Reset
    Valve_Open --> Idle: Reset
    Igniter_Off --> Idle: Reset
    Aborted --> Idle: Reset

    Idle --> Aborted: Error
    Standby --> Aborted: Error
    Igniter_On --> Aborted: Error
    Valve_Open --> Aborted: Error
    Igniter_Off --> Aborted: Error

    [*] --> Idle
    Idle --> Standby: Arm
    Standby --> Igniter_On: Launch
    Igniter_On --> Valve_Open: Open_Valve_Timer
    Valve_Open --> Igniter_Off: Stop_Igniter_Timer
end
```