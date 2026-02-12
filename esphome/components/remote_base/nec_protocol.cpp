#include "nec_protocol.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.nec";

static const uint32_t HEADER_HIGH_US = 9000;
static const uint32_t HEADER_LOW_US = 4500;
static const uint32_t REPEAT_LOW_US = 2250;
static const uint32_t BIT_HIGH_US = 560;
static const uint32_t BIT_ONE_LOW_US = 1690;
static const uint32_t BIT_ZERO_LOW_US = 560;

// ---- Repeat cache (minimal state, protocol-local) ----
static optional<NECData> last_nec;
static uint32_t last_nec_time = 0;
static const uint32_t NEC_REPEAT_TIMEOUT_MS = 300;

void NECProtocol::encode(RemoteTransmitData *dst, const NECData &data) {
  ESP_LOGD(TAG, "Sending NEC: address=0x%04X, command=0x%04X command_repeats=%d",
           data.address, data.command, data.command_repeats);

  dst->reserve(2 + 32 + 32 * data.command_repeats + 2);
  dst->set_carrier_frequency(38000);

  dst->item(HEADER_HIGH_US, HEADER_LOW_US);

  for (uint16_t mask = 1; mask; mask <<= 1) {
    dst->item(BIT_HIGH_US,
              (data.address & mask) ? BIT_ONE_LOW_US : BIT_ZERO_LOW_US);
  }

  for (uint16_t repeats = 0; repeats < data.command_repeats; repeats++) {
    for (uint16_t mask = 1; mask; mask <<= 1) {
      dst->item(BIT_HIGH_US,
                (data.command & mask) ? BIT_ONE_LOW_US : BIT_ZERO_LOW_US);
    }
  }

  dst->mark(BIT_HIGH_US);
}

optional<NECData> NECProtocol::decode(RemoteReceiveData src) {
  const uint32_t now = millis();

  // ---- Expire stale cache ----
  if (last_nec.has_value() && (now - last_nec_time > NEC_REPEAT_TIMEOUT_MS)) {
    ESP_LOGD(TAG, "NEC repeat cache expired");
    last_nec.reset();
  }

  // ---- Detect NEC repeat frame ----
  if (src.expect_mark(HEADER_HIGH_US) &&
      src.expect_space(REPEAT_LOW_US) &&
      src.expect_mark(BIT_HIGH_US)) {

    if (last_nec.has_value()) {
      NECData data = *last_nec;
      data.command_repeats = 2;  // repeat indicator
      last_nec_time = now;

      ESP_LOGD(TAG,
               "Received NEC REPEAT: address=0x%04X command=0x%04X",
               data.address, data.command);

      return data;
    }

    // Repeat without prior command → ignore
    ESP_LOGD(TAG, "Received NEC repeat without prior frame, ignoring");
    return {};
  }

  // ---- Full NEC frame ----
  NECData data{
      .address = 0,
      .command = 0,
      .command_repeats = 1,
  };

  if (!src.expect_item(HEADER_HIGH_US, HEADER_LOW_US))
    return {};

  for (uint16_t mask = 1; mask; mask <<= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.address |= mask;
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.address &= ~mask;
    } else {
      return {};
    }
  }

  for (uint16_t mask = 1; mask; mask <<= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.command |= mask;
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.command &= ~mask;
    } else {
      return {};
    }
  }

  src.expect_mark(BIT_HIGH_US);

  // ---- Cache for repeats ----
  last_nec = data;
  last_nec_time = now;

  ESP_LOGD(TAG,
           "Received NEC PRESS: address=0x%04X command=0x%04X",
           data.address, data.command);

  return data;
}

void NECProtocol::dump(const NECData &data) {
  ESP_LOGI(TAG,
           "Received NEC: address=0x%04X command=0x%04X command_repeats=%d",
           data.address, data.command, data.command_repeats);
}

}  // namespace remote_base
}  // namespace esphome
