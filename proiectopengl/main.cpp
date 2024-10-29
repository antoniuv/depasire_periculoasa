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
VaoId,
VboId,
EboId,
ProgramId,
viewLocation,
projLocation,
myMatrixLocation;

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
float xMin = -800.f, xMax = 800, yMin = -600, yMax = 600;
glm::mat4 projection;
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
	//  Coordonatele varfurilor;
	static const GLfloat cars[] =
	{
		//coordonate                      coordonate texturare
		-207.0f,  0.0f,   0.0f, 1.0f,    0.0f, 0.0f,
		 0.0f,    0.0f,   0.0f, 1.0f,    1.0f, 0.0f,
		 0.0f,    388.0f, 0.0f, 1.0f,    1.0f, 1.0f,
		-207.0f,  388.0f, 0.0f, 1.0f,    0.0f, 1.0f,
	};


	//	Indicii care determina ordinea de parcurgere a varfurilor;
	static const GLuint CarIndices[] =
	{
		//prima masina
		0, 1, 2,
		3, 0, 2, 
	};

	//  Transmiterea datelor prin buffere;

	//  Se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO;
	glGenVertexArrays(1, &VaoId);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId);

	//  Se creeaza un buffer comun pentru VARFURI - COORDONATE si CULORI;
	glGenBuffers(1, &VboId);																//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId);													//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(cars), cars, GL_STATIC_DRAW);


	//	Se creeaza un buffer pentru INDICI;
	glGenBuffers(1, &EboId);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CarIndices), CarIndices, GL_STATIC_DRAW);

	//	Se activeaza lucrul cu atribute;
//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//  Se asociaza atributul (1 =  coordonate texturare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
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
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &EboId);

	//  Eliberaea obiectelor de tip VAO;
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
}

//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void)
{
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
	LoadTexture("formula_1_car.png");

	CreateShaders();							//  Initilizarea shaderelor;
	//	Instantierea variabilelor uniforme pentru a "comunica" cu shaderele;
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

}

//  Functia de desenarea a graficii pe ecran;
void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT);			//  Se curata ecranul OpenGL pentru a fi desenat noul continut;

	resizeMatrix = glm::ortho(xMin, xMax, yMin, yMax);
	// Activarea / legarea texturii active
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(ProgramId, "CarTexture"), 0);

	//masina de sus
	matrScale1 = glm::scale(glm::mat4(1.0f), glm::vec3(0.4, 0.4, 0.0));
	matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(100.0, -30.0, 0.0));
	myMatrix = resizeMatrix * matrScale1 * matrTransl;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));


	//masina de jos
	glm::mat4 matrTransl2 = glm::translate(glm::mat4(1.0f), glm::vec3(100.0, -600.0, 0.0));
	myMatrix = resizeMatrix * matrScale1 * matrTransl2 ;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));

	glutSwapBuffers();
	glFlush();
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);	
	
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
	glutCloseFunc(Cleanup);				//  Eliberarea resurselor alocate de program;


	glutMainLoop();

	return 0;
}


