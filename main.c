#include "pico/stdlib.h"
#include "pico/unique_id.h"

#include "tusb.h"
#include "class/hid/hid_host.h"
#include "class/hid/hid_device.h"

// -----------------------------
// HID Report Descriptor
// -----------------------------
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_GAMEPAD(HID_REPORT_ID(1))
};

// Switch report buffer
static uint8_t switch_report[8];

// -----------------------------
// Helpers
// -----------------------------
void send_switch_report(void) {
    if (tud_hid_ready()) {
        tud_hid_report(1, switch_report, sizeof(switch_report));
    }
}

// -----------------------------
// TinyUSB Device HID callbacks
// -----------------------------
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
// TinyUSB Host HID callbacks
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
// TinyUSB Device Descriptors
// -----------------------------
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = 0xCafe, // example VID
    .idProduct          = 0x4000, // example PID
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void) {
    return (uint8_t const *) &desc_device;
}

uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance) {
    (void) instance;
    return desc_hid_report;
}

enum { ITF_NUM_HID, ITF_NUM_TOTAL };
#define CONFIG_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

uint8_t const desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN,
                          TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE,
                       sizeof(desc_hid_report), 0x81, 16, 10)
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
    (void) index;
    return desc_configuration;
}

// String descriptors
char const* string_desc_arr[] = {
    (const char[]){ 0x09, 0x04 }, // 0: English (0x0409)
    "ChaosCo",                    // 1: Manufacturer
    "Pico Switch Bridge",         // 2: Product
    "123456",                     // 3: Serial
};

static uint16_t _desc_str[32];

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void) langid;

    uint8_t chr_count;
    if ( index == 0 ) {
        _desc_str[1] = 0x0409;
        chr_count = 1;
    } else {
        const char* str = string_desc_arr[index];
        chr_count = strlen(str);
        if ( chr_count > 31 ) chr_count = 31;
        for(uint8_t i=0; i<chr_count; i++) {
            _desc_str[1+i] = str[i];
        }
    }

    _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);
    return _desc_str;
}

// -----------------------------
// Main entry
// -----------------------------
int main(void) {
    stdio_init_all();
    tusb_init();

    while (1) {
        tud_task(); // Device task
        tuh_task(); // Host task
    }

    return 0;
}
