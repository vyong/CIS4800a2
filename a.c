
/* Derived from scene.c in the The OpenGL Programming Guide */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glut.h>

// #include <OpenGL/gl.h>
// #include <OpenGL/glu.h>
// #include <GLUT/glut.h>

struct Vertexes {
	float x, y, z;

};
typedef struct Vertexes Vertex;

struct Triangles {
	struct Vertexes *v1, *v2, *v3;
	struct Triangles * nextTri;
};
typedef struct Triangles Triangle;

struct Normals {
	float Nx, Ny, Nz;
	struct Normals * nextNormal;

};
typedef struct Normals Normal;

	/* flags used to control the appearance of the image */
int lineDrawing = 1;	// draw polygons as solid or lines
int lighting = 0;	// use diffuse and specular lighting
int smoothShading = 0;  // smooth or flat shading
int textures = 0;

float **heightMap;
int width, height, depth, maxDepth = 0, lButtonPressed = 0, rButtonPressed = 0;
float camX, camY, camZ;

// the key states. These variables will be zero
//when no key is being presses
float deltaAngle = 0.0f;
float deltaMove = 0;

GLubyte  Image[64][64][4];
GLuint   textureID[1];

Normal * head;

/*  Initialize material property and light source.
 */
void init (void)
{
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_full_off[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light_full_on[] = {1.0, 1.0, 1.0, 1.0};

	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	/* if lighting is turned on then use ambient, diffuse and specular
		lights, otherwise use ambient lighting only */
	if (lighting == 1) {
		glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
	} else {
		glLightfv (GL_LIGHT0, GL_AMBIENT, light_full_on);
		glLightfv (GL_LIGHT0, GL_DIFFUSE, light_full_off);
		glLightfv (GL_LIGHT0, GL_SPECULAR, light_full_off);
	}
	glLightfv (GL_LIGHT0, GL_POSITION, light_position);
	
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void display (void)
{
GLfloat blue[]  = {0.0, 0.0, 1.0, 1.0};
GLfloat red[]   = {1.0, 0.0, 0.0, 1.0};
GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_gray[] = {0.3, 0.3, 0.3, 0.3};
float x, z;
float xModified, zModified;
float vector1x, vector1y, vector1z, vector2x, vector2y, vector2z, normalx, normaly, normalz;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw surfaces as either smooth or flat shaded */
	if (smoothShading == 1)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	/* draw polygons as either solid or outlines */
	if (lineDrawing == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/* turn texturing on */
	if (textures == 1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID[0]);
	/* if textured, then use white as base colour */
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	}

	//glutSolidSphere (1.0, 15, 15);

	if (textures == 1) 
		glDisable(GL_TEXTURE_2D);

	// /* set colour of torus */
	// glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	// glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	//  move to location for object then draw it 
	// glPushMatrix ();
	// glTranslatef (-0.75, 0.5, 0.0); 
	// glRotatef (90.0, 1.0, 0.0, 0.0);
	// glutSolidTorus (0.275, 0.85, 15, 15);
	// glPopMatrix ();
	/*test inputs */
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, light_gray);
	glMaterialfv(GL_FRONT, GL_SPECULAR, light_gray);

	for (x = 0; x < width-1; x++) { 
		for (z = 0; z < height-1; z++) {
			glBegin(GL_TRIANGLES);

			/* calculate Normal for top left triangle */

			vector1x = x - x;
			vector1y = (heightMap[(int)x][(int)z+1]) - (heightMap[(int)x][(int)z]);
			vector1z = (z+1) - z;

			vector2x = (x+1) - x;
			vector2y = (heightMap[(int)x+1][(int)z]) - (heightMap[(int)x][(int)z]);
			vector2z = z - z;

			normalx = vector1y * vector2z - vector2y * vector1z;
			normaly = vector1z * vector2x - vector2z * vector1x;
			normalz = vector1x * vector2y - vector2x * vector1y;


			xModified = x;
			zModified = z;
			glNormal3f(normalx, normaly, normalz);
			glVertex3f(xModified, (heightMap[(int)x][(int)z]), zModified);

			xModified = x;
			zModified = (z+1);
			glVertex3f(xModified, (heightMap[(int)x][(int)z+1]), zModified);


			xModified = (x+1);
			zModified = z;
			glVertex3f(xModified, (heightMap[(int)x+1][(int)z]), zModified);

			
			/* calculate Normal for bottom right triangle */
			vector1x = (x+1) - x;
			vector1y = (heightMap[(int)x+1][(int)z+1]) - (heightMap[(int)x][(int)z+1]);
			vector1z = (z+1) - (z+1);

			vector2x = (x+1) - x;
			vector2y = (heightMap[(int)x+1][(int)z]) - (heightMap[(int)x][(int)z+1]);
			vector2z = z - (z+1);

			normalx = vector1y * vector2z - vector2y * vector1z;
			normaly = vector1z * vector2x - vector2z * vector1x;
			normalz = vector1x * vector2y - vector2x * vector1y;

			xModified = x;
			zModified = (z+1);
			glNormal3f(normalx, normaly, normalz);
			glVertex3f(xModified, (heightMap[(int)x][(int)z+1]), zModified);
			
			xModified = (x+1);
			zModified = (z+1);
			glVertex3f(xModified, (heightMap[(int)x+1][(int)z+1]), zModified);

			xModified = (x+1);
			zModified = z;
			glVertex3f(xModified, (heightMap[(int)x+1][(int)z]), zModified);
			glEnd();
		}
	}
	glPopMatrix ();

	glPopMatrix ();
	glFlush ();
}

void reshape(int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 1000.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	// Set the camera
	gluLookAt(	fmod(width,7)/2, fmod(depth,7)/2, ((height/7)+10)*(-1),
			fmod(width,7)/2, fmod(depth,7)/2, fmod(height,7)/2,
			0.0, 1.0,  0.0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
		case 'q':
			exit(0);
			break;
		case '1':		// draw polygons as outlines
			lineDrawing = 1;
			lighting = 0;
			smoothShading = 0;
			textures = 0;
			init();
			display();
			break;
		case '2':		// draw polygons as filled
			lineDrawing = 0;
			lighting = 0;
			smoothShading = 0;
			textures = 0;
			init();
			display();
			break;
		case '3':		// diffuse and specular lighting, flat shading
			lineDrawing = 0;
			lighting = 1;
			smoothShading = 0;
			textures = 0;
			init();
			display();
			break;
		case '4':		// diffuse and specular lighting, smooth shading
			lineDrawing = 0;
			lighting = 1;
			smoothShading = 1;
			textures = 0;
			init();
			display();
			break;
		case '5':		// texture with  smooth shading
			lineDrawing = 0;
			lighting = 1;
			smoothShading = 1;
			textures = 1;
			init();
			display();
			break;
		case 'w':		// draw polygons as outlines
			glTranslatef(0.0, 0.0, 0.5);
			init();
			display();
			break;
		case 'd':		// draw polygons as outlines
			glTranslatef(-1.0, 0.0, 0.0);
			init();
			display();
			break;
		case 'a':		// draw polygons as outlines
			glTranslatef(1.0, 0.0, 0.0);
			init();
			display();
			break;
		case 's':		// draw polygons as outlines
			glTranslatef(0.0, 0.0,-0.5);
			init();
			display();
			break;
		case 'f':		// draw polygons as outlines
			glTranslatef(0.0, -1.0, 0.0);
			init();
			display();
			break;
		case 'v':		// draw polygons as outlines
			glTranslatef(0.0, 1.0,0.0);
			init();
			display();
			break;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { //left button and pressed
		lButtonPressed = x;
		// printf("left button pressed %i\n", lButtonPressed);
	}
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		lButtonPressed = -1;
		// printf("left button released %i\n", lButtonPressed);
	}

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) { //right button and pressed
		rButtonPressed = y;
		// printf("right button pressed %i\n", rButtonPressed);
	}
	else {
		rButtonPressed = -1;
		// printf("right button released %i\n", rButtonPressed);
	}
}

