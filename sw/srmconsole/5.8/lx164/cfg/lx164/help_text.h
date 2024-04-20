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
\t            [-protocols <enet_protocol>] [-halt] \n\
\t            [<boot_device>][,<boot_device>] " };

char topic_006 [] = { "deposit" };
char  text_006 [] = { "Write data to a specified address." };
char   syn_006 [] = { "deposit [-{b,w,l,q,o,h}] [-{physical,virtual,gpr,fpr,ipr}] \n\
\t            [-n <count>] [-s <step>] \n\
\t            [<device>:]<address> <data> " };

char topic_007 [] = { "examine" };
char  text_007 [] = { "Display data at a specified address." };
char   syn_007 [] = { "examine [-{b,w,l,q,o,h,d}] [-{physical,virtual,gpr,fpr,ipr}] \n\
\t            [-n <count>] [-s <step>] \n\
\t            [<device>:]<address> " };

char topic_008 [] = { "show iobq" };
char  text_008 [] = { "Show the I/O counter blocks." };
char   syn_008 [] = { "show_iobq " };

char topic_009 [] = { "isacfg" };
char  text_009 [] = { "sets or shows user inputted isa configuration data." };
char   syn_009 [] = { "isacfg ([-slot <slot#>] \n\
\t            [-dev <device#>] \n\
\t            [-all|-rm|-mk|-mod|-init] [-<field> <value>] " };

char topic_010 [] = { "isp1020_edit" };
char  text_010 [] = { "Edit ISP1020 NVRAM parameters." };
char   syn_010 [] = { "isp1020_edit [-sd] [-offset] [-id] [-allids] [-value] \n\
\t            [-bit] [-set] -[clear] [<isp1020_controller_name>] " };

char topic_011 [] = { "show hwrpb" };
char  text_011 [] = { "Display the address of the HWRPB." };
char   syn_011 [] = { "show hwrpb () " };

char topic_012 [] = { "show map" };
char  text_012 [] = { "Display the system virtual memory map." };
char   syn_012 [] = { "show map () " };

char topic_013 [] = { "set" };
char  text_013 [] = { "Set or modify the value of an environment variable." };
char   syn_013 [] = { "set <envar> <value> \n\
\t            [-integer] [-string] \n\
\t            where \n\
\t            <envar>={auto_action,bootdef_dev,boot_file,boot_osflags,...} " };

char topic_014 [] = { "show" };
char  text_014 [] = { "Display an environment variable value or other information." };
char   syn_014 [] = { "show [{bios,config,device,hwrpb,memory,pal,version,...}] \n\
\t            [<envar>] \n\
\t            where: \n\
\t            <envar>={auto_action,bootdef_dev,boot_file,boot_osflags,...} " };

char topic_015 [] = { "clear" };
char  text_015 [] = { "Clear an option or an environment variable." };
char   syn_015 [] = { "clear [option] or envar " };

char topic_016 [] = { "set host" };
char  text_016 [] = { "Connect the console to another console or server." };
char   syn_016 [] = { "set host [-dup] [-task <task_name>] <target> " };

char topic_017 [] = { "show cluster" };
char  text_017 [] = { "Display open virtual circuits." };
char   syn_017 [] = { "show cluster " };

char topic_018 [] = { "crash" };
char  text_018 [] = { "crash the system." };
char   syn_018 [] = { "crash " };

char topic_019 [] = { "halt" };
char  text_019 [] = { "halt the specified processor or device." };
char   syn_019 [] = { "halt [-drivers [<device_prefix>]] [<processor_num>] " };

char topic_020 [] = { "stop" };
char  text_020 [] = { "Stop the specified processor or device." };
char   syn_020 [] = { "stop [-drivers [<device_prefix>]] [<processor_num>] " };

char topic_021 [] = { "start" };
char  text_021 [] = { "Start a processor at the specified address or drivers." };
char   syn_021 [] = { "start [-drivers [<device_prefix>]] [<address>] " };

char topic_022 [] = { "continue" };
char  text_022 [] = { "Resume program execution on the specified processor." };
char   syn_022 [] = { "continue " };

