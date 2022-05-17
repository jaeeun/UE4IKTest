// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MollisenHAND.h"
#include "MollisenHANDBPLibrary.h"

#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#include "EngineUtils.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

#include "fts.device.h"

#include <functional>
#include <vector>

#define GETENUMSTRING(etype, evalue) ( (FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetNameStringByIndex((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )

#define LOCTEXT_NAMESPACE "FMollisenHANDModule"

void DelegateOnCallback(int type, const wchar_t* message);
void DelegateOnCallbackConnect(int device_type, FTS::Handle handler);
void DelegateOnCallbackDisconnect(int device_type, FTS::Handle handler);

void FMollisenHANDModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
    // Get the base directory of this plugin
    FString strBaseDir = IPluginManager::Get().FindPlugin(TEXT("MollisenHAND"))->GetBaseDir();
    FString LibraryPath;

#if PLATFORM_ANDROID
    LibraryPath = TEXT("libFtsameAPI.so");
#elif PLATFORM_WINDOWS
    #if PLATFORM_WINDOWS && PLATFORM_64BITS
        LibraryPath = FPaths::Combine(*strBaseDir, TEXT("ThirdParty/MollisenAPI/lib/x86_64/ftsame.api.dll"));
    #elif PLATFORM_WINDOWS && PLATFORM_32BITS
        #error Unsupported 32Bit platform!
    #else
        #error Unsupported platform!
    #endif
#endif
    
    _lib = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
    if (_lib != nullptr) {
        UE_LOG(LogTemp, Log, TEXT("Mollisen API] Init Start."));
        FTSCallback(DelegateOnCallback);
        FTSCallbackConnect(DelegateOnCallbackConnect);
        FTSCallbackDisconnect(DelegateOnCallbackDisconnect);

        _devices.insert({ FTS::DeviceType::HandL, new FTSDevice(EDeviceType::HandL) });
        _devices.insert({ FTS::DeviceType::HandR, new FTSDevice(EDeviceType::HandR) });

        if (FTSInitlaize()) {
            this->SetStateDegreeRange(0.0f, 90.0f);
            this->SetStateSensitivity(0.04f);

            UE_LOG(LogTemp, Log, TEXT("Mollisen API] Init Successed."));
        }
    }
}

void FMollisenHANDModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
    if (_lib != nullptr) {
        UE_LOG(LogTemp, Log, TEXT("Mollisen API] Shutdown Module."));
        FTSCleanup();
        UE_LOG(LogTemp, Log, TEXT("Mollisen API] Shutdown Module - FTSCleanup"));
        // Free the dll handle
        FPlatformProcess::FreeDllHandle(_lib);
    }
    _lib = nullptr;
}

FTSDevice* FMollisenHANDModule::GetDevice(FTS::DeviceType device_type)
{
    auto it = _devices.find(device_type);
    if (it != _devices.end()) {
        return it->second;
    }
    return nullptr;
}

int FMollisenHANDModule::GetBufferSize(const EDeviceDataType& type)
{
    static TMap<EDeviceDataType, FTS::DeviceDataType> convert_map {
        { EDeviceDataType::Quaternion, FTS::DeviceDataType::Quaternion },
        { EDeviceDataType::Joint, FTS::DeviceDataType::Joint },
        { EDeviceDataType::Battery, FTS::DeviceDataType::Battery }
    };

    int length = 0;
    if (FTSGetBufferSize(convert_map[type], &length))
        return length;
    return -1;
}

void FMollisenHANDModule::AddCallbackTask(TFunction<void(void)> function)
{
    if (function)
        _callback_queue.Enqueue(function);
}

bool FMollisenHANDModule::GetCallbackTask(TFunction<void(void)>& function)
{
    if (!_callback_queue.IsEmpty()) {
        return _callback_queue.Dequeue(function);
    }
    return false;
}

std::pair<float, float> FMollisenHANDModule::GetStateDegreeRange(void) const
{
    return _state_degree_range;
}

float FMollisenHANDModule::GetStateSensitivity(void) const
{
    return _state_sensitivity;
}

void FMollisenHANDModule::SetStateDegreeRange(const float& min_value, const float& max_value)
{
    if (min_value < max_value)
        _state_degree_range = { min_value, max_value };
}

void FMollisenHANDModule::SetStateSensitivity(const float& value)
{
    const auto min_value = 0.03f;
    const auto max_value = 0.1f;

    _state_sensitivity = FMath::Clamp(value, min_value, max_value);
}
    
void FMollisenHANDModule::OnCallback(int type, FString message)
{
    UE_LOG(LogTemp, Log, TEXT("MollisenAPI] %s"), *message);
}

