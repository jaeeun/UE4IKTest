// Copyright (c) 2019 Nineva Studios

#include "MqttClientBase.h"

UMqttClientBase::~UMqttClientBase() {}

void UMqttClientBase::Connect(FMqttConnectionData connectionData) {
  // Not implementable
}

void UMqttClientBase::Disconnect() {
  // Not implementable
}

void UMqttClientBase::Subscribe(FString topic, int qos,
                                const FOnMessageHandlerDelegate &handler) {
  // Not implementable
}

void UMqttClientBase::Subscribe(FString topic, int qos,
                                IMqttMessageHandlerInterface *handler) {
  // Not implementable
}

void UMqttClientBase::Unsubscribe(FString topic) {
  // Not implementable
}

void UMqttClientBase::Publish(FMqttMessage message) {
  // Not implementable
}

void UMqttClientBase::SetOnConnectCallback(
    const FOnConnectDelegate &onConnectCallback) {
  OnConnectDelegate = onConnectCallback;
}

void UMqttClientBase::SetOnDisconnectCallback(
    const FOnDisconnectDelegate &onDisconnectCallback) {
  OnDisconnectDelegate = onDisconnectCallback;
}

void UMqttClientBase::SetOnPublishCallback(
    const FOnPublishDelegate &onPublishCallback) {
  OnPublishDelegate = onPublishCallback;
}

void UMqttClientBase::SetOnMessageCallback(
    const FOnMessageDelegate &onMessageCallback) {
  OnMessageDelegate = onMessageCallback;
}

void UMqttClientBase::SetOnSubscribeCallback(
    const FOnSubscribeDelegate &onSubscribeCallback) {
  OnSubscribeDelegate = onSubscribeCallback;
}

void UMqttClientBase::SetOnUnsubscribeCallback(
    const FOnUnsubscribeDelegate &onUnsubscribeCallback) {
  OnUnsubscribeDelegate = onUnsubscribeCallback;
}

void UMqttClientBase::SetOnErrorCallback(
    const FOnMqttErrorDelegate &onErrorCallback) {
  OnErrorDelegate = onErrorCallback;
}

void UMqttClientBase::Init(FMqttClientConfig configData) {
  // Not implementable. Platform specific MQTT-client initialization
}