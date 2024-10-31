#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
							//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
							//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
							//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <SOIL.h>


//  Identificatorii obiectelor de tip OpenGL;
GLuint
VaoId1,
VboId1,
EboId1,
VaoId2,
VboId2,
EboId2,
VaoId3,
VboId3,
EboId3,
ProgramId,
viewLocation,
projLocation,
myMatrixLocation;

GLuint 
	roadTexture, carTexture, backgroundTexture;
GLuint
texture;
//	Dimensiunile ferestrei de afisare;
GLfloat
winWidth = 1200, winHeight = 1000;
//	Variabile catre matricile de transformare;
glm::mat4
myMatrix, resizeMatrix, matrScale1, matrTransl;

glm::mat4 view;
//	Elemente pentru matricea de proiectie;
float xMin = -600.f, xMax = 600, yMin = -600, yMax = 600;
glm::mat4 projection;

#include "road.h" //include toate proprietatile pentru randarea roadului
#include "cars.h" //include toate proprietatile pentru randarea masinilor
#include "background.h" //include toate proprietatile pentru randarea backgroundului (iarba)


//	Functia de incarcare a texturilor in program;
void LoadTexture(const char* texturePath)
{
	// Generarea unui obiect textura si legarea acestuia;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//	Desfasurarea imaginii pe orizontala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Modul in care structura de texeli este aplicata pe cea de pixeli;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Incarcarea texturii si transferul datelor in obiectul textura; 
	int width, height;
	unsigned char* image = SOIL_load_image(texturePath, &width, &height, 0, SOIL_LOAD_RGBA);
	if (image == nullptr) {
		printf("Failed to load texture: %s\n", texturePath);
		return;
	}
	else {
		printf("Texture loaded: %s \n", texturePath);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Eliberarea resurselor
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}
//  Crearea si compilarea obiectelor de tip shader;
//	Trebuie sa fie in acelasi director cu proiectul actual;
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei;
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
void CreateShaders(void)
{
	ProgramId = LoadShaders("depasire.vert", "depasire.frag");
	glUseProgram(ProgramId);
}

//  Se initializeaza un Vertex Buffer Object (VBO) pentru tranferul datelor spre memoria placii grafice (spre shadere);
//  In acesta se stocheaza date despre varfuri (coordonate, culori, indici, texturare etc.);
void CreateVBO(void)
{
	BackgroundPoints();
	CarPoints();
	RoadPoints();
}

//  Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

//  Eliminarea obiectelor de tip VBO dupa rulare;
void DestroyVBO(void)
{
	//  Eliberarea atributelor din shadere (pozitie, culoare, texturare etc.);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//  Stergerea bufferelor pentru VARFURI(Coordonate + Culori), INDICI;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId1);
	glDeleteBuffers(1, &EboId1);
	glDeleteBuffers(1, &VboId2);
	glDeleteBuffers(1, &EboId2);
	glDeleteBuffers(1, &VboId3);
	glDeleteBuffers(1, &EboId3);
	//  Eliberaea obiectelor de tip VAO;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId1);
	glDeleteVertexArrays(1, &VaoId2);
	glDeleteVertexArrays(1, &VaoId3);

}

//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void)
{
	glDeleteTextures(1, &texture); // Delete your texture
	glDeleteTextures(1, &roadTexture);
	glDeleteTextures(1, &carTexture);
	glDeleteTextures(1, &backgroundTexture);

	DestroyShaders();
	DestroyVBO();
}

//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);		//  Culoarea de fond a ecranului;
	
	CreateVBO();								//  Trecerea datelor de randare spre bufferul folosit de shadere;

	//	Incarcarea texturii;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	LoadTexture("background.jpg");
	backgroundTexture = texture;

	LoadTexture("road.jpg");
	roadTexture = texture; // Store road texture ID
	LoadTexture("formula_1_car.png");
	carTexture = texture; // Store car texture ID

	

	CreateShaders();							//  Initilizarea shaderelor;
	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);	//  Se curata ecranul OpenGL pentru a fi desenat noul continut;

	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
	DrawBackground();
	DrawRoad();
	DrawCars();

	glFlush();
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);	
	
	//aici am calculat dimensiunea ecranului pentru a pune aplicatia in centru
	int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
	int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

	int windowX = (screenWidth - winWidth) / 2;
	int windowY = (screenHeight - winHeight) / 2;

	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(windowX, windowY);
	glutCreateWindow("Depasire periculoasa");


	glewInit();

	Initialize();						//  Setarea parametrilor necesari pentru fereastra de vizualizare; 
	glutDisplayFunc(RenderFunction);	//  Desenarea scenei in fereastra;
	glutIdleFunc(RenderFunction);			//	Asigura rularea continua a randarii;
	glutTimerFunc(16, MoveRoad, 0); // Start the timer for smooth animation
	glutCloseFunc(Cleanup);				//  Eliberarea resurselor alocate de program;


	glutMainLoop();

	return 0;
}