void FMollisenHANDModule::OnCallbackConnect(FTS::DeviceType device_type, Handle handle)
{
    if (auto device = this->GetDevice(device_type)) {
        device->SetDeviceHandle(handle);
        for (TActorIterator<AActor> it(GWorld); it; ++it) {
            if (it->GetClass()->ImplementsInterface(UMollisenHANDInterface::StaticClass())) {
                IMollisenHANDInterface::Execute_OnConnectedDevice(*it, device->GetDeviceType());
                UE_LOG(LogTemp, Log, TEXT("MollisenAPI] Connected Call : %s"), *it->GetName());
            }
        }
    }
}

void FMollisenHANDModule::OnCallabckDisconnect(FTS::DeviceType device_type, Handle handle)
{
    if (auto device = this->GetDevice(device_type)) {
        device->SetDeviceHandle(nullptr);
        for (TActorIterator<AActor> it(GWorld); it; ++it) {
            if (it->GetClass()->ImplementsInterface(UMollisenHANDInterface::StaticClass())) {
                IMollisenHANDInterface::Execute_OnDisconnectedDevice(*it, device->GetDeviceType());
                UE_LOG(LogTemp, Log, TEXT("MollisenAPI] Disconnected Call : %s"), *it->GetName());
            }
        }
    }
}

void FMollisenHANDModule::BluetoothPair(void)
{
    FTSBluetoothPair();
}

void FMollisenHANDModule::BluetoothUnpair(void)
{
    FTSBluetoothUnpair();
}

void FMollisenHANDModule::BluetoothPairDevice(FString address)
{
    FTSBluetoothPairTarget(TCHAR_TO_ANSI(*address));
}


void DelegateOnCallback(int type, const wchar_t* message)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    auto message_str = FString(message);
    if (module != nullptr) {
        module->OnCallback(type, message_str);
    }
    UE_LOG(LogTemp, Log, TEXT("MollisenAPI.Raw] %s"), *message_str);
}

void DelegateOnCallbackConnect(int device_type, FTS::Handle handler)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    if (module != nullptr) {
        module->AddCallbackTask([=]() {
            module->OnCallbackConnect((FTS::DeviceType)device_type, handler);
        });
    }
    UE_LOG(LogTemp, Log, TEXT("MollisenAPI.Raw] Connected."));
}

void DelegateOnCallbackDisconnect(int device_type, FTS::Handle handler)
{
    auto module = (FMollisenHANDModule*)FModuleManager::Get().GetModule("MollisenHAND");
    if (module != nullptr) {
        module->AddCallbackTask([=]() {
            module->OnCallabckDisconnect((FTS::DeviceType)device_type, handler);
        });
    }
    UE_LOG(LogTemp, Log, TEXT("MollisenAPI.Raw] Disconnected."));
}



FTSDevice::FTSDevice(const EDeviceType& type)
    : _handle(nullptr), _type(type)
{
    _calibrations.insert({ FTS::DeviceDataType::Joint, {
            {1814, 1638, 1123, 1017, 955, 872, 1232, 1041, 1369, 1003 },
            {2200, 1946, 1237, 1227, 1191, 1076, 1457, 1335, 1539, 1259 }
    } });

    FString save_string_data;
    if (FFileHelper::LoadFileToString(save_string_data, *CalibrationDataPath())) {
        FString cali_min_values;
        FString cali_max_values;

        if (save_string_data.Split(TEXT("|"), &cali_min_values, &cali_max_values)) {
            TArray<FString> str_min_values;
            TArray<float> min_values;

            cali_min_values.ParseIntoArray(str_min_values, TEXT(","));
            for (int n = 0; n < str_min_values.Num(); ++n)
                min_values.Add(FCString::Atof(*str_min_values[n]));

            TArray<FString> str_max_values;
            TArray<float> max_values;

            cali_max_values.ParseIntoArray(str_max_values, TEXT(","));
            for (int n = 0; n < str_max_values.Num(); ++n)
                max_values.Add(FCString::Atof(*str_max_values[n]));

            SetCalibarationData(ECalibrationType::Max, max_values, false);
            SetCalibarationData(ECalibrationType::Min, min_values, false);
        }
    }
}

FTSDevice::~FTSDevice(void)
{

}

EDeviceType FTSDevice::GetDeviceType(void) const
{
    return _type;
}

std::pair<float*, int> FTSDevice::GetDataRaw(FTS::DeviceDataType data_type)
{
    if (_handle != nullptr) {
        auto it = _buffers.find(data_type);
        if (it != _buffers.end())
            return it->second;
    }
    return { nullptr, -1 };
}

