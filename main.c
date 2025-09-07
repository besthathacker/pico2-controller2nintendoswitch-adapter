#include "bsp/board.h"
#include "tusb.h"
#include <string.h>
#include <stdio.h>

// ===================== Switch Pro HID Report =====================
typedef struct __attribute__((packed)) {
    uint16_t buttons;
    uint8_t  hat;     // D-Pad
    int8_t   lx, ly;  // Left stick
    int8_t   rx, ry;  // Right stick
} switch_report_t;

static switch_report_t switch_report = {0};

// HID descriptor for Switch Pro style gamepad
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_GAMEPAD( HID_REPORT_ID(1) )
};

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
    return desc_hid_report;
}

// ===================== TinyUSB Device callbacks =====================
void send_switch_report(void) {
    if (tud_hid_ready()) {
        tud_hid_report(1, &switch_report, sizeof(switch_report));
    }
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                               hid_report_type_t report_type,
                               uint8_t* buffer, uint16_t reqlen) {
    return 0;
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type,
                           uint8_t const* buffer, uint16_t bufsize) {}

// ===================== TinyUSB Host callbacks =====================
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance,
                      uint8_t const* desc_report, uint16_t desc_len) {
    printf("HID device mounted (addr=%d, inst=%d)\n", dev_addr, instance);
    tuh_hid_receive_report(dev_addr, instance);
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    printf("HID device unmounted\n");
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance,
                                uint8_t const* report, uint16_t len) {
    if (len >= 10) {
        uint16_t xb_buttons = report[2] | (report[3] << 8);
        int16_t lx = report[6] - 128;
        int16_t ly = 128 - report[7];
        int16_t rx = report[8] - 128;
        int16_t ry = report[9] - 128;

        uint16_t sw_btns = 0;
        if (xb_buttons & 0x1000) sw_btns |= (1 << 1); // A->B
        if (xb_buttons & 0x2000) sw_btns |= (1 << 0); // B->A
        if (xb_buttons & 0x4000) sw_btns |= (1 << 3); // X->Y
        if (xb_buttons & 0x8000) sw_btns |= (1 << 2); // Y->X
        if (xb_buttons & 0x0100) sw_btns |= (1 << 4); // LB->L
        if (xb_buttons & 0x0200) sw_btns |= (1 << 5); // RB->R
        if (xb_buttons & 0x0400) sw_btns |= (1 << 6); // LT->ZL
        if (xb_buttons & 0x0800) sw_btns |= (1 << 7); // RT->ZR
        if (xb_buttons & 0x0010) sw_btns |= (1 << 9); // Start->+
        if (xb_buttons & 0x0020) sw_btns |= (1 << 8); // Back->-

        // Map D-Pad (hat switch)
        uint8_t hat = 0x08; // centered
        if (xb_buttons & 0x0001) hat = 0;     // Up
        if (xb_buttons & 0x0002) hat = 2;     // Down
        if (xb_buttons & 0x0004) hat = 6;     // Left
        if (xb_buttons & 0x0008) hat = 4;     // Right

        // Update Switch report
        switch_report.buttons = sw_btns;
        switch_report.hat = hat;
        switch_report.lx = lx / 2;
        switch_report.ly = ly / 2;
        switch_report.rx = rx / 2;
        switch_report.ry = ry / 2;
    }

    send_switch_report();
    tuh_hid_receive_report(dev_addr, instance);
}

// ===================== Main =====================
int main(void) {
    board_init();
    tusb_init();

    while (1) {
        tud_task();
        tuh_task();
    }
}
