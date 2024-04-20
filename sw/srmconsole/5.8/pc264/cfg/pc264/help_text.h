/* This file contains the console help text data structures.
 * It is arranged as an array of topic text strings,
 * referenced through pointer triples in the array helpbuf.
 */

char topic_001 [] = { "shell" };
char  text_001 [] = { "Create a new shell process." };
char   syn_001 [] = { "shell [-{v|x}] [<arg>...] " };

char topic_002 [] = { "kill" };
char  text_002 [] = { "Stop and delete a process." };
char   syn_002 [] = { "kill [<pid>...] " };

char topic_003 [] = { "exit" };
char  text_003 [] = { "Exit the current shell." };
char   syn_003 [] = { "exit <exit_value> " };

char topic_004 [] = { "break" };
char  text_004 [] = { "Break out of a for, while, or until loop." };
char   syn_004 [] = { "break <break_level> " };

char topic_005 [] = { "boot" };
char  text_005 [] = { "Bootstrap the system." };
char   syn_005 [] = { "boot [-file <filename>] [-flags <longword>[,<longword>]] \n\
\t               [-protocols <enet_protocol>] [-halt] \n\
\t               [<boot_device>][,<boot_device>] " };

char topic_006 [] = { "buildfru" };
char  text_006 [] = { "Build a FRU EEPROM data structure from command arguments." };
char   syn_006 [] = { "buildfru <fru_name> <part_num> <serial_num> [<model> [<alias>]] \n\
\t               or \n\
\t               -s <fru_name> <offset> <byte> [<byte>...] " };

char topic_007 [] = { "show fru" };
char  text_007 [] = { "Show the FRUs in a system." };
char   syn_007 [] = { "show fru [-e] " };

char topic_008 [] = { "clear_error" };
char  text_008 [] = { "Clear error information in the serial control bus EEROM." };
char   syn_008 [] = { "clear_error {all,cpu0,cpu1,io,mem0,mem1,mem2,mem3} " };

char topic_009 [] = { "deposit" };
char  text_009 [] = { "Write data to a specified address." };
char   syn_009 [] = { "deposit [-{b,w,l,q,o,h}] [-{physical,virtual,gpr,fpr,ipr}] \n\
\t               [-n <count>] [-s <step>] \n\
\t               [<device>:]<address> <data> " };

char topic_010 [] = { "examine" };
char  text_010 [] = { "Display data at a specified address." };
char   syn_010 [] = { "examine [-{b,w,l,q,o,h,d}] [-{physical,virtual,gpr,fpr,ipr}] \n\
\t               [-n <count>] [-s <step>] \n\
\t               [<device>:]<address> " };

char topic_011 [] = { "show iobq" };
char  text_011 [] = { "Show the I/O counter blocks." };
char   syn_011 [] = { "show_iobq " };

char topic_012 [] = { "isacfg" };
char  text_012 [] = { "sets or shows user inputted isa configuration data." };
char   syn_012 [] = { "isacfg ([-slot <slot#>] \n\
\t               [-dev <device#>] \n\
\t               [-all|-rm|-mk|-mod|-init] [-<field> <value>] " };

char topic_013 [] = { "isp1020_edit" };
char  text_013 [] = { "Edit ISP1020 NVRAM parameters." };
char   syn_013 [] = { "isp1020_edit [-sd] [-offset] [-id] [-allids] [-value] \n\
\t               [-bit] [-set] -[clear] [<isp1020_controller_name>] " };

char topic_014 [] = { "show hwrpb" };
char  text_014 [] = { "Display the address of the HWRPB." };
char   syn_014 [] = { "show hwrpb () " };

char topic_015 [] = { "show map" };
char  text_015 [] = { "Display the system virtual memory map." };
char   syn_015 [] = { "show map () " };

char topic_016 [] = { "set" };
char  text_016 [] = { "Set or modify the value of an environment variable." };
char   syn_016 [] = { "set <envar> <value> \n\
\t               [-integer] [-string] \n\
\t               where \n\
\t               <envar>={auto_action,bootdef_dev,boot_file,boot_osflags,...} " };

char topic_017 [] = { "show" };
char  text_017 [] = { "Display an environment variable value or other information." };
char   syn_017 [] = { "show [{bios,config,device,hwrpb,memory,pal,version,...}] \n\
\t               [<envar>] \n\
\t               where: \n\
\t               <envar>={auto_action,bootdef_dev,boot_file,boot_osflags,...} " };

