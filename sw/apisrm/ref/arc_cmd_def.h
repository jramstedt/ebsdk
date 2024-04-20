#define ARC_FLASH_OFFSET 0xc0000
#define FIRMWARE 0
#define ECU 1
#define RCU 2

#define ARCFW_SCB_ADDRESS 0xf00000
#define SCB_SIGNATURE 0x46536362
#define SCB_VERSION 1
#define SCB_COM1 1
#define SCB_APP_PATH 2
#define SCB_PLATFORM_DATA 4
#define SCB_MAX_APP_PATH_LEN 256
#define SCB_PLATFORM_DATA_OFFSET 0x200
#define SCB_FW_ECU_LOCATION "eisa()disk()fdisk()cf.exe"
#define SCB_PLATFORM_DATA_VERSION 1
#define MAX_MEMORY_DESCRIPTOR 8
#define MAX_MODULE_DESCRIPTOR 8

struct MEMORY_DESCRIPTOR {
    u_int address;
};

struct MODULE_DESCRIPTOR {
    u_int slot;
    u_int type;
    u_int serial_number;
    u_int hw_rev;
    u_int fw_rev;
};

struct ARCFW_SCB {
    u_int signature;
    u_int version;
    u_int flags;
    u_char app_path[SCB_MAX_APP_PATH_LEN];
    u_int platform_offset;
};
 
struct ARCFW_SCB_PLATFORM {
    u_int version;
    u_int flags;
    UINT cpu_mask;
    UINT gcd_mask;
    UINT iod_mask;
    u_int memory_descriptor_count;
    struct MEMORY_DESCRIPTOR *memdsc[MAX_MEMORY_DESCRIPTOR];
    u_int module_descriptor_count;
    struct MODULE_DESCRIPTOR *moddsc[MAX_MODULE_DESCRIPTOR];    
};