char topic_023 [] = { "initialize" };
char  text_023 [] = { "Initialize the specified drivers or driver phase." };
char   syn_023 [] = { "init [-driver <device_or_phase>] " };

char topic_024 [] = { "uptime" };
char  text_024 [] = { "Display the elapsed time since system initialization." };
char   syn_024 [] = { "uptime() " };

char topic_025 [] = { "xcmd" };
char  text_025 [] = { "Load a file via serial port 1." };
char   syn_025 [] = { "xcmd (load_address load_size) " };

char topic_026 [] = { "show memory" };
char  text_026 [] = { "Show memory configuration." };
char   syn_026 [] = { "show memory( " };

char topic_027 [] = { "show config" };
char  text_027 [] = { "" };
char   syn_027 [] = { "show config " };

char topic_028 [] = { "dynamic" };
char  text_028 [] = { "Show the state of dynamic memory." };
char   syn_028 [] = { "dynamic [-h] [-c] [-r] [-p] \n\
\t            [-extend <byte_count>] [-z <heap_address>] " };

char topic_029 [] = { "alloc" };
char  text_029 [] = { "Allocate a block of memory from the heap." };
char   syn_029 [] = { "alloc <size> [<modulus>] [<remainder>] [-flood] [-z <heap_address>] " };

char topic_030 [] = { "free" };
char  text_030 [] = { "Return an allocated block of memory to the heap." };
char   syn_030 [] = { "free <address>... " };

char topic_031 [] = { "bpt" };
char  text_031 [] = { "Invoke the console XDELTA debugger." };
char   syn_031 [] = { "bpt \n\
\t            sub_commands: \n\
\t            \n\
\t            ;P ;C S O U V T \n\
\t            [<ga>];G \n\
\t            [<ba>][,<bn>][,<da>][,<ca>];B \n\
\t            [{B,W,L,Q,I,C,A} \n\
\t            [<sa>][,<ea>]/ or [<sa>][,<ea>]! or [<sa>][,<ea>]<double_quote> \n\
\t            CTRL/J, CTRL/[, CTRL/I, RETURN \n\
\t            <ca>;E <a>,<n>;X \n\
\t            pd_address,stradr;A \n\
\t            pd_address,p0,p1,...;Z \n\
\t            ;D ;L ;O ;S ;W ;Y \n\
\t            <expression>= " };

char topic_032 [] = { "cat" };
char  text_032 [] = { "Catenate files to standard output." };
char   syn_032 [] = { "cat [-length <num>] [-block <size>] [-start <offset>] \n\
\t            [-quiet] [<file>...] " };

char topic_033 [] = { "check" };
char  text_033 [] = { "Evaluate a string or the attributes of a inode." };
char   syn_033 [] = { "check [-{f,r,w,x,b}] [!] [<string>] " };

char topic_034 [] = { "chmod" };
char  text_034 [] = { "Change the mode of a file." };
char   syn_034 [] = { "chmod [{-|+|=}{r,w,x,b,z}] [<file>...] " };

char topic_035 [] = { "cmp" };
char  text_035 [] = { "Compare two files." };
char   syn_035 [] = { "cmp [-n <bytes>] [-b <size>] [-s] <file1> <file2> [<skip1>] [<skip2>] " };

char topic_036 [] = { "echo" };
char  text_036 [] = { "Echo the command line." };
char   syn_036 [] = { "echo [-n] <args>... " };

char topic_037 [] = { "edit" };
char  text_037 [] = { "Invoke the console BASIC-like line editor on a file." };
char   syn_037 [] = { "edit <file> \n\
\t            [Subcommands: HELP, LIST, RENUMBER, EXIT or CTL/Z, QUIT] \n\
\t            [<nn> : Delete line number <nn>.] \n\
\t            [<nn> <text> : Add or overwrite line <nn> with <text>.] " };

char topic_038 [] = { "eval" };
char  text_038 [] = { "Evaluate a postfix expression." };
char   syn_038 [] = { "eval [-{ib,io,id,ix}] [-{b|d|o|x}] <postfix_expression> " };

