#pragma once

#include <string>
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

namespace aiu {

class AIU : public esphome::Component {
 public:
  void set_negative(esphome::GPIOPin* pin) { this->negative_ = pin; }
  void set_positive(esphome::GPIOPin* pin) { this->positive_ = pin; }

  void setup() override;
  void display(const std::string& data);

 private:
  void set_none();
  void set_negative();
  void set_positive();
  void write_start_porch();
  void write_positive_pulse();
  void write_negative_pulse();
  void write_code(unsigned int code);

  esphome::GPIOPin* negative_;
  esphome::GPIOPin* positive_;
};

}  // namespace aiu