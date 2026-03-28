#include "AudioManager.h"

bool AudioManager::begin() {
  i2s_config_t config;
  memset(&config, 0, sizeof(config));
  config.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX);
  config.sample_rate = 16000;
  config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
  config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  config.dma_buf_count = 8;
  config.dma_buf_len = 256;
  config.use_apll = false;
  config.tx_desc_auto_clear = true;
  config.fixed_mclk = 0;

  i2s_pin_config_t pins;
  memset(&pins, 0, sizeof(pins));
  pins.bck_io_num = Pins::I2S_BCLK_PIN;
  pins.ws_io_num = Pins::I2S_LRCK_PIN;
  pins.data_out_num = Pins::I2S_DIN_PIN;
  pins.data_in_num = I2S_PIN_NO_CHANGE;

  if (i2s_driver_install(I2S_NUM_0, &config, 0, nullptr) != ESP_OK) {
    return false;
  }

  return i2s_set_pin(I2S_NUM_0, &pins) == ESP_OK;
}

bool AudioManager::loadAlarm(const char* path) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    return false;
  }

  audioSize_ = file.size();
  audio_.reset(new uint8_t[audioSize_]);
  if (!audio_) {
    audioSize_ = 0;
    return false;
  }

  file.read(audio_.get(), audioSize_);
  cursor_ = 0;
  return audioSize_ > 0;
}

void AudioManager::startLoop() {
  if (!audio_ || audioSize_ == 0) {
    return;
  }
  cursor_ = 0;
  playing_ = true;
}

void AudioManager::stop() {
  playing_ = false;
  cursor_ = 0;
  i2s_zero_dma_buffer(I2S_NUM_0);
}

void AudioManager::update() {
  if (!playing_ || !audio_ || audioSize_ == 0) {
    return;
  }

  const size_t remaining = audioSize_ - cursor_;
  const size_t chunk = remaining > kChunkSize ? kChunkSize : remaining;
  size_t written = 0;
  i2s_write(I2S_NUM_0, audio_.get() + cursor_, chunk, &written, 0);
  cursor_ += written;

  if (cursor_ >= audioSize_) {
    cursor_ = 0;
  }
}

bool AudioManager::playing() const {
  return playing_;
}
