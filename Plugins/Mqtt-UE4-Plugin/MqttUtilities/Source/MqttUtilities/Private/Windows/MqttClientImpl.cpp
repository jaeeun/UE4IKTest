// Copyright (c) 2019 Nineva Studios

#include "MqttClientImpl.h"
#include "MqttRunnable.h"

MqttClientImpl::MqttClientImpl(const char *id) : mosqpp::mosquittopp(id) {}

MqttClientImpl::~MqttClientImpl() {}

void MqttClientImpl::on_connect(int rc) {
  if (rc != 0) {
    return;
  }

  UE_LOG(LogTemp, Warning, TEXT("MQTT => Impl: Connected"));

  Task->OnConnect();
}

void MqttClientImpl::on_disconnect(int rc) {
  if (rc != 0) {
    return;
  }

  UE_LOG(LogTemp, Warning, TEXT("MQTT => Impl: Disconnected"));

  Task->OnDisconnect();
}

void MqttClientImpl::on_publish(int mid) {
  UE_LOG(LogTemp, Warning, TEXT("MQTT => Impl: Mesage published"));

  Task->OnPublished(mid);
}

void MqttClientImpl::on_message(const mosquitto_message *src) {
  if (!src->topic) {
    UE_LOG(LogTemp, Warning, TEXT("MQTT => Impl: Topic is NULL"));
    return;
  }
  if (!src->payload) {
    UE_LOG(LogTemp, Warning, TEXT("MQTT => Impl: Payload is NULL"));
    return;
  }
  //UE_LOG(LogTemp, Warning, TEXT("MQTT => Impl: Message received"));

  FMqttMessage msg;

  msg.Topic = FString(src->topic);
  msg.Qos = src->qos;
  msg.Retain = src->retain;
  int PayloadLength = src->payloadlen;

  std::vector<uint8_t> buf(PayloadLength);
  memcpy(buf.data(), src->payload, PayloadLength);

  msg.Message.Append(&buf[0], PayloadLength);

  Task->OnMessage(msg);
}

void MqttClientImpl::on_subscribe(int mid, int qos_count,
                                  const int *granted_qos) {
  UE_LOG(LogTemp, Warning, TEXT("MQTT => Impl: Subscribed"));

  TArray<int> qos;

  for (auto p = granted_qos; p < granted_qos + qos_count; ++p) {
    qos.Add(*p);
  }

  Task->OnSubscribe(mid, qos);
}

void MqttClientImpl::on_unsubscribe(int mid) {
  UE_LOG(LogTemp, Warning, TEXT("MQTT => Impl: Unsubscribed"));

  Task->OnUnsubscribe(mid);
}