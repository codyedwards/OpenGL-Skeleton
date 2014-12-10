/* 
	Simple skeleton program that pops open an sdl
	window and draws a triangle, good to use so you don't have 
	to repeat boilerplate crude
*/


// Headers
#include <cstdio>
#include <cstdlib>


#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

using namespace std;

// Global OpenGL variables
GLuint vao;
GLuint vbo;

// Global SDL variables
SDL_Window* window;
SDL_GLContext context;

// Shader sources
GLchar* vertexFile = "./vertex.vshader";
GLchar* fragFile = "./frag.fshader";

// Shaders
const GLchar* vertexSource;
const GLchar* fragmentSource;

// Vertices
GLfloat vertices[] =
{
	0.0f, 0.5f,
	0.5f, -0.5f,
	-0.5f, -0.5f
};


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
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posAttrib);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);

	printf("%u\n", vertexBuffer);
}

// Draw stuff
void display()
{
	SDL_Event windowEvent;

	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT) break;
		}

		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw
		glDrawArrays(GL_TRIANGLES, 0, 3);

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
	initShaders();
	display();

	return 0;
}