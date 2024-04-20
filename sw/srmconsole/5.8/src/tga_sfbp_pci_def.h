#ifdef TGA
struct pci_config_type 
        {
        unsigned short int Vendor;
        unsigned short int DeviceId;        
        unsigned short int Command;        
        unsigned short int Status;
        union RevStuffType
             {
             struct RevReg
                {
                unsigned RevId  :8;
                unsigned Class :24;
                }RevBits;
              unsigned int  RevWhole;                
              }RevStuff;

        union CacheStuffType 
             {
             struct CacheReg
               {        
                unsigned CacheSize : 8;
                unsigned Latency : 8;
                unsigned Header : 8;
                unsigned BIST : 8;
                }CacheBits;
             unsigned int CacheWhole;
              }CacheStuff;
                           
        int BaseAddressReg[6];
        int Reserved[2];
        int ExpansionRom;
        int Reserved2[2];
        union InterruptStuffType 
           {
           struct 
               {
                unsigned InterruptLine : 8;
                unsigned InterruptPin  : 8;
                unsigned Min_Gnt       : 8;
                unsigned Max_Lat       : 8;
                }InterruptBits;                        
            unsigned int InterruptWhole;            
            }InterruptStuff ;

        union VGAStuffType 
             {
             struct VGAReg
                {
                unsigned Command :16;
                unsigned Status  :16;
                }VGABits;
              unsigned int  VGAWhole;                
              }VGAStuffType ;

        };

union DeviceRegType 
    {
    struct 
        {
        unsigned IOSpace        : 1;
        unsigned MemorySpace    : 1;
        unsigned BusMaster      : 1;
        unsigned SpecialCycles  : 1;
        unsigned MemEnable      : 1;
        unsigned VGASnoop       : 1;
        unsigned ParityError    : 1;
        unsigned WaitControl    : 1;
        unsigned SERREnable     : 1;
        unsigned Reserved       :23;
        } DeviceBits;
     unsigned int DeviceWhole ;
     };

#endif


