Lessar LS-HE18KCE2 / LU-HE18KCE2
=====

Air conditioner control via ESPHome Midea component.

## Hardware

ESP32-C3 Mini connected to indoor unit UART.

- TX: GPIO21
- RX: GPIO20
- Baud: 9600

## MQTT Actions

| Topic | Payload | Description |
|-------|---------|-------------|
| `bus/devices/lessar-ac/power/set` | `on`/`off`/`toggle` | Power control |
| `bus/devices/lessar-ac/display/set` | `toggle` | Toggle display |
| `bus/devices/lessar-ac/beeper/set` | `on`/`off` | Beeper control |
| `bus/devices/lessar-ac/swing_step/set` | any | Step swing position |
| `bus/devices/lessar-ac/follow_me/set` | `0-37` | Follow me temperature |

## Climate Control

| Topic | Payload | Description |
|-------|---------|-------------|
| `.../climate/mode/set` | `OFF`, `COOL`, `HEAT`, `FAN_ONLY`, `DRY`, `HEAT_COOL` | Set mode |
| `.../climate/temperature/set` | `17-30` | Set target temperature |
| `.../climate/swing_mode/set` | `OFF`, `VERTICAL`, `HORIZONTAL`, `BOTH` | Set swing mode |
| `.../climate/fan_mode/set` | `auto`, `low`, `medium`, `high` | Set fan mode |
| `.../climate/preset/set` | `none`, `eco`, `boost`, `sleep` | Set preset |

Note: Custom fan modes (`silent`, `turbo`) and presets (`freeze_protection`) use lowercase.

## Flash

```bash
esphome run lessar-ac.yaml
```

## Logs

```bash
esphome logs lessar-ac.yaml
```