char topic_018 [] = { "clear" };
char  text_018 [] = { "Clear an option or an environment variable." };
char   syn_018 [] = { "clear [option] or envar " };

char topic_019 [] = { "set host" };
char  text_019 [] = { "Connect the console to another console or server." };
char   syn_019 [] = { "set host [-dup] [-task <task_name>] <target> " };

char topic_020 [] = { "show cluster" };
char  text_020 [] = { "Display open virtual circuits." };
char   syn_020 [] = { "show cluster " };

char topic_021 [] = { "crash" };
char  text_021 [] = { "crash the system." };
char   syn_021 [] = { "crash " };

char topic_022 [] = { "halt" };
char  text_022 [] = { "halt the specified processor or device." };
char   syn_022 [] = { "halt [-drivers [<device_prefix>]] [<processor_num>] " };

char topic_023 [] = { "stop" };
char  text_023 [] = { "Stop the specified processor or device." };
char   syn_023 [] = { "stop [-drivers [<device_prefix>]] [<processor_num>] " };

char topic_024 [] = { "start" };
char  text_024 [] = { "Start a processor at the specified address or drivers." };
char   syn_024 [] = { "start [-drivers [<device_prefix>]] [<address>] " };

char topic_025 [] = { "continue" };
char  text_025 [] = { "Resume program execution on the specified processor." };
char   syn_025 [] = { "continue " };

char topic_026 [] = { "initialize" };
char  text_026 [] = { "Initialize the specified drivers or driver phase." };
char   syn_026 [] = { "init [-driver <device_or_phase>] " };

char topic_027 [] = { "tr" };
char  text_027 [] = { "Translate chararacters from stdin to stdout." };
char   syn_027 [] = { "tr [-{c,d,s}] [<string1>] [<string2>] " };

char topic_028 [] = { "uptime" };
char  text_028 [] = { "Display the elapsed time since system initialization." };
char   syn_028 [] = { "uptime() " };

char topic_029 [] = { "show power" };
char  text_029 [] = { "Display power supply and fan status." };
char   syn_029 [] = { "show power " };

char topic_030 [] = { "show cpu" };
char  text_030 [] = { "Display the status of each CPU." };
char   syn_030 [] = { "show cpu " };

char topic_031 [] = { "show memory" };
char  text_031 [] = { "Show memory configuration." };
char   syn_031 [] = { "show memory " };

char topic_032 [] = { "show config" };
char  text_032 [] = { "Display the system configuration." };
char   syn_032 [] = { "show config " };

char topic_033 [] = { "info" };
char  text_033 [] = { "Display info about various console state." };
char   syn_033 [] = { "info [n] " };

char topic_034 [] = { "dynamic" };
char  text_034 [] = { "Show the state of dynamic memory." };
char   syn_034 [] = { "dynamic [-h] [-c] [-r] [-p] \n\
\t               [-extend <byte_count>] [-z <heap_address>] " };

char topic_035 [] = { "alloc" };
char  text_035 [] = { "Allocate a block of memory from the heap." };
char   syn_035 [] = { "alloc <size> [<modulus>] [<remainder>] [-flood] [-z <heap_address>] " };

char topic_036 [] = { "free" };
char  text_036 [] = { "Return an allocated block of memory to the heap." };
char   syn_036 [] = { "free <address>... " };

char topic_037 [] = { "bpt" };
char  text_037 [] = { "Invoke the console XDELTA debugger." };
char   syn_037 [] = { "bpt \n\
\t               sub_commands: \n\
\t               \n\
\t               ;P ;C S O U V T \n\
\t               [<ga>];G \n\
\t               [<ba>][,<bn>][,<da>][,<ca>];B \n\
\t               [{B,W,L,Q,I,C,A} \n\
\t               [<sa>][,<ea>]/ or [<sa>][,<ea>]! or [<sa>][,<ea>]<double_quote> \n\
\t               CTRL/J, CTRL/[, CTRL/I, RETURN \n\
\t               <ca>;E <a>,<n>;X \n\
\t               pd_address,stradr;A \n\
\t               pd_address,p0,p1,...;Z \n\
\t               ;D ;L ;O ;S ;W ;Y \n\
\t               <expression>= " };

