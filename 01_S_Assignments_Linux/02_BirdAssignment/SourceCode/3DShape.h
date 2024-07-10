#ifndef _SHAPE_H
#define _SHAPE_H

void drawSphere(float radius, int slices, int stacks, float *vertices, float *normals, float *texCoords, unsigned short *indices); 

void drawHemiSphere(float radius, int slices, int stacks, float *vertices, float *normals, float *texCoords, unsigned short *indices);

void drawCylinder(double baseRadius, double topRadius,double height, int slices, int stacks, float *vertices, float *normals, float *texCoords, unsigned short *indices);

void drawPartialDisk(double innerRadius,double outerRadius, int slices, int loops,double startAngle, double sweepAngle,  float *vertices, float *normals, float *texCoords, unsigned short *indices);

void drawDisk(double innerRadius, double outerRadius, int slices, int loops, float *vertices, float *normals, float *texCoords, unsigned short *indices);

void drawTrapezoidalCube(double baseLength, double baseWidth, double baseHeight, float *vertices, float *normals, float *texCoords, unsigned short *indices);

void drawCube(double baseLength, double baseWidth, double baseHeight, float *vertices, float *normals, float *texCoords, unsigned short *indices);
#endif 

