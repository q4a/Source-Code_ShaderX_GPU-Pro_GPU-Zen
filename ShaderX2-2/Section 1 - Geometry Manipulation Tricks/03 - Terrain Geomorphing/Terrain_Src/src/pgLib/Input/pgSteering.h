///////////////////////////////////////////////////////////////////
//
// project:     Pluto Gaming Engine
// class:       pgSteering
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSteering.h,v 1.5 2003/01/05 15:10:34 markus Exp $
///////////////////////////////////////////////////////////////////

#ifndef __STEERING_HEADERFILE__
#define __STEERING_HEADERFILE__


#include <pgLib/pgLib.h>
#include <pgLib/Input/pgIInput.h>
#include <pgLib/Math/pgMatrix.h>
#include <pgLib/Math/pgVec3.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


class pgIInput;
class pgString;

//struct PGLIB_API D3DXMATRIX;


/// pgSteering provides basic camera movement.
/**
 *  pgSteering has two moving modes: MODE_INSPECT and MODE_FLY
 *  MODE_INSPECT is designed to provide as pricise and free
 *  movement as possible.
 *  MODE_FLY moves the camera like a flying aeroplane.
 */
class PGLIB_API pgSteering
{
public:
	pgSteering();

	/// Steering Mode
	enum MODE {
				MODE_INSPECT,		/**<  Designed to provide as pricise and free movement as possible  */
				MODE_FLY,			/**<  Moves the camera like a flying aeroplane  */
				MODE_ROTATE,		/**<  Rotates the camera around a point in front of it instead of strafing  */
				MODE_SPECTATE
	};


	/// Sets a new movement mode
	void setMode(MODE nMode);
	MODE getMode() {return mode;}


	/// Switches to the next movement mode
	void switchMode();


	/// Sets the current position
	void setNewPosition(const pgVec3& nPos);


	/// Sets the position before update was called
	/**
	 *  pgSteering stores the previous (old) position when a new one
	 *  is calculated, which is needed for collision handling. This
	 *  method lets you specify this old position explicitly.
	 */
	void setOldPosition(const pgVec3& nPos)  {  oldPos = nPos;  }


	/// Sets a new direction
	void setDirection(const pgEuler& nDir)  {  dir = nDir;  }


	/// Sets the inspect-mode moving and rotation speed
	void setInspectModeSpeed(float nForwardSpeed, float nPitchSpeed, float nUpSpeed, float nRotateSpeed);


	/// Sets the inspect-mode speedup and mouse factors
	/**
	 *  Speedup is activated if the speedup key is pressed.
	 *  (Default: left shift)
	 */
	void setInspectModeFactors(float nSpeedupFactor, float nMouseXFactor, float nMouseYFactor)  {  inspectSpeedupFactor = nSpeedupFactor; mouseXFactor = nMouseXFactor;  mouseYFactor = nMouseYFactor;  }


	/// Sets heading properties for fly-mode
	void setFlyModeHead(float nDHeadSpeed, float nMaxHeadSpeed)  {  flyDHeadSpeed = nDHeadSpeed;  flyMaxHeadSpeed = nMaxHeadSpeed;  }


	/// Sets pitching properties for fly-mode
	void setFlyModePitch(float nDPitchSpeed, float nMaxPitchSpeed, float nMaxPitch)  {  flyDPitchSpeed = nDPitchSpeed;  flyMaxPitchSpeed = nMaxPitchSpeed;  flyMaxPitch = nMaxPitch;  }


	/// Sets forward moving properties for fly-mode
	void setFlyModeForward(float nDForwardSpeed, float nMaxForwardSpeed)  {  flyDForwardSpeed = nDForwardSpeed;  flyMaxForwardSpeed = nMaxForwardSpeed;  }


	/// Sets the rotate-mode moving and rotation speed
	void setRotateModeSpeed(float nForwardSpeed, float nPitchSpeed, float nUpSpeed, float nRotateSpeed);


	/// Sets the rotate-mode speedup and mouse factors
	void setRotateModeFactors(float nSpeedupFactor, float nMouseXFactor, float nMouseYFactor)  {  rotateSpeedupFactor = nSpeedupFactor; rotateMouseXFactor = nMouseXFactor;  rotateMouseYFactor = nMouseYFactor;  }


	/// Sets the spectate-mode moving and rotation speed
	void setSpectateModeSpeed(float nForwardSpeed, float nPitchSpeed, float nRotateSpeed);