char topic_038 [] = { "cat" };
char  text_038 [] = { "Catenate files to standard output." };
char   syn_038 [] = { "cat [-length <num>] [-block <size>] [-start <offset>] \n\
\t               [-quiet] [<file>...] " };

char topic_039 [] = { "check" };
char  text_039 [] = { "Evaluate a string or the attributes of a inode." };
char   syn_039 [] = { "check [-{f,r,w,x,b}] [!] [<string>] " };

char topic_040 [] = { "chmod" };
char  text_040 [] = { "Change the mode of a file." };
char   syn_040 [] = { "chmod [{-|+|=}{r,w,x,b,z}] [<file>...] " };

char topic_041 [] = { "cmp" };
char  text_041 [] = { "Compare two files." };
char   syn_041 [] = { "cmp [-n <bytes>] [-b <size>] [-s] <file1> <file2> [<skip1>] [<skip2>] " };

char topic_042 [] = { "element" };
char  text_042 [] = { "Parse element from a delimited list." };
char   syn_042 [] = { "element ([-i <index>] [-d <delimiter>] [-l <line>] [-t <terminator>]) " };

char topic_043 [] = { "echo" };
char  text_043 [] = { "Echo the command line." };
char   syn_043 [] = { "echo [-n] <args>... " };

char topic_044 [] = { "edit" };
char  text_044 [] = { "Invoke the console BASIC-like line editor on a file." };
char   syn_044 [] = { "edit <file> \n\
\t               [Subcommands: HELP, LIST, RENUMBER, EXIT or CTL/Z, QUIT] \n\
\t               [<nn> : Delete line number <nn>.] \n\
\t               [<nn> <text> : Add or overwrite line <nn> with <text>.] " };

char topic_045 [] = { "eval" };
char  text_045 [] = { "Evaluate a postfix expression." };
char   syn_045 [] = { "eval [-{ib,io,id,ix}] [-{b|d|o|x}] <postfix_expression> " };

char topic_046 [] = { "exer" };
char  text_046 [] = { "Exercise one or more devices." };
char   syn_046 [] = { "exer [-sb <start_block>] [-eb <end_block>] [-p <pass_count>] \n\
\t               [-l <blocks>] [-bs <block_size>] [-bc <block_per_io>] \n\
\t               [-d1 <buf1_string>] [-d2 <buf2_string>] [-a <action_string>] \n\
\t               [-sec <seconds>] [-m] [-v] [-delay <milliseconds>] \n\
\t               <device_name>... " };

char topic_047 [] = { "find_field" };
char  text_047 [] = { "Extract a field from each input line and write it." };
char   syn_047 [] = { "find_field <field_number> [-d <delimiter>]) " };

char topic_048 [] = { "grep" };
char  text_048 [] = { "Globally search for regular expressions and print matches." };
char   syn_048 [] = { "grep [-{c|i|n|v}] [-f <file>] [<expression>] [<file>...] " };

char topic_049 [] = { "lpinit" };
char  text_049 [] = { "start up secondary partitions." };
char   syn_049 [] = { "lpinit " };

char topic_050 [] = { "galaxy" };
char  text_050 [] = { "alias for lpinit." };
char   syn_050 [] = { "galaxy " };

char topic_051 [] = { "migrate" };
char  text_051 [] = { "send all or one cpu to run in designated partition." };
char   syn_051 [] = { "migrate -{cpu <cpu_id>,all} -partition <partition_number> " };

char topic_052 [] = { "gct" };
char  text_052 [] = { "Build a Galaxy configuration tree." };
char   syn_052 [] = { "gct " };

char topic_053 [] = { "GCT_PLATFORM_TREE" };
char  text_053 [] = { "" };
char   syn_053 [] = { "" };

char topic_054 [] = { "gctverify" };
char  text_054 [] = { "Verify the integrity of the Config tree." };
char   syn_054 [] = { "gctverify " };

char topic_055 [] = { "hd" };
char  text_055 [] = { "Dump the contents of a file in hexadecimal and ASCII." };
char   syn_055 [] = { "hd [-{byte|word|long|quad}] [-{sb|eb} <n>] <file>[:<offset>]... " };

