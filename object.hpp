#ifndef _HNODE_HPP_
#define _HNODE_HPP_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "gl_framework.hpp"
#include "glm/ext.hpp"

using namespace std;

// A simple class that represents an object with textrures
class sceneObject {
	private:
	string path;
    std::vector < glm::vec4 > vertices;
    std::vector < glm::vec2 > uvs;
    std::vector < glm::vec4 > normals;
    //std::vector < glm::vec3 > faces;

    GLuint vao,vbo;
	public:

	sceneObject();
    void readOBJ(string filename);

    void render();
    void translate(float x,float y, float z);
    bool intersects(glm::vec3 currpos,glm::vec3 nextpos, glm::vec3& normal);
    void updateBuffers();

};

glm::mat4* multiply_stack(std::vector <glm::mat4> );

#endif
	

