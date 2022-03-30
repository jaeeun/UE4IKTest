// Copyright (c) 2019 Nineva Studios

#include "MqttRunnable.h"

#include "MqttClient.h"
#include "MqttClientImpl.h"

#include "Async/Async.h"

FMqttRunnable::FMqttRunnable(UMqttClient *mqttClient)
    : FRunnable(),
      TaskQueue(new std::queue<FMqttTaskPtr>()),
      TaskQueueLock(new FCriticalSection()),
      client(mqttClient) {}

FMqttRunnable::~FMqttRunnable() {
  if (TaskQueueLock != nullptr) {
    delete TaskQueueLock;
    TaskQueueLock = nullptr;
  }

  if (TaskQueue != nullptr) {
    delete TaskQueue;
    TaskQueue = nullptr;
  }
}

bool FMqttRunnable::Init() {
  bKeepRunning = true;
  return true;
}

void FMqttRunnable::Stop() { StopRunning(); }

uint32 FMqttRunnable::Run() {
  mosqpp::lib_init();

  MqttClientImpl connection(ClientId.c_str());

  connection.max_inflight_messages_set(0);
  connection.Task = this;

  int returnCode = 0;

  if (!Username.empty()) {
    connection.username_pw_set(Username.c_str(), Password.c_str());
  }

  returnCode = connection.connect(Host.c_str(), Port, 10);

  if (returnCode != 0) {
    UE_LOG(LogTemp, Error, TEXT("MQTT => Connection error: %s"),
           ANSI_TO_TCHAR(mosquitto_strerror(returnCode)));
    OnError(returnCode, FString(ANSI_TO_TCHAR(mosquitto_strerror(returnCode))));
    bKeepRunning = false;
  }

  while (bKeepRunning) {
    TaskQueueLock->Lock();

    while (TaskQueue != nullptr) {
      if (TaskQueue->empty()) {
        break;
      }

      FMqttTaskPtr task = TaskQueue->front();
      TaskQueue->pop();

      switch (task->type) {
        case MqttTaskType::Subscribe: {
          auto taskSubscribe = StaticCastSharedPtr<FMqttSubscribeTask>(task);
          returnCode = connection.subscribe(NULL, taskSubscribe->sub,
                                            taskSubscribe->qos);
          if (returnCode == 0) {
            FString topic(ANSI_TO_TCHAR(taskSubscribe->sub));
            switch (taskSubscribe->handler_type) {
              case HandlerType::EventDelegate: {
                auto taskEventHandler =
                    StaticCastSharedPtr<FMqttSubscribeEventDelegateTask>(
                        taskSubscribe);
                if (taskEventHandler->handler.IsBound() == true) {
                  m_MsgEventHandler.Emplace(topic, taskEventHandler->handler);
                }
                break;
              }
              case HandlerType::InterfaceFunction: {
                auto taskFuncHandler =
                    StaticCastSharedPtr<FMqttSubscribeInterfaceFuncTask>(
                        taskSubscribe);
                if (taskFuncHandler->handler != nullptr) {
                  m_MsgFuncHandler.Emplace(topic, taskFuncHandler->handler);
                }
                break;
              }
            }
          }
          break;
        }
        case MqttTaskType::Unsubscribe: {
          auto taskUnsubscribe =
              StaticCastSharedPtr<FMqttUnsubscribeTask>(task);
          returnCode = connection.unsubscribe(NULL, taskUnsubscribe->sub);
          FString topic(ANSI_TO_TCHAR(taskUnsubscribe->sub));
          m_MsgEventHandler.Remove(topic);
          m_MsgFuncHandler.Remove(topic);
          break;
        }
        case MqttTaskType::Publish: {
          auto taskPublish = StaticCastSharedPtr<FMqttPublishTask>(task);
          returnCode = connection.publish(
              NULL, taskPublish->topic, taskPublish->payloadlen,
              taskPublish->payload, taskPublish->qos, taskPublish->retain);
          break;
        }
      }

      if (returnCode != 0) {
        UE_LOG(LogTemp, Error, TEXT("MQTT => Output error: %s"),
               ANSI_TO_TCHAR(mosquitto_strerror(returnCode)));
        OnError(returnCode,
                FString(ANSI_TO_TCHAR(mosquitto_strerror(returnCode))));
      }
    }

    TaskQueueLock->Unlock();

    returnCode = connection.loop(Timeout);

    if (returnCode != 0) {
      UE_LOG(LogTemp, Error, TEXT("MQTT => Connection error: %s"),
             ANSI_TO_TCHAR(mosquitto_strerror(returnCode)));

      if (returnCode == MOSQ_ERR_CONN_REFUSED) {
        OnError(returnCode,
                FString(ANSI_TO_TCHAR(mosquitto_strerror(returnCode))));
        bKeepRunning = false;
      } else {
        if (returnCode == MOSQ_ERR_CONN_LOST) {
          OnError(returnCode,
                  FString(ANSI_TO_TCHAR(mosquitto_strerror(returnCode))));
        }
        connection.reconnect();
      }
    }
  }

  returnCode = connection.disconnect();

  if (returnCode != 0) {
    UE_LOG(LogTemp, Error, TEXT("MQTT => %s"),
           ANSI_TO_TCHAR(mosquitto_strerror(returnCode)));
  }

  delete TaskQueue;
  delete TaskQueueLock;

  TaskQueue = nullptr;
  TaskQueueLock = nullptr;

  return 0;
}

