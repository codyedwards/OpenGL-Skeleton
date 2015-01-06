/* 
	Simple skeleton program that pops open an sdl
	window and draws a triangle, good to use so you don't have 
	to repeat boilerplate crud
*/


// Headers
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <math.h>

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

// Global OpenGL variables
GLuint vao;
GLuint vbo;
GLuint ebo;

// Global SDL variables
SDL_Window* window;
SDL_GLContext context;

// Shader sources
GLchar* vertexFile = "./vertex.vshader";
GLchar* fragFile = "./frag.fshader";

// Shaders
const GLchar* vertexSource;
const GLchar* fragmentSource;
GLuint shaderProgram;

// OpenGL color
GLint uniColor;

// Vertices
GLfloat vertices[] =
{// Position    //Colour           //Texcoords
	-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,// Top-left
	0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,// Top-right
	0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
	-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f// Bottom-left
};

// Elements
GLuint elements[] =
{
	0, 1, 2,
	2, 3, 0
};

// Textures
GLuint textures[2];
char* kitty = "./content/sample.png";
char* puppy = "./content/sample2.png";


char* filetobuf(char *file)
{
	FILE *fptr;
	long length;
	char *buf;
	errno_t err;

	err = fopen_s(&fptr, file, "rb"); /* Open file for reading */
	if (!fptr) /* Return NULL on failure */
		return NULL;
	fseek(fptr, 0, SEEK_END); /* Seek to the end of the file */
	length = ftell(fptr); /* Find out how many bytes into the file we are */
	buf = (char*)malloc(length + 1); /* Allocate a buffer for the entire length of the file and a null terminator */
	fseek(fptr, 0, SEEK_SET); /* Go back to the beginning of the file */
	fread(buf, length, 1, fptr); /* Read the contents of the file in to the buffer */
	fclose(fptr); /* Close the file */
	buf[length] = 0; /* Null terminator */

	return buf; /* Return the buffer */
}

// initilize SDL, basically create the window
void initSDL()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);

	context = SDL_GL_CreateContext(window);
}

// Initialize GLEW, the magic that ties everything together
void initGLEW()
{
	glewExperimental = GL_TRUE;
	glewInit();
}

// Initialize Vertex Array Objects
void initVAOs()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}

// Initialize Vertex Buffer Objects, copy vertex data to it
void initVBOs()
{
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void initEBOs()
{
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
}

// Initialize shaders, bind them
void initShaders()
{
	// Read in shaders
	vertexSource = filetobuf(vertexFile);
	fragmentSource = filetobuf(fragFile);

	// Create and compile the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// Create and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	// Link the vertex and fragment shader into a shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
	
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);

	printf("%u\n", vertexBuffer);
}

// Initialize the Textures
void initTextures()
{
	glGenTextures(2, textures);
	
	SDL_Surface* surface;

	// load in Kitty
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	surface = IMG_Load(kitty);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	SDL_FreeSurface(surface);
	glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	surface = IMG_Load(puppy);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	SDL_FreeSurface(surface);
	glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// Draw stuff
void display()
{
	SDL_Event windowEvent;
	
	// Set up projection
	glm::mat4 view = glm::lookAt(
		glm::vec3(1.2f, 1.2f, 1.2f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
		);
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 10.0f);
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT) break;
		}

		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Calculate transformation
		glm::mat4 model;
		model = glm::rotate(model, (float)clock() / (float)CLOCKS_PER_SEC * 3.1415f, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::vec4 result = model * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		printf("%f,, %f, %f\n", result.x, result.y, result.z);

		GLint uniTrans = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(model));

		// draw
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Swap buffers
		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(context);
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	initSDL();
	initGLEW();
	initVAOs();
	initVBOs();
	initEBOs();
	initShaders();
	initTextures();
	display();

	return 0;
}