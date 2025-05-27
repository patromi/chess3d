/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "myCube.h"
#include "myTeapot.h"
#include "myBoard.h"

#include "Model.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

float speed_x = 0;
float speed_y = 0;
float aspectRatio = 1;
ShaderProgram* sp;
ShaderProgram* spTextured;
float* texCoords = myTeapotTexCoords;
float* colors = myTeapotColors;
int vertexCount = myTeapotVertexCount;
GLuint tex0;
GLuint tex1; //uchwyt na teksturę
struct ChessPiece {
	std::string name;       // np. "Pawn", "King"
	glm::vec3 position;     // pozycja na planszy
};

//std::vector<ChessPiece> whitePieces = {
//	{"Rook",   {-8.3f, 1.2f, -11.5f}},
//	{"Knight", {-1.0f, 1.2f, -7.0f}},
//	{"Bishop", {-3.0f, 1.2f, -7.0f}},
//	{"Queen",  {-1.0f, 1.2f, -7.0f}},
//	{"King",   { 1.0f, 1.2f, -7.0f}},
//	{"Bishop", { 3.0f, 1.2f, -7.0f}},
//	{"Knight", { 5.0f, 1.2f, -7.0f}},
//	{"Rook",   { 7.0f, 1.2f, -7.0f}},
//	{"Pawn",   {-7.0f, 1.2f, -5.0f}}, {"Pawn", {-5.0f, 1.2f, -5.0f}},
//	{"Pawn",   {-3.0f, 1.2f, -5.0f}}, {"Pawn", {-1.0f, 1.2f, -5.0f}},
//	{"Pawn",   { 1.0f, 1.2f, -5.0f}}, {"Pawn", { 3.0f, 1.2f, -5.0f}},
//	{"Pawn",   { 5.0f, 1.2f, -5.0f}}, {"Pawn", { 7.0f, 1.2f, -5.0f}},
//};
//std::vector<ChessPiece> blackPieces = {
//	{"Rook",   {-8.3f, 1.2f, 3.3f}},
//	{"Knight", {-5.0f, 1.2f, 7.0f}},
//	{"Bishop", {-3.0f, 1.2f, 7.0f}},
//	{"Queen",  {-1.0f, 1.2f, 7.0f}},
//	{"King",   { 1.0f, 1.2f, 7.0f}},
//	{"Bishop", { 3.0f, 1.2f, 7.0f}},
//	{"Knight", { 5.0f, 1.2f, 7.0f}},
//	{"Rook",   { 7.0f, 1.2f, 7.0f}},
//	{"Pawn",   {-7.0f, 1.2f, 5.0f}}, {"Pawn", {-5.0f, 1.2f, 5.0f}},
//	{"Pawn",   {-3.0f, 1.2f, 5.0f}}, {"Pawn", {-1.0f, 1.2f, 5.0f}},
//	{"Pawn",   { 1.0f, 1.2f, 5.0f}}, {"Pawn", { 3.0f, 1.2f, 5.0f}},
//	{"Pawn",   { 5.0f, 1.2f, 5.0f}}, {"Pawn", { 7.0f, 1.2f, 5.0f}},
//};

std::unordered_map<std::string, std::string> pieceMeshMap = {
	{"Pawn", "pawn"},
	{"Rook", "Cylinder"},       // rook.obj → ostatni mesh
	{"Knight", "knight"},
	{"Bishop", "BezierCircle_001"}, // jedyny mesh
	{"Queen", "Cylinder"},      // queen.obj → mesh 4
	{"King", "Cube_002"}        // king.obj → mesh 3
};
Model ChessModel;
std::vector<ChessPiece> whitePieces;
std::vector<ChessPiece> blackPieces;

std::vector<std::string> rowOrder = {
	"Rook", "Knight", "Bishop", "Queen", "King", "Bishop", "Knight", "Rook"
};

float startX = -8.3f;
float deltaX = 2.09f;
float pieceY = 1.2f;

float whiteZ = -11.5f;
float whitePawnZ = whiteZ + 2.2f;

float blackZ = 11.5f-8.2f;
float blackPawnZ = blackZ - 2.2f;

GLuint readTexture(const char* filename) {
	GLuint tex;
	std::vector<unsigned char> image; 
	unsigned width, height;
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);
	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return tex;
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
		if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
		if (key == GLFW_KEY_UP) speed_y = PI / 2;
		if (key == GLFW_KEY_DOWN) speed_y = -PI / 2;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_x = 0;
		if (key == GLFW_KEY_RIGHT) speed_x = 0;
		if (key == GLFW_KEY_UP) speed_y = 0;
		if (key == GLFW_KEY_DOWN) speed_y = 0;
	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	
	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	spTextured = new ShaderProgram("v_textured.glsl", NULL, "f_textured.glsl");
	//sp = new ShaderProgram("v_constant.glsl", NULL, "f_constant.glsl");

	tex0 = readTexture("cell-0.png");
	tex1 = readTexture("cell-1.png");


	ChessModel.loadModel("bishop.obj");
	ChessModel.loadModel("pawn.obj");
	ChessModel.loadModel("rook.obj");
	ChessModel.loadModel("king.obj");
	ChessModel.loadModel("queen.obj");
	ChessModel.loadModel("knight.obj");

	for (int i = 0; i < 8; ++i) {
		float x = startX + i * deltaX;
		whitePieces.push_back({ rowOrder[i], {x, pieceY, whiteZ} });
		whitePieces.push_back({ "Pawn",      {x, pieceY, whitePawnZ} });

		blackPieces.push_back({ rowOrder[i], {x, pieceY, blackZ} });
		blackPieces.push_back({ "Pawn",      {x, pieceY, blackPawnZ} });
	}
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************

	delete sp;

}


