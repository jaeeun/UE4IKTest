// Copyright 2021 Samsung Electronics. All rights reserved.

#include "FlexbuffersFunctionLibrary.h"

#include <flatbuffers/flexbuffers.h>

SampleHandler::SampleHandler() { count = 0; }

SampleHandler &SampleHandler::Instance() {
  static SampleHandler instance;

  return instance;
}

void SampleHandler::MessageHandler(const FMqttMessage &message) {
  if (message.Message.Num() > 0) {
    count += message.Message[0];
  }
}

int SampleHandler::GetCount() { return count; }

void UFlexbuffersFunctionLibrary::StartMqttSubTest(
    const TScriptInterface<IMqttClientInterface> &client, FString topic) {
  client->Subscribe(topic, 0, &(SampleHandler::Instance()));
}

int UFlexbuffersFunctionLibrary::GetCount() {
  return SampleHandler::Instance().GetCount();
}

TArray<uint8> UFlexbuffersFunctionLibrary::FlexbufferDataFromFString(FString data) {
  TArray<uint8> ret;

  flexbuffers::Builder fbb;
  fbb.Clear();
  fbb.Map([&]() { fbb.String("pose", std::string(TCHAR_TO_ANSI(*data))); });
  fbb.Finish();

  std::vector<uint8_t> flex_buf;
  flex_buf = fbb.GetBuffer();

  ret.Append(&flex_buf[0], flex_buf.size());

  return ret;
}

FString UFlexbuffersFunctionLibrary::FStringFromFlexbufferData(TArray<uint8> data) {
  FString ret;

  std::vector<uint8_t> buf(data.Num());
  memcpy(&buf[0], data.GetData(), data.Num());

  auto map = flexbuffers::GetRoot(buf).AsMap();
  ret = FString(UTF8_TO_TCHAR(map["pose"].AsString().c_str()));

  return ret;
}

FVector StringToFingerVector(FString arr) {
    TArray<FString> array = {};
    arr.ParseIntoArray(array, TEXT(","));
    float x = FCString::Atof(*array[0].TrimQuotes());
    float y = FCString::Atof(*array[1].TrimQuotes());
    float z = FCString::Atof(*array[2].TrimQuotes());

    return FVector(x, y, z);
}

FFingerPose UFlexbuffersFunctionLibrary::FingersFromFexbufferData(TArray<uint8> data) {
    FFingerPose ret;

    std::vector<uint8_t> buf(data.Num());
    memcpy(&buf[0], data.GetData(), data.Num());

    auto map = flexbuffers::GetRoot(buf).AsMap();

    ret.hand = FString(UTF8_TO_TCHAR(map["hand"].AsString().c_str()));

    FString finger = FString(UTF8_TO_TCHAR(map["fin0"].AsString().c_str()));
    ret.Thumb.Add(StringToFingerVector(finger));

    finger = FString(UTF8_TO_TCHAR(map["fin1"].AsString().c_str()));
    ret.Index.Add(StringToFingerVector(finger));

    finger = FString(UTF8_TO_TCHAR(map["fin2"].AsString().c_str()));
    ret.Middle.Add(StringToFingerVector(finger));

    finger = FString(UTF8_TO_TCHAR(map["fin3"].AsString().c_str()));
    ret.Ring.Add(StringToFingerVector(finger));

    finger = FString(UTF8_TO_TCHAR(map["fin4"].AsString().c_str()));
    ret.Pinky.Add(StringToFingerVector(finger));


    return ret;
}