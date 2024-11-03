float heightCar = 1000.0f; //folosit in MoveCar
float speed1 = 600.0f; //de aici se poate schimba viteza
float offset1 = -400.0f;

struct BoundingBox {
	float xMin;
	float xMax;
	float yMin;
	float yMax;
};

BoundingBox GetCarBoundingBox(float tx, float ty, float scale, float width, float height) {
	BoundingBox box;
	box.xMin = tx;
	box.xMax = tx + width * scale;
	box.yMin = ty - height * scale;
	box.yMax = ty;
	return box;
}

bool CheckCollision(const BoundingBox& box1, const BoundingBox& box2) {
	return (box1.xMin < box2.xMax && box1.xMax > box2.xMin &&
		box1.yMin < box2.yMax && box1.yMax > box2.yMin);
}

void CheckAndHandleCollisions() {
	BoundingBox car1 = GetCarBoundingBox(350.0f, -191.0f, 0.45, 207.0f, 388.0f);
	BoundingBox car2 = GetCarBoundingBox(tx, ty, 0.45, 207.0f, 388.0f);
	BoundingBox car3 = GetCarBoundingBox(-300.0f, offset1, 0.45, 207.0f, 388.0f);

	if (CheckCollision(car1, car2) || CheckCollision(car1, car3) || CheckCollision(car2, car3)) {
		// Handle collision between any two cars
		//slowingDown = true; // Start slowing down the road
		elapsedTime = 0.0f; // Reset the slowdown timer
		speed1 = 0;
		speed = 0;
	}
}



void ProcessSpecialKeys(int key, int xx, int yy)
{
	switch (key)			//	Procesarea tastelor 'LEFT', 'RIGHT', 'UP', 'DOWN'
	{						//	duce la deplasarea patratului pe axele Ox si Oy;
	case GLUT_KEY_LEFT:
		tx -= 10;
		ty += 5;
		if (angle < 0.2f)angle += 0.2f;	//	Rotire spre stanga;
		break;
	case GLUT_KEY_RIGHT:
		tx += 10;
		ty += 5;
		if (angle > -0.2f)angle -= 0.2f;
		break;
	case GLUT_KEY_UP:
		ty += 10;
		angle = 0;
		break;
	case GLUT_KEY_DOWN:
		ty -= 10;
		angle = 0;
		break;
	}
}

void CarPoints(void) {

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
	glGenVertexArrays(1, &VaoId1);                                                   //  Generarea VAO si indexarea acestuia catre variabila VaoId;
	glBindVertexArray(VaoId1);

	//  Se creeaza un buffer comun pentru VARFURI - COORDONATE si CULORI;
	glGenBuffers(1, &VboId1);																//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId1);													//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(cars), cars, GL_STATIC_DRAW);

	//	Se activeaza lucrul cu atribute;
//  Se asociaza atributul (0 = coordonate) pentru shader;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//  Se asociaza atributul (1 =  coordonate texturare) pentru shader;
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	//	Se creeaza un buffer pentru INDICI;
	glGenBuffers(1, &EboId1);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CarIndices), CarIndices, GL_STATIC_DRAW);

}
// aici folosim o scalare pentru a micsora masinile, si translate pentru a le muta pe banda din dreapta
// carTexture provine din Initialize(void) in main
void DrawCars(void) {
	matrScale1 = glm::scale(glm::mat4(1.0f), glm::vec3(0.45, 0.45, 0.0));
	matrRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));
	matrRot2 = glm::rotate(glm::mat4(1.0f), angle1, glm::vec3(0.0, 0.0, 1.0));
	// Bind cu texturi si cu VBO, EBO
	glBindVertexArray(VaoId1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, carTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "CarTexture"), 0);

	//masina de sus
	matrTransl = glm::translate(glm::mat4(1.0f), glm::vec3(350.0, -30.0, 0.0));
	myMatrix = resizeMatrix * matrScale1 * matrTransl;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));


	//masina de jos
	glm::mat4 matrTransl2 = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, 0.0));
	myMatrix = resizeMatrix * matrScale1 * matrTransl2 * matrRot;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));

	//masina de pe banda opusa
		glm::mat4 matrTransl3 = glm::translate(glm::mat4(1.0f), glm::vec3(-300.0, offset1, 0.0));
		myMatrix = resizeMatrix * matrScale1 * matrTransl3 * matrRot2;
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));
}



void MoveCar(int value) {
	float deltaTime = 0.016f;
	offset1 -= speed1 * deltaTime;

	SlowdownRoad(deltaTime);

	// Check for collision
	CheckAndHandleCollisions();

	// Reset offset if it exceeds the height of a road segment
	if (offset1 <= -heightCar - 200.0f) {
		offset1 += heightCar*5;
	}
	glutPostRedisplay();
	glutTimerFunc(16, MoveCar, 0);
}

