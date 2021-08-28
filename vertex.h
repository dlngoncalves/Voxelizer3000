//
//  vertex.h
//  Octree
//
//  Created by Diego Gonçalves on 4/30/15.
//  Copyright (c) 2015 Diego Gonçalves. All rights reserved.
//

#ifndef Octree_vertex_h
#define Octree_vertex_h
class vertex {

public:
    float x;
    float y;
    float z;

    vertex();
    vertex(float x, float y, float z);
    static float dot(vertex a, vertex b);
    static vertex cross(vertex a,vertex b);
    static vertex normalize(vertex noNormal);
};

vertex::vertex()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
}

vertex::vertex(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

vertex operator+(const vertex& a, const vertex& b)
{
    vertex c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    
    return c;
}

vertex operator-(const vertex& a, const vertex& b)
{
    vertex c;
    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    
    return c;
}

float vertex::dot(vertex a, vertex b)
{
    float c;
    c = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    
    return c;
}

vertex vertex::cross(vertex a,vertex b)
{
    vertex c;
    
    c.x = (a.y * b.z) - (a.z * b.y);
    c.y = (a.z * b.x) - (a.x * b.z);
    c.z = (a.x * b.y) - (a.y * b.x);
    
    return c;
}

vertex vertex::normalize(vertex noNormal)
{
    vertex normal;
    float length;
    
    length = sqrtf(powf(noNormal.x,2) + powf(noNormal.y,2) + powf(noNormal.z,2));
    
    normal.x = noNormal.x /length;
    normal.y = noNormal.y /length;
    normal.z = noNormal.z /length;
    
    return normal;
}
#endif
