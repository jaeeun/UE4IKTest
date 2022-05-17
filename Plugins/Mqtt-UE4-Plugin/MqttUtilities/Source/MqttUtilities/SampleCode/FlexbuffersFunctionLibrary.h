// Copyright 2021 Samsung Electronics. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "Interface/MqttClientInterface.h"

#include "FlexbuffersFunctionLibrary.generated.h"

class SampleHandler : public IMqttMessageHandlerInterface {
  SampleHandler();

  void MessageHandler(const FMqttMessage &message) override;

public:
  static SampleHandler &Instance();
  int GetCount();
  int count;
};

USTRUCT(BlueprintType)
struct FFingerPose {
  GENERATED_BODY()
public:
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MQTT")
  FString hand;
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MQTT")
  TArray<FVector> Thumb;
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MQTT")
  TArray<FVector> Index;
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MQTT")
  TArray<FVector> Middle;
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MQTT")
  TArray<FVector> Ring;
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MQTT")
  TArray<FVector> Pinky;
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MQTT")
  FString param1;
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MQTT")
  FString param2;
  UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MQTT")
  FString param3;
};

/**
 *
 */
UCLASS()
class MQTTUTILITIES_API UFlexbuffersFunctionLibrary
    : public UBlueprintFunctionLibrary {
  GENERATED_BODY()

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  static void
  StartMqttSubTest(const TScriptInterface<IMqttClientInterface> &client,
                   FString topic);

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  static int GetCount();

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  static TArray<uint8> FlexbufferDataFromFString(FString data);

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  static FString FStringFromFlexbufferData(TArray<uint8> data);

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  static FFingerPose FingersFromFexbufferData(TArray<uint8> data);
};
