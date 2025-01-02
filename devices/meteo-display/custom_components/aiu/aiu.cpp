#include "aiu.h"

#include "esphome/core/log.h"

using namespace esphome;

namespace aiu {

static const char* const TAG = "aiu";

static const unsigned int CODES[11] = {0b10010, 0b00010, 0b00110, 0b01111,
                                       0b10101, 0b11110, 0b01010, 0b00011,
                                       0b10011, 0b11100, 0b00111};

static unsigned int char_to_code(char c) {
  if (c == '-') {
    return CODES[10];
  }
  return CODES[c - '0'];
}

static bool is_data_valid(const std::string& data) {
  if (data.size() != 47) {
    return false;
  }
  for (char c : data) {
    if (c != '-' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

void AIU::set_none() {
  this->negative_->digital_write(false);
  this->positive_->digital_write(false);
}

void AIU::set_negative() {
  this->positive_->digital_write(false);
  this->negative_->digital_write(true);
}

void AIU::set_positive() {
  this->negative_->digital_write(false);
  this->positive_->digital_write(true);
}

void AIU::setup() {
  this->negative_->setup();
  this->positive_->setup();

  this->set_negative();
}

void AIU::write_start_porch() {
  this->set_positive();
  delayMicroseconds(30000);
  this->set_none();
  delayMicroseconds(4000);
}

void AIU::write_positive_pulse() {
  this->set_positive();
  delayMicroseconds(4000);
  this->set_none();
  delayMicroseconds(2000);
}

void AIU::write_negative_pulse() {
  this->set_negative();
  delayMicroseconds(4000);
  this->set_none();
  delayMicroseconds(2000);
}

void AIU::write_code(unsigned int code) {
  for (int i = 4; i >= 0; i--) {
    if (code & (1 << i)) {
      this->write_positive_pulse();
    } else {
      this->write_negative_pulse();
    }
  }
}

void AIU::display(const std::string& data) {
  if (!is_data_valid(data)) {
    ESP_LOGE(TAG, "Data is invalid (not ^([0-9\\-]{47})$)");
    return;
  }

  this->write_start_porch();
  for (char c : data) {
    this->write_positive_pulse();
    this->write_code(char_to_code(c));
    this->write_negative_pulse(); 
    delayMicroseconds(4000);
  }
  this->set_negative();
}

}  // namespace aiu