float heightRoad = 586.0f; //folosit in MoveRoads
const int numRoadSegments = 5;
float roadYPos[numRoadSegments] = { yMax - 3 * heightRoad, yMax - 2 * heightRoad, yMax - heightRoad, yMax, yMax + heightRoad }; // sunt 5 drumuri una legata de cealalta
float speed = 600.0f; //de aici se poate schimba viteza
float offset = 0.0f;

void RoadPoints(void) {
	static const GLfloat road[] = {
		//coordonate                     coordonate texturare
		-418.0f, 0.0f,    0.0f, 1.0f,    0.0f, 0.0f,
		418.0f,  0.0f,    0.0f, 1.0f,    1.0f, 0.0f,
		418.0f,  586.0f,  0.0f, 1.0f,    1.0f, 1.0f,
		-418.0f, 586.0f,  0.0f, 1.0f,    0.0f, 1.0f,
	};

	static const GLuint RoadIndices[] =
	{
		0, 1, 2,
		3, 0, 2
	};
	glGenVertexArrays(1, &VaoId2);
	glBindVertexArray(VaoId2);

	glGenBuffers(1, &VboId2);																//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId2);													//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(road), road, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	glGenBuffers(1, &EboId2);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RoadIndices), RoadIndices, GL_STATIC_DRAW);
}
//aici folosim scalare pentru micsorarea dimensiunii strazii, un for care trece roadYPos care se afla mai sus in acest fisier, si offsetul care este scazut la fiecare frame
void DrawRoad(void) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, roadTexture);
	//desenarea drumului
	glBindVertexArray(VaoId2);

	matrScale1 = glm::scale(glm::mat4(1.0f), glm::vec3(0.6, 0.6, 0.0));

	for (int i = 0; i < numRoadSegments; i++) {
		glm::mat4 roadTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, roadYPos[i] + offset, 0.0f));
		myMatrix = resizeMatrix * matrScale1 * roadTransform;
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
	}
}

//functie folosita pentru a masica drumul folosind un offset (se duce inapoi de unde a inceput fluent)
//acel glutTimerFunc ne lasa sa controlam mai bine frameurile (in acest exemplu, o data la 16ms se randeaza imaginea)
//deltaTime ajuta tot la acest aspect de randare pe frameuri.

void MoveRoad(int value) {
	float deltaTime = 0.016f;
	offset -= speed * deltaTime;

	// Reset offset if it exceeds the height of a road segment
	if (offset <= -heightRoad) {
		offset += heightRoad;
	}
	glutPostRedisplay();
	glutTimerFunc(16, MoveRoad, 0);
}