//HLSL code for npatch in a vertex shader

// enough for a cube (12 triangles)
#define NUM_BASE_TRIANGLE 12

vector VertexPos[3 * NUM_BASE_TRIANGLE];
vector VertexNorm[3 * NUM_BASE_TRIANGLE];
matrix MVP;

void generateControlPoints(	// input
						float3 v0, float3 v1, float3 v2, 
						float3 n0, float3 n1, float3 n2,
						// output
						out float3 b300, out float3 b030, out float3 b003,
						out float3 b210, out float3 b120, 
						out float3 b021, out float3 b012,
						out float3 b102, out float3 b201,
						out float3 b111,
						out float3 n200, out float3 n020, out float3 n002 )
{
	float w01,w10,w12,w21,w20,w02;
	float x,y,z;
	float E,V;

	const float rcp3 = 1.f / 3.f;
	const float rcp6 = 1.f / 6.f;
	const float rcp2 = 1.f / 2.f;
	const float rcp4 = 1.f / 4.f;

	b300 = v0; 
 	b030 = v1; 
	b003 = v2;
	
	//		  v1 6
	//			3 7
	//		   1 4 8
	//     v2 0 2 5 9 v3

	//                   /' b300 = v0
	//                  /  '
	//                 /    '
	//            b210/      'b201
	//               /        '
	//              /          '
	//         b120/     b111   'b102
	//            /              '
	//           /                '
	// v1 = b030/__________________'b003 = v2
	//               b021    b012
	
	float3 edge, tangent1, tangent2;
	float tmpf;
	
    /***********/
    /* Edge #1 */
    /***********/
	
	edge = v1 - v0;
	// E - (E.N)N
	tangent1 = edge;
	tmpf = dot( tangent1, n0 );
	tangent1 -= n0 * tmpf;
	b210 = v0 + (tangent1 * rcp3);

    // E - (E.N)N
	tangent2 = edge;
	tmpf = dot( tangent2, n1 );
	tangent2 -= n1 * tmpf;
	b120 = v1 - (tangent2 * rcp3);

	/***********/
	/* Edge #2 */
	/***********/

	edge = v2 - v1;
	//E - (E.N)N
	tangent1 = edge;
	tmpf = dot( tangent1, n1);
	tangent1 -= n1 * tmpf;
	b021 = v1 + (tangent1 * rcp3);

	//E - (E.N)N
	tangent2 = edge;
	tmpf = dot( tangent2, n2);
	tangent2 -= n2 * tmpf;
	b012 = v2 - (tangent2 * rcp3);

	/***********/
	/* Edge #3 */
	/***********/

	edge = v2 - v0;
	//E - (E.N)N
    tangent1 = edge;
    tmpf = dot( tangent1 , n0);
    tangent1 -= n0 * tmpf;
    b201 = v0 + (tangent1 * rcp3);

	//E - (E.N)N
    tangent2 = edge;
    tmpf = dot( tangent2 , n2);
    tangent2 -= n2 * tmpf;
    b102 = v2 - (tangent2 * rcp3);
    
    // Farin's Method
    b111 = (b120 + b210 + b021 + b012 + b201 + b102) * rcp4;
    b111 -= (b300 + b030 + b003) * rcp6; 

	// normal control points
	n200 = n0; 
	n020 = n1; 
	n002 = n2; 
	
	// quadratic normal code can go here if you choose to implement them
}

void evaluateNPatchLinearNormal( // input
								float i, float j, 
								float3 b300, float3 b030, float3 b003,
								float3 b210, float3 b120, 
								float3 b021, float3 b012,
								float3 b102, float3 b201,
								float3 b111,
								float3 n200, float3 n020, float3 n002,
								// output
								out float3 pos, out float3 norm )
{
	float k = 1 - i - j;
	float k2 = k * k;
	float k3 = k2 * k;
	float i2 = i * i;
	float i3 = i2 * i;
	float j2 = j * j;
	float j3 = j2 * j;

	// bicubic position
	pos =	(b300*i3) +
			(b210*3*i2*j) + (b201*3*i2*k)  + 
			(b120*3*i*j2) + (b111*6*i*j*k) + (b102*3*i*k2) +
			(b030*j3)     + (b021*3*j2*k)  + (b012*3*j*k2) + (b003*k3);

	// linear normal
	norm = (i * n200) + (j * n020) + (k * n002);
	
	norm = normalize( norm );

}

