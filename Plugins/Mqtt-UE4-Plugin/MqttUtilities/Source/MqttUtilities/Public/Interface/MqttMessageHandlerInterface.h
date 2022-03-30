// Copyright 2021 Samsung Electronics. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/MqttMessage.h"
#include "UObject/Interface.h"

#include "MqttMessageHandlerInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMessageHandlerDelegate, FMqttMessage,
                                  message);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMqttMessageHandlerInterface : public UInterface {
  GENERATED_BODY()
};

/**
 *
 */
class MQTTUTILITIES_API IMqttMessageHandlerInterface {
  GENERATED_BODY()

  // Add interface functions to this class. This is the class that will be
  // inherited to implement this interface.
public:
  /**
   * Handler Function for Subscribe
   * @param FMqttMessage - structure with message data (topic, QoS, payload
   * etc.)
   */
  virtual void MessageHandler(const FMqttMessage &message) = 0;
};

typedef TSharedPtr<IMqttMessageHandlerInterface>
    IMqttMessageHandlerInterfacePtr;