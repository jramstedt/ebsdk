VERSION 5.00
Begin VB.Form VBUnzFrm 
   AutoRedraw      =   -1  'True
   Caption         =   "VBUnzFrm"
   ClientHeight    =   3195
   ClientLeft      =   780
   ClientTop       =   525
   ClientWidth     =   10425
   BeginProperty Font 
      Name            =   "Fixedsys"
      Size            =   9
      Charset         =   0
      Weight          =   400
      Underline       =   0   'False
      Italic          =   0   'False
      Strikethrough   =   0   'False
   EndProperty
   LinkTopic       =   "VBUnzFrm"
   ScaleHeight     =   3195
   ScaleWidth      =   10425
End
Attribute VB_Name = "VBUnzFrm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
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


Private Sub Form_Click()
    Dim prom%, over%, mess%, dirs%, numf&, numx&
    Dim zipfile$, unzipdir$

    crlf = Chr$(13) + Chr$(10)
    Cls
    ' init global message variables
    vbzipinf = ""
    vbzipnum = 0
    ' select unzip options - change as required!
    prom = 1  ' 1=prompt to overwrite
    over = 0  ' 1=always overwrite files
    ' change the next line to do the actual unzip!
    mess = 1  ' 1=list contents of zip 0=extract
    dirs = 1  ' 1=honour zip directories
    ' select filenames if required
    '   vbzipnam.s(0) = "sfx16.dat"
    '   vbzipnam.s(1) = "sfx32.dat"
    '   vbzipnam.s(2) = "windll.h"
    '   numf = 3
    ' or just select all files
    vbzipnam.s(0) = vbNullString
    numf = 0
    ' select filenames to exclude from processing
    vbxnames.s(0) = vbNullString
    numx = 0
    ' Change the next 2 lines as required!
    zipfile = "e:\load\mike9.zip"
    unzipdir = "C:\WORK"
    '
    Call VBUnzip(zipfile, unzipdir, _
        prom, over, mess, dirs, numf, numx)
    If Len(vbzipmes) > 0 Then Print vbzipmes
    If Len(vbzipinf) > 0 Then
        Print "vbzipinf is:"
        Print vbzipinf
    End If
    If vbzipnum > 0 Then Print "Number of files: " + Str$(vbzipnum)
End Sub

