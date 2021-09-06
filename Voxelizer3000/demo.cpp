#include <iostream>
#include "objLoader.h"
#include <math.h>
#include <GLUT/glut.h>
#include <vector>
#include "vertex.h"
#include <float.h>
#include <thread>
#include <mutex>
#include <string>

using namespace std;

GLfloat fAspect2;
GLfloat ang_cam = 60;

//define o tamanho máximo de voxels e o tamanho de cada um
int MODEL_INDEX = 32;
float MODEL_SIZE = (float)MODEL_INDEX;
float VOXEL_SIZE = 1/MODEL_SIZE;


float posX = 0.0f;
float posY = 0.5f;
float posZ = 0.0f;
float t = 0.0f;
float r = 1.0f;
float size = 0.0f;
bool rotaciona = true;

vector<vertex> posicoes;
static mutex barrier;

struct ray {
    vertex origin;
    vertex direction;
};

struct triangle{
    vertex V1;
    vertex V2;
    vertex V3;
};

//Algoritmo de Moller-Trumbore, testa a interseção do raio com o triangulo (adaptado da wikipedia)
int triangle_intersection( const vertex   V1,const vertex   V2, const vertex   V3,const vertex O, const vertex D,float* out )
{
    vertex e1, e2;  //Edge1, Edge2
    vertex P, Q, T;
    float det, inv_det, u, v;
    float t;
    
    //Find vectors for two edges sharing V1
    e1 = V2-V1;
    e2 = V3-V1;
    //    SUB(e1, V2, V1);
    //    SUB(e2, V3, V1);
    //Begin calculating determinant - also used to calculate u parameter
    P = vertex::cross(D, e2);
    //    CROSS(P, D, e2);
    //if determinant is near zero, ray lies in plane of triangle
    det = vertex::dot(e1, P);
	
    //NOT CULLING
    //if(det == 0.0f)
    //    return 0;
    //if(det > -FLT_EPSILON && det < FLT_EPSILON) return 0;
	
	if (fabs(det) < FLT_EPSILON) return 0;

    inv_det = 1.0f / det;
    
    //calculate distance from V1 to ray origin
    T = O - V1;
    //    SUB(T, O, V1);
    
    //Calculate u parameter and test bound
    u = vertex::dot(T, P) * inv_det;
    //The intersection lies outside of the triangle
    if(u < 0.f || u > 1.f) return 0;
    
    //Prepare to test v parameter
    Q = vertex::cross(T, e1);
    //    CROSS(Q, T, e1);
    
    //Calculate V parameter and test bound
    v = vertex::dot(D, Q) * inv_det;
    //The intersection lies outside of the triangle
    if(v < 0.f || u + v  > 1.f) return 0;
    
    t = vertex::dot(e2, Q) * inv_det;

	//limita o tamanho para garantir que está dentro do voxel desejado
    if(t > 0.0f && t < 1.0f){
        *out = t;
        return 1;
    }
    
    //if(t > FLT_EPSILON) { //ray intersection
    //    *out = t;
    //    return 1;
    //}
    
    // No hit, no win
    return 0;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLineWidth(3);
    for(int i = 0; i < posicoes.size() ; i++){
        glPushMatrix();
        glColor3f(0.8,0.0,0.0);
        glTranslatef((posicoes[i].x), (posicoes[i].y), (posicoes[i].z));
        glutSolidCube(VOXEL_SIZE -size);
        glColor3f(0.0,0.0,0.0);
        glutWireCube(VOXEL_SIZE);
        glPopMatrix();
    }

    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}

void ReshapeCallback(GLsizei w, GLsizei h) {
    // Para previnir uma divisão por zero
    if ( h == 0 ) h = 1;
    
    // Especifica as dimensõe da viewport
    glViewport(0, 0, w, h);
    
    // Calcula a correção de aspecto
    fAspect2 = (GLfloat)w/(GLfloat)h;
    
	glEnable(GL_LIGHTING);
    float AmbientColor[]    = { 0.5f, 0.5f, 0.5f, 1.0f };         glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
    float DiffuseColor[]    = { 0.5f, 0.5f, 0.5f, 1.0f };         glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
    float SpecularColor[]   = { 0.5f, 0.5f, 0.5f, 1.0f };         glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
    float Position[]        = { 0.0f, 2.0f, 0.0f, 1.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);
    glEnable(GL_LIGHT0);
}

