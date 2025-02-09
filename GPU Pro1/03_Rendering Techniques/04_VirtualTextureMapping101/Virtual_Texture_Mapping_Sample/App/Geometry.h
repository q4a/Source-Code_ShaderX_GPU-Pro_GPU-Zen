/*
	Virtual texture mapping demo app
    Copyright (C) 2008, 2009 Matth�us G. Chajdas
    Contact: shaderx8@anteru.net

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef VTM_GEOMETRY_H
#define VTM_GEOMETRY_H

#include <vector>

struct VertexBufferData_t
{
	VertexBufferData_t (void* d, int size, int elementSize);

	std::vector<unsigned char> data;
	int elementSize;
};

struct Geometry_t
{
	std::vector<unsigned int>			indices;
	std::vector<VertexBufferData_t>		vertices;

	D3D10_PRIMITIVE_TOPOLOGY			type;
};

#endif