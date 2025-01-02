import esphome.config_validation as cv
import esphome.codegen as cg
from esphome import pins
from esphome import automation

from esphome.const import (
    CONF_ID
)

aiu_ns = cg.esphome_ns.namespace("aiu")

AIU = aiu_ns.class_("AIU", cg.Component)

DisplayAction = aiu_ns.class_(
    "DisplayAction", automation.Action, cg.Parented.template(AIU)
)

CONF_POSITIVE = 'positive'
CONF_NEGATIVE = 'negative'
CONF_DATA = 'data'

CONFIG_SCHEMA = cv.Schema({
  cv.GenerateID(CONF_ID): cv.declare_id(AIU),
  cv.Required(CONF_POSITIVE): pins.gpio_output_pin_schema,
  cv.Required(CONF_NEGATIVE): pins.gpio_output_pin_schema
}).extend(cv.COMPONENT_SCHEMA)

@automation.register_action(
    "aiu.display",
    DisplayAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(CONF_ID): cv.use_id(AIU),
            cv.Required(CONF_DATA): cv.templatable(cv.string),
        },
        key=CONF_DATA,
    ),
)
async def aiu_display_action(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    template_ = await cg.templatable(config[CONF_DATA], args, cg.std_string)
    cg.add(var.set_value(template_))
    return var

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    positive = await cg.gpio_pin_expression(config[CONF_POSITIVE])
    cg.add(var.set_positive(positive))
    negative = await cg.gpio_pin_expression(config[CONF_NEGATIVE])
    cg.add(var.set_negative(negative))