void idle()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(ang_cam,fAspect2,0.1,2000);
    
    posX = r * cos(t);
    posZ = r* sin(t);

    if(rotaciona){
        if(t >= 360)
            t = 0.0f;

        t += 0.001;
    }
    
    gluLookAt (posX,posY,posZ,0.0,0.0,0.0, 0,1,0);
    

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutPostRedisplay();
}

void KeyboardCallback(unsigned char key, int x, int y)
{
    
    switch(key){
        case 27:
            exit(0);
            break;
        case 'q':
            size += 0.001f;
            break;
        case 'e':
            size -= 0.001f;
            break;
        case 'w':
            posY -= 0.1f;
            break;
        case 's':
            posY += 0.1f;
            break;
        case 'a':
            if(!rotaciona) t -= 0.1f;
            break;
        case 'd':
            if(!rotaciona) t += 0.1f;
            break;
		case 'z':
			r += 0.1f;
			break;
		case 'x':
			r -= 0.1f;
			break;
        case ' ':
            rotaciona = !rotaciona;
            break;
            
    };
    
    
}

//busca os vertices do triangulo de indice index, funcao criada para organizar melhor o codigo 
triangle getTriangle(objLoader *modelo,int index)
{

    triangle newTriangle;
    
    int verticeIndex1 = modelo->objectMesh.triangles[index].verts[0] -1;
    int verticeIndex2 = modelo->objectMesh.triangles[index].verts[1] -1;
    int verticeIndex3 = modelo->objectMesh.triangles[index].verts[2] -1;

    Vertex vertice1Original = modelo->objectMesh.vertices[verticeIndex1];
    Vertex vertice2Original = modelo->objectMesh.vertices[verticeIndex2];
    Vertex vertice3Original = modelo->objectMesh.vertices[verticeIndex3];

    vertex vertice1 = vertex(vertice1Original.x,vertice1Original.y, vertice1Original.z);
    vertex vertice2 = vertex(vertice2Original.x,vertice2Original.y, vertice2Original.z);
    vertex vertice3 = vertex(vertice3Original.x,vertice3Original.y, vertice3Original.z);

    newTriangle.V1 = vertice1;
    newTriangle.V2 = vertice2;
    newTriangle.V3 = vertice3;
    
    return newTriangle;
}

//função que, dado um voxel e um eixo, posiciona o raio baseado nestes parametros
//depois percorre todos os triangulos do obj com este raio e testa a intersção
//caso encontre, retorna true
bool verificaEixo(objLoader *modelo, vertex centroVoxel,char eixo){
	ray raio;
	float hitPoint;

	raio.origin = centroVoxel;
	raio.direction = centroVoxel;

	switch (eixo)
	{
		case 'x':
			raio.origin.x = centroVoxel.x - VOXEL_SIZE / 2;
			raio.direction.x = centroVoxel.x + VOXEL_SIZE / 2;	
			/*raio.direction.x = raio.direction.x - raio.origin.x;
			raio.direction.y = 0;
			raio.direction.z = 0;*/
			break;

		case 'y':
			raio.origin.y = centroVoxel.y - VOXEL_SIZE / 2;
			raio.direction.y = centroVoxel.y + VOXEL_SIZE / 2;
			/*raio.direction.y = raio.direction.y - raio.origin.y;
			raio.direction.x = 0;
			raio.direction.z = 0;*/
			break;

		case 'z':
			raio.origin.z = centroVoxel.z - VOXEL_SIZE / 2;
			raio.direction.z = centroVoxel.z + VOXEL_SIZE / 2;	
			/*raio.direction.z = raio.direction.z - raio.origin.z;
			raio.direction.x = 0;
			raio.direction.y = 0;*/
			break;
	}

	raio.direction.x = raio.direction.x - raio.origin.x;
	raio.direction.y = raio.direction.y - raio.origin.y;
	raio.direction.z = raio.direction.z - raio.origin.z;
	//raio.direction.normalize(raio.direction);
	for (int i = 0; i < modelo->objectMesh.triangles.size(); i++){
		triangle atual = getTriangle(modelo, i);
		if (triangle_intersection(atual.V1, atual.V2, atual.V3, raio.origin, raio.direction, &hitPoint) == 1)
			return true;
	}
	return false;
}


