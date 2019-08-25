/*

  Main module

  # Modified by Kyle T. Gabriel to fix issue with incorrect GPS data for TTNMapper

  Copyright (C) 2018 by Xose Pérez <xose dot perez at gmail dot com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "configuration.h"
#include "rom/rtc.h"

// Message counter, stored in RTC memory, survives deep sleep
RTC_DATA_ATTR uint32_t count = 0;
// -----------------------------------------------------------------------------
// Submodules
// -----------------------------------------------------------------------------
#include <TinyGPS++.h>

#if defined(PAYLOAD_USE_FULL)
  // includes number of satellites and accuracy
  uint8_t txBuffer[10];
#elif defined(PAYLOAD_USE_CAYENNE)
  // CAYENNE DF
  static uint8_t txBuffer[11] = {0x03, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif

// -----------------------------------------------------------------------------
// Application
// -----------------------------------------------------------------------------

void send() {
  char buffer[40];
  snprintf(buffer, sizeof(buffer), "Latitude: %10.6f\n", gps_latitude());
  screen_print(buffer);
  snprintf(buffer, sizeof(buffer), "Longitude: %10.6f\n", gps_longitude());
  screen_print(buffer);
  snprintf(buffer, sizeof(buffer), "Error: %4.2fm\n", gps_hdop());
  screen_print(buffer);

  buildPacket(txBuffer);

#if LORAWAN_CONFIRMED_EVERY > 0
  bool confirmed = (count % LORAWAN_CONFIRMED_EVERY == 0);
#else
  bool confirmed = false;
#endif

  ttn_cnt(count);
  ttn_send(txBuffer, sizeof(txBuffer), LORAWAN_PORT, confirmed);

  count++;
}

void sleep() {
#if SLEEP_BETWEEN_MESSAGES

  // Show the going to sleep message on the screen
  char buffer[20];
  snprintf(buffer, sizeof(buffer), "Sleeping in %3.1fs\n", (MESSAGE_TO_SLEEP_DELAY / 1000.0));
  screen_print(buffer);

  // Wait for MESSAGE_TO_SLEEP_DELAY millis to sleep
  delay(MESSAGE_TO_SLEEP_DELAY);

  // Turn off screen
  screen_off();

  // Set the user button to wake the board
  sleep_interrupt(BUTTON_PIN, LOW);

  // We sleep for the interval between messages minus the current millis
  // this way we distribute the messages evenly every SEND_INTERVAL millis
  uint32_t sleep_for = (millis() < SEND_INTERVAL) ? SEND_INTERVAL - millis() : SEND_INTERVAL;
  sleep_millis(sleep_for);

#endif
}

void callback(uint8_t message) {
  if (EV_JOINING == message) screen_print("Joining TTN...\n");
  if (EV_JOINED == message) screen_print("TTN joined!\n");
  if (EV_JOIN_FAILED == message) screen_print("TTN join failed\n");
  if (EV_REJOIN_FAILED == message) screen_print("TTN rejoin failed\n");
  if (EV_RESET == message) screen_print("Reset TTN connection\n");
  if (EV_LINK_DEAD == message) screen_print("TTN link dead\n");
  if (EV_ACK == message) screen_print("ACK received\n");
  if (EV_PENDING == message) screen_print("Message discarded\n");
  if (EV_QUEUED == message) screen_print("Message queued\n");

  if (EV_TXCOMPLETE == message) {
    screen_print("Message sent\n");
    sleep();
  }

  if (EV_RESPONSE == message) {

    screen_print("[TTN] Response: ");

    size_t len = ttn_response_len();
    uint8_t data[len];
    ttn_response(data, len);

    char buffer[6];
    for (uint8_t i = 0; i < len; i++) {
      snprintf(buffer, sizeof(buffer), "%02X", data[i]);
      screen_print(buffer);
    }
    screen_print("\n");
  }
}

uint32_t get_count() {
  return count;
}

void setup() {
  // Serial.begin(115200);

  // Debug
  #ifdef DEBUG_PORT
  DEBUG_PORT.begin(SERIAL_BAUD);
  #endif

  #ifdef T_BEAM_V10
  axp192_found = 1;
  if (axp192_found) {
      if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
          Serial.println("AXP192 Begin PASS");
      } else {
          Serial.println("AXP192 Begin FAIL");
      }
      // axp.setChgLEDMode(LED_BLINK_4HZ);
      // Serial.printf("DCDC1: %s\n", axp.isDCDC1Enable() ? "ENABLE" : "DISABLE");
      // Serial.printf("DCDC2: %s\n", axp.isDCDC2Enable() ? "ENABLE" : "DISABLE");
      // Serial.printf("LDO2: %s\n", axp.isLDO2Enable() ? "ENABLE" : "DISABLE");
      // Serial.printf("LDO3: %s\n", axp.isLDO3Enable() ? "ENABLE" : "DISABLE");
      // Serial.printf("DCDC3: %s\n", axp.isDCDC3Enable() ? "ENABLE" : "DISABLE");
      // Serial.printf("Exten: %s\n", axp.isExtenEnable() ? "ENABLE" : "DISABLE");
      // Serial.println("----------------------------------------");

      axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
      axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
      axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
      axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
      axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
      axp.setDCDC1Voltage(3300);

      // Serial.printf("DCDC1: %s\n", axp.isDCDC1Enable() ? "ENABLE" : "DISABLE");
      // Serial.printf("DCDC2: %s\n", axp.isDCDC2Enable() ? "ENABLE" : "DISABLE");
      // Serial.printf("LDO2: %s\n", axp.isLDO2Enable() ? "ENABLE" : "DISABLE");
      // Serial.printf("LDO3: %s\n", axp.isLDO3Enable() ? "ENABLE" : "DISABLE");
      // Serial.printf("DCDC3: %s\n", axp.isDCDC3Enable() ? "ENABLE" : "DISABLE");
      // Serial.printf("Exten: %s\n", axp.isExtenEnable() ? "ENABLE" : "DISABLE");

      pinMode(PMU_IRQ, INPUT_PULLUP);
      attachInterrupt(PMU_IRQ, [] {
          pmu_irq = true;
      }, FALLING);

      axp.adc1Enable(AXP202_BATT_CUR_ADC1, 1);
      axp.enableIRQ(AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_BATT_REMOVED_IRQ | AXP202_BATT_CONNECT_IRQ, 1);
      axp.clearIRQ();

      if (axp.isChargeing()) {
          baChStatus = "Charging";
      }
  } else {
      // Serial.println("AXP192 not found");
  }

  Serial1.begin(GPS_BANUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  #endif

  // Buttons & LED
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  // Hello
  DEBUG_MSG(APP_NAME " " APP_VERSION "\n");

  // Display
  screen_setup();

  // Init GPS
  gps_setup();

  // Show logo on first boot
  if (0 == count) {
    screen_print(APP_NAME " " APP_VERSION, 0, 0);
    screen_show_logo();
    screen_update();
    delay(LOGO_DELAY);
  }

  // TTN setup
  if (!ttn_setup()) {
    screen_print("[ERR] Radio module not found!\n");
    delay(MESSAGE_TO_SLEEP_DELAY);
    screen_off();
    sleep_forever();
  }

  ttn_register(callback);
  ttn_join();
  ttn_sf(LORAWAN_SF);
  ttn_adr(LORAWAN_ADR);
}

void loop() {
  gps_loop();
  ttn_loop();
  screen_loop();

  // Send every SEND_INTERVAL millis
  static uint32_t last = 0;
  static bool first = true;
  if (0 == last || millis() - last > SEND_INTERVAL) {
    if (0 < gps_hdop() && gps_hdop() < 50 && gps_latitude() != 0 && gps_longitude() != 0) {
      last = millis();
      first = false;
      Serial.println("TRANSMITTING");
      send();
    } else {
      if (first) {
        screen_print("Waiting GPS lock\n");
        first = false;
      }
      if (millis() > GPS_WAIT_FOR_LOCK) {
        sleep();
      }
    }
  }
}
