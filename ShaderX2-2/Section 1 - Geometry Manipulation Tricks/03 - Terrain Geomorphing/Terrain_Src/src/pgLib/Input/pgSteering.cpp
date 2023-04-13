///////////////////////////////////////////////////////////////////
//
// project:     Geisterschloss
// class:       pgSteering
// creator:     Daniel Wagner
// description: 
//
///////////////////////////////////////////////////////////////////
// $Id: pgSteering.cpp,v 1.10 2003/01/06 12:51:11 daniel Exp $
///////////////////////////////////////////////////////////////////

#define DIRECTINPUT_VERSION  0x0800

#include "pgSteering.h"

#include <pgLib/Base/pgSettingsFile.h>
#include <pgLib/Base/pgIDirectX.h>
#include <pgLib/Base/pgITime.h>
#include <pgLib/Math/pgIMathTool.h>


#include <DINPUT.h>



pgSteering::pgSteering()
{
	invertPitch = 1.0f;
	toggleSpeedUp = false;

	keyToggleWireframe = pgIInput::KEY_F;
	keyToggleFixVis = pgIInput::KEY_V;
	keyToggleSteeringMode = pgIInput::KEY_M;

	keyRight = pgIInput::KEY_RIGHT;
	keyLeft = pgIInput::KEY_LEFT;
	keyUp = pgIInput::KEY_UP;
	keyDown = pgIInput::KEY_DOWN;
	keyToggleSpeedup = pgIInput::KEY_CAPSLOCK;
	keySpeedUp = pgIInput::KEY_LSHIFT;

	keyToggleWireframe = pgIInput::KEY_F;
	keyToggleFixVis = pgIInput::KEY_V;

	mouseXFactor = 0.15f;
	mouseYFactor = 0.10f;

	// bench properties
	//
	benchKeyWriteStart = pgIInput::KEY_HOME;
	benchKeyWriteEnd = pgIInput::KEY_END;
	benchKeyStart = pgIInput::KEY_PGUP;
	benchKeyStop = pgIInput::KEY_PGDN;


	// inspect properties
	//
	inspectKeyForward = pgIInput::KEY_W;
	inspectKeyBack = pgIInput::KEY_S;
	inspectKeyLeft = pgIInput::KEY_A;
	inspectKeyRight = pgIInput::KEY_D;
	inspectKeyUp = pgIInput::KEY_E;
	inspectKeyDown = pgIInput::KEY_Q;
	inspectKeySingleStep = pgIInput::KEY_SPACE;

	inspectForwardSpeed = 50.0f;
	inspectStrafeSpeed = 50.0f;
	inspectUpSpeed = 100.0f;
	inspectRotSpeed = 80.0f;
	inspectSpeedupFactor = 4.0f;

	// fly properties
	//
	flyHeadSpeed = 0.0f;
	flyDHeadSpeed = 150.0f;
	flyMaxHeadSpeed = 80.0f;
	flyPitchSpeed = 0.0f;
	flyDPitchSpeed = 300.0f;
	flyMaxPitchSpeed = 200.0f;
	flyMaxPitch = 30.0f;
	flyForwardSpeed = 0.0f;
	flyDForwardSpeed = 6000.0f;
	flyMaxForwardSpeed = 2500.0f;
	flySpeedupFactor = 4.0f;


	// rotate properties
	rotateLength = 10000.0f;
	rotateForwardSpeed = 3000.0f;
	rotateStrafeSpeed = 1500.0f;
	rotateUpSpeed = 1000.0f;
	rotateRotSpeed = 40.0f;
	rotateSpeedupFactor = 4.0f;
	rotateMouseXFactor = 0.08f;
	rotateMouseYFactor = 0.05f;


	// spectate properties
	//
	spectateKeyLeft = pgIInput::KEY_A;
	spectateKeyRight= pgIInput::KEY_D;
	spectateKeyUp	= pgIInput::KEY_W;
	spectateKeyDown = pgIInput::KEY_S;
	spectateKeyFwd	= pgIInput::KEY_E;
	spectateKeyBack	= pgIInput::KEY_Q;;
	
	spectateForwardSpeed = 50.0f;
	spectateStrafeSpeed = 200.0f;
	//spectateUpSpeed = 100.0f;
	spectateRotSpeed = 80.0f;
	spectateSpeedupFactor = 4.0f;


	setMode(MODE_INSPECT);
}


