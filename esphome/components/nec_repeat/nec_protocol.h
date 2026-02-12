#pragma once

#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace nec_repeat {

class NECRepeatProtocol : public remote_base::RemoteProtocol {
 public:
  void encode(remote_base::RemoteTransmitData *dst, const remote_base::NECData &data);
  optional<remote_base::NECData> decode(remote_base::RemoteReceiveData src);
  void dump(const remote_base::NECData &data);
};

}  // namespace nec_repeat
}  // namespace esphome