char topic_056 [] = { "man" };
char  text_056 [] = { "Display information about console commands." };
char   syn_056 [] = { "man [<command>...] \n\
\t               Command synopsis conventions: \n\
\t               item Implies a placeholder for user specified item. \n\
\t               item... Implies an item or list of items. \n\
\t               [] Implies optional keyword or item. \n\
\t               {a,b,c} Implies any one of a, b, c. \n\
\t               {a|b|c} Implies any combination of a, b, c. " };

char topic_057 [] = { "help" };
char  text_057 [] = { "Display information about console commands." };
char   syn_057 [] = { "help [<command>...] \n\
\t               Command synopsis conventions: \n\
\t               <item> Implies a placeholder for user specified item. \n\
\t               <item>... Implies an item or list of items. \n\
\t               [] Implies optional keyword or item. \n\
\t               {a,b,c} Implies any one of a, b, c. \n\
\t               {a|b|c} Implies any combination of a, b, c. " };

char topic_058 [] = { "line" };
char  text_058 [] = { "Read a line from standard input and write it to standard output." };
char   syn_058 [] = { "line " };

char topic_059 [] = { "login" };
char  text_059 [] = { "Login to a secure console." };
char   syn_059 [] = { "login \n\
\t               Please enter the password: <password> " };

char topic_060 [] = { "ls" };
char  text_060 [] = { "List files or inodes in file system." };
char   syn_060 [] = { "ls [-l] [<file>...] " };

char topic_061 [] = { "more" };
char  text_061 [] = { "Display output one screen at a time." };
char   syn_061 [] = { "more [-<pagesize> ] [<file>...] " };

char topic_062 [] = { "net" };
char  text_062 [] = { "Access a network port and execute MOP maintenance functions." };
char   syn_062 [] = { "net [-s] [-sa] [-rsa] [-env] [-ri] [-ic] [-id] [-l0] [-l1] [-rb] \n\
\t               [-csr] [-els] [-kls] [-start] [-stop] [-cm <mode_string>] \n\
\t               [-da <node_address>] [-l <file_name>] [-lw <wait_in_secs>] \n\
\t               [-sv <mop_version>] <port_name> " };

char topic_063 [] = { "prcache" };
char  text_063 [] = { "Manipulate an optional NVRAM disk cache." };
char   syn_063 [] = { "prcache [-z] [-f] [-u] [nvram_bus]) " };

char topic_064 [] = { "ps" };
char  text_064 [] = { "Print process status and statistics." };
char   syn_064 [] = { "ps " };

char topic_065 [] = { "sa" };
char  text_065 [] = { "Set process affinity." };
char   syn_065 [] = { "sa <process_id> <affinity_mask> " };

char topic_066 [] = { "sp" };
char  text_066 [] = { "Set process priority." };
char   syn_066 [] = { "sp <process_id> <new_priority> " };

char topic_067 [] = { "rm" };
char  text_067 [] = { "Remove files from the file system." };
char   syn_067 [] = { "rm <file>... " };

char topic_068 [] = { "set secure" };
char  text_068 [] = { "Place the console in secure mode." };
char   syn_068 [] = { "set secure " };

char topic_069 [] = { "set password" };
char  text_069 [] = { "Set the password for the secure function." };
char   syn_069 [] = { "set password \n\
\t               Please enter the password: <15-30 characters> \n\
\t               Please enter the password again: <verify password> " };

char topic_070 [] = { "clear password" };
char  text_070 [] = { "Clear the password for the secure function." };
char   syn_070 [] = { "clear password \n\
\t               Please enter the password: <password> " };

char topic_071 [] = { "semaphore" };
char  text_071 [] = { "Show system semaphores." };
char   syn_071 [] = { "semaphore " };

char topic_072 [] = { "sleep" };
char  text_072 [] = { "Suspend execution for a time." };
char   syn_072 [] = { "sleep [-v] <time_in_secs> " };

char topic_073 [] = { "sort" };
char  text_073 [] = { "Sort file(s) and write the sorted data into another file." };
char   syn_073 [] = { "sort <file...> " };

char topic_074 [] = { "sound" };
char  text_074 [] = { "cause the speaker to emit a sound (or a tune)." };
char   syn_074 [] = { "sound [-frequency <value>] [-duration <length>] " };

char topic_075 [] = { "true" };
char  text_075 [] = { "Return success status, always." };
char   syn_075 [] = { "true " };

char topic_076 [] = { "false" };
char  text_076 [] = { "Return failure status, always." };
char   syn_076 [] = { "false " };

