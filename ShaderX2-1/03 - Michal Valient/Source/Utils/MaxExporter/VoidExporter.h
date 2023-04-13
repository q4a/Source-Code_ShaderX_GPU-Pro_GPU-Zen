//*****************************************************************************
//* This file is part of the Dimension3 renderer library                      *
//* Copyright (C) 2003  Michal Valient (valient@host.sk)                      *
//*                                                                           *
//* Dimension3 renderer can be found at http://www.dimension3.host.sk         *
//*                                                                           *
//* This program is free software; you can redistribute it and/or modify      *
//* it under the terms of the GNU General Public License as published by      *
//* the Free Software Foundation; either version 2 of the License, or         *
//* (at your option) any later version.                                       *
//*                                                                           *
//* This program is distributed in the hope that it will be useful,           *
//* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
//* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
//* GNU General Public License for more details.                              *
//*                                                                           *
//* You should have received a copy of the GNU General Public License         *
//* along with this program; if not, write to the Free Software               *
//* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA * 
//*****************************************************************************
//hdr_utility_remove_tag


#pragma once

static const float SceneVersion = 2.0;
//
//This is exporter to dimension3 scene format
	class Dimension3Scene : public ExportFormat 	{
    private:
        CMapWordToOb   *Meshes;
        CMapWordToOb   *Objects;
        CMapWordToOb   *Cameras;
        CMapWordToOb   *Lights;
        CMapWordToOb   *Textures;
        CMapWordToOb   *Materials;
        CMapWordToOb   *Controllers;

        MainDescriptor  SceneInformations;
	public:
	// Constructor/Destructor
										Dimension3Scene();
		virtual							~Dimension3Scene();

	// Exporter init
		override(ExportFormat)	bool	Init(bool motion);

	// Main scene info
		override(ExportFormat)	bool	SetSceneInfo		(const MainDescriptor& maininfo);

	// Export loop
		override(ExportFormat)	bool	ExportCamera		(const CameraDescriptor& camera);
		override(ExportFormat)	bool	ExportController	(const ControllerDescriptor& controller);
		override(ExportFormat)	bool	ExportHelper		(const HelperDescriptor& helper);
		override(ExportFormat)	bool	ExportLight			(const LightDescriptor& light);
		override(ExportFormat)	bool	ExportMaterial		(const MaterialDescriptor& material);
		override(ExportFormat)	bool	ExportMesh			(const MeshDescriptor& mesh);
		override(ExportFormat)	bool	ExportMotion		(const MotionDescriptor& motion);
		override(ExportFormat)	bool	ExportShape			(const ShapeDescriptor& shape);
		override(ExportFormat)	bool	ExportTexture		(const TextureDescriptor& texture);

	// End of export notification
		override(ExportFormat)	bool	EndExport			(const StatsDescriptor& stats);

        override(ExportFormat)	void    About               ();
	private:
	};
