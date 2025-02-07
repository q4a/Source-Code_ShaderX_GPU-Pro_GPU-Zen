/*

Copyright 2013,2014 Sergio Ruiz, Benjamin Hernandez

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>

In case you, or any of your employees or students, publish any article or
other material resulting from the use of this  software, that publication
must cite the following references:

Sergio Ruiz, Benjamin Hernandez, Adriana Alvarado, and Isaac Rudomin. 2013.
Reducing Memory Requirements for Diverse Animated Crowds. In Proceedings of
Motion on Games (MIG '13). ACM, New York, NY, USA, , Article 55 , 10 pages.
DOI: http://dx.doi.org/10.1145/2522628.2522901

Sergio Ruiz and Benjamin Hernandez. 2015. A Parallel Solver for Markov Decision Process
in Crowd Simulations. Fourteenth Mexican International Conference on Artificial
Intelligence (MICAI), Cuernavaca, 2015, pp. 107-116.
DOI: 10.1109/MICAI.2015.23

*/
#include "cAnimation.h"

//=======================================================================================
//
Animation::Animation( void )
{
	multitextureID	= 0;
	num_frames		= 0;
	step			= 0;
}
//
//=======================================================================================
//
Animation::~Animation( void )
{

}
//
//=======================================================================================
//
void Animation::loadMultitexture(	string&		dir,
									string&		ext,
									string&		rest_name,
									string&		frame_name,
								    GLuint		start_frame,
									GLuint		end_frame	)
{
	vector<string> filenames;
	vector<GLint> parameters;

	string rest = dir;
	rest.append( "/" );
	rest.append( rest_name );
	rest.append( "." );
	rest.append( ext );
	std::transform( rest.begin(), rest.end(), rest.begin(), ::tolower );

	num_frames	= 0;
	step		= 0;
	for( unsigned int f = start_frame; f <= end_frame; f++ )
	{
		string fName = dir;
		fName.append( "/" );
		fName.append( frame_name );
		string frame_str = static_cast<ostringstream*>( &(ostringstream() << f) )->str();
		fName.append( frame_str );
		fName.append( "." );
		fName.append( ext );
		std::transform( fName.begin(), fName.end(), fName.begin(), ::tolower );
		filenames.push_back( fName );
		parameters.push_back( GL_NEAREST );
		num_frames++;
	}
	multitextureID = TextureManager::getInstance()->loadRawTexture3D(	filenames,
																		parameters,
																		4,
																		GL_RGBA32F,
																		GL_RGBA,
																		GL_TEXTURE_2D_ARRAY	);
}
//
//=======================================================================================
//
GLuint Animation::getLength( void )
{
	return num_frames;
}
//
//=======================================================================================
//
void Animation::nextFrame( void )
{
	step++;
	if( step >= num_frames )
	{
		step = 0;
	}
}
//
//=======================================================================================