TArray<float> FTSDevice::GetData(FTS::DeviceDataType data_type)
{
    TArray<float> range_01;

    auto data = this->GetDataRaw(data_type);
    auto cali = _calibrations.find(data_type);
    if (cali != _calibrations.end() && data.second != -1) {
        for (int n = 0; n < data.second; ++n) {
            auto value = data.first[n];
            auto min_value = cali->second.first[n];
            auto max_value = cali->second.second[n];

            auto angle01 = (value - min_value) / (max_value - min_value);

            if (FGenericPlatformMath::IsNaN(angle01))
                angle01 = 0.0f;

            range_01.Add(FMath::Clamp(angle01, -0.1f, 1.0f));
        }
    }
    else {
        int size = 0;
        FTSGetBufferSize(data_type, &size);
        range_01.Init(0.0f, size);
    }
    return range_01;
}

TArray<float> FTSDevice::GetDataPriv(FTS::DeviceDataType data_type)
{
    auto it = _buffers_priv.find(data_type);
    if (it != _buffers_priv.end()) {
        return it->second;
    }
    TArray<float> empty;
    int size = 0;
    FTSGetBufferSize(data_type, &size);
    empty.Init(0.0f, size);
    return empty;
}

bool FTSDevice::Vibrator(FTS::FingerType finger_type, int power)
{
    return FTSVibratorPower(_handle, (int)finger_type, power);
}

void FTSDevice::VibratorStop(void)
{
    FTSVibratorStop(_handle);
}

void FTSDevice::SetDataPriv(FTS::DeviceDataType data_type, TArray<float> data)
{
    auto it = _buffers_priv.find(data_type);
    if (it != _buffers_priv.end()) {
        it->second = data;
    }
    else {
        _buffers_priv.insert({ data_type, data });
    }
}

bool FTSDevice::UpdateDeviceInfo(void)
{
    const char* raw_device_name = nullptr;
    const char* raw_address = nullptr;

    if (FTSBluetoothPairedDevice(_handle, &raw_device_name, &raw_address)) {
        _paired_device_name = raw_device_name;
        _paired_address = raw_address;

        if (_paired_address.Equals("00:00:00:00:00:00"))
            _is_paired = false;
        else
            _is_paired = true;

        return true;
    }
    return false;
}

bool FTSDevice::IsConnected(void) const
{
    return _handle != nullptr;
}

bool FTSDevice::IsPaired(void) const
{
    return _is_paired;
}

FString FTSDevice::GetPairedDeviceName(void) const
{
    return _paired_device_name;
}

FString FTSDevice::GetPairedDeviceAddress(void) const
{
    return _paired_address;
}

void FTSDevice::SetDeviceHandle(FTS::Handle handle)
{
    float* buffer = nullptr;
    int    length = -1;

    std::vector<FTS::DeviceDataType> types { 
        FTS::DeviceDataType::Quaternion,
        FTS::DeviceDataType::Joint, 
        FTS::DeviceDataType::Battery 
    };

    if (handle == nullptr) {
        _handle = nullptr;
        _buffers.clear();
    }
    else if (_handle != handle) {
        _handle = handle;
        for (auto type : types) {
            FTSGetBuffer(handle, type, &buffer, &length);
            _buffers.insert({ type, {buffer, length} });
        }
    }
}

void FTSDevice::SetCalibarationData(const ECalibrationType& type, TArray<float> data, bool is_save)
{
    auto cali = _calibrations.find(FTS::DeviceDataType::Joint);
    if (cali != _calibrations.end() && data.Num() > 0) {
        switch (type) {
        case ECalibrationType::Min: cali->second.first = data; break;
        case ECalibrationType::Max: cali->second.second = data; break;
        }

        if (is_save) {
            auto min_array = FString::SanitizeFloat(cali->second.first[0]);
            for (int n = 1; n < cali->second.first.Num(); ++n)
                min_array += "," + FString::SanitizeFloat(cali->second.first[n]);

            auto max_array = FString::SanitizeFloat(cali->second.second[0]);
            for (int n = 1; n < cali->second.second.Num(); ++n)
                max_array += "," + FString::SanitizeFloat(cali->second.second[n]);

            auto contents = FString::Printf(TEXT("%s|%s"), *min_array, *max_array);

            FFileHelper::SaveStringToFile(contents, *CalibrationDataPath());
        }
    }
}

FString FTSDevice::CalibrationDataPath(void) const
{
    auto device_name = GETENUMSTRING("EDeviceType", _type);
    auto file_path = FString::Printf(TEXT("%s.txt"), *device_name);

    return FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() + file_path);
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMollisenHANDModule, MollisenHAND)