Attribute VB_Name = "VBUnzBas"
Option Explicit
'
' Sample VB 5 code to drive unzip32.dll
' Contributed to the Info-Zip project by Mike Le Voi
'
' Contact me at: mlevoi@modemss.brisnet.org.au
'
' Visit my home page at: http://modemss.brisnet.org.au/~mlevoi
'
' Use this code at your own risk. Nothing implied or warranted
' to work on your machine :-)
'

' argv
Private Type ZIPnames
    s(0 To 99) As String
End Type

' Callback large "string" (sic)
Private Type CBChar
    ch(32800) As Byte
End Type

' Callback small "string" (sic)
Private Type CBCh
    ch(256) As Byte
End Type

' DCL structure
Private Type DCLIST
    ExtractOnlyNewer As Long
    SpaceToUnderscore As Long
    PromptToOverwrite As Long
    fQuiet As Long
    ncflag As Long
    ntflag As Long
    nvflag As Long
    nUflag As Long
    nzflag As Long
    ndflag As Long
    noflag As Long
    naflag As Long
    nZIflag As Long
    C_flag As Long
    fPrivilege As Long
    Zip As String
    ExtractDir As String
End Type

' Userfunctions structure
Private Type USERFUNCTION
    DllPrnt As Long
    DLLSND As Long
    DLLREPLACE As Long
    DLLPASSWORD As Long
    DLLMESSAGE As Long
    cchComment As Integer
    TotalSizeComp As Long
    TotalSize As Long
    CompFactor As Long
    NumMembers As Long
End Type

' Unzip32.dll version structure
Private Type UZPVER
    structlen As Long
    flag As Long
    beta As String * 10
    date As String * 20
    zlib As String * 10
    unzip(1 To 4) As Byte
    zipinfo(1 To 4) As Byte
    os2dll As Long
    windll(1 To 4) As Byte
End Type

' This assumes unzip32.dll is in
' your \windows\system directory!
Private Declare Function windll_unzip Lib "unzip32.dll" _
    (ByVal ifnc As Long, ByRef ifnv As ZIPnames, _
     ByVal xfnc As Long, ByRef xfnv As ZIPnames, _
     dcll As DCLIST, Userf As USERFUNCTION) As Long

' This assumes unzip32.dll is in
' your \windows\system directory!
Private Declare Sub UzpVersion2 Lib "unzip32.dll" _
    (uzpv As UZPVER)

' Private structures
Dim MYDCL As DCLIST
Dim MYUSER As USERFUNCTION
Dim MYVER As UZPVER

Global vbzipnum As Long, vbzipmes As String
Global vbzipinf As String
Global vbzipnam As ZIPnames, vbxnames As ZIPnames
Global crlf$

' Puts a function pointer in a structure
Function FnPtr(ByVal lp As Long) As Long
    FnPtr = lp
End Function

' Callback for unzip32.dll
Sub ReceiveDllMessage(ByVal ucsize As Long, _
    ByVal csiz As Long, _
    ByVal cfactor As Integer, _
    ByVal mo As Integer, _
    ByVal dy As Integer, _
    ByVal yr As Integer, _
    ByVal hh As Integer, _
    ByVal mm As Integer, _
    ByVal c As Byte, ByRef fname As CBCh, _
    ByRef meth As CBCh, ByVal crc As Long, _
    ByVal fCrypt As Byte)

    Dim s0$, xx As Long
    Dim strout As String * 80

    ' always put this in callback routines!
    On Error Resume Next
    strout = Space(80)
    If vbzipnum = 0 Then
        Mid$(strout, 1, 50) = "Filename:"
        Mid$(strout, 53, 4) = "Size"
        Mid$(strout, 62, 4) = "Date"
        Mid$(strout, 71, 4) = "Time"
        vbzipmes = strout + crlf
        strout = Space(80)
    End If
    s0 = ""
    For xx = 0 To 255
        If fname.ch(xx) = 0 Then xx = 99999 Else s0 = s0 & Chr$(fname.ch(xx))
    Next xx
    Mid$(strout, 1, 50) = Mid$(s0, 1, 50)
    Mid$(strout, 51, 7) = Right$("        " + Str$(ucsize), 7)
    Mid$(strout, 60, 3) = Right$(Str$(dy), 2) + "/"
    Mid$(strout, 63, 3) = Right$("0" + Trim$(Str$(mo)), 2) + "/"
    Mid$(strout, 66, 2) = Right$("0" + Trim$(Str$(yr)), 2)
    Mid$(strout, 70, 3) = Right$(Str$(hh), 2) + ":"
    Mid$(strout, 73, 2) = Right$("0" + Trim$(Str$(mm)), 2)
    ' Mid$(strout, 75, 2) = Right$(" " + Str$(cfactor), 2)
    ' Mid$(strout, 78, 8) = Right$("        " + Str$(csiz), 8)
    ' s0 = ""
    ' For xx = 0 To 255
    '     If meth.ch(xx) = 0 Then xx = 99999 Else s0 = s0 + Chr(meth.ch(xx))
    ' Next xx
    vbzipmes = vbzipmes + strout + crlf
    vbzipnum = vbzipnum + 1
End Sub

' Callback for unzip32.dll
Function DllPrnt(ByRef fname As CBChar, ByVal x As Long) As Long
    Dim s0$, xx As Long

    ' always put this in callback routines!
    On Error Resume Next
    s0 = ""
    For xx = 0 To x
        If fname.ch(xx) = 0 Then xx = 99999 Else s0 = s0 + Chr(fname.ch(xx))
    Next xx
    vbzipinf = vbzipinf + s0
    DllPrnt = 0