char topic_039 [] = { "exer" };
char  text_039 [] = { "Exercise one or more devices." };
char   syn_039 [] = { "exer [-sb <start_block>] [-eb <end_block>] [-p <pass_count>] \n\
\t            [-l <blocks>] [-bs <block_size>] [-bc <block_per_io>] \n\
\t            [-d1 <buf1_string>] [-d2 <buf2_string>] [-a <action_string>] \n\
\t            [-sec <seconds>] [-m] [-v] [-delay <milliseconds>] \n\
\t            <device_name>... " };

char topic_040 [] = { "find_field" };
char  text_040 [] = { "Extract a field from each input line and write it." };
char   syn_040 [] = { "find_field <field_number> [-d <delimiter>]) " };

char topic_041 [] = { "grep" };
char  text_041 [] = { "Globally search for regular expressions and print matches." };
char   syn_041 [] = { "grep [-{c|i|n|v}] [-f <file>] [<expression>] [<file>...] " };

char topic_042 [] = { "hd" };
char  text_042 [] = { "Dump the contents of a file in hexadecimal and ASCII." };
char   syn_042 [] = { "hd [-{byte|word|long|quad}] [-{sb|eb} <n>] <file>[:<offset>]... " };

char topic_043 [] = { "man" };
char  text_043 [] = { "Display information about console commands." };
char   syn_043 [] = { "man [<command>...] \n\
\t            Command synopsis conventions: \n\
\t            item Implies a placeholder for user specified item. \n\
\t            item... Implies an item or list of items. \n\
\t            [] Implies optional keyword or item. \n\
\t            {a,b,c} Implies any one of a, b, c. \n\
\t            {a|b|c} Implies any combination of a, b, c. " };

char topic_044 [] = { "help" };
char  text_044 [] = { "Display information about console commands." };
char   syn_044 [] = { "help [<command>...] \n\
\t            Command synopsis conventions: \n\
\t            <item> Implies a placeholder for user specified item. \n\
\t            <item>... Implies an item or list of items. \n\
\t            [] Implies optional keyword or item. \n\
\t            {a,b,c} Implies any one of a, b, c. \n\
\t            {a|b|c} Implies any combination of a, b, c. " };

char topic_045 [] = { "line" };
char  text_045 [] = { "Read a line from standard input and write it to standard output." };
char   syn_045 [] = { "line " };

char topic_046 [] = { "login" };
char  text_046 [] = { "Login to a secure console." };
char   syn_046 [] = { "login \n\
\t            Please enter the password: <password> " };

char topic_047 [] = { "ls" };
char  text_047 [] = { "List files or inodes in file system." };
char   syn_047 [] = { "ls [-l] [<file>...] " };

char topic_048 [] = { "more" };
char  text_048 [] = { "Display output one screen at a time." };
char   syn_048 [] = { "more [-<pagesize> ] [<file>...] " };

char topic_049 [] = { "net" };
char  text_049 [] = { "Access a network port and execute MOP maintenance functions." };
char   syn_049 [] = { "net [-s] [-sa] [-rsa] [-env] [-ri] [-ic] [-id] [-l0] [-l1] [-rb] \n\
\t            [-csr] [-els] [-kls] [-start] [-stop] [-cm <mode_string>] \n\
\t            [-da <node_address>] [-l <file_name>] [-lw <wait_in_secs>] \n\
\t            [-sv <mop_version>] <port_name> " };

char topic_050 [] = { "ps" };
char  text_050 [] = { "Print process status and statistics." };
char   syn_050 [] = { "ps " };

char topic_051 [] = { "sa" };
char  text_051 [] = { "Set process affinity." };
char   syn_051 [] = { "sa <process_id> <affinity_mask> " };

char topic_052 [] = { "sp" };
char  text_052 [] = { "Set process priority." };
char   syn_052 [] = { "sp <process_id> <new_priority> " };

char topic_053 [] = { "rm" };
char  text_053 [] = { "Remove files from the file system." };
char   syn_053 [] = { "rm <file>... " };

char topic_054 [] = { "set secure" };
char  text_054 [] = { "Place the console in secure mode." };
char   syn_054 [] = { "set secure " };

