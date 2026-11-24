#include "Position_generator.h"
#include <cmath>

//we don't define a constructor for the Position_generator class since there are no objects to be created from it, only subclasses. The constructor is defined in the subclasses.

//static position generator returns the same position regardless of time
Position Position_generator::position_getter(float time) const {
    return pos_init;
}

Velocity Position_generator::velocity_getter(float time) const {
    return Velocity(0.0f, 0.0f, 0.0f);
}

Acceleration Position_generator::acceleration_getter(float time) const {
    return Acceleration(0.0f, 0.0f, 0.0f);
}

//horizontal position generator returns a position that changes in the x direction based on time and velocity
Position Horizontal_position_generator::position_getter(float time) const {
    Position pos;
    pos.x = pos_init.x + velocity.x * time + 0.5f * acceleration.x * time * time;
    pos.y = pos_init.y + velocity.y * time + 0.5f * acceleration.y * time * time;
    pos.z = pos_init.z + velocity.z * time + 0.5f * acceleration.z * time * time;
    return pos;
}

Velocity Horizontal_position_generator::velocity_getter(float time) const {
    Velocity vel;
    vel.x = velocity.x + acceleration.x * time;
    vel.y = velocity.y + acceleration.y * time;
    vel.z = velocity.z + acceleration.z * time;
    return vel;
}

Acceleration Horizontal_position_generator::acceleration_getter(float time) const {
    return acceleration;
}


//vertical position generator returns a position that changes in the y direction based on time and velocity
Position Vertical_position_generator::position_getter(float time) const {
    Position pos;
    pos.x = pos_init.x + velocity.x * time + 0.5f * acceleration.x * time * time;
    pos.y = pos_init.y + velocity.y * time + 0.5f * acceleration.y * time * time;
    pos.z = pos_init.z + velocity.z * time + 0.5f * acceleration.z * time * time;
    return pos;
}

Velocity Vertical_position_generator::velocity_getter(float time) const {
    Velocity vel;
    vel.x = velocity.x + acceleration.x * time;
    vel.y = velocity.y + acceleration.y * time;
    vel.z = velocity.z + acceleration.z * time;
    return vel;
}

Acceleration Vertical_position_generator::acceleration_getter(float time) const {
    return acceleration;
}

//circular position generator returns a position that changes in a circular path based on time, radius, and angular velocity
Position Circular_position_generator::position_getter(float time) const {
    Position pos;
    // Use the center position for the circular motion
    Position center = center_pos;

    // Calculate the radius based on the initial position and center position (functional to 2D and 3D)
    float radius = sqrt(pow(pos_init.x - center.x, 2) + pow(pos_init.y - center.y, 2) + pow(pos_init.z - center.z, 2));

    // Calculate the angle in the xy-plane and the elevation angle for 3D motion
    //azimuth angle is the angle in the xy-plane and changes with time, while the elevation angle is the angle in the z direction and remains constant for circular motion in 3D
    float azimuth_angle = atan2(pos_init.y - center.y, pos_init.x - center.x); //when working in 2D, this is the angle in the xy-plane
    float elevation_angle = atan2(pos_init.z - center.z, sqrt(pow(pos_init.x - center.x, 2) + pow(pos_init.y - center.y, 2))); //for 3D

    // Calculate the new position based on the circular motion equations, using spherical coordinates for 3D motion
    pos.x = center.x + radius *cos(elevation_angle) * cos(azimuth_angle + angular_velocity * time);
    pos.y = center.y + radius *cos(elevation_angle) * sin(azimuth_angle + angular_velocity * time);
    pos.z = center.z + radius * sin(elevation_angle); //z position remains constant for circular motion in 3D, but can be modified for helical motion if desired
    return pos;

}

Velocity Circular_position_generator::velocity_getter(float time) const {

    Position center = center_pos;

    // Calculate the radius based on the initial position and center position (functional to 2D and 3D)
    float radius = sqrt(pow(pos_init.x - center.x, 2) + pow(pos_init.y - center.y, 2) + pow(pos_init.z - center.z, 2));

    // Calculate the angle in the xy-plane and the elevation angle for 3D motion
    //azimuth angle is the angle in the xy-plane and changes with time, while the elevation angle is the angle in the z direction and remains constant for circular motion in 3D
    float azimuth_angle = atan2(pos_init.y - center.y, pos_init.x - center.x); //when working in 2D, this is the angle in the xy-plane
    float elevation_angle = atan2(pos_init.z - center.z, sqrt(pow(pos_init.x - center.x, 2) + pow(pos_init.y - center.y, 2))); //for 3D

    Velocity vel;

    // Calculate the velocity based on the circular motion equations, using spherical coordinates for 3D motion
    vel.x = -radius * cos(elevation_angle) * sin(azimuth_angle + angular_velocity * time) * angular_velocity;
    vel.y = radius * cos(elevation_angle) * cos(azimuth_angle + angular_velocity * time) * angular_velocity;
    vel.z = 0.0f; //z velocity remains constant for circular motion in 3D, but
    return vel;

}

Acceleration Circular_position_generator::acceleration_getter(float time) const {

    Position center = center_pos;

    // Calculate the radius based on the initial position and center position (functional to 2D and 3D)
    float radius = sqrt(pow(pos_init.x - center.x, 2) + pow(pos_init.y - center.y, 2) + pow(pos_init.z - center.z, 2));

    // Calculate the angle in the xy-plane and the elevation angle for 3D motion
    //azimuth angle is the angle in the xy-plane and changes with time, while the elevation angle is the angle in the z direction and remains constant for circular motion in 3D
    float azimuth_angle = atan2(pos_init.y - center.y, pos_init.x - center.x); //when working in 2D, this is the angle in the xy-plane
    float elevation_angle = atan2(pos_init.z - center.z, sqrt(pow(pos_init.x - center.x, 2) + pow(pos_init.y - center.y, 2))); //for 3D

    Acceleration acc;

    // Calculate the acceleration based on the circular motion equations, using spherical coordinates for 3D motion
    acc.x = -radius * cos(elevation_angle) * cos(azimuth_angle + angular_velocity * time) * angular_velocity * angular_velocity;
    acc.y = -radius * cos(elevation_angle) * sin(azimuth_angle + angular_velocity * time) * angular_velocity * angular_velocity;
    acc.z = 0.0f; //z acceleration remains constant for circular motion in 3D, but
    return acc;

}