char topic_077 [] = { "wc" };
char  text_077 [] = { "Count bytes, words and lines and report totals." };
char   syn_077 [] = { "wc [-{l|w|c}] [<file>...] " };

char topic_078 [] = { "memtest" };
char  text_078 [] = { "Exercise/test a section of memory." };
char   syn_078 [] = { "memtest [-sa <start_address>] [-ea <end_address>] [-l <length>] \n\
\t               [-bs <block_size>] [-i <address_inc>] [-p <pass_count>] \n\
\t               [-d <data_pattern>] [-rs <random_seed>] [-ba <block_address>] \n\
\t               [-t <test_mask>] [-se <soft_error_threshold>] \n\
\t               [-g <group_name>] [-rb] [-f] [-m] [-z] [-h] [-mb] " };

char topic_079 [] = { "nettest" };
char  text_079 [] = { "Test the network ports using MOP loopback." };
char   syn_079 [] = { "nettest [-f <file>] [-mode <port_mode>] [-p <pass_count>] \n\
\t               [-sv <mop_version>] [-to <loop_time>] [-w <wait_time>] \n\
\t               [<port>] " };

char topic_080 [] = { "lfu" };
char  text_080 [] = { "Update or Verify firmware." };
char   syn_080 [] = { "lfu [<cmd>] " };

char topic_081 [] = { "ewrk3_config" };
char  text_081 [] = { "Change the DE205 configuration, find addresses of modules." };
char   syn_081 [] = { "ewrk3_config ([-curaddr <addr#>][-ioaddr <hex value>][-irq <3,5,9,10,15>] \n\
\t               [-fbus <0,1>][-ena16 <0,1>][-memaddr <hex>] \n\
\t               [-bufsize <2,32,64>][-swflag <hexvalue>] \n\
\t               [-netmgt <hex value>][-zws <0,1>][-ra<0,1>][-wb<0,1>] \n\
\t               [-default]) " };

char topic_082 [] = { "wwidmgr" };
char  text_082 [] = { "Manage the wwid registration." };
char   syn_082 [] = { "wwidmgr [-quickset {-item <#>], -udid <#> } ] \n\
\t               [-set {wwid] | port} -item <#> [-unit <#>] [-col <#>] [-filter <'string>] ] \n\
\t               [-show {wwid] | port} [-full] [-filter <'string'>] ] \n\
\t               [-show {ev | reachability} ] \n\
\t               [-clear {all | wwid<#> | N<#>} ] \n\
\t               \n\
\t               Diagnostic mode command \n\
\t               " };

char topic_083 [] = { "mc_diag" };
char  text_083 [] = { "Memory Channel Read/Write & Loopback diagnostic." };
char   syn_083 [] = { "mc_diag [-d] " };

char topic_084 [] = { "mc_cable" };
char  text_084 [] = { "Memory Channel cable check diagnostic." };
char   syn_084 [] = { "mc_cable " };

char topic_085 [] = { "memexer" };
char  text_085 [] = { "Invoke Gray code memory exerciser." };
char   syn_085 [] = { "memexer [<number>] " };

char topic_086 [] = { "sys_exer" };
char  text_086 [] = { "exercise all the devices in the system concurrently." };
char   syn_086 [] = { "sys_exer [-lb] " };

char topic_087 [] = { "show_status" };
char  text_087 [] = { "Show the status of any currently executing diagnostics." };
char   syn_087 [] = { "show_status " };

char topic_088 [] = { "kill_diags" };
char  text_088 [] = { "Kill all currently executing diagnostic processes." };
char   syn_088 [] = { "kill_diags " };

char topic_089 [] = { "test" };
char  text_089 [] = { "Test the system." };
char   syn_089 [] = { "test [-lb] [-t <time>] \n\
\t               \n\
\t               COMMAND ARGUMENT(S): " };

char topic_090 [] = { "save_nvram" };
char  text_090 [] = { "Save NVRAM data in EEROM/TOY to a floppy file." };
char   syn_090 [] = { "save_nvram {all,arc,srm,toy} " };

char topic_091 [] = { "restore_nvram" };
char  text_091 [] = { "Restore NVRAM data to EEROM/TOY from a floppy file." };
char   syn_091 [] = { "restore_nvram {all,arc,srm,toy} " };


char topic_092 [] = { "" };
char  text_092 [] = { "" };
char   syn_092 [] = { "" };


