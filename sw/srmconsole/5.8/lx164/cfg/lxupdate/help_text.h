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

char topic_006 [] = { "deposit" };
char  text_006 [] = { "Write data to a specified address." };
char   syn_006 [] = { "deposit [-{b,w,l,q,o,h}] [-{physical,virtual,gpr,fpr,ipr}] \n\
\t               [-n <count>] [-s <step>] \n\
\t               [<device>:]<address> <data> " };

char topic_007 [] = { "examine" };
char  text_007 [] = { "Display data at a specified address." };
char   syn_007 [] = { "examine [-{b,w,l,q,o,h,d}] [-{physical,virtual,gpr,fpr,ipr}] \n\
\t               [-n <count>] [-s <step>] \n\
\t               [<device>:]<address> " };

char topic_008 [] = { "show iobq" };
char  text_008 [] = { "Show the I/O counter blocks." };
char   syn_008 [] = { "show_iobq " };

char topic_009 [] = { "isacfg" };
char  text_009 [] = { "sets or shows user inputted isa configuration data." };
char   syn_009 [] = { "isacfg ([-slot <slot#>] \n\
\t               [-dev <device#>] \n\
\t               [-all|-rm|-mk|-mod|-init] [-<field> <value>] " };

char topic_010 [] = { "show hwrpb" };
char  text_010 [] = { "Display the address of the HWRPB." };
char   syn_010 [] = { "show hwrpb () " };

char topic_011 [] = { "show map" };
char  text_011 [] = { "Display the system virtual memory map." };
char   syn_011 [] = { "show map () " };

char topic_012 [] = { "set" };
char  text_012 [] = { "Set or modify the value of an environment variable." };
char   syn_012 [] = { "set <envar> <value> \n\
\t               [-integer] [-string] \n\
\t               where \n\
\t               <envar>={auto_action,bootdef_dev,boot_file,boot_osflags,...} " };

char topic_013 [] = { "show" };
char  text_013 [] = { "Display an environment variable value or other information." };
char   syn_013 [] = { "show [{bios,config,device,hwrpb,memory,pal,version,...}] \n\
\t               [<envar>] \n\
\t               where: \n\
\t               <envar>={auto_action,bootdef_dev,boot_file,boot_osflags,...} " };

char topic_014 [] = { "clear" };
char  text_014 [] = { "Clear an option or an environment variable." };
char   syn_014 [] = { "clear [option] or envar " };

char topic_015 [] = { "set host" };
char  text_015 [] = { "Connect the console to another console or server." };
char   syn_015 [] = { "set host [-dup] [-task <task_name>] <target> " };

char topic_016 [] = { "show cluster" };
char  text_016 [] = { "Display open virtual circuits." };
char   syn_016 [] = { "show cluster " };

char topic_017 [] = { "crash" };
char  text_017 [] = { "crash the system." };
char   syn_017 [] = { "crash " };

char topic_018 [] = { "halt" };
char  text_018 [] = { "halt the specified processor or device." };
char   syn_018 [] = { "halt [-drivers [<device_prefix>]] [<processor_num>] " };

char topic_019 [] = { "stop" };
char  text_019 [] = { "Stop the specified processor or device." };
char   syn_019 [] = { "stop [-drivers [<device_prefix>]] [<processor_num>] " };

char topic_020 [] = { "start" };
char  text_020 [] = { "Start a processor at the specified address or drivers." };
char   syn_020 [] = { "start [-drivers [<device_prefix>]] [<address>] " };

char topic_021 [] = { "continue" };
char  text_021 [] = { "Resume program execution on the specified processor." };
char   syn_021 [] = { "continue " };

char topic_022 [] = { "initialize" };
char  text_022 [] = { "Initialize the specified drivers or driver phase." };
char   syn_022 [] = { "init [-driver <device_or_phase>] " };

char topic_023 [] = { "uptime" };
char  text_023 [] = { "Display the elapsed time since system initialization." };
char   syn_023 [] = { "uptime() " };

char topic_024 [] = { "xcmd" };
char  text_024 [] = { "Load a file via serial port 1." };
char   syn_024 [] = { "xcmd (load_address load_size) " };

