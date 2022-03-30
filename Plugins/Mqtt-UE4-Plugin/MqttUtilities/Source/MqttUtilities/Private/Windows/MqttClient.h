// Copyright (c) 2019 Nineva Studios

#pragma once

#include "CoreMinimal.h"
#include "MqttClientBase.h"

#include "MqttClient.generated.h"

class FMqttRunnable;

UCLASS()
class UMqttClient : public UMqttClientBase {
  GENERATED_BODY()

  friend class FMqttRunnable;

public:
  void BeginDestroy() override;

  void Connect(FMqttConnectionData connectionData) override;

  void Disconnect() override;

  void Subscribe(FString topic, int qos,
                 const FOnMessageHandlerDelegate &handler) override;

  void Subscribe(FString topic, int qos,
                 IMqttMessageHandlerInterface *handler) override;

  void Unsubscribe(FString topic) override;

  void Publish(FMqttMessage message) override;

public:
  void Init(FMqttClientConfig configData) override;

private:
  FMqttRunnable *Task;
  FRunnableThread *Thread;
  FMqttClientConfig ClientConfig;
};