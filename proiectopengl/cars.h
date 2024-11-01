
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
	glm::mat4 matrTransl2 = glm::translate(glm::mat4(1.0f), glm::vec3(350.0, -600.0, 0.0));
	myMatrix = resizeMatrix * matrScale1 * matrTransl2;
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0));
}