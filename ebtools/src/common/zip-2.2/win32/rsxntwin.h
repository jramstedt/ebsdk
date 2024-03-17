/* rsxntwin.h
 *
 * fills some gaps in the rsxnt 1.3 win32 header files (<windows.h>) that are
 * required for compiling Info-ZIP sources for Win NT / Win 95
 */

#ifdef __RSXNT__
#if !defined (_RSXNTWIN_H)
#define _RSXNTWIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define PASCAL __stdcall

#ifndef TIME_ZONE_ID_UNKNOWN
#  define TIME_ZONE_ID_UNKNOWN  0
#endif

#define FILE_ATTRIBUTE_HIDDEN   0x00000002
#define FILE_ATTRIBUTE_SYSTEM   0x00000004

#define FILE_SHARE_DELETE       0x00000004

#define HFILE_ERROR        -1

#define _get_osfhandle(h) h


BOOL WINAPI DosDateTimeToFileTime(WORD, WORD, LPFILETIME);


#define SetVolumeLabel TFUNCT(SetVolumeLabel)
BOOL WINAPI SetVolumeLabel(LPCTSTR, LPCTSTR);


#define GetDriveType TFUNCT(GetDriveType)
DWORD GetDriveType(LPCTSTR);

#define DRIVE_REMOVABLE   2

#ifdef __cplusplus
}
#endif

#if (defined(ZIP) || defined(UNZIP_INTERNAL))
#  ifndef NO_NTSD_WITH_RSXNT
#    define NO_NTSD_WITH_RSXNT  /* RSXNT windows.h does not yet support NTSD */
#  endif
#endif

#endif /* !defined (_RSXNTWIN_H) */
#endif /* __RSXNT__ */
