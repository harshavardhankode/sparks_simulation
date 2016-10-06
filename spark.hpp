#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "object.hpp"

using namespace std;


class spark{

    GLuint vao,vbo;
	vector<glm::vec4> spark_path;
	vector<glm::vec4> spark_color;

	int spark_length;
	float life,currlife;
	float vx,vy,vz;
	float x,y,z;

public:

	spark(float xpos, float ypos, float zpos, float maxlife, int max_length);
	spark(float xpos, float ypos, float zpos, float maxlife, int max_length, float avx, float avy, float avz);
	~spark();	

	void render();
	
	bool detect_ground_collisions();

	bool handle_object_collisions(glm::vec3 currpos,glm::vec3 nextpos,sceneObject* obj, glm::vec3& normal);

	void step(float timestep);
	bool dead();

};