void FMqttRunnable::StopRunning() { bKeepRunning = false; }

bool FMqttRunnable::IsAlive() const { return bKeepRunning; }

void FMqttRunnable::PushTask(FMqttTaskPtr task) {
  TaskQueueLock->Lock();

  if (TaskQueue != nullptr) {
    TaskQueue->push(task);
  }

  TaskQueueLock->Unlock();
}

void FMqttRunnable::OnConnect() {
  AsyncTask(ENamedThreads::GameThread,
            [=]() { client->OnConnectDelegate.ExecuteIfBound(); });
}

void FMqttRunnable::OnDisconnect() {
  AsyncTask(ENamedThreads::GameThread,
            [=]() { client->OnDisconnectDelegate.ExecuteIfBound(); });
}

void FMqttRunnable::OnPublished(int mid) {
  AsyncTask(ENamedThreads::GameThread,
            [=]() { client->OnPublishDelegate.ExecuteIfBound(mid); });
}

void FMqttRunnable::OnMessage(FMqttMessage message) {
  auto func_handler = m_MsgFuncHandler.Find(message.Topic);
  if (func_handler != nullptr && (*func_handler) != nullptr) {
    (*func_handler)->MessageHandler(message);
  }

  auto event_handler = m_MsgEventHandler.Find(message.Topic);
  if (event_handler != nullptr) {
    AsyncTask(ENamedThreads::GameThread,
              [=]() { event_handler->ExecuteIfBound(message); });
  }

  AsyncTask(ENamedThreads::GameThread,
            [=]() { client->OnMessageDelegate.ExecuteIfBound(message); });
}

void FMqttRunnable::OnSubscribe(int mid, const TArray<int> qos) {
  AsyncTask(ENamedThreads::GameThread,
            [=]() { client->OnSubscribeDelegate.ExecuteIfBound(mid, qos); });
}

void FMqttRunnable::OnUnsubscribe(int mid) {
  AsyncTask(ENamedThreads::GameThread,
            [=]() { client->OnUnsubscribeDelegate.ExecuteIfBound(mid); });
}

void FMqttRunnable::OnError(int errCode, FString message) {
  AsyncTask(ENamedThreads::GameThread, [=]() {
    client->OnErrorDelegate.ExecuteIfBound(errCode, message);
  });
}