float boardBorderVertsLeft[] = {
	// Lewa krawędź
	5.0f, 0.0f, -6.0f, 1.0f,
	5.0f, 0.5f, -6.0f, 1.0f,
	5.0f, 0.5f, -5.0f, 1.0f,
	5.0f, 0.0f, -6.0f, 1.0f,
	5.0f, 0.5f, -5.0f, 1.0f,
	5.0f, 0.0f, -5.0f, 1.0f

};


void drawBoard(glm::mat4 M, glm::mat4 V, glm::mat4 P, float angle_x, float angle_y) {
	spTextured->use();
	glUniformMatrix4fv(spTextured->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spTextured->u("V"), 1, false, glm::value_ptr(V));

	// Atrybuty
	glEnableVertexAttribArray(spTextured->a("vertex"));
	glVertexAttribPointer(spTextured->a("vertex"), 4, GL_FLOAT, false, 0, boardVerts);

	glEnableVertexAttribArray(spTextured->a("texCoord"));
	glVertexAttribPointer(spTextured->a("texCoord"), 2, GL_FLOAT, false, 0, myBoardTexCoords); // <- musisz zadeklarować texCoords[]

	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 4; i++) {
			// Macierz i pozycjonowanie
			M = glm::mat4(1.0f);
			M = glm::scale(M, glm::vec3(0.3f, 0.3f, 0.3f));
			M = glm::rotate(M, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
			M = glm::rotate(M, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
			M = glm::translate(M, glm::vec3(i * -2.0f, 0.0f, j * 2.0f));

			// Brązowa kostka → tex0
			glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(M));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex0); // <- upewnij się, że tex0 jest zainicjalizowany
			glUniform1i(spTextured->u("texSampler"), 0); // tex0 = jednostka 0
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// Biała kostka → tex1
			M = glm::translate(M, glm::vec3(-1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(M));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex1); // <- tex1 = biała tekstura
			glUniform1i(spTextured->u("texSampler"), 0);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		for (int i = 0; i < 4; i++) {
			M = glm::mat4(1.0f);
			M = glm::scale(M, glm::vec3(0.3f, 0.3f, 0.3f));
			M = glm::rotate(M, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
			M = glm::rotate(M, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
			M = glm::translate(M, glm::vec3(i * -2.0f, 0.0f, j * 2.0f + 1));

			// Pierwsza biała → tex1
			glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(M));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex1);
			glUniform1i(spTextured->u("texSampler"), 0);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// Druga brązowa → tex0
			M = glm::translate(M, glm::vec3(-1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(M));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex0);
			glUniform1i(spTextured->u("texSampler"), 0);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	glDisableVertexAttribArray(spTextured->a("vertex"));
	glDisableVertexAttribArray(spTextured->a("texCoord"));

	sp->use();
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

	for (const auto& piece : whitePieces) {
		std::string meshName = pieceMeshMap[piece.name];  // znajdź mesh

		glm::mat4 pieceModel = glm::mat4(1.0f);
		pieceModel = glm::scale(pieceModel, glm::vec3(0.145f));
		pieceModel = glm::rotate(pieceModel, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
		pieceModel = glm::rotate(pieceModel, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
		pieceModel = glm::translate(pieceModel, piece.position);

		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(pieceModel));
		ChessModel.drawMeshByName(meshName, sp);
	}
	for (const auto& piece : blackPieces) {
		std::string meshName = pieceMeshMap[piece.name];  // znajdź mesh

		glm::mat4 pieceModel = glm::mat4(1.0f);
		pieceModel = glm::scale(pieceModel, glm::vec3(0.145f));
		pieceModel = glm::rotate(pieceModel, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
		pieceModel = glm::rotate(pieceModel, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
		pieceModel = glm::translate(pieceModel, piece.position);

		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(pieceModel));
		ChessModel.drawMeshByName(meshName, sp);
	}

}

void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 M = glm::mat4(1.0f);
	//M = glm::scale(M, glm::vec3(0.2f, 0.2f, 0.2f));
	M = glm::rotate(M, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::rotate(M, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));

	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 3.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f);

	drawBoard(M, V, P, angle_x, angle_y);


	glfwSwapBuffers(window);
}




int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(900, 900, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące



	//Główna pętla
	float angle_x = 0; //Aktualny kąt obrotu obiektu
	float angle_y = 0; //Aktualny kąt obrotu obiektu
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		angle_x += speed_x * glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		angle_y += speed_y * glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		glfwSetTime(0); //Zeruj timer
		drawScene(window, angle_x, angle_y); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
