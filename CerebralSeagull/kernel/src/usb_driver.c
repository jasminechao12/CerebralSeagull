
#include <rcc.h>
#include <systick.h>
#include <gpio.h>
#include <nvic.h>
#include <string.h>

#include "tusb.h"
#include "class/hid/hid_device.h"

#define RCC_PLLCFGR_PLLM (16 << 0)
#define RCC_PLLCFGR_PLLN (192 << 6)
#define RCC_PLLCFGR_PLLQ (4 << 24)
#define RCC_PLLCFGR_PLLSRC (0 << 22)

#define RCC_CR_PLLON (1 << 24)
#define RCC_CR_PLLRDY (1 << 25)

#define RCC_AHB1ENR_GPIOAEN 1
#define RCC_AHB2ENR_OTGFSEN (1 << 7)
#define OTG_FS_IRQ 67

#define FLASH_ACR (*((volatile uint32_t *)0x40023C00))

void usb_init() {
  struct rcc_reg_map *rcc = RCC_BASE;

  // 1. Clock Setup (16MHz HSI -> 48MHz SysClk & USB)
  rcc->apb1_enr |= (1 << 28); 
  FLASH_ACR = (FLASH_ACR & ~0x7) | 0x3; // 3 Wait States for 48MHz
  rcc->pll_cfgr = (8 << 24) | (3 << 16) | (192 << 6) | (8 << 0);
  rcc->cr |= (1 << 24); 
  while (!(rcc->cr & (1 << 25))); 
  rcc->cfgr = (rcc->cfgr & ~0x3) | 0x2;
  while (((rcc->cfgr >> 2) & 0x3) != 0x2); 

  // 2. Sync Timing: Vital for TinyUSB state machine
  systick_init(1000);

  // 3. Enable Peripheral Clocks
  rcc->ahb1_enr |= (1 << 0); // GPIOA
  rcc->ahb2_enr |= (1 << 7); // OTGFS

  // 4. USB Core Soft Reset: Clears hardware from any previous "ghost" states
  while (!(*((volatile uint32_t *)0x50000010) & (1 << 31))); // Wait for AHB Idle
  *((volatile uint32_t *)0x50000010) |= (1 << 0);
  while (*((volatile uint32_t *)0x50000010) & (1 << 0));
  systick_delay(50); 

  // 5. GPIO Setup (AF10 for USB)
  gpio_init(GPIO_A, 11, MODE_ALT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_HIGH, PUPD_NONE, ALT10);
  gpio_init(GPIO_A, 12, MODE_ALT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_HIGH, PUPD_NONE, ALT10);

  // 6. Stack Init
  tusb_init(); 

  // 7. Mandatory Overrides for Manual Wiring (Nucleo-F401RE)
  // GUSBCFG: TRDT=0x9 (48MHz), Force Device Mode
  *((volatile uint32_t *)0x5000000C) = (1 << 30) | (0x9 << 10) | (0x7 << 0);
  
  // GCCFG: PWRDWN=1 (Transceiver ON), NOVBUSSENS=1 (Bypass VBUS Pin PA9)
  *((volatile uint32_t *)0x50000038) = (1 << 16) | (1 << 21);

  // GOTGCTL: Force B-Session valid (Tells chip it is plugged into a PC)
  *((volatile uint32_t *)0x50000000) |= (1 << 7) | (1 << 6); 

  // 8. Connect/Pull-up Trigger
  // Clear Soft Disconnect bit to physically connect the 1.5k pull-up on D+
  *((volatile uint32_t *)0x50000804) &= ~(1 << 1); 

  // 9. Enable Interrupts
  *((volatile uint32_t *)0x50000008) |= 0x1; // Global Interrupt Enable
  nvic_irq(67, 1);
}


uint8_t const hid_report_desc[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                    // USAGE (Joystick)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x01,                    //   USAGE (Pointer)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x02,                    //     REPORT_COUNT (2)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
    0x05, 0x09,                    //   USAGE_PAGE (Button)
    0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
    0x29, 0x08,                    //   USAGE_MAXIMUM (Button 8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void) instance;
    return hid_report_desc;
}

uint16_t tud_hid_descriptor_report_size_cb(uint8_t instance)
{
    (void) instance;
    return sizeof(hid_report_desc);
}

// IRQ handler
void OTG_FS_IRQHandler(void) {
  tud_int_handler(0); 
}

// For tinyUSB
uint32_t board_millis(void)
{
  return total_ticks;
}






uint32_t tusb_time_millis_api(void) {
    return board_millis();
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
  (void) instance; (void) report_id; (void) report_type; (void) buffer; (void) reqlen;
  return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
  (void) instance; (void) report_id; (void) report_type; (void) buffer; (void) bufsize;
}







// --- Device Descriptor ---
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00, 
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xCAFE,
    .idProduct          = 0x4004,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void) {
  return (uint8_t const *) &desc_device;
}

// --- Configuration Descriptor ---
enum { ITF_NUM_HID, ITF_COUNT };
#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

uint8_t const desc_configuration[] = {
  // Config number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, ITF_COUNT, 0, CONFIG_TOTAL_LEN, 0, 100),
  // Interface number, string index, protocol, report descriptor len, EP In & Out addr, size & polling interval
  TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(hid_report_desc), 0x81, CFG_TUD_HID_EP_BUFSIZE, 10)
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
  (void) index;
  return desc_configuration;
}

// --- String Descriptors ---
char const* string_desc_arr [] = {
  (const char[]) { 0x09, 0x04 }, // 0: English language ID
  "Jasmine Chao",                // 1: Manufacturer Name
  "Custom Joystick",             // 2: Product Name
  "SN-001",                      // 3: Serial Number
};

static uint16_t _desc_str[32];

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
  (void) langid;
  uint8_t chr_count;

  if (index == 0) {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  } else {
    if (index >= sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) return NULL;
    const char* str = string_desc_arr[index];
    chr_count = strlen(str);
    if (chr_count > 31) chr_count = 31;
    for(uint8_t i=0; i<chr_count; i++) _desc_str[1+i] = str[i];
  }

  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);
  return _desc_str;
}