void motion(int x, int y) {
	if(lButtonPressed >= 0){
		camX = -(x/(width/6)) * -sinf(10*(M_PI/180)) * cosf((45)*(M_PI/180));
		camY = -(x/(depth/6)) * -sinf((45)*(M_PI/180));
		camZ = (x/(height/6)) * cosf((10)*(M_PI/180)) * cosf((45)*(M_PI/180));
		//printf("Mouse dragged with left button at %i, %i\n", x, y);

		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();

		gluLookAt(camX,camY,camZ,   // Camera position
		  width/2, 0, height/2,    // Look at point
		  0.0, 1.0, 0.0);   // Up vector
		// gluLookAt(	fmod(x,17), fmod(depth,7)/2, (height/7)*(-1),
		// 	fmod(width,7)/2, fmod(depth,7)/2, fmod(height,7)/2,
		// 	1.0f, 1.0f,  0.0f);
		display ();
	}

	else if(rButtonPressed >= 1){
		//printf("Mouse dragged with right button at %i, %i\n", x, y);

		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity ();

		gluLookAt(width/2, y, height/2,    // Look at point
			width/2, 0, height/2,
        	0.0, 1.0, 0.0);   // Up vector
		// gluLookAt(	fmod(x,17), fmod(depth,7)/2, (height/7)*(-1),
		// 	fmod(width,7)/2, fmod(depth,7)/2, fmod(height,7)/2,
		// 	1.0f, 1.0f,  0.0f);
		display ();

	}
}

