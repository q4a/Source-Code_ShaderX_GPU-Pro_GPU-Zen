#define USING_DIRECT3D

#include "../Framework/Framework.h"
#include "../Framework/Util/T3dLoader.h"
#include "../Framework/Util/DemoLooper.h"

struct TexVertex {
	Vertex vertex;
	float s, t;
	Vertex uVec, vVec, normal;
};

D3DVERTEXELEMENT9 texVertexFormat[] = {
	{ 0, 0,                                      D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{ 0, 1 * sizeof(Vertex),                     D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{ 0, 1 * sizeof(Vertex) + 2 * sizeof(float), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
	{ 0, 2 * sizeof(Vertex) + 2 * sizeof(float), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
	{ 0, 3 * sizeof(Vertex) + 2 * sizeof(float), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
	D3DDECL_END()
};

struct State {
	Material *material;
	unsigned int indexIndex, nIndices;
};

struct Light {
	Vertex pos;
	float atten;
	DemoLooper demoLooper;
};

/* ---------------------------------------------------------- */

class MainApp : public Direct3DApp {
protected:
	TextureID tex0, tex1, tex2, tex3, tex4;
	TextureID tex0Bump, tex1Bump, tex2Bump, tex3Bump, tex4Bump;
	TextureID tex0Gloss, tex1Gloss, tex2Gloss, tex3Gloss, tex4Gloss;
	TextureID lightTex, renderTexture;

	ShaderID vertexShader, fragmentShader;	
	ShaderID vDist, fDist;

    MaterialTable matTable;

	Set <State> states;

	Light lights[2];

	TexVertex *vertices;
	unsigned int *indices;
	unsigned int nVertices, nIndices;

	LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	LPDIRECT3DINDEXBUFFER9 indexBuffer;
	LPDIRECT3DVERTEXDECLARATION9 vDecl;

public:
	bool checkBaseCapabilities();

	void resetCamera();
	void selectPixelFormat(PixelFormat &pixelFormat);

	bool init();
	bool load();
	bool unload();

	void drawDepth();
	void drawLit();
	void drawLights();

	bool drawFrame();
};

bool MainApp::checkBaseCapabilities(){

	if (caps.PixelShaderVersion < D3DPS_VERSION(2,0) || caps.VertexShaderVersion < D3DVS_VERSION(2,0)){
		String error("This demo requires at least pixel shader 2.0 and vertex shader 2.0.\n"
					 "Your card and driver only supports pixel shader ");
		error.sprintf("%d.%d", (caps.PixelShaderVersion  >> 8) & 255, caps.PixelShaderVersion  & 255);
		error += " and vertex shader ";
		error.sprintf("%d.%d", (caps.VertexShaderVersion >> 8) & 255, caps.VertexShaderVersion & 255);
		error += "\n\nThis demo will now exit.";

		ALERT(error, "Error");
		return false;
	}

	return true;
}

void MainApp::resetCamera(){
	position = Vertex(580, 250, 410);
	Wx = 0.441f;
	Wy = 2.348f;
	Wz = 0;
}

void MainApp::selectPixelFormat(PixelFormat &pixelFormat){
	pixelFormat.redBits   = 8;
	pixelFormat.greenBits = 8;
	pixelFormat.blueBits  = 8;
	pixelFormat.alphaBits = 8;

	pixelFormat.depthBits = 24;
}

bool MainApp::init(){
	projection.loadProjectionMatrix(PI / 2, 0.75f, 2, 3000);
	return true;
}

bool polygonFilter(const int flags){
	return (flags & PF_Invisible) == 0 && (flags & PF_Translucent) == 0 && (flags >= 0);
}

int comp(class Polygon *const &elem0, class Polygon *const &elem1){
	return (elem0->getMaterial() - elem1->getMaterial());
}

bool MainApp::load(){
	renderTexture = renderer->addRenderTexture(512, 512, false, true, RPF_R32f);
	if (renderTexture == TEXTURE_NONE){
		ALERT("Could not create R32f render target", "Error");
		return false;
	}

	vertexShader   = renderer->addVertexShader("lighting.vsh");
	fragmentShader = renderer->addFragmentShader(0, "lighting.fsh");

	vDist = renderer->addVertexShader("distance.vsh");
	fDist = renderer->addFragmentShader(0, "distance.fsh");

	tex0      = renderer->addTexture("../Textures/Wood.dds");
	tex0Bump  = renderer->addTexture("../Textures/WoodBump.png", TEX_NORMALMAP);
	tex0Gloss = renderer->addTexture("../Textures/WoodGloss.png");

	tex1      = renderer->addTexture("../Textures/hexpave2.dds");
	tex1Bump  = renderer->addTexture("../Textures/hexpave2Bump.png", TEX_NORMALMAP);
	tex1Gloss = renderer->addTexture("../Textures/hexpave2Gloss.png");

	tex2      = renderer->addTexture("../Textures/wooddecoration1.dds");
	tex2Bump  = renderer->addTexture("../Textures/wooddecoration1Bump.png", TEX_NORMALMAP);
	tex2Gloss = renderer->addTexture("../Textures/wooddecoration1Gloss.png");

	tex3      = renderer->addTexture("../Textures/16tile01.dds");
	tex3Bump  = renderer->addTexture("../Textures/16tile01Bump.png", TEX_NORMALMAP);
	tex3Gloss = renderer->addTexture("../Textures/16tile01Gloss.png");

	tex4      = renderer->addTexture("../Textures/Wooddoor2.dds");
	tex4Bump  = renderer->addTexture("../Textures/Wooddoor2Bump.png", TEX_NORMALMAP);
	tex4Gloss = renderer->addTexture("../Textures/Wooddoor2Gloss.png");


	lightTex = renderer->addTexture("../Textures/Corona2.png");
	textTexture = renderer->addTexture("../Textures/font2.dds");

	Set <class Polygon *> polys;

	Material *mat;

	mat = new Material();
	mat->setTextures(tex0, tex0Bump, tex0Gloss);
    matTable.addMaterialName("Boards1", 256, 256, mat, 0);

	mat = new Material();
	mat->setTextures(tex1, tex1Bump, tex1Gloss);
    matTable.addMaterialName("C_flr13", 256, 256, mat, 0);

	mat = new Material();
	mat->setTextures(tex2, tex2Bump, tex2Gloss);
    matTable.addMaterialName("HIWALL1B", 128, 128, mat, 0);

	mat = new Material();
	mat->setTextures(tex3, tex3Bump, tex3Gloss);
    matTable.addMaterialName("Aceilin1", 256, 256, mat, 0);

	mat = new Material();
	mat->setTextures(tex4, tex4Bump, tex4Gloss);
    matTable.addMaterialName("Door2b", 256, 256, mat, 0);

	T3dLoader t3dLoader;
	t3dLoader.loadFromFile("map.t3d", polys, matTable, polyCreator, polygonFilter, Vertex(0,0,0));

	fixTJunctions(polys);
	polys.sort(comp);

	unsigned int i, j, len = polys.getSize();
	
	nVertices = 0;
	for (i = 0; i < len; i++){
		nVertices += polys[i]->getnVertices();
	}
	nIndices = 3 * nVertices - 6 * len;

	vertices = new TexVertex[nVertices];
	indices = new unsigned int[nIndices];

	unsigned int *iDest = indices;
	unsigned int fvai, vai = 0;
	unsigned int last = 0, ni = 0;

	mat = polys[0]->getMaterial();
	i = 0;
	do {
		if (i == len || mat != polys[i]->getMaterial()){
			State s;
			s.material = mat;
			s.indexIndex = last;
			s.nIndices = ni - last;
			states.add(s);

			if (i == len) break;

			last = ni;
			mat = polys[i]->getMaterial();
		}

		fvai = vai;
		for (j = 0; j < polys[i]->getnVertices(); j++){
			if (j > 2){
				*iDest++ = fvai;
				*iDest++ = vai - 1;
			}
			*iDest++ = vai;

			vertices[vai].vertex = polys[i]->getVertex(j);
			vertices[vai].s = (polys[i]->getVertex(j) - polys[i]->getOrigin()) * (polys[i]->getS() * polys[i]->getScaleS());
			vertices[vai].t = (polys[i]->getVertex(j) - polys[i]->getOrigin()) * (polys[i]->getT() * polys[i]->getScaleT());
			vertices[vai].uVec = polys[i]->getS();
			vertices[vai].vVec = polys[i]->getT();
			vertices[vai].normal = polys[i]->getPlane().normal;

			vai++;
		}
		ni += 3 * (polys[i]->getnVertices() - 2);
		
		delete polys[i];

		i++;
	} while (true);

	void *dest;
	dev->CreateVertexBuffer(nVertices * sizeof(TexVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &vertexBuffer, NULL);
	vertexBuffer->Lock(0, 0, &dest, 0);
	memcpy(dest, vertices, nVertices * sizeof(TexVertex));
	vertexBuffer->Unlock();

	dev->CreateIndexBuffer(nIndices * sizeof(unsigned int), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &indexBuffer, NULL);
	indexBuffer->Lock(0, 0, &dest, 0);
	memcpy(dest, indices, nIndices * sizeof(unsigned int));
	indexBuffer->Unlock();

	dev->CreateVertexDeclaration(texVertexFormat, &vDecl);


	lights[0].demoLooper.loadFromFile("Path0.pth");
	lights[0].demoLooper.setClampMode(false);
	lights[0].atten = 0.000005f;
	lights[1].demoLooper.loadFromFile("Path1.pth");
	lights[1].demoLooper.setClampMode(false);
	lights[1].atten = 0.000005f;

	return true;
}

bool MainApp::unload(){
	vDecl->Release();

	delete indices;
	delete vertices;

	indexBuffer->Release();
	vertexBuffer->Release();

	matTable.clear();
	states.clear();		

	return true;
}

void MainApp::drawDepth(){
	renderer->setVertexShader(vDist);
	renderer->setFragmentShader(fDist);
	renderer->apply();

	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVertices, 0, nIndices / 3);
}

void MainApp::drawLit(){
	dev->SetVertexDeclaration(vDecl);
	dev->SetIndices(indexBuffer);
	dev->SetStreamSource(0, vertexBuffer, 0, sizeof(TexVertex));

	for (unsigned int i = 0; i < sizeof(lights) / sizeof(Light); i++){

		dev->SetVertexShaderConstantF(5, lights[i].pos, 1);

		renderer->changeRenderTarget(renderTexture);

		Matrix proj, mv;
		proj.loadProjectionMatrix(PI / 2, 1.0f, 1.0f, 3000.0f);

		for (unsigned int k = 0; k < 6; k++){
			renderer->changeRenderFace(renderTexture, k);

			dev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0);

			mv.loadCubemapMatrix(k);
			mv.translate(-lights[i].pos);

			dev->SetVertexShaderConstantF(0, transpose(proj * mv), 4);

			drawDepth();
		}

		renderer->changeRenderTarget();

		dev->SetVertexShaderConstantF(0, transpose(projection * modelView), 4);
		dev->SetVertexShaderConstantF(4, position, 1);

		float lightConstants[] = { 1.0f, lights[i].atten, (i == 0)? 0.23f : 0.0f, 0.0f };
		dev->SetPixelShaderConstantF(2, lightConstants, 1);


		for (unsigned j = 0; j < states.getSize(); j++){
			renderer->setTextures(states[j].material->textures[0], states[j].material->textures[1], states[j].material->textures[2], renderTexture);
			renderer->setVertexShader(vertexShader);
			renderer->setFragmentShader(fragmentShader);
			if (i > 0) renderer->setBlending(D3DBLEND_ONE, D3DBLEND_ONE);
			renderer->apply();

			dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVertices, states[j].indexIndex, states[j].nIndices / 3);
		}
	}
}

struct qVertex {
	Vertex vertex;
	float u, v;
};

void MainApp::drawLights(){
	dev->SetFVF(D3DFVF_XYZ | (1 << D3DFVF_TEXCOUNT_SHIFT) | D3DFVF_TEXCOORDSIZE2(0));
	renderer->setTextures(lightTex);
	renderer->setBlending(D3DBLEND_ONE, D3DBLEND_ONE);
	renderer->apply();

	dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	qVertex qv[4];

	qv[0].u = 0; qv[0].v = 0;
	qv[1].u = 1; qv[1].v = 0;
	qv[2].u = 1; qv[2].v = 1;
	qv[3].u = 0; qv[3].v = 1;

	Vertex dx(modelView[0], modelView[4], modelView[8]);
	Vertex dy(modelView[1], modelView[5], modelView[9]);
	for (unsigned i = 0; i < sizeof(lights) / sizeof(Light); i++){
		qv[0].vertex = lights[i].pos + 30 * (-dx + dy);
		qv[1].vertex = lights[i].pos + 30 * ( dx + dy);
		qv[2].vertex = lights[i].pos + 30 * ( dx - dy);
		qv[3].vertex = lights[i].pos + 30 * (-dx - dy);

		dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, qv, sizeof(Vertex) + 2 * sizeof(float));
	}

	dev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}

bool MainApp::drawFrame(){
    dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0);

	dev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	dev->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	modelView.loadRotateZXY(-Wx, -Wy, -Wz);
	modelView.translate(-position);
	
	dev->SetTransform(D3DTS_VIEW,       (D3DMATRIX *) (const float *) modelView);
	dev->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) (const float *) projection);


	for (unsigned int i = 0; i < sizeof(lights) / sizeof(Light); i++){
		LoopNode node = lights[i].demoLooper.getNode(currTime / 2);
		lights[i].pos = Vertex(node.x, node.y, node.z);
	}




	dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	drawLit();
	drawLights();

	return true;
}

DECLARE_MAIN_APP(MainApp)