void evaluateBezierPatchLinearNormal( // input
								float i, float j, 
								float3 b300, float3 b030, float3 b003,
								float3 b210, float3 b120, 
								float3 b021, float3 b012,
								float3 b102, float3 b201,
								float3 b111,
								float3 n200, float3 n020, float3 n002,
								// output
								out float3 pos, out float3 norm )
{
	float k = 1 - i - j;
	
	float b1 = i;
	float b2 = j;
	float b3 = k;
	
	float3 buff[6];

	// 6 = b300
	// 3 = b210, 7 = b201
	// 1 = b120, 4 = b111, 8 = b102
	// 0 = b030, 2 = b021, 5 = b012, 9 = b003

    buff[0] = b030*b2 + b120*b1 + b021*b3; // 0 1 2
    buff[1] = b120*b2 + b210*b1 + b111*b3; // 1 3 4
    buff[2] = b021*b2 + b111*b1 + b012*b3; // 2 4 5
    buff[3] = b210*b2 + b300*b1 + b201*b3; // 3 6 7
    buff[4] = b111*b2 + b201*b1 + b102*b3; // 4 7 8
    buff[5] = b012*b2 + b102*b1 + b003*b3; // 5 8 9

	buff[0] = buff[0]*b2 + buff[1]*b1 + buff[2]*b3; // 0 1 2
	buff[1] = buff[1]*b2 + buff[3]*b1 + buff[4]*b3; // 2 3 4
	buff[2] = buff[2]*b2 + buff[4]*b1 + buff[5]*b3; // 3 4 5

    pos = buff[0]*b2 + buff[1]*b1 + buff[2]*b3;

	// linear normal
	norm = (i * n200) + (j * n020) + (k * n002);
	
	norm = normalize( norm );

}


struct VS_OUT
{
	float4 pos : POSITION;
	float4 col : COLOR;
};

VS_OUT mainVS( float3 vertexStream : POSITION0  ) 
{
	VS_OUT outp;
	
	float i =  vertexStream.x;
	float j =  vertexStream.y;
	float baseIndex =  vertexStream.z * 256;

	// position and normals of this triangle
	float3 v0 =	VertexPos[ (baseIndex * 3) + 0 ];
	float3 v1 = VertexPos[ (baseIndex * 3) + 1 ];
	float3 v2 = VertexPos[ (baseIndex * 3) + 2 ];
	float3 n0 = VertexNorm[ (baseIndex * 3) + 0 ];
	float3 n1 = VertexNorm[ (baseIndex * 3) + 1 ];
	float3 n2 = VertexNorm[ (baseIndex * 3) + 2 ];

	// control points
	float3 b300, b030, b003;
	float3 b210, b120;
	float3 b021, b012;
	float3 b102, b201;
	float3 b111;
	float3 n200, n020, n002;
	
	generateControlPoints( v0, v1, v2, n0, n1, n2,
							b300, b030, b003,
							b210, b120,
							b021, b012,
							b102, b201,
							b111,
							n200, n020, n002 );

	// resultant position and normal	
	float3 pos, norm;
	evaluateNPatchLinearNormal( i, j, 
								b300, b030, b003,
								b210, b120,
								b021, b012,
								b102, b201,
								b111,
								n200, n020, n002,
								pos, norm );
	//Alternative form of the evaluation function
/*	evaluateBezierPatchLinearNormal( i, j, 
								b300, b030, b003,
								b210, b120,
								b021, b012,
								b102, b201,
								b111,
								n200, n020, n002,
								pos, norm );*/

	// transform position
	outp.pos = mul( float4(pos,1), MVP );
	outp.col = float4( (norm*0.5)+0.5,1);
//	outp.col = vector(i,j,1-i-j,1);

	return outp;
}

technique T0
{
	pass P0
	{ 
		VertexShader = compile vs_1_1 mainVS();
	}
}
