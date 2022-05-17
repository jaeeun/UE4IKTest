// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "fts.device.h"
#include "Containers/Queue.h"

#include <unordered_map>

enum class EDeviceType : uint8;
enum class ECalibrationType : uint8;
enum class EDeviceDataType : uint8;

struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};

class FTSDevice;
class FMollisenHANDModule : public IModuleInterface
{
    using Handle = void*;

private:
    Handle _lib;

    std::unordered_map<FTS::DeviceType, FTSDevice*, EnumClassHash> _devices;
    TQueue<TFunction<void(void)>>                   _callback_queue;

private:
    std::pair<float, float> _state_degree_range;
    float                   _state_sensitivity;

public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:
    FTSDevice*  GetDevice(FTS::DeviceType device_type);
    int         GetBufferSize(const EDeviceDataType& type);

    void AddCallbackTask(TFunction<void(void)> function);
    bool GetCallbackTask(TFunction<void(void)>& function);

public:
    std::pair<float, float> GetStateDegreeRange(void) const;
    float                   GetStateSensitivity(void) const;

    void SetStateDegreeRange(const float& min_value, const float& max_value);
    void SetStateSensitivity(const float& value);

public:
    void BluetoothPair(void);
    void BluetoothUnpair(void);
    void BluetoothPairDevice(FString address);
    
public:
    void OnCallback(int type, FString message);
    void OnCallbackConnect(FTS::DeviceType device_type, Handle handle);
    void OnCallabckDisconnect(FTS::DeviceType device_type, Handle handle);
};

class FTSDevice
{
    typedef std::pair<float*, int> Buffer;
    typedef std::pair<TArray<float>, TArray<float>> Calibration;

private:
    FTS::Handle _handle;
    std::unordered_map<FTS::DeviceDataType, Buffer, EnumClassHash>         _buffers;
    std::unordered_map<FTS::DeviceDataType, TArray<float>, EnumClassHash>  _buffers_priv;
    std::unordered_map<FTS::DeviceDataType, Calibration, EnumClassHash>    _calibrations;

private:
    EDeviceType _type;

    bool    _is_paired;
    FString _paired_device_name;
    FString _paired_address;

public:
    FTSDevice(void) = delete;
    FTSDevice(const EDeviceType& type);
    ~FTSDevice(void);

public:
    EDeviceType GetDeviceType(void) const;

public:
    std::pair<float*, int>  GetDataRaw(FTS::DeviceDataType data_type);
    TArray<float>           GetData(FTS::DeviceDataType data_type);
    TArray<float>           GetDataPriv(FTS::DeviceDataType data_type);

    bool Vibrator(FTS::FingerType finger_type, int power);
    void VibratorStop(void);

public:
    void SetDataPriv(FTS::DeviceDataType data_type, TArray<float> data);
    bool UpdateDeviceInfo(void);

public:
    bool IsConnected(void) const;
    bool IsPaired(void) const;

    FString GetPairedDeviceName(void) const;
    FString GetPairedDeviceAddress(void) const;

    void SetDeviceHandle(FTS::Handle handle);
    void SetCalibarationData(const ECalibrationType& type, TArray<float> data, bool is_save = true);

private:
    FString CalibrationDataPath(void) const;
};
