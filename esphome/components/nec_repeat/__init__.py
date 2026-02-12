import esphome.codegen as cg
from esphome.components import remote_base
import esphome.config_validation as cv

CODEOWNERS = ["@Ator27"]

# Namespace for our custom protocol
nec_repeat_ns = cg.esphome_ns.namespace("nec_repeat")

# C++ class declaration
NECRepeatProtocol = nec_repeat_ns.class_(
    "NECRepeatProtocol",
    remote_base.RemoteProtocol,
)

# This allows: dump: nec_repeat
CONFIG_SCHEMA = remote_base.validate_protocol(
    {
        cv.GenerateID(): cv.declare_id(NECRepeatProtocol),
    }
)

# Tell ESPHome we depend on remote_base
AUTO_LOAD = ["remote_base"]

async def to_code(config):
    # Register the protocol instance
    var = cg.new_Pvariable(config[cg.CONF_ID])
    cg.add(var)
