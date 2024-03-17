#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <windows.h>
#include <winreg.h>
#include <string.h>

#define YES 1
#define NO 0

int AcrobatReaderInstalled();
char * FX32ExecutableDirectory();

void mkdirpath(char * pathname)
{
    char drive[8];
    char *dir, *fname, *ext;

printf("mkdirpath: %s\n", pathname);

    dir = malloc(strlen(pathname)+1);
    fname = malloc(strlen(pathname)+1);
    ext = malloc(strlen(pathname)+1);
    _splitpath(pathname, drive, dir, fname, ext);
    if(strcmp(fname, "\\") == 0) {
	/* We're already at the root directory... so we're done */
	free(dir);
	free(fname);
	free(ext);
	return;
    }

    /* If we get here, then we have to make the directory.  Try to do so.
     * If we fail, try to make the parent directory.
     */
printf("mkdirpath: calling _mkdir(%s)\n", pathname);
    if(_mkdir(pathname) == -1) {
	char *parentpath;
printf("_mkdir failed, trying to make parent\n");
	parentpath = malloc(strlen(pathname)+1);
	_makepath(parentpath, drive, dir, NULL, NULL);
	mkdirpath(parentpath);
	free(parentpath);
	_mkdir(pathname);
    }
    free(dir);
    free(fname);
    free(ext);
    return;
}

    
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
    char * install_dir;
    char zipfile[_MAX_PATH];
    char unzipper[_MAX_PATH];
    char cmd[512];

    printf("======== EBSDK V4.0 installation ========\n");

    if(_access("LICENSE", 04) != -1) {

        printf("\n\nBefore you can use the EBSDK, you must read\n");
        printf("and accept the Compaq EBSDK license agreement\n");
        printf("Press ENTER to continue: ");
        fgets(cmd, sizeof(cmd), stdin);
	system("NOTEPAD LICENSE");
	if(get_yesno("Do you accept the license agreement?", NO) == NO) {
	    printf("Exiting install procedure\n");
	    exit(-1);
	}
    }

    /* Find out if Adobe Acrobat Reader is installed.  If not, ask
     * the user if it's desired.
     */
    if(!AcrobatReaderInstalled()) {
	printf("The SDK documentation is in PDF format and requires\n");
	printf("Adobe Acrobat Reader to read it.  Your system currently\n");
	printf("does not have Acrobat Reader installed.\n");
	if(get_yesno("Do you wish to install Acrobat Reader now?", YES) == YES) {
	    char * fxdir;

	    if((fxdir = FX32ExecutableDirectory()) == NULL) {
		printf("Acrobat Reader is a 32-bit x86 application.\n");
		printf("In order to run on NT/Alpha, you must install\n");
		printf("Compaq FX!32.\n");
		if(get_yesno("Install FX!32 now?", YES) == YES) {
		    printf("You will have to reboot the system after installing\n");
		    printf("FX!32 in order to install Acrobat Reader.  After rebooting,\n");
		    printf("run SETUP again to install Acrobat Reader and the SDK components.\n");
		    printf("Press any key to continue: ");
		    fgets(cmd, 512, stdin);
		    if(!_fullpath(cmd, "fx32v1-4.exe", sizeof(cmd))) {
			printf("Unable to find FX32 installer.\n");
			if(get_yesno("Continue with SDK installation?", NO) == NO) {
				exit(-1);
			}
			else {
				goto sdkinstall;
			}
		    }
		    system(cmd);
		}
		else {
		    printf("Skipping install of FX!32 and Acrobat Reader\n");
		    
	    	    if(get_yesno("Continue with SDK installation?", NO) == NO) {
			exit(-1);
	    	    }
		    else {
			goto sdkinstall;
		    }
		}
	    }
	    else {
		char	acropath[512];

		if(!_fullpath(acropath, "ar302.exe", sizeof(acropath))) {
		    printf("Cannot find Acrobat Reader install file\n");
	    	    if(get_yesno("Continue with SDK installation?", NO) == NO) {
			exit(-1);
	    	    }
		    else {
			goto sdkinstall;
		    }
		}
		sprintf(cmd, "%s%s %s", fxdir, "instlx86.exe", acropath);
		system(cmd);
	    }
	}
    }

sdkinstall:

    install_dir = "C:\\EBSDK";
    if(!_fullpath(zipfile, "ebsdk-nt.zip", sizeof(zipfile))) {
	printf("Unable to compute absolute pathname for zip file\n");
	exit(-1);
    }

    if(_access(zipfile, 04) == -1) {
	printf("Cannot find %s: exiting\n", zipfile);
	exit(-1);
    }

    if(!_fullpath(unzipper, "ebsdk\\ebtools\\ntalpha\\unzip.exe", sizeof(unzipper))) {
	printf("Unable to compute absolute pathname for unzip utility\n");
	exit(-1);
    }

    if(_access(unzipper, 04) == -1) {
	printf("Cannot find %s: exiting\n", unzipper);
	exit(-1);
    }


    install_dir = get_param("Enter directory where EBSDK\nis to be installed", install_dir);
    make_backslash(install_dir);

    if(_access(install_dir, 0) == -1) {
	printf("%s does not exist\n", install_dir);
	if(get_yesno("Create it", YES)) {
	    mkdirpath(install_dir);
	}
    }

    if(_chdir(install_dir) == -1) {
	printf("Cannot change to %s, exiting\n", install_dir);
	exit(-1);
    }
    printf("Unpacking EBSDK into %s\n", install_dir);
    sprintf(cmd, "%s %s", unzipper, zipfile);
    system(cmd);
}   

/* Functions to query the registry to find out about varous
 * programs
 */

int 
AcrobatReaderInstalled()
{
	LONG	result;
	HKEY	adobe;
	DWORD	i;
	char	subkey_name[512];
	DWORD	subkey_name_size = 512;
	char	subkey_class[512];
	DWORD	subkey_class_size = 512;
	FILETIME	lastwrite;

	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				"SOFTWARE\\Adobe",
				0,
				KEY_READ,
				&adobe);

	if(result != ERROR_SUCCESS) {
		return(0);
	}

	i = 0;

	do {
		result = RegEnumKeyEx(adobe, i, subkey_name,
			&subkey_name_size, NULL,
			subkey_class, &subkey_class_size,
			&lastwrite);
		if(result == ERROR_SUCCESS) {
			if(strncmp(subkey_name, "Acrobat Reader", 14) == 0) {
				RegCloseKey(adobe);
				return(1);
			}
			i++;
		}
		else if (result != ERROR_NO_MORE_ITEMS) {
			RegCloseKey(adobe);
			return(0);
		}
	} while(result != ERROR_NO_MORE_ITEMS);



	/* When we're done... */
	RegCloseKey(adobe);

	/* If we get here, we didn't find it */
	return(0);
}

char	fx32_exedir[512];

char * 
FX32ExecutableDirectory()
{
	LONG	result;
	HKEY	fx32;
	DWORD	value_size = 512;
	DWORD	value_type;

	result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				"SOFTWARE\\DigitalEquipmentCorporation\\FX!32\\GlobalInfo",
				0,
				KEY_READ,
				&fx32);

	if(result != ERROR_SUCCESS) {
		return(NULL);
	}


	result = RegQueryValueEx(fx32, "ExecutableDirectory", NULL,
				&value_type, (LPBYTE)fx32_exedir, &value_size);
	if((result == ERROR_SUCCESS) && 
		(value_type == REG_SZ)) {
		RegCloseKey(fx32);
		return(fx32_exedir);
	}


	/* When we're done... */
	RegCloseKey(fx32);

	/* If we get here, we didn't find it */
	return(NULL);
}
