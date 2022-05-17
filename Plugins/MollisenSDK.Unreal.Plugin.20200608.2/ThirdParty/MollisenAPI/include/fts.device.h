#pragma once

#ifdef __MSVC_LIB__
#ifdef FTSDEVICELIB_EXPORTS
#define FTSDEVICE_API __declspec(dllexport) 
#else
#define FTSDEVICE_API __declspec(dllimport) 
#endif
#else
#define FTSDEVICE_API
#endif

namespace FTS
{
    typedef void* Handle;

    typedef void(*FuncMessage)(int type, const wchar_t* message);
    typedef void(*FuncEvent)(int device_type, Handle handle);
    typedef void(*FuncRawData)(int device_type, const unsigned char* packet, const int length);

    enum DeviceType
    {
        None = 0x0000,
        HandL = 0x0101,
        HandR = 0x0102,
    };

    enum DeviceDataType
    {
        Joint = 0x0001,
        Battery = 0x0002,

        // IMU & AHRS Data
        Acceleration = 0x0101,
        Gyroscope = 0x0102,
        Magnetic = 0x0103,
        Quaternion = 0x0104,
        Rotation = 0x0105,
    };

    enum DeviceBufferType
    {
        Int8 = 0x01,
        Int16 = 0x02,
        Int32 = 0x03,
        Int64 = 0x04,
        UInt8 = 0x05,
        UInt16 = 0x06,
        UInt32 = 0x07,
        UInt64 = 0x08,
        Float = 0x11,
        Double = 0x12,
    };

    enum FingerType
    {
        Thumb = 0x01000001,
        Index = 0x01000002,
        Middle = 0x01000003,
        Ring = 0x01000004,
        Pinky = 0x01000005,
    };

    struct DeviceInfo
    {
        char        ID[128];
        wchar_t     Name[128];
        DeviceType  Type;
    };
}

extern "C"
{
    FTSDEVICE_API bool  FTSInitlaize(void);
    FTSDEVICE_API void  FTSCleanup(void);

    FTSDEVICE_API void  FTSCallback(FTS::FuncMessage func);
    FTSDEVICE_API void  FTSCallbackConnect(FTS::FuncEvent func);
    FTSDEVICE_API void  FTSCallbackDisconnect(FTS::FuncEvent func);
    FTSDEVICE_API void  FTSCallbackRawData(FTS::FuncRawData func);

    FTSDEVICE_API bool  FTSGetDeviceHandle(int dev_type, FTS::Handle* handle);
    FTSDEVICE_API bool  FTSGetDeviceInfo(FTS::Handle handle, FTS::DeviceInfo* info);
    FTSDEVICE_API bool  FTSGetBuffer(FTS::Handle handle, int raw_data_type, float** buffer, int* length);
    FTSDEVICE_API bool  FTSGetBufferSize(int raw_data_type, int* length);

    FTSDEVICE_API bool  FTSVibratorPower(FTS::Handle handle, int finger_type, int power);
    FTSDEVICE_API void  FTSVibratorStop(FTS::Handle handle);
    FTSDEVICE_API void  FTSImuSetOrientation(void);

    FTSDEVICE_API void  FTSBluetoothPair(void);
    FTSDEVICE_API void  FTSBluetoothPairTarget(const char* address);
    FTSDEVICE_API void  FTSBluetoothUnpair(void);
    FTSDEVICE_API bool  FTSBluetoothPairedDevice(FTS::Handle handle, const char** device_name, const char** address);
}
