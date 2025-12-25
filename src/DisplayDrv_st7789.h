#pragma once

#include <LovyanGFX.hpp>
#include "globals.h"

class LGFX_ST7789 : public lgfx::LGFX_Device
{
public:
    LGFX_ST7789();

private:
    lgfx::Panel_ST7789 _panel;
    lgfx::Bus_SPI _bus;
    lgfx::Light_PWM _light;
};

// Create a display panel instance with manual settings
LGFX_ST7789::LGFX_ST7789()
{
    // SPI Bus configuration
    {
        auto cfg = _bus.config();
        cfg.spi_host = SPI2_HOST; // ESP32-C3 uses SPI2_HOST
        cfg.pin_sclk = TFT_SCLK_PIN;
        cfg.pin_mosi = TFT_MOSI_PIN;
        cfg.pin_miso = TFT_MISO_PIN; // unused
        cfg.pin_dc = TFT_DC_PIN;
        cfg.freq_write = 40000000; // 40MHz
        cfg.freq_read = 16000000;
        _bus.config(cfg);
        _panel.setBus(&_bus);
    }

    // Panel configuration
    {
        auto cfg = _panel.config();
        cfg.pin_cs = TFT_CS_PIN;
        cfg.pin_rst = TFT_RST_PIN;
        cfg.pin_busy = -1; // Busy (-1 = unused)

        // Display parameters - adjust based on your display specs
        cfg.panel_width = 172;
        cfg.panel_height = 320;
        cfg.offset_x = 34;
        cfg.offset_y = 0;
        cfg.offset_rotation = 2;
        cfg.invert = true;
        cfg.rgb_order = false;
        cfg.dlen_16bit = false;
        cfg.bus_shared = true;
        _panel.config(cfg);
    }

    // Backlight configuration (if available)
    if (TFT_BL_PIN != -1)
    {
        auto cfg = _light.config();
        cfg.pin_bl = TFT_BL_PIN;
        cfg.invert = false;  // Set to true if LOW turns backlight ON
        cfg.freq = 44100;    // PWM frequency
        cfg.pwm_channel = 7; // PWM channel
        _light.config(cfg);
        _panel.setLight(&_light);
    }

    setPanel(&_panel);
}