struct {
        char *topic;
        char *text;
        char *syn;
} helpbuf [] = {
    { topic_053, text_053, syn_053 },
    { topic_035, text_035, syn_035 },
    { topic_005, text_005, syn_005 },
    { topic_037, text_037, syn_037 },
    { topic_004, text_004, syn_004 },
    { topic_006, text_006, syn_006 },
    { topic_038, text_038, syn_038 },
    { topic_039, text_039, syn_039 },
    { topic_040, text_040, syn_040 },
    { topic_018, text_018, syn_018 },
    { topic_070, text_070, syn_070 },
    { topic_008, text_008, syn_008 },
    { topic_041, text_041, syn_041 },
    { topic_025, text_025, syn_025 },
    { topic_021, text_021, syn_021 },
    { topic_009, text_009, syn_009 },
    { topic_034, text_034, syn_034 },
    { topic_043, text_043, syn_043 },
    { topic_044, text_044, syn_044 },
    { topic_042, text_042, syn_042 },
    { topic_045, text_045, syn_045 },
    { topic_081, text_081, syn_081 },
    { topic_010, text_010, syn_010 },
    { topic_046, text_046, syn_046 },
    { topic_003, text_003, syn_003 },
    { topic_076, text_076, syn_076 },
    { topic_047, text_047, syn_047 },
    { topic_036, text_036, syn_036 },
    { topic_050, text_050, syn_050 },
    { topic_052, text_052, syn_052 },
    { topic_054, text_054, syn_054 },
    { topic_048, text_048, syn_048 },
    { topic_022, text_022, syn_022 },
    { topic_055, text_055, syn_055 },
    { topic_057, text_057, syn_057 },
    { topic_033, text_033, syn_033 },
    { topic_026, text_026, syn_026 },
    { topic_012, text_012, syn_012 },
    { topic_013, text_013, syn_013 },
    { topic_002, text_002, syn_002 },
    { topic_088, text_088, syn_088 },
    { topic_080, text_080, syn_080 },
    { topic_058, text_058, syn_058 },
    { topic_059, text_059, syn_059 },
    { topic_049, text_049, syn_049 },
    { topic_060, text_060, syn_060 },
    { topic_056, text_056, syn_056 },
    { topic_084, text_084, syn_084 },
    { topic_083, text_083, syn_083 },
    { topic_085, text_085, syn_085 },
    { topic_078, text_078, syn_078 },
    { topic_051, text_051, syn_051 },
    { topic_061, text_061, syn_061 },
    { topic_062, text_062, syn_062 },
    { topic_079, text_079, syn_079 },
    { topic_063, text_063, syn_063 },
    { topic_064, text_064, syn_064 },
    { topic_091, text_091, syn_091 },
    { topic_067, text_067, syn_067 },
    { topic_065, text_065, syn_065 },
    { topic_090, text_090, syn_090 },
    { topic_071, text_071, syn_071 },
    { topic_016, text_016, syn_016 },
    { topic_019, text_019, syn_019 },
    { topic_069, text_069, syn_069 },
    { topic_068, text_068, syn_068 },
    { topic_001, text_001, syn_001 },
    { topic_017, text_017, syn_017 },
    { topic_020, text_020, syn_020 },
    { topic_032, text_032, syn_032 },
    { topic_030, text_030, syn_030 },
    { topic_007, text_007, syn_007 },
    { topic_014, text_014, syn_014 },
    { topic_011, text_011, syn_011 },
    { topic_015, text_015, syn_015 },
    { topic_031, text_031, syn_031 },
    { topic_029, text_029, syn_029 },
    { topic_087, text_087, syn_087 },
    { topic_072, text_072, syn_072 },
    { topic_073, text_073, syn_073 },
    { topic_074, text_074, syn_074 },
    { topic_066, text_066, syn_066 },
    { topic_024, text_024, syn_024 },
    { topic_023, text_023, syn_023 },
    { topic_086, text_086, syn_086 },
    { topic_089, text_089, syn_089 },
    { topic_027, text_027, syn_027 },
    { topic_075, text_075, syn_075 },
    { topic_028, text_028, syn_028 },
    { topic_077, text_077, syn_077 },
    { topic_082, text_082, syn_082 },
    { topic_092, text_092, syn_092 },
    { 0, 0, 0 }
};