char topic_055 [] = { "set password" };
char  text_055 [] = { "Set the password for the secure function." };
char   syn_055 [] = { "set password \n\
\t            Please enter the password: <15-30 characters> \n\
\t            Please enter the password again: <verify password> " };

char topic_056 [] = { "clear password" };
char  text_056 [] = { "Clear the password for the secure function." };
char   syn_056 [] = { "clear password \n\
\t            Please enter the password: <password> " };

char topic_057 [] = { "semaphore" };
char  text_057 [] = { "Show system semaphores." };
char   syn_057 [] = { "semaphore " };

char topic_058 [] = { "sleep" };
char  text_058 [] = { "Suspend execution for a time." };
char   syn_058 [] = { "sleep [-v] <time_in_secs> " };

char topic_059 [] = { "sort" };
char  text_059 [] = { "Sort file(s) and write the sorted data into another file." };
char   syn_059 [] = { "sort <file...> " };

char topic_060 [] = { "sound" };
char  text_060 [] = { "cause the speaker to emit a sound (or a tune)." };
char   syn_060 [] = { "sound [-frequency <value>] [-duration <length>] " };

char topic_061 [] = { "tr" };
char  text_061 [] = { "Translate chararacters from stdin to stdout." };
char   syn_061 [] = { "tr [-{c,d,s}] [<string1>] [<string2>] " };

char topic_062 [] = { "true" };
char  text_062 [] = { "Return success status, always." };
char   syn_062 [] = { "true " };

char topic_063 [] = { "false" };
char  text_063 [] = { "Return failure status, always." };
char   syn_063 [] = { "false " };

char topic_064 [] = { "wc" };
char  text_064 [] = { "Count bytes, words and lines and report totals." };
char   syn_064 [] = { "wc [-{l|w|c}] [<file>...] " };

char topic_065 [] = { "memtest" };
char  text_065 [] = { "Exercise/test a section of memory." };
char   syn_065 [] = { "memtest [-sa <start_address>] [-ea <end_address>] [-l <length>] \n\
\t            [-bs <block_size>] [-i <address_inc>] [-p <pass_count>] \n\
\t            [-d <data_pattern>] [-rs <random_seed>] [-ba <block_address>] \n\
\t            [-t <test_mask>] [-se <soft_error_threshold>] \n\
\t            [-g <group_name>] [-rb] [-f] [-m] [-z] [-h] [-mb] " };

char topic_066 [] = { "nettest" };
char  text_066 [] = { "Test the network ports using MOP loopback." };
char   syn_066 [] = { "nettest [-f <file>] [-mode <port_mode>] [-p <pass_count>] \n\
\t            [-sv <mop_version>] [-to <loop_time>] [-w <wait_time>] \n\
\t            [<port>] " };

char topic_067 [] = { "ewrk3_config" };
char  text_067 [] = { "Change the DE205 configuration, find addresses of modules." };
char   syn_067 [] = { "ewrk3_config ([-curaddr <addr#>][-ioaddr <hex value>][-irq <3,5,9,10,15>] \n\
\t            [-fbus <0,1>][-ena16 <0,1>][-memaddr <hex>] \n\
\t            [-bufsize <2,32,64>][-swflag <hexvalue>] \n\
\t            [-netmgt <hex value>][-zws <0,1>][-ra<0,1>][-wb<0,1>] \n\
\t            [-default]) " };

char topic_068 [] = { "test" };
char  text_068 [] = { "Test the system." };
char   syn_068 [] = { "test [-lb] " };

char topic_069 [] = { "memexer" };
char  text_069 [] = { "Invoke Gray code memory exerciser." };
char   syn_069 [] = { "memexer [<number>] " };

char topic_070 [] = { "sys_exer" };
char  text_070 [] = { "exercise all the devices in the system concurrently." };
char   syn_070 [] = { "sys_exer [-lb] " };

char topic_071 [] = { "exer_read" };
char  text_071 [] = { "Randomly read blocks of data from a device." };
char   syn_071 [] = { "exer_read [<device>...] " };