char topic_025 [] = { "show memory" };
char  text_025 [] = { "Show memory configuration." };
char   syn_025 [] = { "show memory( " };

char topic_026 [] = { "show config" };
char  text_026 [] = { "" };
char   syn_026 [] = { "show config " };

char topic_027 [] = { "dynamic" };
char  text_027 [] = { "Show the state of dynamic memory." };
char   syn_027 [] = { "dynamic [-h] [-c] [-r] [-p] \n\
\t               [-extend <byte_count>] [-z <heap_address>] " };

char topic_028 [] = { "alloc" };
char  text_028 [] = { "Allocate a block of memory from the heap." };
char   syn_028 [] = { "alloc <size> [<modulus>] [<remainder>] [-flood] [-z <heap_address>] " };

char topic_029 [] = { "free" };
char  text_029 [] = { "Return an allocated block of memory to the heap." };
char   syn_029 [] = { "free <address>... " };

char topic_030 [] = { "bpt" };
char  text_030 [] = { "Invoke the console XDELTA debugger." };
char   syn_030 [] = { "bpt \n\
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

char topic_031 [] = { "cat" };
char  text_031 [] = { "Catenate files to standard output." };
char   syn_031 [] = { "cat [-length <num>] [-block <size>] [-start <offset>] \n\
\t               [-quiet] [<file>...] " };

char topic_032 [] = { "check" };
char  text_032 [] = { "Evaluate a string or the attributes of a inode." };
char   syn_032 [] = { "check [-{f,r,w,x,b}] [!] [<string>] " };

char topic_033 [] = { "chmod" };
char  text_033 [] = { "Change the mode of a file." };
char   syn_033 [] = { "chmod [{-|+|=}{r,w,x,b,z}] [<file>...] " };

char topic_034 [] = { "cmp" };
char  text_034 [] = { "Compare two files." };
char   syn_034 [] = { "cmp [-n <bytes>] [-b <size>] [-s] <file1> <file2> [<skip1>] [<skip2>] " };

char topic_035 [] = { "echo" };
char  text_035 [] = { "Echo the command line." };
char   syn_035 [] = { "echo [-n] <args>... " };

char topic_036 [] = { "edit" };
char  text_036 [] = { "Invoke the console BASIC-like line editor on a file." };
char   syn_036 [] = { "edit <file> \n\
\t               [Subcommands: HELP, LIST, RENUMBER, EXIT or CTL/Z, QUIT] \n\
\t               [<nn> : Delete line number <nn>.] \n\
\t               [<nn> <text> : Add or overwrite line <nn> with <text>.] " };

char topic_037 [] = { "eval" };
char  text_037 [] = { "Evaluate a postfix expression." };
char   syn_037 [] = { "eval [-{ib,io,id,ix}] [-{b|d|o|x}] <postfix_expression> " };

char topic_038 [] = { "find_field" };
char  text_038 [] = { "Extract a field from each input line and write it." };
char   syn_038 [] = { "find_field <field_number> [-d <delimiter>]) " };

char topic_039 [] = { "grep" };
char  text_039 [] = { "Globally search for regular expressions and print matches." };
char   syn_039 [] = { "grep [-{c|i|n|v}] [-f <file>] [<expression>] [<file>...] " };

char topic_040 [] = { "hd" };
char  text_040 [] = { "Dump the contents of a file in hexadecimal and ASCII." };
char   syn_040 [] = { "hd [-{byte|word|long|quad}] [-{sb|eb} <n>] <file>[:<offset>]... " };

char topic_041 [] = { "line" };
char  text_041 [] = { "Read a line from standard input and write it to standard output." };
char   syn_041 [] = { "line " };

char topic_042 [] = { "login" };
char  text_042 [] = { "Login to a secure console." };
char   syn_042 [] = { "login \n\
\t               Please enter the password: <password> " };

char topic_043 [] = { "ls" };
char  text_043 [] = { "List files or inodes in file system." };
char   syn_043 [] = { "ls [-l] [<file>...] " };

char topic_044 [] = { "more" };
char  text_044 [] = { "Display output one screen at a time." };
char   syn_044 [] = { "more [-<pagesize> ] [<file>...] " };

char topic_045 [] = { "ps" };
char  text_045 [] = { "Print process status and statistics." };
char   syn_045 [] = { "ps " };

char topic_046 [] = { "sa" };
char  text_046 [] = { "Set process affinity." };
char   syn_046 [] = { "sa <process_id> <affinity_mask> " };

char topic_047 [] = { "sp" };
char  text_047 [] = { "Set process priority." };
char   syn_047 [] = { "sp <process_id> <new_priority> " };

char topic_048 [] = { "rm" };
char  text_048 [] = { "Remove files from the file system." };
char   syn_048 [] = { "rm <file>... " };

char topic_049 [] = { "set secure" };
char  text_049 [] = { "Place the console in secure mode." };
char   syn_049 [] = { "set secure " };

char topic_050 [] = { "set password" };
char  text_050 [] = { "Set the password for the secure function." };
char   syn_050 [] = { "set password \n\
\t               Please enter the password: <15-30 characters> \n\
\t               Please enter the password again: <verify password> " };

char topic_051 [] = { "clear password" };
char  text_051 [] = { "Clear the password for the secure function." };
char   syn_051 [] = { "clear password \n\
\t               Please enter the password: <password> " };

char topic_052 [] = { "semaphore" };
char  text_052 [] = { "Show system semaphores." };
char   syn_052 [] = { "semaphore " };

char topic_053 [] = { "sleep" };
char  text_053 [] = { "Suspend execution for a time." };
char   syn_053 [] = { "sleep [-v] <time_in_secs> " };

char topic_054 [] = { "sort" };
char  text_054 [] = { "Sort file(s) and write the sorted data into another file." };
char   syn_054 [] = { "sort <file...> " };

char topic_055 [] = { "sound" };
char  text_055 [] = { "cause the speaker to emit a sound (or a tune)." };
char   syn_055 [] = { "sound [-frequency <value>] [-duration <length>] " };

char topic_056 [] = { "tr" };
char  text_056 [] = { "Translate chararacters from stdin to stdout." };
char   syn_056 [] = { "tr [-{c,d,s}] [<string1>] [<string2>] " };

char topic_057 [] = { "true" };
char  text_057 [] = { "Return success status, always." };
char   syn_057 [] = { "true " };

char topic_058 [] = { "false" };
char  text_058 [] = { "Return failure status, always." };
char   syn_058 [] = { "false " };

char topic_059 [] = { "wc" };
char  text_059 [] = { "Count bytes, words and lines and report totals." };
char   syn_059 [] = { "wc [-{l|w|c}] [<file>...] " };

char topic_060 [] = { "ewrk3_config" };
char  text_060 [] = { "Change the DE205 configuration, find addresses of modules." };
char   syn_060 [] = { "ewrk3_config ([-curaddr <addr#>][-ioaddr <hex value>][-irq <3,5,9,10,15>] \n\
\t               [-fbus <0,1>][-ena16 <0,1>][-memaddr <hex>] \n\
\t               [-bufsize <2,32,64>][-swflag <hexvalue>] \n\
\t               [-netmgt <hex value>][-zws <0,1>][-ra<0,1>][-wb<0,1>] \n\
\t               [-default]) " };

char topic_061 [] = { "lfu" };
char  text_061 [] = { "Update or Verify firmware." };
char   syn_061 [] = { "lfu [<cmd>] " };

char topic_062 [] = { "test" };
char  text_062 [] = { "Test the system." };
char   syn_062 [] = { "test [-lb] " };

char topic_063 [] = { "memexer" };
char  text_063 [] = { "Invoke Gray code memory exerciser." };
char   syn_063 [] = { "memexer [<number>] " };

char topic_064 [] = { "sys_exer" };
char  text_064 [] = { "exercise all the devices in the system concurrently." };
char   syn_064 [] = { "sys_exer [-lb] " };

char topic_065 [] = { "exer_read" };
char  text_065 [] = { "Randomly read blocks of data from a device." };
char   syn_065 [] = { "exer_read [<device>...] " };

char topic_066 [] = { "exer_write" };
char  text_066 [] = { "Randomly do non-destructive writes to a device." };
char   syn_066 [] = { "exer_write [<device>...] " };

char topic_067 [] = { "show_status" };
char  text_067 [] = { "Show the status of any currently executing diagnostics." };
char   syn_067 [] = { "show_status " };

char topic_068 [] = { "kill_diags" };
char  text_068 [] = { "Kill all currently executing diagnostic processes." };
char   syn_068 [] = { "kill_diags " };

char topic_069 [] = { "save_nvram" };
char  text_069 [] = { "Save NVRAM data in EEROM/TOY to a floppy file." };
char   syn_069 [] = { "save_nvram {all,arc,srm,toy} " };

char topic_070 [] = { "restore_nvram" };
char  text_070 [] = { "Restore NVRAM data to EEROM/TOY from a floppy file." };
char   syn_070 [] = { "restore_nvram {all,arc,srm,toy} " };


char topic_071 [] = { "" };
char  text_071 [] = { "" };
char   syn_071 [] = { "" };


struct {
        char *topic;
        char *text;
        char *syn;
} helpbuf [] = {
    { topic_028, text_028, syn_028 },
    { topic_005, text_005, syn_005 },
    { topic_030, text_030, syn_030 },
    { topic_004, text_004, syn_004 },
    { topic_031, text_031, syn_031 },
    { topic_032, text_032, syn_032 },
    { topic_033, text_033, syn_033 },
    { topic_014, text_014, syn_014 },
    { topic_051, text_051, syn_051 },
    { topic_034, text_034, syn_034 },
    { topic_021, text_021, syn_021 },
    { topic_017, text_017, syn_017 },
    { topic_006, text_006, syn_006 },
    { topic_027, text_027, syn_027 },
    { topic_035, text_035, syn_035 },
    { topic_036, text_036, syn_036 },
    { topic_037, text_037, syn_037 },
    { topic_060, text_060, syn_060 },
    { topic_007, text_007, syn_007 },
    { topic_065, text_065, syn_065 },
    { topic_066, text_066, syn_066 },
    { topic_003, text_003, syn_003 },
    { topic_058, text_058, syn_058 },
    { topic_038, text_038, syn_038 },
    { topic_029, text_029, syn_029 },
    { topic_039, text_039, syn_039 },
    { topic_018, text_018, syn_018 },
    { topic_040, text_040, syn_040 },
    { topic_022, text_022, syn_022 },
    { topic_009, text_009, syn_009 },
    { topic_002, text_002, syn_002 },
    { topic_068, text_068, syn_068 },
    { topic_061, text_061, syn_061 },
    { topic_041, text_041, syn_041 },
    { topic_042, text_042, syn_042 },
    { topic_043, text_043, syn_043 },
    { topic_063, text_063, syn_063 },
    { topic_044, text_044, syn_044 },
    { topic_045, text_045, syn_045 },
    { topic_070, text_070, syn_070 },
    { topic_048, text_048, syn_048 },
    { topic_046, text_046, syn_046 },
    { topic_069, text_069, syn_069 },
    { topic_052, text_052, syn_052 },
    { topic_012, text_012, syn_012 },
    { topic_015, text_015, syn_015 },
    { topic_050, text_050, syn_050 },
    { topic_049, text_049, syn_049 },
    { topic_001, text_001, syn_001 },
    { topic_013, text_013, syn_013 },
    { topic_016, text_016, syn_016 },
    { topic_026, text_026, syn_026 },
    { topic_010, text_010, syn_010 },
    { topic_008, text_008, syn_008 },
    { topic_011, text_011, syn_011 },
    { topic_025, text_025, syn_025 },
    { topic_067, text_067, syn_067 },
    { topic_053, text_053, syn_053 },
    { topic_054, text_054, syn_054 },
    { topic_055, text_055, syn_055 },
    { topic_047, text_047, syn_047 },
    { topic_020, text_020, syn_020 },
    { topic_019, text_019, syn_019 },
    { topic_064, text_064, syn_064 },
    { topic_062, text_062, syn_062 },
    { topic_056, text_056, syn_056 },
    { topic_057, text_057, syn_057 },
    { topic_023, text_023, syn_023 },
    { topic_059, text_059, syn_059 },
    { topic_024, text_024, syn_024 },
    { topic_071, text_071, syn_071 },
    { 0, 0, 0 }
};