void loadTexture() {
FILE *fp;
int  i, j;
int  red, green, blue;

	if ((fp = fopen("image.txt", "r")) == 0) {
		printf("Error, failed to find the file named image.txt.\n");
		exit(0);
	} 

	for(i=0; i<64; i++) {
		for(j=0; j<64; j++) {
			fscanf(fp, "%d %d %d", &red, &green, &blue);
			Image[i][j][0] = red;
			Image[i][j][1] = green;
			Image[i][j][2] = blue;
			Image[i][j][3] = 255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1,textureID);
	glBindTexture(GL_TEXTURE_2D, textureID[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, Image);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	fclose(fp);
}

/* read data file and store in arrays */
void readFile(char **argv) {
FILE *fp;
char instr[1024];
char * buffer;
int count = 0, x, z = 0, convertedNum, front = 0;
Triangle * curr, * node;


	/* open file and print error message if name incorrect */
	if ((fp = fopen(argv[1], "r")) == NULL) {
		printf("ERROR, could not open file.\n");
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	/* the code to read the input file goes here */
	/* numlevels is set to the number of lines in the file not including the first comment line */
	else {
		while (fgets(instr, sizeof(instr), fp)) {
			if (instr[0] == '#' || instr[0] == 'P') {
				//skip reading in line
			}

			else {
					if(count == 0) {
						buffer = strtok(instr, " ");
						width = atoi(buffer);

						heightMap = malloc(width * sizeof(float *));

						buffer = strtok(NULL, " ");
						height = atoi(buffer);

						for(x = 0; x < width; x++){
							heightMap[x] = malloc(height * sizeof(float));
						}

						count++;
						x = 0;

					}

					else if( count == 1) {
						depth = atof(instr);
						count++;
						//printf("Width: %d, Height: %d, Depth: %d\n", width, height, depth);

						

					}

					else {
						buffer = strtok(instr, "  ");

						while (buffer != NULL){
							//printf("%d\n", atoi(buffer));
							convertedNum = atof(buffer);

							heightMap[x][z] = convertedNum;
							if (convertedNum > maxDepth){
								maxDepth = convertedNum;
							}

							x++;
							if(x == width){
								z++;
								x = 0;
							}
							buffer = strtok(NULL, "  ");
						}//end buffer

						// printf("%f\n", heightMap[1][1]);

						// node = (Triangle *)malloc(sizeof(Triangle));
						// node->v1 = (Vertex *)malloc(sizeof(Vertex));
						// node->v2 = (Vertex *)malloc(sizeof(Vertex));
						// node->v3 = (Vertex *)malloc(sizeof(Vertex));
						// node->nextTri = NULL;

						// curr = (Triangle *)malloc(sizeof(Triangle));
						// curr->v1 = (Vertex *)malloc(sizeof(Vertex));
						// curr->v2 = (Vertex *)malloc(sizeof(Vertex));
						// curr->v3 = (Vertex *)malloc(sizeof(Vertex));
						// curr->nextTri = NULL;

						// printf("%f\n", heightMap[1][1]);

						// for (x = 0; x < width-1; x++) { 
						// 	for (z = 0; z < height-1; z++) {

						// 		printf("%f\n", heightMap[x][z]);
						// 		// //top left triangle
						// 		// curr->v1->x = x;
						// 		// curr->v1->y = heightMap[x][z];
						// 		// curr->v1->z = z;

						// 		// curr->v2->x = x;
						// 		// curr->v2->y = heightMap[x][z+1];
						// 		// curr->v2->z = z+1;

						// 		// curr->v3->x = x+1;
						// 		// curr->v3->y = heightMap[x+1][z];
						// 		// curr->v3->z = z;

						// // 		if(front == 0){
						// // 			head = curr;
						// // 			node = curr;
						// // 			front = 1;
						// // 		}
						// // 		else{

						// // 		}


						// // 		//bottom right triangle
						// // 		curr->v1->x = x;
						// // 		curr->v1->y = heightMap[x][z+1];
						// // 		curr->v1->z = z+1;

						// // 		curr->v1->x = x+1;
						// // 		curr->v1->y = heightMap[x+1][z+1];
						// // 		curr->v1->z = z+1;

						// // 		curr->v1->x = x+1;
						// // 		curr->v1->y = heightMap[x+1][z];
						// // 		curr->v1->z = z;
						//  	}
						// }
					}
			}//end else
		}//end while
		
	}//end else
	fclose(fp);	
}

/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv)
{

	head = NULL;
	readFile(argv);
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize (1024, 768);
	glutCreateWindow (argv[0]);
	init();
	loadTexture();
	glutReshapeFunc (reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc (keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();
	return 0; 
}

