#define FIRMWARE 0
#define ECU 1
#define RCU 2

#define ARCFW_SCB_ADDRESS 0xf00000
#define SCB_SIGNATURE 0x46536362
#define SCB_VERSION 2
#define SCB_VERSION_3 3
#define SCB_COM1 1
#define SCB_APP_PATH 2
#define SCB_PLATFORM_DATA 4
#define SCB_COMMON_CONSOLE_DATA 8
#define SCB_MAX_APP_PATH_LEN 256
#define SCB_PLATFORM_DATA_OFFSET 0x200
#define SCB_FW_ECU_LOCATION "eisa()disk()fdisk()cf.exe"
#define SCB_PLATFORM_DATA_VERSION 1
#define MAX_MEMORY_DESCRIPTOR 512
#define SCB_COMMON_CONSOLE_DATA_VERSION 2
#define SCB_CCD_BOOT 1
#define SCB_CCD_INSTALL 2

struct MEMORY_DESCRIPTOR {
    unsigned int type;
    unsigned int base_page;
    unsigned int page_count;
    unsigned int tested;
};

struct ARCFW_SCB {
    unsigned int signature;
    unsigned int version;
    unsigned int flags;
    unsigned char app_path[SCB_MAX_APP_PATH_LEN];
    unsigned int platform_offset;
    unsigned int hwrpb;
    unsigned int common_console_data;
};
 
struct ARCFW_SCB_PLATFORM_CONFIG {
    unsigned int valid;
    unsigned char system_manufacturer[80];
    unsigned char system_model[80];
    unsigned char system_serial[16];
    unsigned char system_revision[4];
    unsigned char system_variation[80];
    unsigned char srm_console_version[80];
};

struct ARCFW_SCB_PLATFORM {
    unsigned int size;
    unsigned int version;
    unsigned __int64 flags;
    unsigned __int64 cpu_mask;
    unsigned __int64 gcd_mask;
    unsigned __int64 iod_mask;
    struct ARCFW_SCB_PLATFORM_CONFIG config;
    unsigned int memory_descriptor_count;
    struct MEMORY_DESCRIPTOR memory_descriptor[MAX_MEMORY_DESCRIPTOR];
};

struct ARCFW_SCB_CCD {
    unsigned int version;
    unsigned int srom_mbx_size;
    unsigned int srom_mbx_offset;
    unsigned int argc;
    unsigned int argv;
    unsigned int envp;
    unsigned int config_tree;
    unsigned int flags;
};