End Function

' Callback for unzip32.dll
Function DllPass(ByRef s1 As Byte, x As Long, _
    ByRef s2 As Byte, _
    ByRef s3 As Byte) As Long

    ' always put this in callback routines!
    On Error Resume Next
    ' not supported - always return 1
    DllPass = 1
End Function

' Callback for unzip32.dll
Function DllRep(ByRef fname As CBChar) As Long
    Dim s0$, xx As Long

    ' always put this in callback routines!
    On Error Resume Next
    DllRep = 100 ' 100=do not overwrite - keep asking user
    s0 = ""
    For xx = 0 To 255
        If fname.ch(xx) = 0 Then xx = 99999 Else s0 = s0 + Chr(fname.ch(xx))
    Next xx
    xx = MsgBox("Overwrite " + s0 + "?", vbYesNoCancel, "VBUnzip - File already exists")
    If xx = vbNo Then Exit Function
    If xx = vbCancel Then
        DllRep = 104 ' 104=overwrite none
        Exit Function
    End If
    DllRep = 102 ' 102=overwrite 103=overwrite all
End Function

' ASCIIZ to String
Function szTrim(szString As String) As String
    Dim pos As Integer, ln As Integer

    pos = InStr(szString, Chr$(0))
    ln = Len(szString)
    Select Case pos
        Case Is > 1
            szTrim = Trim(Left(szString, pos - 1))
        Case 1
            szTrim = ""
        Case Else
            szTrim = Trim(szString)
    End Select
End Function

' Main subroutine
Sub VBUnzip(fname As String, extdir As String, _
    prom As Integer, ovr As Integer, _
    mess As Integer, dirs As Integer, numfiles As Long, numxfiles As Long)
    Dim xx As Long ' , s1 As String * 20, s2 As String * 256

'    ChDrive Mid$(extdir, 1, 1)
'    ChDir extdir
    '
    MYDCL.ExtractOnlyNewer = 0      ' 1=extract only newer
    MYDCL.SpaceToUnderscore = 0     ' 1=convert space to underscore
    MYDCL.PromptToOverwrite = prom  ' 1=prompt to overwrite required
    MYDCL.fQuiet = 0                ' 2=no messages 1=less 0=all
    MYDCL.ncflag = 0                ' 1=write to stdout
    MYDCL.ntflag = 0                ' 1=test zip
    MYDCL.nvflag = mess             ' 0=extract 1=list contents
    MYDCL.nUflag = 0                ' 1=extract only newer
    MYDCL.nzflag = 0                ' 1=display zip file comment
    MYDCL.ndflag = dirs             ' 1=honour directories
    MYDCL.noflag = ovr              ' 1=overwrite files
    MYDCL.naflag = 0                ' 1=convert CR to CRLF
    MYDCL.nZIflag = 0               ' 1=Zip Info Verbose
    MYDCL.C_flag = 0                ' 1=Case insensitivity, 0=Case Sensitivity
    MYDCL.fPrivilege = 0            ' 1=ACL 2=priv
    MYDCL.Zip = fname               ' ZIP name
    MYDCL.ExtractDir = extdir       ' Extraction directory, NULL if extracting
                                    ' to current directory
    '
    MYUSER.DllPrnt = FnPtr(AddressOf DllPrnt)
    MYUSER.DLLSND = 0&
    MYUSER.DLLREPLACE = FnPtr(AddressOf DllRep)
    MYUSER.DLLPASSWORD = FnPtr(AddressOf DllPass)
    MYUSER.DLLMESSAGE = FnPtr(AddressOf ReceiveDllMessage)
    '
    With MYVER
        .structlen = Len(MYVER)
        .beta = Space$(9) & vbNullChar
        .date = Space$(19) & vbNullChar
        .zlib = Space$(9) & vbNullChar
    End With
    '
    Call UzpVersion2(MYVER)
    'VBUnzFrm.Print MYVER.structlen
    'VBUnzFrm.Print Hex$(MYVER.flag)
    'VBUnzFrm.Print szTrim(MYVER.beta)
    VBUnzFrm.Print "DLL Date: " & szTrim(MYVER.date)
    'VBUnzFrm.Print szTrim(MYVER.zlib)
    'VBUnzFrm.Print Hex$(MYVER.unzip(1)) + "." + Hex$(MYVER.unzip(2)) + Hex$(MYVER.unzip(3))
    VBUnzFrm.Print "Zip Info: " & Hex$(MYVER.zipinfo(1)) + "." + Hex$(MYVER.zipinfo(2)) + Hex$(MYVER.zipinfo(3))
    'VBUnzFrm.Print Hex$(MYVER.os2dll)
    VBUnzFrm.Print "DLL Version: " & Hex$(MYVER.windll(1)) + "." + Hex$(MYVER.windll(2)) + Hex$(MYVER.windll(3))
    VBUnzFrm.Print "----------"
    '
    xx = windll_unzip(numfiles, vbzipnam, numxfiles, vbxnames, MYDCL, MYUSER)
    If xx <> 0 Then MsgBox xx
    'Debug.Print "--------------"
    'Debug.Print MYUSER.cchComment
    'Debug.Print MYUSER.TotalSizeComp
    'Debug.Print MYUSER.TotalSize
    'Debug.Print MYUSER.CompFactor
    'Debug.Print MYUSER.NumMembers
    'Debug.Print "--------------"
    'MsgBox ""
End Sub
