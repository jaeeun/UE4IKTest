// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "fts.device.h"

#include "Engine/EngineTypes.h"
#include "UObject/Interface.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MollisenHANDBPLibrary.generated.h"

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
class FMollisenHANDModule;

UENUM(BlueprintType)
enum class EDeviceType : uint8
{
    None            UMETA(DisplayName = "None"),
    HandL           UMETA(DisplayName = "Hand Left"),
    HandR           UMETA(DisplayName = "Hand Right"),
};

UENUM(BlueprintType)
enum class EDeviceDataType : uint8
{
    None            UMETA(DisplayName = "None"),

    Joint           UMETA(DisplayName = "Joint"),
    Battery         UMETA(DisplayName = "Battery"),

    // IMU & AHRS Data
    Acceleration    UMETA(DisplayName = "Acceleration"),
    Gyroscope       UMETA(DisplayName = "Gyroscope"),
    Magnetic        UMETA(DisplayName = "Magnetic"),
    Quaternion      UMETA(DisplayName = "Quaternion"),
    Rotation        UMETA(DisplayName = "Rotation"),
};

UENUM(BlueprintType)
enum class EJointType : uint8
{
    ThumbCMC    UMETA(DisplayName = "Thumb CMC"),
    ThumbMCP    UMETA(DisplayName = "Thumb MCP"),
    ThumbPIP    UMETA(DisplayName = "Thumb PIP"),
    IndexMCP    UMETA(DisplayName = "Index MCP"),
    IndexPIP    UMETA(DisplayName = "Index PIP"),
    IndexDIP    UMETA(DisplayName = "Index DIP"),
    MiddleMCP   UMETA(DisplayName = "Middle MCP"),
    MiddlePIP   UMETA(DisplayName = "Middle PIP"),
    MiddleDIP   UMETA(DisplayName = "Middle DIP"),
    RingMCP     UMETA(DisplayName = "Ring MCP"),
    RingPIP     UMETA(DisplayName = "Ring PIP"),
    RingDIP     UMETA(DisplayName = "Ring DIP"),
    PinkyMCP    UMETA(DisplayName = "Pinky MCP"),
    PinkyPIP    UMETA(DisplayName = "Pinky PIP"),
    PinkyDIP    UMETA(DisplayName = "Pinky DIP"),
};

UENUM(BlueprintType)
enum class EFingerType : uint8
{
    None    UMETA(DisplayName = "None"),
    Thumb   UMETA(DisplayName = "Thumb"),
    Index   UMETA(DisplayName = "Index"),
    Middle  UMETA(DisplayName = "Middle"),
    Ring    UMETA(DisplayName = "Ring"),
    Pinky   UMETA(DisplayName = "Pinky"),
};

UENUM(BlueprintType)
enum class ECalibrationType : uint8
{
    Min UMETA(DisplayName = "Calibration MIN"),
    Max UMETA(DisplayName = "Calibration MAX"),
};



UCLASS(meta=(BlueprintThreadSafe))
class UMollisenHANDBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
	static TArray<float> GetJointRatioArray(EDeviceType device_type);

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static TArray<float> GetJointDegreeArray(EDeviceType device_type);

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static float GetJointRadio(EDeviceType device_type, EJointType joint_type);

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static float GetJointDegree(EDeviceType device_type, EJointType joint_type);

    UFUNCTION(BlueprintPure, Category = "MollisenHAND")
    static void GetFingerRadio(EDeviceType device_type, EFingerType finger_type, float& joint_1, float& joint_2, float& joint_3);

    UFUNCTION(BlueprintPure, Category = "MollisenHAND")
    static void GetFingerDegree(EDeviceType device_type, EFingerType finger_type, float& joint_1, float& joint_2, float& joint_3);

    UFUNCTION(BlueprintPure, Category = "MollisenHAND")
    static FVector GetAcceleration(EDeviceType device_type);

    UFUNCTION(BlueprintPure, Category = "MollisenHAND")
    static FVector GetGyroscope(EDeviceType device_type);

    UFUNCTION(BlueprintPure, Category = "MollisenHAND")
    static FVector GetMagnetic(EDeviceType device_type);

    UFUNCTION(BlueprintPure, Category = "MollisenHAND")
    static void GetQuaternion(EDeviceType device_type, float& x, float& y, float& z, float& w);

    UFUNCTION(BlueprintPure, Category = "MollisenHAND")
    static FRotator GetRotation(EDeviceType device_type);

    UFUNCTION(BlueprintPure, Category = "MollisenHAND")
    static float GetBatteryLevel(EDeviceType device_type);

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static void DeviceCalibration(EDeviceType device_type, ECalibrationType calibration_type);

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static void SetStateDegreeRange(float min_degree = 0.0f, float max_degree = 90.0f);

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static void SetStateSensitivity(float sensitivity = 0.04f);

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static void SetVibratorPower(EDeviceType device_type, EFingerType finger_type, int power);

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static void StopVibrator(EDeviceType device_type);

    UFUNCTION(BlueprintPure, Category = "MollisenHAND")
    static bool IsConnectDevice(EDeviceType device_type);

    UFUNCTION(BlueprintPure, Category = "MollisenHAND")
    static void BluetoothPairedDevice(EDeviceType device_type, bool& is_paired, FString& device_name, FString& address);

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static FTimerHandle StartNotifyTask();

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static void StopNotifyTask(FTimerHandle handle);

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static TArray<float> GetDeviceRaw(EDeviceType type);

    UFUNCTION(BlueprintCallable, Category = "MollisenHAND")
    static bool SetDeviceCalibration(EDeviceType type, ECalibrationType cali_type, TArray<float> raw_data);

private:
    static FTS::DeviceType ConvertType(const EDeviceType& type);
    static FTS::FingerType ConvertType(const EFingerType& type);
};



UINTERFACE(Blueprintable)
class UMollisenHANDInterface : public UInterface
{
    GENERATED_BODY()
};

class IMollisenHANDInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "MollisenHAND")
    void OnConnectedDevice(EDeviceType DeviceType);

    UFUNCTION(BlueprintNativeEvent, Category = "MollisenHAND")
    void OnDisconnectedDevice(EDeviceType DeviceType);

    UFUNCTION(BlueprintNativeEvent, Category = "MollisenHAND")
    void OnBeginCalibration(EDeviceType DeviceType, ECalibrationType CalibrationType);

    UFUNCTION(BlueprintNativeEvent, Category = "MollisenHAND")
    void OnEndCalibration(EDeviceType DeviceType, ECalibrationType CalibrationType);
};