//funcao que varre os voxels, calcula seus centros e passa os parametros para a função de teste
//só testa um eixo se não encontrar colisao nos demais (iniciando por x)
//parametros de inicio e fim da varredura são passados como parametro da função para facilitar a paralelização
//caso exista uma interseção, adiciona a posição daquele voxel no vetor de posições
void voxelizerThreadFunction(objLoader *modelo, int startX, int endX, int startY, int endY, int startZ, int endZ)
{
	vertex centroVoxel;
	bool acertou = false;
	for (int z = startZ; z < endZ; z++){
		for (int y = startY; y < endY; y++){
			for (int x = startX; x < endX; x++){
				centroVoxel.x = -0.5 + ((x + 1) / MODEL_SIZE - (VOXEL_SIZE / 2));
				centroVoxel.y = -0.5 + ((y + 1) / MODEL_SIZE - (VOXEL_SIZE / 2));
				centroVoxel.z = -0.5 + ((z + 1) / MODEL_SIZE - (VOXEL_SIZE / 2));

				acertou = verificaEixo(modelo, centroVoxel,'x');
				if (!acertou){
					acertou = verificaEixo(modelo, centroVoxel,'y');
					if (!acertou){
						acertou = verificaEixo(modelo, centroVoxel,'z');
					}
				}

				if (acertou){						
					lock_guard<mutex> block_threads_until_finish_this_job(barrier);
					posicoes.push_back(centroVoxel);
					acertou = false;
				}
			}
		}
	}
}


int main(int argc, char* argv[])
{
    
    objLoader * modelo;
    std::cout << argv[1];
    modelo = new objLoader(argv[1]);//assuming only one file and everything is alright
    
    vertex centroVoxel;
    bool acertou = false;
    
	//cria oito threads
	thread voxelizerThread[8];
	
	//chama a função de voxelização 8 vezes, uma para octante do espaço
	voxelizerThread[0] = thread(voxelizerThreadFunction, modelo, 0, MODEL_INDEX / 2, 0, MODEL_INDEX / 2, 0, MODEL_INDEX / 2);
	voxelizerThread[1] = thread(voxelizerThreadFunction, modelo, MODEL_INDEX/2, MODEL_INDEX, 0, MODEL_INDEX / 2, 0, MODEL_INDEX / 2);
	voxelizerThread[2] = thread(voxelizerThreadFunction, modelo, 0, MODEL_INDEX / 2, 0, MODEL_INDEX / 2, MODEL_INDEX/2, MODEL_INDEX);
	voxelizerThread[3] = thread(voxelizerThreadFunction, modelo, MODEL_INDEX/2, MODEL_INDEX, 0, MODEL_INDEX / 2, MODEL_INDEX / 2, MODEL_INDEX);

	voxelizerThread[4] = thread(voxelizerThreadFunction, modelo, 0, MODEL_INDEX / 2, MODEL_INDEX / 2, MODEL_INDEX,  0, MODEL_INDEX / 2);
	voxelizerThread[5] = thread(voxelizerThreadFunction, modelo, MODEL_INDEX / 2, MODEL_INDEX, MODEL_INDEX / 2, MODEL_INDEX, 0, MODEL_INDEX / 2);
	voxelizerThread[6] = thread(voxelizerThreadFunction, modelo, 0, MODEL_INDEX / 2, MODEL_INDEX / 2, MODEL_INDEX, MODEL_INDEX / 2, MODEL_INDEX);
	voxelizerThread[7] = thread(voxelizerThreadFunction, modelo, MODEL_INDEX / 2, MODEL_INDEX, MODEL_INDEX / 2, MODEL_INDEX, MODEL_INDEX / 2, MODEL_INDEX);

	//espera todas as threads concluirem
	for (auto &t : voxelizerThread) t.join();

        
    //inicialização da glut e funções de callback
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);    
	glutInitWindowSize(1024,768);
	glutInit(&argc, argv);
    glutCreateWindow("VOXELIZER 3000");
    glutDisplayFunc(display);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutIdleFunc(idle);	
    glutMainLoop();

    return 0;
}
