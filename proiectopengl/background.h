

void BackgroundPoints(void) {
	static const GLfloat background[] = {
		//coordonate                     coordonate texturare
		-612.0f, 0.0f,    0.0f, 1.0f,    0.0f, 0.0f,
		612.0f,  0.0f,    0.0f, 1.0f,    1.0f, 0.0f,
		612.0f,  612.0f,  0.0f, 1.0f,    1.0f, 1.0f,
		-612.0f, 612.0f,  0.0f, 1.0f,    0.0f, 1.0f,
	};

	static const GLuint BackgroundIndices[] =
	{
		0, 1, 2,
		3, 0, 2
	};
	glGenVertexArrays(1, &VaoId3);
	glBindVertexArray(VaoId3);

	glGenBuffers(1, &VboId3);																//  Generarea bufferului si indexarea acestuia catre variabila VboId;
	glBindBuffer(GL_ARRAY_BUFFER, VboId3);													//  Setarea tipului de buffer - atributele varfurilor;
	glBufferData(GL_ARRAY_BUFFER, sizeof(background), background, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));

	glGenBuffers(1, &EboId3);														//  Generarea bufferului si indexarea acestuia catre variabila EboId;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BackgroundIndices), BackgroundIndices, GL_STATIC_DRAW);
}


void DrawBackground(void) {

	float scaleX = (xMax - xMin - 600) / 612.0f; 
	float scaleY = (yMax - yMin) / 612.0f;

	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, yMin, 0.0f));
	matrScale1 = glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, 1.0));
	// Bind cu texturi si cu VBO, EBO
	glBindVertexArray(VaoId3);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, backgroundTexture);
	glUniform1i(glGetUniformLocation(ProgramId, "CarTexture"), 0);

	myMatrix = resizeMatrix * translationMatrix * matrScale1;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));
}