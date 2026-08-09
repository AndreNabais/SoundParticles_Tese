#include "PhysicalObject.h"
#include "Velocity.h"
#include "Acceleration.h"

    Position PhysicalObject::getPositionSample(int n) const {
        return positions[n];
    }

    void PhysicalObject::updatePosition(int n, float delta_t) {
        if (n == 0) return; // Avoid accessing positions[-1] for the first sample

        else {
            positions[n].x = positions[n-1].x + Velocity_history[n-1].x * delta_t; //for now, set the source to move only along the x-axis. In the future, we can extend this to 3D movement.
            positions[n].y = positions[n-1].y + Velocity_history[n-1].y * delta_t; //for now, set the source to move only along the y-axis. In the future, we can extend this to 3D movement.
            positions[n].z = positions[n-1].z + Velocity_history[n-1].z * delta_t; //for now, set the source to move only along the z-axis. In the future, we can extend this to 3D movement.
        }

    }

    void PhysicalObject::updateVelocity(int n, float delta_t) {
        if (n == 0) return; // Avoid accessing positions[-1] for the first sample

        else {
            Velocity_history[n].x = Velocity_history[n-1].x + Acceleration_history[n-1].x * delta_t;
            Velocity_history[n].y = Velocity_history[n-1].y + Acceleration_history[n-1].y * delta_t;
            Velocity_history[n].z = Velocity_history[n-1].z + Acceleration_history[n-1].z * delta_t;
        }
    }

    void PhysicalObject::setVelocity(Velocity v) {
        Velocity_history[0] = v;
    }

    Velocity PhysicalObject::getVelocitySample(int n) const {
        return Velocity_history[n];
    }

    void PhysicalObject::setAcceleration(Acceleration a) {
        for (auto& a_n : Acceleration_history) {
            a_n = a;
        }
    }
    
    Acceleration PhysicalObject::getAccelerationSample(int n) const {
        return Acceleration_history[n];
    }