void
pgSteering::setMode(MODE nMode)
{
	mode = nMode;

	if(mode==MODE_ROTATE)
		initModeRotate();
}

void
pgSteering::switchMode()
{
	switch(mode)
	{
	case MODE_INSPECT:
		setMode(MODE_FLY);
		break;

	case MODE_FLY:
		setMode(MODE_ROTATE);
		break;

	case MODE_ROTATE:
		setMode(MODE_SPECTATE);
		break;

	case MODE_SPECTATE:
		setMode(MODE_INSPECT);
		break;
	}
}


void
pgSteering::setNewPosition(const pgVec3& nPos)
{
	pos = nPos;
	pgIDirectX::setCameraPos(pos);
}


void
pgSteering::setInspectModeSpeed(float nForwardSpeed, float nPitchSpeed, float nUpSpeed, float nRotateSpeed)
{
	inspectForwardSpeed = nForwardSpeed;
	inspectStrafeSpeed = nPitchSpeed;
	inspectUpSpeed = nUpSpeed;
	inspectRotSpeed = nRotateSpeed;
}


void
pgSteering::setRotateModeSpeed(float nForwardSpeed, float nPitchSpeed, float nUpSpeed, float nRotateSpeed)
{
	rotateForwardSpeed = nForwardSpeed;
	rotateStrafeSpeed = nPitchSpeed;
	rotateUpSpeed = nUpSpeed;
	rotateRotSpeed = nRotateSpeed;
}


void
pgSteering::setSpectateModeSpeed(float nForwardSpeed, float nPitchSpeed, float nRotateSpeed)
{
	spectateForwardSpeed = nForwardSpeed;
	spectateStrafeSpeed = nPitchSpeed;
	spectateRotSpeed = nRotateSpeed;
}


void 
pgSteering::update(const pgIInput& nInput)
{
	oldPos = pos;

	switch(mode)
	{
	case MODE_INSPECT:
		updateModeInspect(nInput);
		break;

	case MODE_FLY:
		updateModeFly(nInput);
		break;

	case MODE_ROTATE:
		updateModeRotate(nInput);
		break;

	case MODE_SPECTATE:
		updateModeSpectate(nInput);
		break;
	}
}


void
pgSteering::createViewMatrix(bool nApply)
{
	float	headR = dir[0]*3.1415f/180.0f,
			pitchR = dir[1]*3.1415f/180.0f,
			rollR = dir[2]*3.1415f/180.0f;


	pgMatrix matRotH, matRotP, matRotR;

	viewMatrix.makeTrans(-pos);
	matRotH.makeRotZXY(headR, 0.0f, 0.0f);
	matRotP.makeRotZXY(0.0f, -pitchR, 0.0f);
	matRotR.makeRotZXY(0.0f, 0.0f, -rollR);

	viewMatrix.postMult(matRotH);
	viewMatrix.postMult(matRotP);
	viewMatrix.postMult(matRotR);

	pgIDirectX::setCameraPos(pos);
	if(nApply)
		pgIDirectX::setViewMatrix(viewMatrix);

	pgIDirectX::setCameraMatrix(viewMatrix);
}


