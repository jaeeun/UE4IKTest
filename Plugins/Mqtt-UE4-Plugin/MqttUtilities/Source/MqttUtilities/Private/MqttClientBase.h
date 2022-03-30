// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/MqttClientInterface.h"

#include "MqttClientBase.generated.h"

UCLASS()
class UMqttClientBase : public UObject, public IMqttClientInterface {
  GENERATED_BODY()

public:
  virtual ~UMqttClientBase();

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void Connect(FMqttConnectionData connectionData) override;

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void Disconnect() override;

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void Subscribe(FString topic, int qos,
                 const FOnMessageHandlerDelegate &handler) override;

  void Subscribe(FString topic, int qos,
                 IMqttMessageHandlerInterface *handler) override;

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void Unsubscribe(FString topic) override;

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void Publish(FMqttMessage message) override;

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void
  SetOnConnectCallback(const FOnConnectDelegate &onConnectCallback) override;

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void SetOnDisconnectCallback(
      const FOnDisconnectDelegate &onDisconnectCallback) override;

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void
  SetOnPublishCallback(const FOnPublishDelegate &onPublishCallback) override;

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void
  SetOnMessageCallback(const FOnMessageDelegate &onMessageCallback) override;

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void SetOnSubscribeCallback(
      const FOnSubscribeDelegate &onSubscribeCallback) override;

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void SetOnUnsubscribeCallback(
      const FOnUnsubscribeDelegate &onUnsubscribeCallback) override;

  UFUNCTION(BlueprintCallable, Category = "MQTT")
  void SetOnErrorCallback(const FOnMqttErrorDelegate &onErrorCallback) override;

public:
  /** Initialize MQTT client (for internal use only) */
  virtual void Init(FMqttClientConfig configData);

protected:
  UPROPERTY()
  FOnConnectDelegate OnConnectDelegate;
  UPROPERTY()
  FOnDisconnectDelegate OnDisconnectDelegate;
  UPROPERTY()
  FOnPublishDelegate OnPublishDelegate;
  UPROPERTY()
  FOnMessageDelegate OnMessageDelegate;
  UPROPERTY()
  FOnSubscribeDelegate OnSubscribeDelegate;
  UPROPERTY()
  FOnUnsubscribeDelegate OnUnsubscribeDelegate;
  UPROPERTY()
  FOnMqttErrorDelegate OnErrorDelegate;
};