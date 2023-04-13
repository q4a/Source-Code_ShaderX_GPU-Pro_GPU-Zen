@REM copies libs from release directory to debug directory so user
@REM can do a debug build without having to ship debug libraries
@REM Script should be run from SDK root directory, from same folder
@REM that contains TOOLS, DEMOS, LIBS, etc.
@ECHO off
xcopy LIBS\lib\release 		LIBS\lib\debug\				/Y
xcopy LIBS\implib\release	LIBS\implib\debug\			/Y
xcopy LIBS\dll\release		LIBS\dll\debug\			  	/Y
xcopy DEMOS\Direct3D9\dll\release\*.dll	DEMOS\Direct3D9\dll\debug\ 	/Y
