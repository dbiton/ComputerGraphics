#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include <string>

using namespace std;

class MeshModel : public Model
{
protected :
	MeshModel() {}
	vec3 *vertex_positions;
	//add more attributes
	mat4 _world_transform;
	mat3 _normal_transform;

	// Geometry musings, because I don't yet know how to work with the 4x4 transformation matrices or whatever...

	// 1.3: given an ordered triplet of 3D points A,B,C, the normal to the surface they identify is (B-A)x(C-B).
	// then you might want to normalize this vector so its length is 1... use functions from vec.h for this! also...:
	// TODO search all /*BUG*/ and fix! or at least get rid of the troublemakers... writing this like this so this shows up in visual studio's task list

	// 1.4: the edges of the bounding box are the minimum of every vertex in the model, and the maximum of every vertex,
	// respectively. then drawing a box from that will use every possible permutation on the coordinates...

	// 1.5: cubes and pyramids (and tetrahedrons, icosahedrons, etc if we want) should be simple enough to define, but spheres
	// (and maybe cylinders, paraboloids etc if we want) require us to settle on what LOD (Level Of Detail) to use when approximating
	// their perfect curvature... maybe implement the same shape but with multiple different LOD's? could be interesting
	// of course we only need at least one, and we should probably start with the cube

public:

	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void draw();
	
};
