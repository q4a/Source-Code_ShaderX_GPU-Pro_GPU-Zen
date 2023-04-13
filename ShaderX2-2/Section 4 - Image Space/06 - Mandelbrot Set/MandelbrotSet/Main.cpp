#define USING_DIRECT3D

#include "../Framework/Framework.h"

class MainApp : public Direct3DApp {
protected:
	TextureID spectrum;
	ShaderID fragmentShader;	

	float left, right, top, bottom;

public:
	bool checkBaseCapabilities();

	void selectPixelFormat(PixelFormat &pixelFormat);

	bool init();
	bool load();

	void controls();

	bool drawFrame();
};

bool MainApp::checkBaseCapabilities(){
	if (caps.PixelShaderVersion < D3DPS_VERSION(1,0) || caps.VertexShaderVersion < D3DVS_VERSION(2,0)){
		String error("This demo requires at least pixel shader 2.0.\n"
					 "Your card and driver only supports pixel shader ");
		error.sprintf("%d.%d", (caps.PixelShaderVersion  >> 8) & 255, caps.PixelShaderVersion  & 255);
		error += ".\n\nThis demo will now exit.";

		ALERT(error, "Error");
		return false;
	}

	return true;
}

void MainApp::controls(){
	float zoomX = (right - left) * time;
	float zoomY = (bottom - top) * time;

	if (keys[upKey]){
		top    -= zoomY;
		bottom -= zoomY;
	}

	if (keys[downKey]){
		top    += zoomY;
		bottom += zoomY;
	}

	if (keys[leftKey]){
		left  -= zoomX;
		right -= zoomX;
	}

	if (keys[rightKey]){
		left  += zoomX;
		right += zoomX;
	}
	
	if (keys[forwardsKey]){
		left   += zoomX;
		right  -= zoomX;
		top    += zoomY;
		bottom -= zoomY;
	}

	if (keys[backwardsKey]){
		left   -= zoomX;
		right  += zoomX;
		top    -= zoomY;
		bottom += zoomY;
	}

	if (keys[resetKey]){
		left   = -1.85f;
		right  =  0.5f;
		top    = -1.2f;
		bottom =  1.2f;
	}
}

void MainApp::selectPixelFormat(PixelFormat &pixelFormat){
	pixelFormat.redBits   = 8;
	pixelFormat.greenBits = 8;
	pixelFormat.blueBits  = 8;
	pixelFormat.alphaBits = 0;

	pixelFormat.depthBits = 0;
}

bool MainApp::init(){
	projection.loadIdentity();
	modelView.loadIdentity();
	return true;
}

bool MainApp::load(){
	fragmentShader = renderer->addFragmentShader(0, "mandel.fsh");

	spectrum    = renderer->addTexture("../Textures/Spectrum2.tga", TEX_CLAMP);
	textTexture = renderer->addTexture("../Textures/font2.dds");

	left   = -1.85f;
	right  =  0.5f;
	top    = -1.2f;
	bottom =  1.2f;

	return true;
}

bool MainApp::drawFrame(){
	dev->SetTransform(D3DTS_VIEW,       (D3DMATRIX *) (const float *) modelView);
	dev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) (const float *) projection);

	dev->SetFVF(D3DFVF_XYZ | (1 << D3DFVF_TEXCOUNT_SHIFT) | D3DFVF_TEXCOORDSIZE2(0));

	renderer->setFragmentShader(fragmentShader);
	renderer->setTextures(spectrum);
	renderer->apply();

	struct {
		float x,y,z;
		float u, v;
	} qv[4];

	qv[0].x = -1; qv[0].y =  1; qv[0].z = 0; qv[0].u = left;  qv[0].v = top;
	qv[1].x =  1; qv[1].y =  1; qv[1].z = 0; qv[1].u = right; qv[1].v = top;
	qv[2].x =  1; qv[2].y = -1; qv[2].z = 0; qv[2].u = right; qv[2].v = bottom;
	qv[3].x = -1; qv[3].y = -1; qv[3].z = 0; qv[3].u = left;  qv[3].v = bottom;

	dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, qv, 5 * sizeof(float));

	return true;
}

DECLARE_MAIN_APP(MainApp)
