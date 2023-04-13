; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAppForm
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "swshaders.h"
LastPage=0

ClassCount=4
Class1=CApp
Class2=CAppForm
Class3=CAppDoc
Class4=CAppFrameWnd

ResourceCount=4
Resource1=IDD_SELECTDEVICE
Resource2=IDR_MAIN_ACCEL
Resource3=IDD_FORMVIEW
Resource4=IDR_MAINFRAME

[CLS:CApp]
Type=0
BaseClass=CWinApp
HeaderFile=SWShaders.h
ImplementationFile=SWShaders.cpp

[CLS:CAppForm]
Type=0
BaseClass=CFormView 
HeaderFile=SWShaders.h
ImplementationFile=SWShaders.cpp
LastObject=IDC_SPIN_MAXZ
Filter=D
VirtualFilter=VWC

[CLS:CAppDoc]
Type=0
BaseClass=CDocument
HeaderFile=SWShaders.h
ImplementationFile=SWShaders.cpp
LastObject=CAppDoc

[CLS:CAppFrameWnd]
Type=0
BaseClass=CFrameWnd
HeaderFile=SWShaders.h
ImplementationFile=SWShaders.cpp

[DLG:IDD_FORMVIEW]
Type=1
Class=CAppForm
ControlCount=23
Control1=IDC_CHANGEDEVICE,button,1342373888
Control2=IDC_VIEWFULLSCREEN,button,1342373888
Control3=IDC_RENDERVIEW,static,1342177287
Control4=IDC_STATIC,button,1342177287
Control5=IDC_VS_INITIAL,button,1342373897
Control6=IDC_VS_OPT,button,1342373897
Control7=IDC_WIREFRAME,button,1342373891
Control8=IDC_STATIC,static,1342308352
Control9=IDC_TESS_SPIN,msctls_updown32,1342177312
Control10=IDC_TESSELLATION,edit,1350770816
Control11=IDC_STATIC,static,1342308352
Control12=IDC_MINX,edit,1350762624
Control13=IDC_STATIC,static,1342308352
Control14=IDC_MINZ,edit,1350762624
Control15=IDC_STATIC,static,1342308352
Control16=IDC_MAXX,edit,1350762624
Control17=IDC_STATIC,static,1342308352
Control18=IDC_MAXZ,edit,1350762624
Control19=IDC_STATIC,button,1342177287
Control20=IDC_SPIN_MINX,msctls_updown32,1342177312
Control21=IDC_SPIN_MINZ,msctls_updown32,1342177312
Control22=IDC_SPIN_MAXX,msctls_updown32,1342177312
Control23=IDC_SPIN_MAXZ,msctls_updown32,1342177312

[ACL:IDR_MAIN_ACCEL]
Type=1
Class=?
Command1=IDM_CHANGEDEVICE
Command2=IDC_VIEWFULLSCREEN
CommandCount=2

[DLG:IDD_SELECTDEVICE]
Type=1
Class=?
ControlCount=29
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308864
Control3=IDC_ADAPTER_COMBO,combobox,1344339971
Control4=IDC_STATIC,static,1342308864
Control5=IDC_DEVICE_COMBO,combobox,1344339971
Control6=IDC_STATIC,button,1342177287
Control7=IDC_WINDOW,button,1342373897
Control8=IDC_FULLSCREEN,button,1342177289
Control9=IDC_STATIC,static,1342308352
Control10=IDC_ADAPTERFORMAT_COMBO,combobox,1344471043
Control11=IDC_STATIC,static,1342308352
Control12=IDC_RESOLUTION_COMBO,combobox,1344471043
Control13=IDC_STATIC,static,1342308352
Control14=IDC_REFRESHRATE_COMBO,combobox,1344471043
Control15=IDC_STATIC,button,1342177287
Control16=IDC_STATIC,static,1342308352
Control17=IDC_BACKBUFFERFORMAT_COMBO,combobox,1344339971
Control18=IDC_STATIC,static,1342308352
Control19=IDC_DEPTHSTENCILBUFFERFORMAT_COMBO,combobox,1344339971
Control20=IDC_STATIC,static,1342308864
Control21=IDC_MULTISAMPLE_COMBO,combobox,1344339971
Control22=IDC_STATIC,static,1342308352
Control23=IDC_MULTISAMPLE_QUALITY_COMBO,combobox,1344339971
Control24=IDC_STATIC,static,1342308352
Control25=IDC_VERTEXPROCESSING_COMBO,combobox,1344339971
Control26=IDC_STATIC,static,1342308352
Control27=IDC_PRESENTINTERVAL_COMBO,combobox,1344339971
Control28=IDOK,button,1342242817
Control29=IDCANCEL,button,1342242816

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=IDM_CHANGEDEVICE
Command2=ID_APP_EXIT
CommandCount=2

