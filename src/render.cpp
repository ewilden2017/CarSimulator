#include "render.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "shader.h"
#include <stdio.h>

GLuint Render::programID = 0;
GLuint Render::MatrixID = 0;
GLuint Render::ColorID = 0;
GLuint Render::vao = 0;
GLuint Render::vbo = 0;

bool Render::init(const GLfloat bufferData[], int size) {
    programID = LoadShaders("SimpleTransform.vertexshader", "SingleColor.fragmentshader");
	glUseProgram(programID);

    MatrixID = glGetUniformLocation(programID, "MVP");
    ColorID = glGetUniformLocation(programID, "inColor");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, bufferData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	return true;
}

void Render::renderTri(int start, int length, glm::mat4 MVP, glm::vec3 color) {
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniform3fv(ColorID, 1, &color[0]);

    glDrawArrays(GL_TRIANGLES, start, length); // 3 indices starting at 0 -> 1 triangle
}

void Render::renderLine(int start, int length, glm::mat4 MVP, glm::vec3 color) {
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniform3fv(ColorID, 1, &color[0]);

    glDrawArrays(GL_LINES, start, length); // 3 indices starting at 0 -> 1 triangle
}
