#include "objLoader.h"
#include <sstream>
#include <math.h>
#include <float.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

objLoader::objLoader(void)
{
}


objLoader::objLoader(char * arquivo)
{

    posicao.x = 0.0f;
    posicao.y = 0.0f;
    posicao.z = 0.0f;
    
	ifstream obj;
	
	string line;

	string aux;
	stringstream buffer;

	obj.open(arquivo,ifstream::in);
	if(obj.is_open()){
		while (obj.good())
		{
			//le o obj
			
			getline(obj,line);

			switch(line[0]){
				case '#' :	break;
				case 's' :	break;
				case 'v' :
                    if(line[1] == ' '){
                        Vertex newVertice;
                        buffer << line;
                        buffer >> aux >> newVertice.x >> newVertice.y >> newVertice.z;
                        //printf("X : %f Y : %f Z : %f \n", newVertice.x, newVertice.y, newVertice.z);
                        objectMesh.vertices.push_back(newVertice);
                        buffer.clear();
                    }
                    else if(line[1] == 'n'){
                        Vertex newVertice;
                        buffer << line;
                        buffer >> aux >> newVertice.x >> newVertice.y >> newVertice.z;
                        //printf("X : %f Y : %f Z : %f \n", newVertice.x, newVertice.y, newVertice.z);
                        objectMesh.normals.push_back(newVertice);
                        buffer.clear();
                    }
                    else if(line[1] == 't'){
                        Vertex newVertice;
                        buffer << line;
                        buffer >> aux >> newVertice.x >> newVertice.y; newVertice.z = 0;// >> newVertice.z;
                        //printf("X : %f Y : %f Z : %f \n", newVertice.x, newVertice.y, newVertice.z);
                        objectMesh.textures.push_back(newVertice);
                        buffer.clear();
                    }
					break;
				case 'f' :
					Triangle newTriangle;					
					buffer << line;
                    string teste = line.substr(0,5);
                  

                    string vertex1,vertex2,vertex3;
                    buffer >> aux >> vertex1 >> vertex2  >> vertex3;
                    
                    int primeiro = vertex1.find_first_of("/");
                    int segundo = vertex1.find_last_of("/");
                   
                    if(primeiro == -1 && segundo == -1){
                        //buffer >> aux >> newTriangle.verts[0] >> newTriangle.verts[1]  >> newTriangle.verts[2];
                        newTriangle.verts[0] = atoi(vertex1.c_str());
                        newTriangle.verts[1] = atoi(vertex2.c_str());
                        newTriangle.verts[2] = atoi(vertex3.c_str());
                        
                    }
                    else{
                    newTriangle.verts[0] = atoi(vertex1.substr(0,primeiro).c_str());
                    
                    if(primeiro + 1 != segundo)
                        newTriangle.textures[0] = atoi(vertex1.substr(primeiro+1,segundo-primeiro-1).c_str());
                    
                    newTriangle.normals[0] = atoi(vertex1.substr(segundo+1).c_str());
                    
                    primeiro = vertex2.find_first_of("/");
                    segundo  = vertex2.find_last_of("/");
                    
                    newTriangle.verts[1] = atoi(vertex2.substr(0,primeiro).c_str());
                    
                    if(primeiro + 1 != segundo)
                        newTriangle.textures[1] = atoi(vertex2.substr(primeiro+1,segundo-primeiro-1).c_str());
                    
                    newTriangle.normals[1] = atoi(vertex2.substr(segundo+1).c_str());

                    primeiro = vertex3.find_first_of("/");
                    segundo  = vertex3.find_last_of("/");
                    
                    newTriangle.verts[2] = atoi(vertex3.substr(0,primeiro).c_str());
                    
                    if(primeiro + 1 != segundo)
                        newTriangle.textures[2] = atoi(vertex3.substr(primeiro+1,segundo-primeiro-1).c_str());
                    
                    newTriangle.normals[2] = atoi(vertex3.substr(segundo+1).c_str());
                    }
                    
					//printf("Vertice 1 : %d Vertice 2 : %d Vertice 3 : %d \n", newTriangle.verts[0], newTriangle.verts[1], newTriangle.verts[2]);
                    
					objectMesh.triangles.push_back(newTriangle);
					buffer.clear();
					break;
			}
		}
	}

	maioresValores.x = 0;
	maioresValores.y = 0;
	maioresValores.z = 0;

	menoresValores.x = FLT_MAX;
	menoresValores.y= FLT_MAX;
	menoresValores.z = FLT_MAX;

    float maiorDosMaiores;
    float menorDosMenores;
    
	for(int i = 0; i < objectMesh.vertices.size(); i++){
		if(objectMesh.vertices[i].x < menoresValores.x){
			menoresValores.x = objectMesh.vertices[i].x; 
		}

		if(objectMesh.vertices[i].y < menoresValores.y){
			menoresValores.y = objectMesh.vertices[i].y; 
		}
		
		if(objectMesh.vertices[i].z < menoresValores.z){
			menoresValores.z = objectMesh.vertices[i].z; 
		}

		if(objectMesh.vertices[i].x > maioresValores.x){
			maioresValores.x = objectMesh.vertices[i].x; 
		}

		if(objectMesh.vertices[i].y > maioresValores.y){
			maioresValores.y = objectMesh.vertices[i].y; 
		}
		
		if(objectMesh.vertices[i].z > maioresValores.z){
			maioresValores.z = objectMesh.vertices[i].z; 
		}
	}
    
    maiorDosMaiores = maioresValores.x;
    if(maiorDosMaiores < maioresValores.y)
        maiorDosMaiores = maioresValores.y;
    
    if(maiorDosMaiores < maioresValores.z)
        maiorDosMaiores = maioresValores.z;

    menorDosMenores = menoresValores.x;
    if(menorDosMenores > menoresValores.y)
        menorDosMenores = menoresValores.y;
    
    if(menorDosMenores > menoresValores.z)
        menorDosMenores = menoresValores.z;
    
    float divisor = maiorDosMaiores - menorDosMenores;
    

    float centroX = (maioresValores.x + menoresValores.x)/2;
    float centroY = (maioresValores.y + menoresValores.y)/2;
    float centroZ = (maioresValores.z + menoresValores.z)/2;
    
    for(int i = 0; i < objectMesh.vertices.size(); i++){
        
        objectMesh.vertices[i].x -= centroX;
        objectMesh.vertices[i].y -= centroY;
        objectMesh.vertices[i].z -= centroZ;
        

        objectMesh.vertices[i].x /= divisor;
        objectMesh.vertices[i].y /= divisor;
        objectMesh.vertices[i].z /= divisor;
    }
    
	obj.close();

}

void objLoader::setPosicao(Vertex novaPosicao)
{
    posicao.x += novaPosicao.x;
    posicao.y += novaPosicao.y;
    posicao.z += novaPosicao.z;
}

Vertex objLoader::getPosicao()
{
    return posicao;
}

bool objLoader::colidindo(Vertex posicao1, Vertex posicao2,Vertex incremento, string direcao)
{
    float deltaX = fabsf(posicao1.x - (posicao2.x + incremento.x));
    float deltaZ = fabsf(posicao1.z - (posicao2.z + incremento.z));

    if(deltaZ < 5.0 && deltaX < 5.0)
        return true;
    else
        return false;
//    //if(direcao == "forward" || "backward"){
//        if(deltaZ < 1.0)
//            return true;
//        else
//            return false;
//    }
//    
//    //if(direcao == "left" || "right"){
//        if(deltaX < 1.0)
//            return true;
//        else
//            return false;
//    }
}

objLoader::~objLoader(void)
{
}
