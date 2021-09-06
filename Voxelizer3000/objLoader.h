#pragma once
#include <vector>
#include <string>

using namespace std;

struct Vertex
{
	float x;
	float y;
	float z;
};

struct Triangle
{
	int verts[3];
    int normals[3];
    int textures[3];
};

struct Mesh
{
	std::vector<Vertex>   vertices;
    std::vector<Vertex>   normals;
    std::vector<Vertex>   textures;
	std::vector<Triangle> triangles;

};

class objLoader
{
private:

    //void carregaFace(string line);
    
    Vertex posicao;


public:
	Mesh objectMesh;
	objLoader(void);
	objLoader(char * arquivo);
	~objLoader(void);
    
	Vertex menoresValores;
	Vertex maioresValores;

    Vertex getPosicao();
    void setPosicao(Vertex novaPosicao);
    static bool colidindo(Vertex posicao1, Vertex posicao2,Vertex incremento, string direcao);
    
    string nome;
};

