/*
 * Copyright (c) 2017 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0
 *
 * SPDX-License-Identifier: EPL-2.0
 */
#include "settings.h"
#include "printer.h"
#include "octopus.h"
#include "hono.h"


ADC_MODE(ADC_VCC); // enable reading in VCC of ESP8266

Octopus octopus;
Hono hono(MQTT_BROKER, MQTT_PORT, MQTT_SERVER_FINGERPRINT);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("                             "); // print some spaces to let the Serial Monitor catch up
  Serial.println();
  
  Printer::printlnMsg("Reset reason", ESP.getResetReason());
  
  octopus.begin();
  octopus.connectToWifi(WIFI_SSID, WIFI_PASSWORD);

  if(!hono.connect()) {
    Printer::printlnMsg("Error", "Could not connect to Hub. Restarting octopus");
    ESP.restart();
  }

  Serial.println();    
}

void loop() {
  if(!hono.deviceIsConnected()) {
    octopus.showColor(1, 0x80, 0, 0, 0); // red
    hono.connectDevice(HONO_DEVICE_ID, HONO_DEVICE_AUTH_ID "@" HONO_TENANT, HONO_DEVICE_PASSWORD);
    octopus.showColor(1, 0, 0x80, 0, 0); // green
  }

  Bme680Values bme680Values = octopus.readBme680();
  Bno055Values bno055Values = octopus.readBno055();
  float vcc = octopus.getVcc();

  printSensorData(vcc, bme680Values, bno055Values);
  publishSensorData(vcc, bme680Values, bno055Values);
  Serial.println();

  delay(LOOP_DELAY);
}
