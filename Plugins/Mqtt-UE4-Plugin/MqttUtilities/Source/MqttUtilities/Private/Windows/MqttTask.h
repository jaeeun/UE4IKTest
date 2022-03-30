// Copyright (c) 2019 Nineva Studios

#pragma once

#include "Interface/MqttMessageHandlerInterface.h"

enum class MqttTaskType {
  Publish,
  Subscribe,
  Unsubscribe,
};

enum class HandlerType {
  EventDelegate,
  InterfaceFunction,
};

struct FMqttTask {
  MqttTaskType type;
};

struct FMqttSubscribeTask : public FMqttTask {
  FMqttSubscribeTask();
  virtual ~FMqttSubscribeTask();

  int qos;
  char *sub;
  HandlerType handler_type;
};

struct FMqttSubscribeEventDelegateTask : public FMqttSubscribeTask {
  FMqttSubscribeEventDelegateTask();
  virtual ~FMqttSubscribeEventDelegateTask();

  FOnMessageHandlerDelegate handler;
};

struct FMqttSubscribeInterfaceFuncTask : public FMqttSubscribeTask {
  FMqttSubscribeInterfaceFuncTask();
  virtual ~FMqttSubscribeInterfaceFuncTask();

  IMqttMessageHandlerInterface *handler;
};

struct FMqttUnsubscribeTask : public FMqttTask {
  FMqttUnsubscribeTask();
  ~FMqttUnsubscribeTask();

  char *sub;
};

struct FMqttPublishTask : public FMqttTask {
  FMqttPublishTask();
  ~FMqttPublishTask();

  char *topic;
  int payloadlen;
  void *payload;
  int qos;
  bool retain;
};

typedef TSharedPtr<FMqttSubscribeTask> FMqttSubscribeTaskPtr;
typedef TSharedPtr<FMqttSubscribeEventDelegateTask>
    FMqttSubscribeEventDelegateTaskPtr;
typedef TSharedPtr<FMqttSubscribeInterfaceFuncTask>
    FMqttSubscribeInterfaceFuncTaskPtr;
typedef TSharedPtr<FMqttUnsubscribeTask> FMqttUnsubscribeTaskPtr;
typedef TSharedPtr<FMqttPublishTask> FMqttPublishTaskPtr;
typedef TSharedPtr<FMqttTask> FMqttTaskPtr;