// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MollisenHANDBPLibrary.h"
#include "MollisenHAND.h"

#include "Runtime/Engine/Public/TimerManager.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/Engine.h"


UMollisenHANDBPLibrary::UMollisenHANDBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
    
}

TArray<float> UMollisenHANDBPLibrary::GetJointRatioArray(EDeviceType device_type)
{
    auto modules = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");

    auto sensitivity = modules->GetStateSensitivity();
    auto dip_weight = 2.0f / 3.0f;
    auto write_index = 1;
    auto new_data = TArray<float>();
    new_data.Init(0.0f, 15);

    auto device = modules->GetDevice(ConvertType(device_type));
    if (device != nullptr) {
        auto raw_data = device->GetData(FTS::DeviceDataType::Joint);
        auto raw_data_priv = device->GetDataPriv(FTS::DeviceDataType::Joint);
        for (int n = 0; n < raw_data.Num(); ++n) {
            auto value = raw_data[n];
            if (FMath::Abs(raw_data_priv[n] - value) < sensitivity) {
                value = raw_data_priv[n];
                raw_data[n] = raw_data_priv[n];
            }
            
            new_data[write_index++] = value;
            if (n > 1 && n % 2 == 1)
                new_data[write_index++] = value*dip_weight;
        }
        device->SetDataPriv(FTS::DeviceDataType::Joint, raw_data);
    }
    
	return new_data;
}

TArray<float> UMollisenHANDBPLibrary::GetJointDegreeArray(EDeviceType device_type)
{
    auto  modules = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto  degree_range = modules->GetStateDegreeRange();

    auto  radio_array = GetJointRatioArray(device_type);
    auto& min_value = degree_range.first;
    auto& max_value = degree_range.second;

    for (int n = 0; n < radio_array.Num(); ++n) {
        radio_array[n] = radio_array[n]*(max_value - min_value) + min_value;
    }

    return radio_array;
}

float UMollisenHANDBPLibrary::GetJointRadio(EDeviceType device_type, EJointType joint_type)
{
    return GetJointRatioArray(device_type)[(int)joint_type];
}

float UMollisenHANDBPLibrary::GetJointDegree(EDeviceType device_type, EJointType joint_type)
{
    return GetJointDegreeArray(device_type)[(int)joint_type];
}

void UMollisenHANDBPLibrary::GetFingerRadio(EDeviceType device_type, EFingerType finger_type, float& joint_1, float& joint_2, float& joint_3)
{
    if (finger_type == EFingerType::None)
        return;

    auto radio_array = GetJointRatioArray(device_type);
    joint_1 = radio_array[((int)finger_type - 1)*3 + 0];
    joint_2 = radio_array[((int)finger_type - 1)*3 + 1];
    joint_3 = radio_array[((int)finger_type - 1)*3 + 2];
}

void UMollisenHANDBPLibrary::GetFingerDegree(EDeviceType device_type, EFingerType finger_type, float& joint_1, float& joint_2, float& joint_3)
{
    if (finger_type == EFingerType::None)
        return;

    auto deg_array = GetJointDegreeArray(device_type);
    joint_1 = deg_array[((int)finger_type - 1)*3 + 0];
    joint_2 = deg_array[((int)finger_type - 1)*3 + 1];
    joint_3 = deg_array[((int)finger_type - 1)*3 + 2];
}


FVector UMollisenHANDBPLibrary::GetAcceleration(EDeviceType device_type)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto device = module->GetDevice(ConvertType(device_type));

    if (device != nullptr) {
        auto value = device->GetData(FTS::DeviceDataType::Acceleration);
        if (value.Num() == 3)
            return FVector(value[0], value[1], value[2]);
    }
    return FVector();
}

FVector UMollisenHANDBPLibrary::GetGyroscope(EDeviceType device_type)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto device = module->GetDevice(ConvertType(device_type));

    if (device != nullptr) {
        auto value = device->GetData(FTS::DeviceDataType::Gyroscope);
        if (value.Num() == 3)
            return FVector(value[0], value[1], value[2]);
    }
    return FVector();
}

FVector UMollisenHANDBPLibrary::GetMagnetic(EDeviceType device_type)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto device = module->GetDevice(ConvertType(device_type));

    if (device != nullptr) {
        auto value = device->GetData(FTS::DeviceDataType::Magnetic);
        if (value.Num() == 3)
            return FVector(value[0], value[1], value[2]);
    }
    return FVector();
}

void UMollisenHANDBPLibrary::GetQuaternion(EDeviceType device_type, float& x, float& y, float& z, float& w)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto device = module->GetDevice(ConvertType(device_type));

    if (device != nullptr) {
        auto value = device->GetData(FTS::DeviceDataType::Quaternion);
        if (value.Num() == 4) {
            x = value[0];
            y = value[1];
            z = value[2];
            w = value[3];
        }
    }
}


FRotator UMollisenHANDBPLibrary::GetRotation(EDeviceType device_type)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto device = module->GetDevice(ConvertType(device_type));

    if (device != nullptr) {
        auto value = device->GetData(FTS::DeviceDataType::Rotation);
        if (value.Num() == 3)
            return FRotator(value[0], value[1], value[2]);
    }
    return FRotator();
}

float UMollisenHANDBPLibrary::GetBatteryLevel(EDeviceType device_type)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto device = module->GetDevice(ConvertType(device_type));

    if (device != nullptr) {
        auto value = device->GetData(FTS::DeviceDataType::Battery);
        if (value.Num() == 1)
            return value[0];
    }
    return -1.0f;
}