void
pgSteering::updateModeInspect(const pgIInput& nInput)
{
	if(nInput.isKeyNewDown(keyToggleSpeedup))
		toggleSpeedUp = !toggleSpeedUp;

	float	dt = pgITime::getLastFrameTime();
	float	speedUp = (nInput.isKeyDown(keySpeedUp) || toggleSpeedUp) ? inspectSpeedupFactor : 1.0f,
			speedUpH = (nInput.isKeyDown(keySpeedUp) || toggleSpeedUp) ? inspectSpeedupFactor*0.5f : 1.0f;

	// rotation
	//
	dir[0] -= mouseXFactor * 0.01f * nInput.getMouseX();
	dir[1] -= invertPitch * mouseYFactor * 0.01f * nInput.getMouseY();

	if(nInput.isKeyDown(keyRight))
		dir[0] -= inspectRotSpeed * dt * speedUpH;

	if(nInput.isKeyDown(keyLeft))
		dir[0] += inspectRotSpeed * dt * speedUpH;

	if(nInput.isKeyDown(keyDown))
		dir[1] -= invertPitch * inspectRotSpeed * dt * speedUpH;

	if(nInput.isKeyDown(keyUp))
		dir[1] += invertPitch * inspectRotSpeed * dt * speedUpH;

	while(dir[0]>=360.0f)	dir[0] -= 360.0f;
	while(dir[0]<0.0f)		dir[0] += 360.0f;
	if(dir[1]>= 89.0f)		dir[1] =  89.0f;
	if(dir[1]<=-89.0f)		dir[1] = -89.0f;


	float dz = (float)cos(dir[0]*3.1415/180) * speedUp,
		  dx = -(float)sin(dir[0]*3.1415/180) * speedUp;


	if((nInput.isKeyDown(inspectKeyUp) && !nInput.isKeyDown(inspectKeySingleStep)) || nInput.isKeyNewDown(inspectKeyUp))
		pos.y += inspectUpSpeed * dt * speedUp;

	if((nInput.isKeyDown(inspectKeyDown) && !nInput.isKeyDown(inspectKeySingleStep)) || nInput.isKeyNewDown(inspectKeyDown))
		pos.y -= inspectUpSpeed * dt * speedUp;

	if(nInput.isKeyDown(inspectKeyDown))
		pos.y += 0.0f;

	if((nInput.isKeyDown(inspectKeyForward) && !nInput.isKeyDown(inspectKeySingleStep)) || nInput.isKeyNewDown(inspectKeyForward))
	{
		pos.x += inspectForwardSpeed * dx * dt;
		pos.z += inspectForwardSpeed * dz * dt;
	}

	if((nInput.isKeyDown(inspectKeyBack) && !nInput.isKeyDown(inspectKeySingleStep)) || nInput.isKeyNewDown(inspectKeyBack))
	{
		pos.x -= inspectForwardSpeed * dx * dt;
		pos.z -= inspectForwardSpeed * dz * dt;
	}

	if((nInput.isKeyDown(inspectKeyRight) && !nInput.isKeyDown(inspectKeySingleStep)) || nInput.isKeyNewDown(inspectKeyRight))
	{
		pos.x += inspectStrafeSpeed * dz * dt;
		pos.z -= inspectStrafeSpeed * dx * dt;
	}

	if((nInput.isKeyDown(inspectKeyLeft) && !nInput.isKeyDown(inspectKeySingleStep)) || nInput.isKeyNewDown(inspectKeyLeft))
	{
		pos.x -= inspectStrafeSpeed * dz * dt;
		pos.z += inspectStrafeSpeed * dx * dt;
	}
}


void
pgSteering::updateModeSpectate(const pgIInput& nInput)
{
	if(nInput.isKeyNewDown(keyToggleSpeedup))
		toggleSpeedUp = !toggleSpeedUp;

	float	dt = pgITime::getLastFrameTime();
	float	speedUp = (nInput.isKeyDown(keySpeedUp) || toggleSpeedUp) ? spectateSpeedupFactor : 1.0f,
			speedUpH = (nInput.isKeyDown(keySpeedUp) || toggleSpeedUp) ? inspectSpeedupFactor*0.5f : 1.0f;	

	// rotation
	//
	dir[0] -= mouseXFactor * 0.01f * nInput.getMouseX();
	dir[1] -= invertPitch * mouseYFactor * 0.01f * nInput.getMouseY();


	if(nInput.isKeyDown(spectateKeyRight))
		dir[0] -= spectateRotSpeed * dt * speedUpH;

	if(nInput.isKeyDown(spectateKeyLeft))
		dir[0] += spectateRotSpeed * dt * speedUpH;

	if(nInput.isKeyDown(spectateKeyDown))
		dir[1] -= invertPitch * spectateRotSpeed * dt * speedUpH;

	if(nInput.isKeyDown(spectateKeyUp))
		dir[1] += invertPitch * spectateRotSpeed * dt * speedUpH;


	if(dir[0]>=360.0f)	dir[0] -= 360.0f;
	else if(dir[0]<0.0f)		dir[0] += 360.0f;
	if(dir[1]>= 89.0f)		dir[1] =  89.0f;
	if(dir[1]<=-89.0f)		dir[1] = -89.0f;



	pgVec3	fwd = pgVec3( -(float)sin(dir[0]*pgDECTORAD) * (float)cos(dir[1]*pgDECTORAD),
						   (float)sin(dir[1]*pgDECTORAD),				
						   (float)cos(dir[0]*pgDECTORAD) * (float)cos(dir[1]*pgDECTORAD) ),
			
			swd = pgVec3( -fwd.z, 
						   0.0f,
						   fwd.x ),

			uwd = pgVec3(  fwd.x * fwd.y,
						  -fwd.x*fwd.x - fwd.z*fwd.z, 
						   fwd.y * fwd.z );

	swd.normalize();
	uwd.normalize();

	fwd*=spectateForwardSpeed * speedUp;
	swd*=spectateStrafeSpeed  * speedUp;
	uwd*=spectateStrafeSpeed  * speedUp;
		  


	
	
	if( nInput.isButtonDown(pgIInput::BUTTON_LEFT) || nInput.isKeyDown(spectateKeyFwd) ) 
	{
		pos.x += 10.0f * fwd.x * dt;
		pos.y += 10.0f * fwd.y * dt;
		pos.z += 10.0f * fwd.z * dt;
	}

	if( nInput.isButtonDown(pgIInput::BUTTON_RIGHT) || nInput.isKeyDown(spectateKeyBack) )  
	{
		pos.x -= 10.0f * fwd.x * dt;
		pos.y -= 10.0f * fwd.y * dt;
		pos.z -= 10.0f * fwd.z * dt;
	}

	if(nInput.isKeyDown(keyRight)) 
	{
		pos.x -= swd.x * dt;
		pos.y -= swd.y * dt;
		pos.z -= swd.z * dt;
	}

	if(nInput.isKeyDown(keyLeft)) 
	{
		pos.x += swd.x * dt;
		pos.y += swd.y * dt;
		pos.z += swd.z * dt;
	}

	if(nInput.isKeyDown(keyUp))
	{
		pos.x -= uwd.x * dt;
		pos.y -= uwd.y * dt;
		pos.z -= uwd.z * dt;
	}

	if(nInput.isKeyDown(keyDown)) 
	{
		pos.x += uwd.x * dt;
		pos.y += uwd.y * dt;
		pos.z += uwd.z * dt;
	}
}


