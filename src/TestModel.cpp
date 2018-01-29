#include "TestModel.h"

void LoadTestModel( std::vector<TestTriangle>& TestTriangles )
{
	using glm::vec3;

	// Defines colors:
	vec3 red(    0.75f, 0.15f, 0.15f );
	vec3 yellow( 0.75f, 0.75f, 0.15f );
	vec3 green(  0.15f, 0.75f, 0.15f );
	vec3 cyan(   0.15f, 0.75f, 0.75f );
	vec3 blue(   0.15f, 0.15f, 0.75f );
	vec3 purple( 0.75f, 0.15f, 0.75f );
	vec3 white(  0.75f, 0.75f, 0.75f );

	TestTriangles.clear();
	TestTriangles.reserve( 5*2*3 );

	// ---------------------------------------------------------------------------
	// Room

	float L = 555;			// Length of Cornell Box side.

	vec3 A(L,0,0);
	vec3 B(0,0,0);
	vec3 C(L,0,L);
	vec3 D(0,0,L);

	vec3 E(L,L,0);
	vec3 F(0,L,0);
	vec3 G(L,L,L);
	vec3 H(0,L,L);

	// Floor:
	TestTriangles.push_back( TestTriangle( C, B, A, green ) );
	TestTriangles.push_back( TestTriangle( C, D, B, green ) );

	// Left wall
	TestTriangles.push_back( TestTriangle( A, E, C, purple ) );
	TestTriangles.push_back( TestTriangle( C, E, G, purple ) );

	// Right wall
	TestTriangles.push_back( TestTriangle( F, B, D, yellow ) );
	TestTriangles.push_back( TestTriangle( H, F, D, yellow ) );

	// Ceiling
	TestTriangles.push_back( TestTriangle( E, F, G, cyan ) );
	TestTriangles.push_back( TestTriangle( F, H, G, cyan ) );

	// Back wall
	TestTriangles.push_back( TestTriangle( G, D, C, white ) );
	TestTriangles.push_back( TestTriangle( G, H, D, white ) );

	// ---------------------------------------------------------------------------
	// Short block

	A = vec3(290,0,114);
	B = vec3(130,0, 65);
	C = vec3(240,0,272);
	D = vec3( 82,0,225);

	E = vec3(290,165,114);
	F = vec3(130,165, 65);
	G = vec3(240,165,272);
	H = vec3( 82,165,225);

	// Front
	TestTriangles.push_back( TestTriangle(E,B,A,red) );
	TestTriangles.push_back( TestTriangle(E,F,B,red) );

	// Front
	TestTriangles.push_back( TestTriangle(F,D,B,red) );
	TestTriangles.push_back( TestTriangle(F,H,D,red) );

	// BACK
	TestTriangles.push_back( TestTriangle(H,C,D,red) );
	TestTriangles.push_back( TestTriangle(H,G,C,red) );

	// LEFT
	TestTriangles.push_back( TestTriangle(G,E,C,red) );
	TestTriangles.push_back( TestTriangle(E,A,C,red) );

	// TOP
	TestTriangles.push_back( TestTriangle(G,F,E,red) );
	TestTriangles.push_back( TestTriangle(G,H,F,red) );

	// ---------------------------------------------------------------------------
	// Tall block

	A = vec3(423,0,247);
	B = vec3(265,0,296);
	C = vec3(472,0,406);
	D = vec3(314,0,456);

	E = vec3(423,330,247);
	F = vec3(265,330,296);
	G = vec3(472,330,406);
	H = vec3(314,330,456);

	// Front
	TestTriangles.push_back( TestTriangle(E,B,A,blue) );
	TestTriangles.push_back( TestTriangle(E,F,B,blue) );

	// Front
	TestTriangles.push_back( TestTriangle(F,D,B,blue) );
	TestTriangles.push_back( TestTriangle(F,H,D,blue) );

	// BACK
	TestTriangles.push_back( TestTriangle(H,C,D,blue) );
	TestTriangles.push_back( TestTriangle(H,G,C,blue) );

	// LEFT
	TestTriangles.push_back( TestTriangle(G,E,C,blue) );
	TestTriangles.push_back( TestTriangle(E,A,C,blue) );

	// TOP
	TestTriangles.push_back( TestTriangle(G,F,E,blue) );
	TestTriangles.push_back( TestTriangle(G,H,F,blue) );


	// ----------------------------------------------
	// Scale to the volume [-1,1]^3

	for( size_t i=0; i<TestTriangles.size(); ++i )
	{
		TestTriangles[i].v0 *= 2/L;
		TestTriangles[i].v1 *= 2/L;
		TestTriangles[i].v2 *= 2/L;

		TestTriangles[i].v0 -= vec3(1,1,1);
		TestTriangles[i].v1 -= vec3(1,1,1);
		TestTriangles[i].v2 -= vec3(1,1,1);

		TestTriangles[i].v0.x *= -1;
		TestTriangles[i].v1.x *= -1;
		TestTriangles[i].v2.x *= -1;

		TestTriangles[i].v0.y *= -1;
		TestTriangles[i].v1.y *= -1;
		TestTriangles[i].v2.y *= -1;

		TestTriangles[i].ComputeNormal();
	}
}