char topic_072 [] = { "exer_write" };
char  text_072 [] = { "Randomly do non-destructive writes to a device." };
char   syn_072 [] = { "exer_write [<device>...] " };

char topic_073 [] = { "show_status" };
char  text_073 [] = { "Show the status of any currently executing diagnostics." };
char   syn_073 [] = { "show_status " };

char topic_074 [] = { "kill_diags" };
char  text_074 [] = { "Kill all currently executing diagnostic processes." };
char   syn_074 [] = { "kill_diags " };


char topic_075 [] = { "" };
char  text_075 [] = { "" };
char   syn_075 [] = { "" };


struct {
        char *topic;
        char *text;
        char *syn;
} helpbuf [] = {
    { topic_029, text_029, syn_029 },
    { topic_005, text_005, syn_005 },
    { topic_031, text_031, syn_031 },
    { topic_004, text_004, syn_004 },
    { topic_032, text_032, syn_032 },
    { topic_033, text_033, syn_033 },
    { topic_034, text_034, syn_034 },
    { topic_015, text_015, syn_015 },
    { topic_056, text_056, syn_056 },
    { topic_035, text_035, syn_035 },
    { topic_022, text_022, syn_022 },
    { topic_018, text_018, syn_018 },
    { topic_006, text_006, syn_006 },
    { topic_028, text_028, syn_028 },
    { topic_036, text_036, syn_036 },
    { topic_037, text_037, syn_037 },
    { topic_038, text_038, syn_038 },
    { topic_067, text_067, syn_067 },
    { topic_007, text_007, syn_007 },
    { topic_039, text_039, syn_039 },
    { topic_071, text_071, syn_071 },
    { topic_072, text_072, syn_072 },
    { topic_003, text_003, syn_003 },
    { topic_063, text_063, syn_063 },
    { topic_040, text_040, syn_040 },
    { topic_030, text_030, syn_030 },
    { topic_041, text_041, syn_041 },
    { topic_019, text_019, syn_019 },
    { topic_042, text_042, syn_042 },
    { topic_044, text_044, syn_044 },
    { topic_023, text_023, syn_023 },
    { topic_009, text_009, syn_009 },
    { topic_010, text_010, syn_010 },
    { topic_002, text_002, syn_002 },
    { topic_074, text_074, syn_074 },
    { topic_045, text_045, syn_045 },
    { topic_046, text_046, syn_046 },
    { topic_047, text_047, syn_047 },
    { topic_043, text_043, syn_043 },
    { topic_069, text_069, syn_069 },
    { topic_065, text_065, syn_065 },
    { topic_048, text_048, syn_048 },
    { topic_049, text_049, syn_049 },
    { topic_066, text_066, syn_066 },
    { topic_050, text_050, syn_050 },
    { topic_053, text_053, syn_053 },
    { topic_051, text_051, syn_051 },
    { topic_057, text_057, syn_057 },
    { topic_013, text_013, syn_013 },
    { topic_016, text_016, syn_016 },
    { topic_055, text_055, syn_055 },
    { topic_054, text_054, syn_054 },
    { topic_001, text_001, syn_001 },
    { topic_014, text_014, syn_014 },
    { topic_017, text_017, syn_017 },
    { topic_027, text_027, syn_027 },
    { topic_011, text_011, syn_011 },
    { topic_008, text_008, syn_008 },
    { topic_012, text_012, syn_012 },
    { topic_026, text_026, syn_026 },
    { topic_073, text_073, syn_073 },
    { topic_058, text_058, syn_058 },
    { topic_059, text_059, syn_059 },
    { topic_060, text_060, syn_060 },
    { topic_052, text_052, syn_052 },
    { topic_021, text_021, syn_021 },
    { topic_020, text_020, syn_020 },
    { topic_070, text_070, syn_070 },
    { topic_068, text_068, syn_068 },
    { topic_061, text_061, syn_061 },
    { topic_062, text_062, syn_062 },
    { topic_024, text_024, syn_024 },
    { topic_064, text_064, syn_064 },
    { topic_025, text_025, syn_025 },
    { topic_075, text_075, syn_075 },
    { 0, 0, 0 }
};