void UMollisenHANDBPLibrary::SetStateDegreeRange(float min_degree, float max_degree)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    if (module != nullptr)
        module->SetStateDegreeRange(min_degree, max_degree);
}

void UMollisenHANDBPLibrary::SetStateSensitivity(float sensitivity)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    if (module != nullptr)
        module->SetStateSensitivity(sensitivity);
}

void UMollisenHANDBPLibrary::DeviceCalibration(EDeviceType device_type, ECalibrationType calibration_type)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto device = module->GetDevice(ConvertType(device_type));
    if (device != nullptr) {
        TArray<AActor*> actors;
        for (TActorIterator<AActor> it(GWorld); it; ++it) {
            if (it->GetClass()->ImplementsInterface(UMollisenHANDInterface::StaticClass())) {
                IMollisenHANDInterface::Execute_OnBeginCalibration(*it, device_type, calibration_type);
                actors.Add(*it);
            }
        }

        auto data = TArray<float>();
        auto raw_data = device->GetDataRaw(FTS::DeviceDataType::Joint);
        for (int n = 0; n < raw_data.second; ++n) {
            data.Add(raw_data.first[n]);
        }

        device->SetCalibarationData(calibration_type, data);
        
        for (auto& actor : actors) {
            IMollisenHANDInterface::Execute_OnEndCalibration(actor, device_type, calibration_type);
        }
    }
}

void UMollisenHANDBPLibrary::SetVibratorPower(EDeviceType device_type, EFingerType finger_type, int power)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto device = module->GetDevice(ConvertType(device_type));

    if (device != nullptr) {
        device->Vibrator(ConvertType(finger_type), power);
    }
}

void UMollisenHANDBPLibrary::StopVibrator(EDeviceType device_type)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto device = module->GetDevice(ConvertType(device_type));

    if (device != nullptr) {
        device->VibratorStop();
    }
}

bool UMollisenHANDBPLibrary::IsConnectDevice(EDeviceType device_type)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    if (module != nullptr) {
        auto type = FTS::DeviceType::None;
        switch (device_type) {
        case EDeviceType::HandL: type = FTS::DeviceType::HandL; break;
        case EDeviceType::HandR: type = FTS::DeviceType::HandR; break;
        }

        auto device = module->GetDevice(type);
        if (device != nullptr)
            return device->IsConnected();
    }
    return false;
}

void UMollisenHANDBPLibrary::BluetoothPairedDevice(EDeviceType device_type, bool& is_paired, FString& device_name, FString& address)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto device = module->GetDevice(ConvertType(device_type));

    if (device != nullptr) {
        device->UpdateDeviceInfo();

        is_paired = device->IsPaired();
        device_name = device->GetPairedDeviceName();
        address = device->GetPairedDeviceAddress();
    }
}

FTimerHandle UMollisenHANDBPLibrary::StartNotifyTask()
{
    FTimerHandle     handle;
    FTimerDelegate   delegate;

    delegate.BindLambda([]() {
        TFunction<void(void)> function;
        auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
        while (module->GetCallbackTask(function)) {
            if (function)
                function();
        }
    });

    GWorld->GetTimerManager().SetTimer(handle, delegate, 0.1f, true);

    return handle;
}

void UMollisenHANDBPLibrary::StopNotifyTask(FTimerHandle handle)
{
    if (handle.IsValid()) {
        GWorld->GetTimerManager().ClearTimer(handle);
        UE_LOG(LogTemp, Log, TEXT("MollisenAPI] Stop Notify Task."));
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("MollisenAPI] Stop Notify Task. Handle is not valid."));
    }
}

TArray<float> UMollisenHANDBPLibrary::GetDeviceRaw(EDeviceType type)
{
    TArray<float> result;
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    if (module != nullptr) {
        auto device = module->GetDevice(ConvertType(type));
        if (device != nullptr) {
            auto raw = device->GetDataRaw(FTS::DeviceDataType::Joint);
            for (int n = 0; n < raw.second; ++n) {
                result.Add(raw.first[n]);
            }
        }
    }
    return result;
}

bool UMollisenHANDBPLibrary::SetDeviceCalibration(EDeviceType type, ECalibrationType cali_type, TArray<float> raw_data)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    if (module != nullptr) {
        auto device = module->GetDevice(ConvertType(type));
        if (device != nullptr) {
            device->SetCalibarationData(cali_type, raw_data);
            return true;
        }
    }
    return false;
}

FTS::DeviceType UMollisenHANDBPLibrary::ConvertType(const EDeviceType& type)
{
    switch (type) {
    case EDeviceType::HandL: return FTS::DeviceType::HandL;
    case EDeviceType::HandR: return FTS::DeviceType::HandR;
    default:
        return FTS::DeviceType::None;
    }
}

FTS::FingerType UMollisenHANDBPLibrary::ConvertType(const EFingerType& type)
{
    switch (type) {
    case EFingerType::Thumb: return FTS::FingerType::Thumb;
    case EFingerType::Index: return FTS::FingerType::Index;
    case EFingerType::Middle: return FTS::FingerType::Middle;
    case EFingerType::Ring: return FTS::FingerType::Ring;
    case EFingerType::Pinky: return FTS::FingerType::Pinky;
    default:
        return FTS::FingerType::Pinky;
    }
}
