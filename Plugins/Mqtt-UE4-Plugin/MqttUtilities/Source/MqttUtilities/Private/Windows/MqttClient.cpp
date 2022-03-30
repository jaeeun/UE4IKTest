// Copyright (c) 2019 Nineva Studios

#include "MqttClient.h"
#include "GenericPlatform/GenericPlatformAffinity.h"
#include "HAL/RunnableThread.h"
#include "MqttRunnable.h"
#include "MqttTask.h"
#include "Utils/StringUtils.h"

void UMqttClient::BeginDestroy() {
  UMqttClientBase::BeginDestroy();

  if (Task != nullptr) {
    Task->StopRunning();
  }
}

void UMqttClient::Connect(FMqttConnectionData connectionData) {
  if (Task != nullptr && Task->IsAlive()) {
    UE_LOG(
        LogTemp, Warning,
        TEXT("MQTT => MQTT task is already running. Disconnect and try again"));
    return;
  }

  if (ClientConfig.ClientId.IsEmpty()) {
    UE_LOG(LogTemp, Warning,
           TEXT("MQTT => Client ID is not set. Connection cancelled."));
    return;
  }

  /**
   * All communication between client and broker should be done in a separate
   * thread. Runnable task stores thread-safe queue for output messages
   * (subscribe, unsubscribe, publish) and receives broker responsen that are
   * redirected to client.
   */

  Task = new FMqttRunnable(this);

  Task->Host = std::string(TCHAR_TO_ANSI(*ClientConfig.HostUrl));
  Task->ClientId = std::string(TCHAR_TO_ANSI(*ClientConfig.ClientId));
  Task->Port = ClientConfig.Port;
  Task->Timeout = ClientConfig.Timeout > 1000 ? 1000 : ClientConfig.Timeout;

  Task->Username = std::string(TCHAR_TO_ANSI(*connectionData.Login));
  Task->Password = std::string(TCHAR_TO_ANSI(*connectionData.Password));

  Thread = FRunnableThread::Create(
      Task, TEXT("MQTT"), 0, EThreadPriority::TPri_Normal,
      FGenericPlatformAffinity::GetNoAffinityMask());
}

void UMqttClient::Disconnect() {
  if (Task != nullptr) {
    Task->StopRunning();
  }

  Task = nullptr;
}

void UMqttClient::Subscribe(FString topic, int qos,
                            const FOnMessageHandlerDelegate &handler) {
  if (Task == nullptr || !Task->IsAlive()) {
    UE_LOG(LogTemp, Warning, TEXT("MQTT => There is no running MQTT task"));
    return;
  }

  char *sub = StringUtils::CopyString(topic);
  FOnMessageHandlerDelegate event_delegate = handler;

  FMqttSubscribeEventDelegateTaskPtr taskSubscribe =
      MakeShared<FMqttSubscribeEventDelegateTask>();
  taskSubscribe->type = MqttTaskType::Subscribe;
  taskSubscribe->qos = qos;
  taskSubscribe->sub = sub;
  taskSubscribe->handler_type = HandlerType::EventDelegate;
  taskSubscribe->handler = handler;

  Task->PushTask(taskSubscribe);
}

void UMqttClient::Subscribe(FString topic, int qos,
                            IMqttMessageHandlerInterface *handler) {
  if (Task == nullptr || !Task->IsAlive()) {
    UE_LOG(LogTemp, Warning, TEXT("MQTT => There is no running MQTT task"));
    return;
  }

  char *sub = StringUtils::CopyString(topic);

  FMqttSubscribeInterfaceFuncTaskPtr taskSubscribe =
      MakeShared<FMqttSubscribeInterfaceFuncTask>();
  taskSubscribe->type = MqttTaskType::Subscribe;
  taskSubscribe->qos = qos;
  taskSubscribe->sub = sub;
  taskSubscribe->handler_type = HandlerType::InterfaceFunction;
  taskSubscribe->handler = handler;

  Task->PushTask(taskSubscribe);
}

void UMqttClient::Unsubscribe(FString topic) {
  if (Task == nullptr || !Task->IsAlive()) {
    UE_LOG(LogTemp, Warning, TEXT("MQTT => There is no running MQTT task"));
    return;
  }

  char *sub = StringUtils::CopyString(topic);

  FMqttUnsubscribeTaskPtr taskUnsubscribe = MakeShared<FMqttUnsubscribeTask>();
  taskUnsubscribe->type = MqttTaskType::Unsubscribe;
  taskUnsubscribe->sub = sub;

  Task->PushTask(taskUnsubscribe);
}

void UMqttClient::Publish(FMqttMessage message) {
  if (Task == nullptr || !Task->IsAlive()) {
    UE_LOG(LogTemp, Warning, TEXT("MQTT => There is no running MQTT task"));
    return;
  }

  int message_size = message.Message.Num();
  char *sub = StringUtils::CopyString(message.Topic);
  void *buffer = (uint8_t *)malloc(message_size);

  if (buffer != NULL) {
    memcpy(buffer, message.Message.GetData(), message_size);
  }

  FMqttPublishTaskPtr taskPublish = MakeShared<FMqttPublishTask>();
  taskPublish->type = MqttTaskType::Publish;
  taskPublish->topic = sub;
  taskPublish->payloadlen = message_size;
  taskPublish->payload = buffer;
  taskPublish->qos = message.Qos;
  taskPublish->retain = message.Retain;

  Task->PushTask(taskPublish);
}

void UMqttClient::Init(FMqttClientConfig configData) {
  ClientConfig = configData;
}
