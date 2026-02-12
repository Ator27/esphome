from esphome.components import remote_receiver
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@Ator27"]

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(remote_receiver.RemoteReceiver),
})
