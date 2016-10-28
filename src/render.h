#ifndef RENDER_H
#define RENDER_H
#include <GL/glew.h>
#include <glm/glm.hpp>

class Render {
public:
	static bool init(const GLfloat bufferData[], int size);

	static void renderTri(int start, int length, glm::mat4 MVP, glm::vec3 color);
	static void renderLine(int start, int length, glm::mat4 MVP, glm::vec3 color);
private:
	static GLuint programID; 
	static GLuint MatrixID;
	static GLuint ColorID;
	static GLuint vao;
	static GLuint vbo;
};

#endif