void
pgSteering::updateModeFly(const pgIInput& nInput)
{
	float	dt = pgITime::getLastFrameTime();
	bool	lShiftDown = nInput.isKeyDown(pgIInput::KEY_LSHIFT);
	pgVec3	d;


	// head
	//
	if(nInput.isKeyDown(pgIInput::KEY_RIGHT))
		flyHeadSpeed += flyDHeadSpeed * dt;
	else if(nInput.isKeyDown(pgIInput::KEY_LEFT))
		flyHeadSpeed -= flyDHeadSpeed * dt;
	else
	{
		flyHeadSpeed *= 0.98f;
		if(fabs(flyHeadSpeed)<2.0)
			flyHeadSpeed = 0.0f;
	}

	if(flyHeadSpeed>flyMaxHeadSpeed)
		flyHeadSpeed = flyMaxHeadSpeed;

	if(flyHeadSpeed<-flyMaxHeadSpeed)
		flyHeadSpeed = -flyMaxHeadSpeed;

	dir[0] -= flyHeadSpeed * dt * 0.8f;
	dir[2] = flyHeadSpeed / 5.0f;

	while(dir[0]>=360.0f)	dir[0] -= 360.0f;
	while(dir[0]<0.0f)		dir[0] += 360.0f;


	// pitch
	//
	if(nInput.isKeyDown(pgIInput::KEY_DOWN))
		flyPitchSpeed += flyDPitchSpeed * dt;
	else if(nInput.isKeyDown(pgIInput::KEY_UP))
		flyPitchSpeed -= flyDPitchSpeed * dt;
	else
	{
		flyPitchSpeed *= 0.98f;
		flyPitchSpeed *= 0.99f;
		if(fabs(flyPitchSpeed)<2.0)
			flyPitchSpeed = 0.0f;
	}

	if(flyPitchSpeed>flyMaxPitchSpeed)
		flyPitchSpeed = flyMaxPitchSpeed;

	if(flyPitchSpeed<-flyMaxPitchSpeed)
		flyPitchSpeed = -flyMaxPitchSpeed;


	dir[1] += flyPitchSpeed * dt * 0.8f;

	if(dir[1]>flyMaxPitch)
		dir[1] = flyMaxPitch;

	if(dir[1]<-flyMaxPitch)
		dir[1] = -flyMaxPitch;


	// movement
	//
	float dz = (float)cos(dir[0]*3.1415/180.0) * (lShiftDown ? flySpeedupFactor : 1.0f),
		  dx = -(float)sin(dir[0]*3.1415/180.0) * (lShiftDown ? flySpeedupFactor : 1.0f),
		  dy = 1.5f*(float)sin(dir[1]*3.1415/180.0) * (lShiftDown ? flySpeedupFactor : 1.0f);


	if(nInput.isKeyDown(pgIInput::KEY_SPACE))
		flyForwardSpeed += flyDForwardSpeed * dt;
	else
	{
		flyForwardSpeed *= 0.98f;
		if(flyForwardSpeed<2.0)
			flyForwardSpeed = 0.0f;
	}

	if(flyForwardSpeed>flyMaxForwardSpeed)
		flyForwardSpeed = flyMaxForwardSpeed;

	pos.x += flyForwardSpeed * dx * dt;
	pos.y += flyForwardSpeed * dy * dt;
	pos.z += flyForwardSpeed * dz * dt;
}


