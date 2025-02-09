#--------------------------------------------------------------------------
# Name         : content.mak
# Title        : Makefile to build content files
# Author       : Auto-generated
# Created      : 10/09/2009
#
# Copyright    : 2007 by Imagination Technologies.  All rights reserved.
#              : No part of this software, either material or conceptual 
#              : may be copied or distributed, transmitted, transcribed,
#              : stored in a retrieval system or translated into any 
#              : human or computer language in any form by any means,
#              : electronic, mechanical, manual or other-wise, or 
#              : disclosed to third parties without the express written
#              : permission of VideoLogic Limited, Unit 8, HomePark
#              : Industrial Estate, King's Langley, Hertfordshire,
#              : WD4 8LZ, U.K.
#
# Description  : Makefile to build content files for demos in the PowerVR SDK
#
# Platform     :
#
# $Revision: 1.5 $
#--------------------------------------------------------------------------

#############################################################################
## Variables
#############################################################################

PVRTEXTOOL 	= ..\..\..\Utilities\PVRTexTool\PVRTexToolCL\Win32\PVRTexTool.exe
FILEWRAP 	= ..\..\..\Utilities\Filewrap\Win32\Filewrap.exe
PVRUNISCO 	= ..\..\..\Utilities\PVRUniSCo\OGLES\Win32\PVRUniSCo.exe

MEDIAPATH = ../Media
CONTENTDIR = Content

#############################################################################
## Instructions
#############################################################################

TEXTURES = \
	Body.pvr \
	Legs.pvr \
	Belt.pvr

BIN_SHADERS = \
	FragShader.fsc \
	VertShader.vsc

RESOURCES = \
	$(CONTENTDIR)/Body.cpp \
	$(CONTENTDIR)/Legs.cpp \
	$(CONTENTDIR)/Belt.cpp \
	$(CONTENTDIR)/FragShader.cpp \
	$(CONTENTDIR)/VertShader.cpp \
	$(CONTENTDIR)/man.cpp

all: resources
	
help:
	@echo Valid targets are:
	@echo resources, textures, binary_shaders, clean
	@echo PVRTEXTOOL, FILEWRAP and PVRUNISCO can be used to override the default paths to these utilities.

clean:
	-rm $(RESOURCES)
	-rm $(BIN_SHADERS)
	-rm $(TEXTURES)

resources: 		$(CONTENTDIR) $(RESOURCES)
textures: 		$(TEXTURES)
binary_shaders:	$(BIN_SHADERS)

$(CONTENTDIR):
	-mkdir $@

Body.pvr: $(MEDIAPATH)/body.png
	$(PVRTEXTOOL) -m -fOGLPVRTC4 -i$(MEDIAPATH)/body.png -o$@

Legs.pvr: $(MEDIAPATH)/legs.png
	$(PVRTEXTOOL) -m -fOGLPVRTC4 -i$(MEDIAPATH)/legs.png -o$@

Belt.pvr: $(MEDIAPATH)/belt.png
	$(PVRTEXTOOL) -m -fOGLPVRTC4 -i$(MEDIAPATH)/belt.png -o$@

$(CONTENTDIR)/Body.cpp: Body.pvr
	$(FILEWRAP)  -o $@ Body.pvr

$(CONTENTDIR)/Legs.cpp: Legs.pvr
	$(FILEWRAP)  -o $@ Legs.pvr

$(CONTENTDIR)/Belt.cpp: Belt.pvr
	$(FILEWRAP)  -o $@ Belt.pvr

$(CONTENTDIR)/FragShader.cpp: FragShader.fsh FragShader.fsc
	$(FILEWRAP)  -s  -o $@ FragShader.fsh
	$(FILEWRAP)  -oa $@ FragShader.fsc

$(CONTENTDIR)/VertShader.cpp: VertShader.vsh VertShader.vsc
	$(FILEWRAP)  -s  -o $@ VertShader.vsh
	$(FILEWRAP)  -oa $@ VertShader.vsc

$(CONTENTDIR)/man.cpp: man.pod
	$(FILEWRAP)  -o $@ man.pod

FragShader.fsc: FragShader.fsh
	$(PVRUNISCO) FragShader.fsh $@  -f 

VertShader.vsc: VertShader.vsh
	$(PVRUNISCO) VertShader.vsh $@  -v 

############################################################################
# End of file (content.mak)
############################################################################
