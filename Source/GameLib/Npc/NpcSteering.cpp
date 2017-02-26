#include "NpcSteering.h"

#include <algorithm>
#include <iostream>

#include <glm/gtx/vector_angle.hpp>

#include <Deliberation/Core/Math/FloatUtils.h>
#include <Deliberation/Core/Math/MathUtils.h>
#include <Deliberation/Core/Math/Pose3D.h>
#include <Deliberation/Core/Math/Random.h>
#include <Deliberation/Core/StreamUtils.h>

#include <Deliberation/Physics/RigidBody.h>

const glm::vec3 & NpcSteering::destination() const
{
    return *m_destination;
}

void NpcSteering::setDestination(const glm::vec3 & destination)
{
    m_destination = destination;
}

void NpcSteering::setStopAtDestination(bool stopAtDestination)
{
    m_stopAtDestination = stopAtDestination;
}

void NpcSteering::update(NpcFlightControl & flightControl, float seconds)
{
    auto & body = flightControl.body();

    const auto destination = m_destination.value_or(body->transform().position());

    Transform3D predictedPose;
    body->predictTransform(seconds, predictedPose);

    const auto linearDeltaToDestination = destination - predictedPose.position();
    const auto localLinearDeltaToDestination = predictedPose.directionWorldToLocal(linearDeltaToDestination);
    const auto distanceToDestination = glm::length(linearDeltaToDestination);
    const auto localLinearVelocity = predictedPose.directionWorldToLocal(body->linearVelocity());
    const auto localAngularVelocity = predictedPose.directionWorldToLocal(body->angularVelocity());

    if (EpsilonEq(distanceToDestination, 0.0f))
    {
        flightControl.setLocalLinearAcceleration(glm::vec3());
        flightControl.setLocalAngularAccceleration(glm::vec3());
        return;
    }

    auto localDirectionToDestination = glm::normalize(localLinearDeltaToDestination);

    /**
     * Angular
     */
    auto localAngleToDestination = 0.0f;
    if (distanceToDestination > 0.05f)
    {
        localAngleToDestination = glm::angle(localDirectionToDestination,
            glm::vec3(0.0f, 0.0f, -1.0f));

        glm::vec3 rotationAxis;

        if (std::abs(localAngleToDestination - glm::pi<float>()) < 0.05f) rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
        else rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), localDirectionToDestination);

        localAngleToDestination = std::asin(glm::length(rotationAxis));

        const auto idealAngularSpeed = std::min(std::sqrt(2 * flightControl.config().angular.acceleration *
            localAngleToDestination), flightControl.config().angular.maxSpeed);

        const auto idealAngularVelocity = glm::normalize(rotationAxis) * idealAngularSpeed;

        const auto idealAngularVelocityCorrection = idealAngularVelocity - localAngularVelocity;
        const auto idealAngularSpeedCorrection = glm::length(idealAngularVelocityCorrection);

        if (EpsilonGt(idealAngularSpeedCorrection, 0.0f))
        {
            flightControl.setLocalAngularAccceleration(flightControl.correctiveAcceleration(
                idealAngularSpeedCorrection, flightControl.config().angular.acceleration, seconds,
                glm::normalize(idealAngularVelocityCorrection)
            ));
        }
        else
        {
            flightControl.setLocalAngularAccceleration(glm::vec3(0.0f));
        }
    }

    /**
     * Linear
     */
    const auto brakeControl = flightControl.config().direction(-localDirectionToDestination);
    const auto accelerationControl = flightControl.config().direction(localDirectionToDestination);

    float idealSpeed;
    if (m_stopAtDestination)
    {
        if (localAngleToDestination < 0.5f || distanceToDestination < 2.0f) {
            idealSpeed = std::min(std::sqrt(2 * brakeControl.acceleration * distanceToDestination),
                                  accelerationControl.maxSpeed);
        } else {
            idealSpeed = 0.0f;
        }
    }
    else
    {
        idealSpeed = accelerationControl.maxSpeed;
    }

    const auto idealLocalVelocity = localDirectionToDestination * idealSpeed;

    const auto idealLocalVelocityCorrection = idealLocalVelocity - localLinearVelocity;
    const auto idealLocalSpeedCorrection = glm::length(idealLocalVelocityCorrection);
    const auto idealLocalVelocityCorrectionControl = flightControl.config().direction(idealLocalVelocityCorrection);

    if (EpsilonGt(glm::length(idealLocalVelocityCorrection), 0.0f))
    {
        const auto feasibleCorrection = idealLocalVelocityCorrectionControl.acceleration * seconds;
        const auto appliedCorrectionFactor = std::min(idealLocalSpeedCorrection / feasibleCorrection, 1.0f);

        const auto appliedAcceleration = glm::normalize(idealLocalVelocityCorrection) * appliedCorrectionFactor *
            idealLocalVelocityCorrectionControl.acceleration;

        flightControl.setLocalLinearAcceleration(flightControl.correctiveAcceleration(
            idealLocalSpeedCorrection, idealLocalVelocityCorrectionControl.acceleration, seconds,
            glm::normalize(idealLocalVelocityCorrection)
        ));
    }
    else
    {
        flightControl.setLocalLinearAcceleration(glm::vec3());
    }
}