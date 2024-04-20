/*
/* Platform header file
 */

#ifndef PLATFORM_H
#define PLATFORM_H 1

/*
 * Architecture:
 */
#define ALPHA 1

/*
 * Platform:
 */
#define PC264 1

/*
 * Options (legal for compilation switches):
 */
#define ALCOR 0
#define ALLOAD 0
#define ALMIN 0
#define ALPHA_CONSOLE 1
#define ALUPDATE 0
#define APC_PLATFORM 1
#define API_PLATFORM 0
#define ARC_CCI 0
#define ARC_SUPPORT 0
#define AVANTI 0
#define AVLOAD 0
#define AVMIN 0
#define BACKING_STORE 1
#define BOOTBIOS 0
#define BOOTP_SUPPORT 1
#define BOOT_RESET 0
#define BWX 0
#define CLIPPER 0
#define CLLOAD 0
#define CLMIN 0
#define CLUNIX 0
#define CLUPDATE 0
#define COMMON 0
#define CONSOLE_DRIVER 0
#define CORELLE 0
#define CORTEX 0
#define CORTEXUPDATE 0
#define CPQPHPC 0
#define CRLOAD 0
#define CRUPDATE 0
#define DE200 0
#define DIAG_SUPPORT 1
#define DPUPDATE 0
#define DRIVERSHUT 0
#define DYNAMIC_PRINTING 0
#define DYNAMIC_TRAPPING 0
#define EB164 0
#define EB164LOAD 0
#define EB164MIN 0
#define EEROM_CALLBACKS 0
#define EIGER 0
#define EILOAD 0
#define EIUPDATE 0
#define EV5 0
#define EV6 1
#define EXTRA 0
#define FALCON 0
#define FALLOAD 0
#define FALMIN 0
#define FALUPDATE 0
#define FIBRE_CHANNEL 1
#define FORCE 0
#define FRU5 1
#define FULL_TGA 0
#define FWUPDATE 0
#define GALAXY 0
#define GALAXY_CALLBACKS 0
#define GAMMA 0
#define GCT6 0
#define GLFSAFE 0
#define GLLOAD 0
#define GLUPDATE 0
#define GLXYTRAIN 0
#define GLYNX 0
#define GMFSAFE 0
#define GMLOAD 0
#define GMMIN 0
#define GMQV 0
#define GMUPDATE 0
#define HIGH_BOOT_PTEPAGES 1
#define IMP 0
#define INTERACTIVE_SCREEN 0
#define IO_BUS_RESET 1
#define ISP 0
#define ISP_MODEL_SUPPORT 0
#define K2 0
#define K2LOAD 0
#define K2UPDATE 0
#define LOG_DRIVER_INIT 0
#define LX164 0
#define LXUPDATE 0
#define LYNX 0
#define M1535C 0
#define M1543C 0
#define MARVEL 0
#define MEDULLA 0
#define MEDULLAUPDATE 0
#define MIKASA 0
#define MINTT 0
#define MKLOAD 0
#define MKUPDATE 0
#define MNLOAD 0
#define MNUPDATE 0
#define MODE64 0
#define MODULAR 0
#define MONET 0
#define MONETLFU 0
#define MSCP 1
#define MSCP_SERVER 0
#define MTU 0
#define MTULOAD 0
#define NAUTILUS 0
#define NET_EI_PORT 0
#define NET_EW_PORT 0
#define NET_EX_PORT 0
#define NET_FF_PORT 0
#define NET_FW_PORT 0
#define NET_FX_PORT 0
#define NET_PDQ_PORT 0
#define NONZERO_CONSOLE_BASE 0
#define NORITAKE 0
#define NO_LFU_FLOPPY 0
#define NRLOAD 0
#define NRUPDATE 0
#define NT_ONLY_SUPPORT 1
#define PAUPDATE 0
#define PINKASA 0
#define PINNACLE 0
#define PINTAKE 0
#define PKUPDATE 0
#define PRIMCOR 0
#define PRIVATEER 0
#define PRLOAD 0
#define PRMIN 0
#define PRUPDATE 0
#define PTUPDATE 0
#define RAWHIDE 0
#define REGATTA 0
#define RELEASE 0
#define RGMIN 0
#define RUNS_ON_EISA 0
#define SABLE 0
#define SBFSAFE 0
#define SBLOAD 0
#define SBMIN 0
#define SBQV 0
#define SBUPDATE 0
#define SCAM 0
#define SECURE 1
#define SEPARATE_PAGE_TABLES 0
#define SHARK 0
#define SHOW_RAID 0
#define SIM 0
#define SIMPLE_COM_DEBUG 0
#define SLFSAFE 0
#define SLLOAD 0
#define SLUPDATE 0
#define SLYNX 0
#define SPORT 0
#define STARTSHUT 0
#define SWORDFISH 0
#define SX164 0
#define SXUPDATE 0
#define SYMBOLS 0
#define TAKARA 0
#define TGA 0
#define TGA_VMS_BUILD 0
#define TINOSA 0
#define TKLOAD 0
#define TKUPDATE 0
#define TL6 0
#define TL67 0
#define TURBO 0
#define UNIPROCESSOR 0
#define UNIVERSE 0
#define USB_SUPPORT 0
#define VALIDATE 0
#define VAX 0
#define WBUPDATE 0
#define WEBBRICK 0
#define WFDIAG 0
#define WFLOAD 0
#define WFMIN 0
#define WFUPDATE 0
#define WILDFIRE 0
#define WINDJAMMER 0
#define WJUPDATE 0
#define XDELTA_INITIAL 0
#define XDELTA_ON 0
#define YUKON 0
#define YUKONA 0
#define YUKONAUPDATE 0
#define YUKONUPDATE 0

/*
 * Values:
 */
#define DEFAULT_PSEC_PER_CYCLE 2000
#define DISCRETETIMER_BASE 16384
#define EEROM_BASE 65536
#define EEROM_LENGTH 65536
#define ENET_ID_BASE 14336
#define HEAP_BASE 0
#define HEAP_SIZE 1048576
#define MAX_ELBUF 8192
#define MAX_GRAPHICS 1
#define MAX_MEMORY_ARRAY 4
#define MAX_MEMORY_CHUNK 2
#define MAX_PHYSMEM_MB 0
#define MAX_PROCESSOR_ID 2
#define MAX_RECALL 16
#define MEMORY_CLUSTER_COUNT 11
#define MIN_HEAP_SIZE 98304
#define NVRAM_EV_LEN 2048
#define RTC_BASE 32768
#define RTC_DATA_REG 113
#define RTC_OFFSET_REG 112
#define SYSTEM_TYPE 34

/*
 * Strings:
 */
#define OPTIONS_FW_FILE "pc264fw"

/*
 * C Header lines:
 */

#endif /* PLATFORM_H */
