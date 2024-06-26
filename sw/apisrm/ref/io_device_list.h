struct io_device io_device[] = {
#if PROBE_EISA
    { TYPE_EISA, 0x70009835, 0x00000000, "dac960", "Mylex DAC960", "dr", &dr_controllers },
    { TYPE_EISA, 0x10116126, 0x00000000, "dq", "ISA IDE", "dq", &dq_controllers },
    { TYPE_EISA, 0x2042a310, 0x00000000, "er", "DE422", "er", &er_controllers },
    { TYPE_EISA, 0x5042a310, 0x00000000, "ew", "DE425", "ew", &ew_controllers },
    { TYPE_EISA, 0x0030a310, 0x00000000, "fr", "DEFEA", "fr", &fr_controllers },
    { TYPE_EISA, 0x00009004, 0x00000000, "pkh", "Adaptec 1740", "pk", &pk_controllers },
    { TYPE_EISA, 0x002ea310, 0x00000000, "pua", "KFESA", "pu", &pu_controllers },
    { TYPE_EISA, 0x1030110e, 0x00000000, "vga", "Compaq Qvision", "vg", &vg_controllers },
    { TYPE_EISA, 0x1031110e, 0x00000000, "vga", "Compaq Qvision", "vg", &vg_controllers },
    { TYPE_EISA, 0x2031110e, 0x00000000, "vga", "Compaq Qvision", "vg", &vg_controllers },
    { TYPE_EISA, 0x40800941, 0x00000000, "vga_bios", "ISA VGA", "vg", &vg_controllers },
    { TYPE_EISA, 0x0025a310, 0x00000000, "eisa_nvram", "DEC EISA NVRAM", 0, &no_controllers },
    { TYPE_EISA, 0x00604f42, 0x00000000, "dw", "PRO6000", "dw", &dw_controllers },
    { TYPE_EISA, 0x00000001, 0x00000000, "DE200-LE", "DEC LEMAC", "en", &en_controllers },
#endif
#if PROBE_PCI
    { TYPE_PCI, 0x050810da, 0x00000000, "dw", "Thomas Conrad Token Ring", "dw", &dw_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x000f1011, 0x00000000, "fw", "DEC PCI FDDI", "fw", &fw_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00181011, 0x00000000, "mc", "DEC PCI MC", "mc", &mc_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00011000, 0x00000000, "n810", "NCR 53C810", "pk", &pk_controllers, "PKE", 0, 0, 0 },
    { TYPE_PCI, 0x00051000, 0x00000000, "n810", "NCR 53C810a", "pk", &pk_controllers, "PKE", 0, 0, 0 },
    { TYPE_PCI, 0x00031000, 0x00000000, "n810", "NCR 53C825", "pk", &pk_controllers, "PKE", 0, 0, 0 },
    { TYPE_PCI, 0x80031000, 0x00000000, "n825_dssi", "DEC KFPSA", "pu", &pu_controllers, "KFPSA", 0, 0, 0 },
    { TYPE_PCI, 0x000f1000, 0x00000000, "n810", "NCR 53C875", "pk", &pk_controllers, "PKE", 0, 0, 0 },
    { TYPE_PCI, 0x000c1000, 0x00000000, "n810", "NCR 53C895", "pk", &pk_controllers, "PKE", 0, 0, 0 },
    { TYPE_PCI, 0x00121000, 0x00000000, "n810", "NCR 53C895A", "pk", &pk_controllers, "PKE", 0, 0, 0 },
    { TYPE_PCI, 0x000b1000, 0x00000000, "n810", "NCR 53C896", "pk", &pk_controllers, "PKE", 0, 0, 0 },
    { TYPE_PCI, 0x04828086, 0x00000000, "pci_eisa", "Intel 82375", 0, &isa_eisa_count, 0, 0, 0, 0 },
    { TYPE_PCI, 0x04848086, 0x00000000, "pci_isa", "Intel 82378", 0, &isa_eisa_count, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00011011, 0x00000000, 0, "DECchip 21050-AA", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00211011, 0x00000000, 0, "DECchip 21052-AA", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00021011, 0x00000000, "ew", "DECchip 21040-AA", "ew", &ew_controllers, "TULIP", 0, 0, 0 },
    { TYPE_PCI, 0x00141011, 0x00000000, "ew", "DECchip 21041-AA", "ew", &ew_controllers, "TULIP", 0, 0, 0 },
    { TYPE_PCI, 0x00091011, 0x00000000, "ew", "DECchip 21140-AA", "ew", &ew_controllers, "TULIP", 0, 0, 0 },
    { TYPE_PCI, 0x00041011, 0x00000000, "tga", "DECchip ZLXp 21030", "tg", &tg_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x000d1011, 0x00000000, "vga_bios", "DEC PowerStorm", "vg", &vg_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00101011, 0x00000000, "vip7407", "DECchip 7407", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x10201077, 0x00000000, "isp1020", "QLogic ISP1020", "pk", &pk_controllers, "ISP1020", 0, 0, 0 },
    { TYPE_PCI, 0x00081011, 0x00000000, "pks", "DEC KZPSA", "pk", &pk_controllers, "PKS", 0, 0, 0 },
    { TYPE_PCI, 0x11001013, 0x00000000, 0, "Cirrus PD6729", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x740211a8, 0x00000000, 0, "Systech EtPlex 7402", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x740811a8, 0x00000000, 0, "Systech EtPlex 7408", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x730211a8, 0x00000000, 0, "Systech EtPlex 7302", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x730811a8, 0x00000000, 0, "Systech EtPlex 7308", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00071011, 0x00000000, "pci_nvram","DEC PCI NVRAM", 0, &no_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x0518102b, 0x00000000, "vga", "Compaq 2000/P", "vg", &vg_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x30320e11, 0x00000000, "vga", "Compaq 1280/P", "vg", &vg_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x88115333, 0x00000000, "vga_bios", "S3 Trio64/Trio32", "vg", &vg_controllers, "VGA", 0, 0, 0 },
    { TYPE_PCI, 0x88c05333, 0x00000000, "vga_bios", "S3 Trio864", "vg", &vg_controllers, "VGA", 0, 0, 0 },
    { TYPE_PCI, 0x00a01013, 0x00000000, "vga_bios", "Cirrus CL-GD5430", "vg", &vg_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00a81013, 0x00000000, "vga_bios", "Cirrus CL-GD5434", "vg", &vg_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x41581002, 0x00000000, "vga_bios", "ATI Mach32", "vg", &vg_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x43541002, 0x00000000, "vga_bios", "ATI Mach64", "vg", &vg_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00011069, 0x00000000, "dac960", "Mylex DAC960", "dr", &dr_controllers, "DAC960", 0, 0, 0 },
    { TYPE_PCI, 0x00021069, 0x00000000, "dac960", "Mylex DAC960", "dr", &dr_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x06601095, 0x00000000, "cipca", "CIPCA", "pu", &pu_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x1ae510df, 0x00000000, "kgpsa", "KGPSA", "pk", &pk_controllers, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00161011, 0x00000000, 0, "Digital ATMworks 350", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x0020102f, 0x00581011, 0, "Digital ATMworks 351", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x0020102f, 0x00781011, 0, "Digital ATMworks 351", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x0020102f, 0x00000000, 0, "Toshiba Meteor", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00221014, 0x00000000, 0, "BIT 3/IBM PCI-to-PCI Bridge", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x000e1011, 0x00000000, 0, "AV321", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00131011, 0x00000000, 0, "AV301", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x906d10b5, 0x113310df, 0, "PBXDP-AA", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x906d10b5, 0x115510df, 0, "PBXDP-AB", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x906d10b5, 0x115610df, 0, "PBXDP-AC", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x113110df, 0x00000000, 0, "PBXDP-AD", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x115110df, 0x00000000, 0, "PBXDP-AE", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x115210df, 0x00000000, 0, "PBXDP-AF", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x5001112f, 0x00000000, 0, "Arnet Sync 570 IP", 0, 0, 0, 0, 0, 0 },
    { TYPE_PCI, 0x00171011, 0x00000000, 0, "DEC PV-PCI Graphics", 0, 0, 0, 0, 0, 0 },
#endif
    { 0, 0, 0, 0, 0, 0 }
    };                                                       

struct io_device i2o_device = 
    { TYPE_PCI, 0, 0, "pz", "I2O", "pz", &i2o_controllers, "I2O", 0, 0, 0 };

struct io_device_name io_name[] = {
    { 0x19608086, 0x03a2113c, "AMI 431" },
    { 0, 0, 0 }
    };
