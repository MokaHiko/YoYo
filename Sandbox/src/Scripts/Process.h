#pragma once

#include <Core/Memory.h>

class Process
{
public:
    enum ProcessState
    {
        // Processes that are neither dead nor alive
        Uninitialized = 0, // created but not running
        Removed, // removed from the process list but not destroyed; this can
        // happen when a process that is already running is parented
        // to another process.

        // Living processes
        Running, // initialized and running
        Paused, // initialized but paused

        // Dead processes
        Succeeded, // completed successfully
        Failed, // failed to complete
        Aborted, // aborted; may not have started
    };

    Process();
    virtual ~Process();
public:
    virtual void OnInit(void) { m_state = Running; }
    virtual void OnUpdate(float dt) = 0;

    // Process end callbacks
    virtual void OnSuccess() {}
    virtual void OnFail() {}
    virtual void OnAbort() {}
public:
    // Functions for ending the process.
    inline void Succeed() { SetState(Succeeded); }
    inline void Abort() { SetState(Aborted); }
    inline void Fail() { SetState(Failed); }

    inline void Pause() { SetState(Paused); }
    inline void Play() { SetState(Running); }
    inline void AttachChild(Ref<Process> process) { m_child_process = process; }

    Ref<Process> RemoveChild();
    Ref<Process> Child();
public:
    // Accessors
    ProcessState State() { return m_state; }
    bool IsAlive() const { return m_state == Running || m_state == Paused; }
    bool IsDead() const { return m_state == Succeeded || m_state == Failed || m_state == Aborted; }
    bool IsRemoved() const { return m_state == Removed; }
    bool IsPaused() const { return m_state == Paused; }
private:
    void SetState(ProcessState new_state) { m_state = new_state; }

    ProcessState m_state = Uninitialized;
    Ref<Process> m_child_process;
};

// Invokes callback after a set delay
class DelayProcess : public Process
{
public:
    DelayProcess(float time, std::function<void(void)> callback = nullptr);
    virtual void OnUpdate(float dt) override;
private:
    float m_time = 0;
    std::function<void(void)> m_callback = nullptr;
};

class FloatLerpProcess : public Process
{
public:
    // Duration and rate are in seconds
    FloatLerpProcess(float* value, float a, float b, float duration, float rate = 1)
        :m_value(value), m_a(a), m_b(b), m_duration(duration), m_rate(rate)
    {
    };

    virtual void OnUpdate(float dt) override
    {
        *m_value = (m_a * (1 - (m_time / m_duration))) + (m_b * (m_time / m_duration));
        m_time += dt * m_rate;

        if (*m_value - m_b <= 0.01f)
        {
            Succeed();
        }
    };
private:
    float* m_value = nullptr;
    float m_a, m_b;
    float m_duration, m_rate;

    float m_time = 0;
};