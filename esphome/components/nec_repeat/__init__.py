import esphome.codegen as cg
from esphome.components import remote_base

CODEOWNERS = ["@Ator27"]

# Create a new namespace for our protocol
nec_repeat_ns = cg.esphome_ns.namespace("nec_repeat")
NECRepeatProtocol = nec_repeat_ns.class_("NECRepeatProtocol", remote_base.RemoteProtocol)

# This is the name you'll use under remote_receiver: dump:
CONFIG_SCHEMA = remote_base.validate_protocol({
    cg.GenerateID(): cg.declare_id(NECRepeatProtocol),
})

async def to_code(config):
    cg.add(NECRepeatProtocol.new())
