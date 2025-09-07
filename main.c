#include "pico/stdlib.h"
#include "pico/unique_id.h"

#include "tusb.h"
#include "class/hid/hid_host.h"
#include "class/hid/hid_device.h"

uint8_t switch_report[8];

void send_switch_report(void) {
    if (tud_hid_ready()) {
        tud_hid_report(1, switch_report, sizeof(switch_report));
    }
}

// HID report descriptor (for a simple gamepad)
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_GAMEPAD(HID_REPORT_ID(1))
};

// -----------------------------
// TinyUSB HID Device callbacks
// -----------------------------

// Called when host requests data from device
uint16_t tud_hid_get_report_cb(uint8_t instance,
                               uint8_t report_id,
                               hid_report_type_t report_type,
                               uint8_t* buffer,
                               uint16_t reqlen) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;
    return 0;
}

// Called when host sends data to device
void tud_hid_set_report_cb(uint8_t instance,
                           uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const* buffer,
                           uint16_t bufsize) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) bufsize;
}

// -----------------------------
// TinyUSB HID Host callbacks
// -----------------------------

void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance,
                      uint8_t const* desc_report, uint16_t desc_len) {
    (void) desc_report;
    (void) desc_len;
    tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    (void) dev_addr;
    (void) instance;
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance,
                                uint8_t const* report, uint16_t len) {
    for (int i = 0; i < len && i < sizeof(switch_report); i++) {
        switch_report[i] = report[i];
    }
    send_switch_report();
    tuh_hid_receive_report(dev_addr, instance);
}

// -----------------------------
// Main loop
// -----------------------------

int main(void) {
    stdio_init_all();
    tusb_init();

    while (1) {
        tud_task(); // TinyUSB device task
        tuh_task(); // TinyUSB host task
    }

    return 0;
}
