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

FInputInfo UFlexbuffersFunctionLibrary::InputInfoFromFexbufferData(TArray<uint8> data) {
	std::vector<uint8_t> buf(data.Num());
	memcpy(&buf[0], data.GetData(), data.Num());
	auto map = flexbuffers::GetRoot(buf).AsMap();

	FInputInfo ret;
	ret.type = FString(UTF8_TO_TCHAR(map["type"].AsString().c_str()));
	FString screensize = FString(UTF8_TO_TCHAR(map["screensize"].AsString().c_str()));

	TArray<FString> array = {};
	screensize.ParseIntoArray(array, TEXT(","));
	if (array.Num() >= 2) {
		ret.screensize.X = FCString::Atof(*array[0].TrimQuotes());
		ret.screensize.Y = FCString::Atof(*array[1].TrimQuotes());
	}
	else {
		ret.screensize = FVector2D(0, 0);
	}

	return ret;
}

FVector StringToFingerVector(FString arr,int index) {
    TArray<FString> array = {};
    arr.ParseIntoArray(array, TEXT(","));
	if (array.Num() >= 3) {
		float x = FCString::Atof(*array[index * 3 + 0].TrimQuotes());
		float y = FCString::Atof(*array[index * 3 + 1].TrimQuotes());
		float z = FCString::Atof(*array[index * 3 + 2].TrimQuotes());
		return FVector(x, y, z);
	}
	return FVector(0,0,0);
}

FFingerPose UFlexbuffersFunctionLibrary::FingersFromFexbufferData(TArray<uint8> data) {
    
    std::vector<uint8_t> buf(data.Num());
    memcpy(&buf[0], data.GetData(), data.Num());
    auto map = flexbuffers::GetRoot(buf).AsMap();

	FFingerPose ret;
    ret.hand = FString(UTF8_TO_TCHAR(map["hand"].AsString().c_str()));

    FString position = FString(UTF8_TO_TCHAR(map["landmark"].AsString().c_str()));
    ret.Palm = StringToFingerVector(position, 0);
	ret.Thumb.Add(StringToFingerVector(position, 1));
	ret.Thumb.Add(StringToFingerVector(position, 2));
	ret.Thumb.Add(StringToFingerVector(position, 3));
	ret.Thumb.Add(StringToFingerVector(position, 4));
    ret.Index.Add(StringToFingerVector(position, 5));
	ret.Index.Add(StringToFingerVector(position, 6));
	ret.Index.Add(StringToFingerVector(position, 7));
	ret.Index.Add(StringToFingerVector(position, 8));
	ret.Middle.Add(StringToFingerVector(position, 9));
	ret.Middle.Add(StringToFingerVector(position, 10));
	ret.Middle.Add(StringToFingerVector(position, 11));
	ret.Middle.Add(StringToFingerVector(position, 12));
	ret.Ring.Add(StringToFingerVector(position, 13));
	ret.Ring.Add(StringToFingerVector(position, 14));
	ret.Ring.Add(StringToFingerVector(position, 15));
	ret.Ring.Add(StringToFingerVector(position, 16));
	ret.Pinky.Add(StringToFingerVector(position, 17));
	ret.Pinky.Add(StringToFingerVector(position, 18));
	ret.Pinky.Add(StringToFingerVector(position, 19));
	ret.Pinky.Add(StringToFingerVector(position, 20));

    return ret;
}

FFingerGesture UFlexbuffersFunctionLibrary::GestureFromFexbufferData(TArray<uint8> data) {
	
	std::vector<uint8_t> buf(data.Num());
	memcpy(&buf[0], data.GetData(), data.Num());
	auto map = flexbuffers::GetRoot(buf).AsMap();

	FFingerGesture ret;
	ret.gesture = FString(UTF8_TO_TCHAR(map["gesture"].AsString().c_str()));
	ret.param.Add(FString(UTF8_TO_TCHAR(map["param1"].AsString().c_str())));
	ret.param.Add(FString(UTF8_TO_TCHAR(map["param2"].AsString().c_str())));
	ret.param.Add(FString(UTF8_TO_TCHAR(map["param3"].AsString().c_str())));

	return ret;
}