	/// Sets the fly speedup factor
	/**
	 *  Speedup is activated if the speedup key is pressed.
	 *  (Default: left shift)
	 */
	void setFlySpeedupFactor(float nSpeedupFactor)  {  flySpeedupFactor = nSpeedupFactor;  }

	/// Returns the currently set mode
	MODE getMode() const  { return mode; }


	/// Returns the position before update() was called
	const pgVec3& getOldPosition() const  { return oldPos; }


	/// Returns the new position
	const pgVec3& getNewPosition() const  { return pos; }


	/// Returns the direction
	const pgEuler& getDirection() const  {  return dir;  }


	/// Calculates a new position and direction
	void update(const pgIInput& nInput);


	/// Creates a matrix from current position and rotation
	/**
	 *  If nApply is true pgSterring calls pgIDirectX::setViewMatrix()
	 *  to set the matrix as current rendering view matrix.
	 */
	void createViewMatrix(bool nApply=true);


	/// Returns the current view matrix
	const pgMatrix& getViewMatrix() const  {  return viewMatrix;  }


	/// Formats a string containing the current position
	void formatPositionString(pgString& nString);


	/// Formats a string containing the current direction
	void formatDirectionString(pgString& nString);


	/// Formats a string containing the mode
	void formatModeString(pgString& nString);


	void setup(const class pgSettingsFile& nSet);


	pgIInput::KEY
		getKeyToggleWireframe() const  {  return keyToggleWireframe;  }

	pgIInput::KEY
		getKeyToggleVisFix() const  {  return keyToggleFixVis;  }

	pgIInput::KEY
		getKeyToggleSteeringMode() const  {  return keyToggleSteeringMode;  }

	pgIInput::KEY
		getKeyBenchWriteStart() const  {  return benchKeyWriteStart;  }

	pgIInput::KEY
		getKeyBenchWriteEnd() const  {  return benchKeyWriteEnd;  }

	pgIInput::KEY
		getKeyBenchStart() const  {  return benchKeyStart;  }

	pgIInput::KEY
		getKeyBenchStop() const  {  return benchKeyStop;  }


	static void calcDirectionVectorRad(float nHead, float nPitch, pgVec3& nDir);
	static void calcDirectionVector(float nHead, float nPitch, pgVec3& nDir);

protected:
	void updateModeInspect(const pgIInput& nInput);
	void updateModeFly(const pgIInput& nInput);
	void initModeRotate();
	void updateModeRotate(const pgIInput& nInput);
	void updateModeSpectate(const pgIInput& nInput);
	void setKey(pgIInput::KEY& nKey, const pgString& nVal);

	// general properties
	//
	MODE			mode;
	pgVec3			pos, oldPos;
	pgEuler			dir;
	float			invertPitch;
	pgIInput::KEY	keyRight,keyLeft,keyUp,keyDown, keySpeedUp, keyToggleSpeedup;
	float			mouseXFactor, mouseYFactor;
	bool			toggleSpeedUp;

	pgIInput::KEY	keyToggleWireframe, keyToggleFixVis, keyToggleSteeringMode;


	// bench properties
	//
	pgIInput::KEY	benchKeyWriteStart, benchKeyWriteEnd, benchKeyStart, benchKeyStop;


	// inspect mode properties
	//
	pgIInput::KEY	inspectKeyForward, inspectKeyBack, inspectKeyLeft, inspectKeyRight, inspectKeyUp, inspectKeyDown, inspectKeySingleStep;

	float			inspectForwardSpeed, inspectStrafeSpeed, inspectUpSpeed, inspectRotSpeed,
					inspectSpeedupFactor;

	// fly mode properties
	//
	float		flyHeadSpeed, flyDHeadSpeed, flyMaxHeadSpeed,
				flyPitchSpeed, flyDPitchSpeed, flyMaxPitchSpeed, flyMaxPitch,
				flyForwardSpeed, flyDForwardSpeed, flyMaxForwardSpeed,
				flySpeedupFactor;

	// rotate
	pgVec3		rotatePos;
	float		rotateLength,
				rotateForwardSpeed, rotateStrafeSpeed, rotateUpSpeed, rotateRotSpeed,
				rotateSpeedupFactor, rotateMouseXFactor, rotateMouseYFactor;

	// spectate mode properties
	//
	pgIInput::KEY	spectateKeyLeft, spectateKeyRight, spectateKeyUp, spectateKeyDown, spectateKeyFwd, spectateKeyBack;

	float			spectateForwardSpeed, spectateStrafeSpeed, spectateRotSpeed,
					spectateSpeedupFactor;


	pgMatrix	viewMatrix;
};


#endif //__STEERING_HEADERFILE__
