#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include "targets.h"

#define YES 1
#define NO 0

char * get_param(char * prompt, char * defval)
{
    char * result;
    static char answer[512];
	int l;

    printf("%s [%s]: ", prompt, defval);
    fgets(answer, 512, stdin);

	if (answer[0] == '\0' || answer[0] == '\r' || answer[0] == '\n') {
	    strcpy(answer, defval);
    }

	l = strlen(answer);
	if(answer[l-1] == '\n') {
		answer[l-1] = '\0';
	};
    result = malloc(l+1);
    strcpy(result, answer);
    return(result);
}

int get_yesno(char * prompt, int defval)
{
    static char answer[512];

    for(;;) {
	printf("%s <Y/N> [%c]: ", prompt, defval ? 'Y' : 'N');
	fgets(answer, 512, stdin);
	if(answer[0] == 'Y' || answer [0] == 'y') {
		return YES;
	}
	else if (answer[0] == 'N' || answer[0] == 'n') {
		return NO;
	}
	else if (answer[0] == '\0' || answer[0] == '\r' || answer[0] == '\n') {
		return defval;
	}
	else {
	 printf("Must answer 'Y' or 'N'\n");
	}
    }
}

char * make_forward_slash(char * s)
{
    int         i;

    for(i = 0; s[i]; i++) {
	if(s[i] == '\\') {
	    s[i] = '/';
	}
    }
    return(s);
}

char * make_backslash(char *s)
{
    int         i;

    for(i = 0; s[i]; i++) {
	if(s[i] == '/') {
	    s[i] = '\\';
	}
    }
    return(s);
}




main()
{
    FILE * conf_file;
    char cfdir[_MAX_PATH];
    char * eb_base;
    char * eb_toolbox;
    char * eb_bootdir;
    char eb_targets[512];
    char tmp[512];
    int i;


    printf("======== EBSDK configuration ========\n");
    /* Get location of build directory */

    if(_getcwd(cfdir, _MAX_PATH) == NULL) {
	perror("_getcwd");
	exit(-1);
    }
    
    eb_base = getenv("EB_BASE");
    if(eb_base == NULL) {
	eb_base = malloc(strlen(cfdir)+16);
	strcpy(eb_base, cfdir);
	strcat(eb_base, "/ebfw");
    }

printf("cfdir = %s\n", cfdir);

    /* Calculate default location of tools directory */
    eb_toolbox = getenv("EB_TOOLBOX");
    if(eb_toolbox == NULL) {
	eb_toolbox = malloc(strlen(cfdir)+32);
	strcpy(eb_toolbox, cfdir);
#ifdef _WIN32
	strcat(eb_toolbox, "/ebtools/ntalpha");
#else
	strcat(eb_toolbox, "/ebtools/unix");
#endif
    }

    eb_bootdir = getenv("EB_BOOTDIR");
    if(eb_bootdir == NULL) {
	eb_bootdir = malloc(strlen(cfdir)+16);
	strcpy(eb_bootdir, cfdir);
	strcat(eb_bootdir, "/ebboot");
    }

    eb_base = get_param("Location of build directory", eb_base);
    eb_toolbox = get_param("Location of build tools directory", eb_toolbox);
    eb_bootdir = get_param("Location of bootable image directory", eb_bootdir);

    if(_access(eb_bootdir, 0) == -1) {
	printf("%s does not exist\n", eb_bootdir);
	if(get_yesno("Create it", YES)) {
	    _mkdir(eb_bootdir);
	}
    }

    strcpy(eb_targets, "");
    printf("Select which targets you would like to build\n");
    for(i = 0; i < NUM_TARGETS; i++) {
	if(get_yesno(targets[i], YES)) {
	    if(eb_targets[0]) strcat(eb_targets, " ");
	    strcat(eb_targets, targets[i]);
	}
    }

    /* Print out the "make" configuration file, followed by the
     * shell configuration file
     */
    strcpy(tmp, cfdir);
    strcat(tmp, "/sdkmake.conf");
    conf_file = fopen(tmp, "w");
    fprintf(conf_file, "EB_BASE=%s\n", make_forward_slash(eb_base));
    fprintf(conf_file, "EB_TOOLBOX=%s\n", make_forward_slash(eb_toolbox));
    fprintf(conf_file, "EB_BOOTDIR=%s\n", make_forward_slash(eb_bootdir));
    fprintf(conf_file, "EB_TARGETS=%s\n", make_forward_slash(eb_targets));
    strcpy(tmp, eb_toolbox);
    strcat(tmp, "/gnumake");
    fprintf(conf_file, "EB_MAKE=%s\n", make_forward_slash(tmp));
    fclose(conf_file);

    /* Now write out the "shell" configuration file */
#ifdef _WIN32
    strcpy(tmp, cfdir);
    strcat(tmp, "\\sdkconf.bat");
    conf_file = fopen(tmp, "w");
    fprintf(conf_file, "SET EB_BASE=%s\n", make_backslash(eb_base));
    fprintf(conf_file, "SET EB_TOOLBOX=%s\n", make_backslash(eb_toolbox));
    fprintf(conf_file, "SET EB_BOOTDIR=%s\n", make_backslash(eb_bootdir));
    fprintf(conf_file, "SET EB_TARGETS=%s\n", make_backslash(eb_targets));
    strcpy(tmp, eb_toolbox);
    strcat(tmp, "\\gnumake.exe");
    fprintf(conf_file, "SET EB_MAKE=%s\n", make_backslash(tmp));
    fclose(conf_file);
#else
    strcpy(tmp, cfdir);
    strcat(tmp, "/sdkshell.conf");
    fprintf(conf_file, "EB_BASE=%s\n", make_forward_slash(eb_base));
    fprintf(conf_file, "EB_TOOLBOX=%s\n", make_forward_slash(eb_toolbox));
    fprintf(conf_file, "EB_BOOTDIR=%s\n", make_forward_slash(eb_bootdir));
    fprintf(conf_file, "EB_TARGETS=\"%s\"\n", make_forward_slash(eb_targets));
    strcpy(tmp, eb_toolbox);
    strcat(tmp, "/gnumake");
    fprintf(conf_file, "EB_MAKE=%s\n", make_forward_slash(tmp));
    fprintf(conf_file, "export EB_BASE EB_TOOLBOX EB_BOOTDIR EB_TARGETS EB_MAKE\n");
    fclose(conf_file);

#endif

}   






