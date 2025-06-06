#ifndef FORZASTRUCT_H
#define FORZASTRUCT_H
#include <stdint.h>

struct TelemetryData {
    int32_t IsRaceOn;
    uint32_t TimestampMS;
    float EngineMaxRpm;
    float EngineIdleRpm;
    float CurrentEngineRpm;
    float AccelerationX;
    float AccelerationY;
    float AccelerationZ;
    float VelocityX;
    float VelocityY;
    float VelocityZ;
    float AngularVelocityX;
    float AngularVelocityY;
    float AngularVelocityZ;
    float Yaw;
    float Pitch;
    float Roll;
    float NormalizedSuspensionTravelFrontLeft;
    float NormalizedSuspensionTravelFrontRight;
    float NormalizedSuspensionTravelRearLeft;
    float NormalizedSuspensionTravelRearRight;
    float TireSlipRatioFrontLeft;
    float TireSlipRatioFrontRight;
    float TireSlipRatioRearLeft;
    float TireSlipRatioRearRight;
    float WheelRotationSpeedFrontLeft;
    float WheelRotationSpeedFrontRight;
    float WheelRotationSpeedRearLeft;
    float WheelRotationSpeedRearRight;
    int32_t WheelOnRumbleStripFrontLeft;
    int32_t WheelOnRumbleStripFrontRight;
    int32_t WheelOnRumbleStripRearLeft;
    int32_t WheelOnRumbleStripRearRight;
    float WheelInPuddleDepthFrontLeft;
    float WheelInPuddleDepthFrontRight;
    float WheelInPuddleDepthRearLeft;
    float WheelInPuddleDepthRearRight;
    float SurfaceRumbleFrontLeft;
    float SurfaceRumbleFrontRight;
    float SurfaceRumbleRearLeft;
    float SurfaceRumbleRearRight;
    float TireSlipAngleFrontLeft;
    float TireSlipAngleFrontRight;
    float TireSlipAngleRearLeft;
    float TireSlipAngleRearRight;
    float TireCombinedSlipFrontLeft;
    float TireCombinedSlipFrontRight;
    float TireCombinedSlipRearLeft;
    float TireCombinedSlipRearRight;
    float SuspensionTravelMetersFrontLeft;
    float SuspensionTravelMetersFrontRight;
    float SuspensionTravelMetersRearLeft;
    float SuspensionTravelMetersRearRight;
    int32_t CarOrdinal;
    int32_t CarClass;
    int32_t CarPerformanceIndex;
    int32_t DrivetrainType;
    int32_t NumCylinders;
    uint8_t HorizonPlaceholder[12];  // raw bytes
    // skipping HorizonPlaceholder (unknown, you can add as needed)
    float PositionX;
    float PositionY;
    float PositionZ;
    float Speed;
    float Power;
    float Torque;
    float TireTempFrontLeft;
    float TireTempFrontRight;
    float TireTempRearLeft;
    float TireTempRearRight;
    float Boost;
    float Fuel;
    float DistanceTraveled;
    float BestLap;
    float LastLap;
    float CurrentLap;
    float CurrentRaceTime;
    uint16_t LapNumber;
    uint8_t RacePosition;
    uint8_t Accel;
    uint8_t Brake;
    uint8_t Clutch;
    uint8_t HandBrake;
    uint8_t Gear;
    int8_t Steer;
    int8_t NormalizedDrivingLine;
    int8_t NormalizedAIBrakeDifference;
};

#endif  // FORZASTRUCT_H