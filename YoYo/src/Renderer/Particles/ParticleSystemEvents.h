#pragma once

#include "Events/Event.h" 

namespace yoyo
{
    class ParticleSystem;
    class ParticleSystemCreatedEvent : public Event
    {
    public:
        EVENT_TYPE(ParticleSystemCreatedEvent)

        ParticleSystemCreatedEvent(Ref<ParticleSystem> new_particle_system)
            :particle_system(new_particle_system) {}
        virtual ~ParticleSystemCreatedEvent() = default;

        Ref<ParticleSystem> particle_system;
    };
}