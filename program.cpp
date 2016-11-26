#define GLEW_STATIC
#include<glew.h>
#include<glfw3.h>

#include"Shader.h"
#include"Mesh.h"
#include"Camera.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

void key_callback(GLFWwindow* window,int key,int scancode,int action,int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window,int button,int action,int mode);
void do_camera_movement();
void rotate(const GLfloat& x, const GLfloat& y);
void function(GLfloat& x, GLfloat& y, GLfloat& z, GLfloat& result);
void function1(GLfloat& x, GLfloat& y, GLfloat& z, GLfloat& result);
void function2(GLfloat& x, GLfloat&y, GLfloat& z, GLfloat& result);

GLuint WIDTH=800, HEIGHT=600;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLfloat xoffset=0.0f, yoffset=0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int keys[1024];

int mouses[3];

bool isDrawLine = true;

glm::vec3 rotateZ=glm::vec3(0.0f,0.0f,1.0f);
GLfloat rotateD=0.0f;

MeshLib::Mesh Mesh;

GLfloat bianchang = 0.3f;
GLfloat bianshu = 25.0f;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH,HEIGHT,"MarchingCube AND DOO-SABIN",nullptr,nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	glewInit();
	glGetError();

	glfwSetKeyCallback(window,key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);


	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PRIMITIVE_RESTART);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glLineWidth(1.0f);


	cout<<"CreateMesh£º£ºSTART::"<<glfwGetTime()<<endl;
			//Mesh.load_obj("Obj/mannequin.obj");
		//Mesh.createMeshByFunction(glm::vec3(0.0f), glm::vec3(25.0f), 0.3f, function1);
	Mesh.createMeshByFunction(glm::vec3(0.0f), glm::vec3(bianshu), bianchang, function1);
	//mesh.createMeshByFunction(glm::vec3(0.0f), glm::vec3(500.0f), 0.008, function2);
		//mesh.createMeshByFunction(glm::vec3(0.0f), glm::vec3(10.0f), 0.7f, function);
	cout << "CreateMesh£º£ºEND::" << glfwGetTime() << endl;

	cout << "UPDATEMesh::START::" << glfwGetTime() << endl;
		Mesh.update();
	cout << "UPDATEMesh::END::" << glfwGetTime() << endl;

	Shader Mshader=Shader("Shaders/VertexShader.glsl", "Shaders/FragmentShader.glsl");
	Shader Lshader = Shader("lineVShader.glsl", "lineFShader.glsl");

	Mesh.setupMesh();

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		do_camera_movement();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		Mshader.Use();

		GLint objectColorLoc = glGetUniformLocation(Mshader.shaderProgram, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(Mshader.shaderProgram, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(Mshader.shaderProgram, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(Mshader.shaderProgram, "viewPos");
		glUniform3f(objectColorLoc, 1.0f, 0.8f, 0.5f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, 0.0, 0.0f, 5.0f);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(Mshader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(Mshader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glm::mat4 model;
	//	model = glm::translate(model, -pos);
		model = glm::rotate(model,rotateD,rotateZ);
		model = glm::scale(model, glm::vec3(0.5f));
		glUniformMatrix4fv(glGetUniformLocation(Mshader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		Mesh.DrawModel(Mshader);

		if (isDrawLine){
			glDisable(GL_CULL_FACE);
			Lshader.Use();
			glUniformMatrix4fv(glGetUniformLocation(Lshader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(glGetUniformLocation(Lshader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		//	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
			glUniformMatrix4fv(glGetUniformLocation(Lshader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

			Mesh.DrawLine(Lshader);

		}
		
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = GL_TRUE;
		else if (action == GLFW_RELEASE)
			keys[key] = GL_FALSE;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		cout << "Doo_Sabin::START::" << glfwGetTime() << endl;
		Mesh = *Mesh.Doo_Sabin();
		cout << "Doo_Sabin::END::" << glfwGetTime() << endl;
		Mesh.update();
		Mesh.setupMesh();
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		isDrawLine = !isDrawLine;
	}
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (mouses[GLFW_MOUSE_BUTTON_RIGHT])
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}
		xoffset += (xpos - lastX);
		yoffset += (lastY - ypos);


		rotateD = sqrt(xoffset*xoffset + yoffset*yoffset)*0.01f;
		rotateZ = glm::normalize(glm::vec3(xoffset, yoffset, 0.0f));

		lastX = xpos;
		lastY = ypos;
	}
	else
	{

	}

//	camera.ProcessMouseMovement(xoffset, yoffset);
//	rotate(xoffset,yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	if (action == GLFW_PRESS)
		mouses[button] = GL_TRUE;
	else if (action == GLFW_RELEASE)
	{
		mouses[button] = GL_FALSE;
		firstMouse = true;
	}
}

void do_camera_movement()
{
	GLfloat cameraSpeed = 5.0f*deltaTime;
	if (keys[GLFW_KEY_W])
	{
		//cameraPos += cameraSpeed*cameraFrout;
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	//	cameraPos -= cameraSpeed*cameraFrout;
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	//	cameraPos -= glm::normalize(glm::cross(cameraFrout, cameraUp))*cameraSpeed;
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	//	cameraPos += glm::normalize(glm::cross(cameraFrout, cameraUp))*cameraSpeed;
	if (keys[GLFW_KEY_Q])
		camera.ProcessKeyboard(UP, deltaTime);
	if (keys[GLFW_KEY_E])
		camera.ProcessKeyboard(DOWN, deltaTime);
}

void rotate(const GLfloat& x, const GLfloat& y)
{
	if (mouses[GLFW_MOUSE_BUTTON_RIGHT])
	{
		rotateD = sqrt(x*x + y*y);
		rotateZ = glm::normalize(glm::vec3(x, y, 0.0f));
	}


}

void function(GLfloat& x,GLfloat& y,GLfloat& z,GLfloat& result){
	if (x == -9999.99f)
		x = sqrt(result + 9.0f - y*y - z*z);
	else if (y == -9999.99f)
		y = sqrt(result + 9.0f - x*x - z*z);
	else if (z == -9999.99f)
		z = sqrt(result + 9.0f - x*x - y*y);
	else
		result = x*x + y*y + z*z - 9.0f;

}

void function1(GLfloat& x, GLfloat& y, GLfloat& z, GLfloat& result){
	if (x == -9999.99f)
		x = sqrt(result + 1.0f - y*y + z*z);
	else if (y == -9999.99f)
		y = sqrt(result + 1.0f - x*x + z*z);
	else if (z == -9999.99f)
		z = sqrt(-result - 1.0f + x*x + y*y);
	else
		result = x*x + y*y - z*z - 1.0f;

}

void function2(GLfloat& x, GLfloat&y, GLfloat& z, GLfloat& result){

	result = pow((x *x+9/4*y *y + z *z - 1), 3) - x *x *z*z*z - (9 / 80)*y *y*z *z*z;

}