void
pgSteering::initModeRotate()
{
	pgVec3 d(-(float)sin(dir[0]*3.1415/180.0),
		      (float)sin(dir[1]*3.1415/180.0),
			  (float)cos(dir[0]*3.1415/180.0));

	rotatePos = pos + d * rotateLength;
}


void
pgSteering::updateModeRotate(const pgIInput& nInput)
{
	float	dt = pgITime::getLastFrameTime();
	bool	lShiftDown = nInput.isKeyDown(pgIInput::KEY_LSHIFT);
	pgVec3	d;



	// changing rotation radius
	//
	if(nInput.isKeyDown(pgIInput::KEY_NUMPADPLUS))
	{
		calcDirectionVector(dir[0], dir[1], d);

		rotateLength += dt * rotateLength * 0.5f;
		rotatePos = pos + d * rotateLength;
	}

	if(nInput.isKeyDown(pgIInput::KEY_NUMPADMINUS))
	{
		calcDirectionVector(dir[0], dir[1], d);

		rotateLength -= dt * rotateLength * 0.5f;
		if(rotateLength<0.0f)
			rotateLength = 0.0f;
		rotatePos = pos + d * rotateLength;
	}

	// rotation
	//
	dir[0] += rotateMouseXFactor * nInput.getMouseX();
	dir[1] += rotateMouseYFactor * nInput.getMouseY();

	if(nInput.isKeyDown(pgIInput::KEY_RIGHT))
		dir[0] -= rotateRotSpeed * dt * (lShiftDown ? rotateSpeedupFactor : 1.0f);

	if(nInput.isKeyDown(pgIInput::KEY_LEFT))
		dir[0] += rotateRotSpeed * dt * (lShiftDown ? rotateSpeedupFactor : 1.0f);

	if(nInput.isKeyDown(pgIInput::KEY_DOWN))
		dir[1] += rotateRotSpeed * dt * (lShiftDown ? rotateSpeedupFactor/2.0f : 1.0f);

	if(nInput.isKeyDown(pgIInput::KEY_UP))
		dir[1] -= rotateRotSpeed * dt * (lShiftDown ? rotateSpeedupFactor/2.0f : 1.0f);

	while(dir[0]>=360.0f)	dir[0] -= 360.0f;
	while(dir[0]<0.0f)		dir[0] += 360.0f;
	if(dir[1]>= 89.0f)		dir[1] =  89.0f;
	if(dir[1]<=-89.0f)		dir[1] = -89.0f;


	float dz = (float)cos(dir[0]*3.1415/180.0) * (lShiftDown ? rotateSpeedupFactor : 1.0f),
		  dx = -(float)sin(dir[0]*3.1415/180.0) * (lShiftDown ? rotateSpeedupFactor : 1.0f);


	if(nInput.isKeyDown(pgIInput::KEY_S))
		rotatePos.y += rotateUpSpeed * dt * (lShiftDown ? rotateSpeedupFactor : 1.0f);

	if(nInput.isKeyDown(pgIInput::KEY_X))
		rotatePos.y -= rotateUpSpeed * dt * (lShiftDown ? rotateSpeedupFactor : 1.0f);

	if(nInput.isKeyDown(pgIInput::KEY_A))
	{
		rotatePos.x += rotateForwardSpeed * dx * dt;
		rotatePos.z += rotateForwardSpeed * dz * dt;
	}

	if(nInput.isKeyDown(pgIInput::KEY_Z))
	{
		rotatePos.x -= rotateForwardSpeed * dx * dt;
		rotatePos.z -= rotateForwardSpeed * dz * dt;
	}

	if(nInput.isKeyDown(pgIInput::KEY_W))
	{
		rotatePos.x += rotateStrafeSpeed * dz * dt;
		rotatePos.z -= rotateStrafeSpeed * dx * dt;
	}

	if(nInput.isKeyDown(pgIInput::KEY_Q))
	{
		rotatePos.x -= rotateStrafeSpeed * dz * dt;
		rotatePos.z += rotateStrafeSpeed * dx * dt;
	}


	calcDirectionVector(dir[0], dir[1], d);


	pos = rotatePos - d * rotateLength;
}


