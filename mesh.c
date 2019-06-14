#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mesh.h"
#include "matrix.h"
#include "draw.h"

/*======== void parse_args() ==========
  Inputs:   char *line

  Parses argument for slashes and removes them
  ====================*/
char * parse_slash(char *line) {
  char * s = calloc (sizeof(char), 1);
  strcpy(s, strsep(&line, "/"));
  return s;
}

/*======== void parse_args() ==========
  Inputs:   char *line

  Parses line into arguments
  ====================*/
char ** parse_args(char *line){
  char ** arr = calloc(sizeof(char*), 10);
  int i = 0;
  while (line) {
    arr[i] = strsep(&line, " ");
    if (strcmp(arr[i], "") != 0) {
      arr[i] = parse_slash(arr[i]);
      i++;
    }
  }
  return arr;
} //end parse_args

/*======== void parse_obj() ==========
  Inputs:   struct matrix *polygons
            char *file

  Parses .obj file for vertices and faces
  Calls add_mesh()
  ====================*/
void parse_obj(struct matrix *polygons, char *file) {
  //matrices to store object data
  struct matrix * vertices;
  struct matrix * faces;

  //file parsing vars
  FILE *f;
  char line[255];

  char type[3];
  double values[4];

  char ** args;

  //initialize matrices, 3 for vertices, 4 for triangle+quadrilateral faces
  vertices = new_matrix(3, 100);
  faces = new_matrix(4, 100);

  f = fopen(file, "r");
  // printf("have opened the mesh boi\n");
  while ( fgets(line, sizeof(line), f) != NULL ) {
    line[strlen(line) - 1] = '\0';
    //printf("%s\n", line);

    //if vertex,
    //then sscanf for type, values -> add vertex to vertices
    if (strncmp(line, "v", 1) == 0) {
      sscanf(line, "%s %lf %lf %lf", type, values, values+1, values+2);
      if (strncmp(type, "v", strlen(type)) == 0) {
        add_point_mesh(vertices, values, VERTEX);
      }
    }

    //if face
    //parse face or values -> add face to faces
    else if (strncmp(line, "f", 1) == 0) {
      args = parse_args(line);
      int i = 0;
      while (args[i+1] && (i < 4)) {
        //printf(args[i]);
        values[i] = atof(args[i+1]);
        i++;
      }
      //printf("\tparts:%d\n", i);
      add_point_mesh(faces, values, FACE);
    }
  }
  // printf("did the whOle while loop\n");

  add_mesh(polygons, vertices, faces);
}

/*======== void add_point_mesh() ==========
Inputs:   struct matrix * points
          double values
Returns:
adds point (x, y, z) or face (a, b, c) to points and increment points.lastcol
if points is full, should call grow on points
====================*/
void add_point_mesh(struct matrix * points, double values[4], int type) {

  if ( points->lastcol == points->cols )
    grow_matrix( points, points->lastcol + 100 );

  points->m[0][ points->lastcol ] = values[0];
  points->m[1][ points->lastcol ] = values[1];
  points->m[2][ points->lastcol ] = values[2];
  if (type == FACE) {
    points->m[3][ points->lastcol ] = values[3];
  }
  points->lastcol++;
} //end add_point_mesh

/*======== void add_mesh() ==========
  Inputs:   struct matrix * polygons
            struct matrix * vertices
            struct matrix * faces

  Adds vertices in correct order to polygon matrix
  ====================*/
void add_mesh(struct matrix *polygons, struct matrix *vertices, struct matrix *faces) {
  // printf("gonna add mesh\n");
  int f;
  int v0,v1,v2,v3;
  for (f = 0; f < faces->lastcol; f++) {
    v0 = ((int) faces->m[0][f]) - 1;
    v1 = ((int) faces->m[1][f]) - 1;
    v2 = ((int) faces->m[2][f]) - 1;
    v3 = ((int) faces->m[3][f]) - 1;

    // printf("face: %d %d %d\n", v0, v1, v2);
    //
    // printf("\tvertice/%d : %f %f %f\n", v0,
    //   vertices->m[0][v0],
    //   vertices->m[1][v0],
    //   vertices->m[2][v0]);
    // printf("\tvertice/%d : %f %f %f\n", v1,
    //   vertices->m[0][v1],
    //   vertices->m[1][v1],
    //   vertices->m[2][v1]);
    // printf("\tvertice/%d : %f %f %f\n", v2,
    //   vertices->m[0][v2],
    //   vertices->m[1][v2],
    //   vertices->m[2][v2]);
    //
    //   printf("hello!\n");
    add_polygon(polygons,
      vertices->m[0][v0],
      vertices->m[1][v0],
      vertices->m[2][v0],
      vertices->m[0][v1],
      vertices->m[1][v1],
      vertices->m[2][v1],
      vertices->m[0][v2],
      vertices->m[1][v2],
      vertices->m[2][v2]
    );
    // printf("this is what's breaking it!!\n");
    //if 4th vertex exists
    if (v3 > 0) {
      //then it's quadrilateral
      add_polygon(polygons,
        vertices->m[0][v0],
        vertices->m[1][v0],
        vertices->m[2][v0],
        vertices->m[0][v2],
        vertices->m[1][v2],
        vertices->m[2][v2],
        vertices->m[0][v3],
        vertices->m[1][v3],
        vertices->m[2][v3]
      );
    }
  }
  // printf("done adding!!!\n");
  free_matrix(vertices);
  free_matrix(faces);
} //end add_mesh
