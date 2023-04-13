; CLW-Datei enthält Informationen für den MFC-Klassen-Assistenten

[General Info]
Version=1
LastClass=CPVSShowDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "PVSShow.h"

ClassCount=3
Class1=CPVSShowApp
Class2=CPVSShowDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_PVSSHOW_DIALOG
Resource4=IDD_ABOUTBOX (Deutsch (Deutschland))
Resource5=IDD_PVSSHOW_DIALOG (Deutsch (Deutschland))

[CLS:CPVSShowApp]
Type=0
HeaderFile=PVSShow.h
ImplementationFile=PVSShow.cpp
Filter=N

[CLS:CPVSShowDlg]
Type=0
HeaderFile=PVSShowDlg.h
ImplementationFile=PVSShowDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_CHECKYD

[CLS:CAboutDlg]
Type=0
HeaderFile=PVSShowDlg.h
ImplementationFile=PVSShowDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Class=CAboutDlg


[DLG:IDD_PVSSHOW_DIALOG]
Type=1
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Class=CPVSShowDlg

[DLG:IDD_PVSSHOW_DIALOG (Deutsch (Deutschland))]
Type=1
Class=CPVSShowDlg
ControlCount=20
Control1=IDC_FRAME,static,1342181383
Control2=IDC_OPEN,button,1342242816
Control3=IDC_MIN,button,1342242816
Control4=IDC_MAX,button,1342242816
Control5=IDC_PVS,button,1476460544
Control6=IDC_HEIGHT,button,1342242816
Control7=IDC_INFO,static,1342312448
Control8=IDC_CALCPVS,button,1342242816
Control9=IDC_LOADPVS,button,1342242816
Control10=IDC_SAVEPVS,button,1476460544
Control11=IDC_HEIGHT_SLIDER,msctls_trackbar32,1342242840
Control12=IDC_HEIGHT_TXT,button,1342275584
Control13=IDC_SHRINKPVS,button,1342242816
Control14=IDC_INFO2,static,1342312448
Control15=IDC_CHECKX,edit,1350631552
Control16=IDC_CHECKY,edit,1350631552
Control17=IDC_CHECKZ,edit,1350631552
Control18=IDC_CHECKV,static,1342312448
Control19=IDC_CHECKXD,edit,1350631552
Control20=IDC_CHECKYD,edit,1350631552

[DLG:IDD_ABOUTBOX (Deutsch (Deutschland))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