void
pgSteering::calcDirectionVectorRad(float nHeadR, float nPitchR, pgVec3& nDir)
{
	nDir[0] = -(float)sin(nHeadR);
	nDir[1] =  (float)sin(nPitchR);
	nDir[2] =  (float)cos(nHeadR);

	nDir.normalize();
}


void
pgSteering::calcDirectionVector(float nHead, float nPitch, pgVec3& nDir)
{
	nDir[0] = -(float)sin(nHead*3.1415/180.0);
	nDir[1] =  (float)sin(nPitch*3.1415/180.0);
	nDir[2] =  (float)cos(nHead*3.1415/180.0);

	nDir.normalize();
}


void
pgSteering::formatPositionString(pgString& nString)
{
	nString.set("Position: %.2f %.2f %.2f", pos.x,pos.y,pos.z);
}


void
pgSteering::formatDirectionString(pgString& nString)
{
	nString.set("Direction: %.2f %.2f %.2f", dir[0],dir[1],dir[2]);
}


void
pgSteering::formatModeString(pgString& nString)
{
	switch(mode)
	{
	case MODE_INSPECT:
		nString = "Mode: INSPECT";
		break;

	case MODE_FLY:
		nString = "Mode: FLY";
		break;

	case MODE_ROTATE:
		nString = "Mode: ROTATE";
		break;

	case MODE_SPECTATE:
		nString = "Mode: SPECTATE";
		break;
	}
}


void
pgSteering::setKey(pgIInput::KEY& nKey, const pgString& nVal)
{
	pgIInput::KEY key = pgIInput::getKeyFromString(nVal);

	if(key==pgIInput::KEY_UNDEFINED)
		pgLog::error("undefined input key: '%s'", nVal);
	else
		nKey = key;
}


void
pgSteering::setup(const pgSettingsFile& nSet)
{
	pgString str;
	bool bVal;
	float fVal;

	if(nSet.getValueYes("InvertPitch", bVal))
		invertPitch = bVal ? -1.0f : 1.0f;

	if(nSet.getValueFloat("mousefactor_x", fVal))
		mouseXFactor = fVal;

	if(nSet.getValueFloat("mousefactor_y", fVal))
		mouseYFactor = fVal;


	if(nSet.getValueString("move_forward", str))
		setKey(inspectKeyForward, str);

	if(nSet.getValueString("move_back", str))
		setKey(inspectKeyBack, str);

	if(nSet.getValueString("move_left", str))
		setKey(inspectKeyLeft, str);

	if(nSet.getValueString("move_right", str))
		setKey(inspectKeyRight, str);

	if(nSet.getValueString("move_up", str))
		setKey(inspectKeyUp, str);

	if(nSet.getValueString("move_down", str))
		setKey(inspectKeyDown, str);

	if(nSet.getValueString("rotate_left", str))
		setKey(keyLeft, str);

	if(nSet.getValueString("rotate_right", str))
		setKey(keyRight, str);

	if(nSet.getValueString("rotate_up", str))
		setKey(keyUp, str);

	if(nSet.getValueString("rotate_down", str))
		setKey(keyDown, str);


	if(nSet.getValueString("singlestep", str))
		setKey(inspectKeySingleStep, str);

	if(nSet.getValueFloat("speedup_factor", fVal))
		inspectSpeedupFactor = fVal;

	if(nSet.getValueString("toggle_speedup", str))
		setKey(keyToggleSpeedup, str);

	if(nSet.getValueString("toggle_wireframe", str))
		setKey(keyToggleWireframe, str);

	if(nSet.getValueString("toggle_vislock", str))
		setKey(keyToggleFixVis, str);

	if(nSet.getValueString("toggle_steeringmode", str))
		setKey(keyToggleSteeringMode, str);

	if(nSet.getValueString("bench_play", str))
		setKey(benchKeyStart, str);

	if(nSet.getValueString("bench_stop", str))
		setKey(benchKeyStop, str);

	if(nSet.getValueString("bench_recordstart", str))
		setKey(benchKeyWriteStart, str);

	if(nSet.getValueString("bench_recordstop", str))
		setKey(benchKeyWriteEnd, str);

}

