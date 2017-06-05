
//Cesar Loya 000919373 Summer 2017

//modified by: Cesar Loya 
//date: 06/01/17
//purpose: hw1
//
//cs3350 Spring 2017 Lab-1
//author: Gordon Griesel
//date: 2014 to present
//This program demonstrates the use of OpenGL and XWindows
//
//Assignment is to modify this program.
//You will follow along with your instructor.
//
//Elements to be learned in this lab...
//
//. general animation framework
//. animation loop
//. object definition and movement
//. collision detection
//. mouse/keyboard interaction
//. object constructor
//. coding style
//. defined constants
//. use of static variables
//. dynamic memory allocation
//. simple opengl components
//. git
//
//elements we will add to program...
//. Game constructor
//. multiple particles
//. gravity
//. collision detection
//. more objects
//
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define MAX_PARTICLES 90000
#define GRAVITY 0.1

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

//Structures

struct Vec {
    float x, y, z;
};

struct Shape {
    float width, height;
    float radius;
    Vec center;
};

struct Particle {
    Shape s;
    Vec velocity;
};

class Game {
    public:
	Shape box; //possibly make more boxes ??
	Shape circle;
	Particle particle[MAX_PARTICLES];
	int n;
	Game() {
	    n=0;
	}
};

//Function prototypes
void initXWindows(void);
void init_opengl(void);
void cleanupXWindows(void);
void check_mouse(XEvent *e, Game *game);
int check_keys(XEvent *e, Game *game);
void movement(Game *game);
void render(Game *game);



//gitHub circle !
void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius){
    int i;
    int triangleAmount = 20; //# of triangles used to draw circle

    GLfloat radius2 = 0.8f; //radius
    GLfloat twicePi = 2.0f * 3.14159f;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center of circle
    for(i = 0; i <= triangleAmount;i++) { 
	glVertex2f(
		x + (radius2 * cos(i *  twicePi / triangleAmount)), 
		y + (radius2 * sin(i * twicePi / triangleAmount))
		);
    }
    glEnd();
}

















//drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius);



int main(void)
{
    int done=0;
    srand(time(NULL));
    initXWindows();
    init_opengl();
    //declare game object
    Game game;
    game.n=0;

    //declare a box shape
    game.box.width = 100;
    game.box.height = 10;
    game.box.center.x = 120 + 5*65;
    game.box.center.y = 500 - 5*60;


    //circle
    game.circle.width=100;
    game.circle.height=100;
    game.circle.radius=3.14;




 
 //start animation
    while (!done) {
	while (XPending(dpy)) {
	    XEvent e;
	    XNextEvent(dpy, &e);
	    check_mouse(&e, &game);
	    done = check_keys(&e, &game);
	}
	movement(&game);
	render(&game);
	glXSwapBuffers(dpy, win);
    }
    cleanupXWindows();
    return 0;
}

void set_title(void)
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "335 Lab1   LMB for particle");
}

void cleanupXWindows(void)
{
    //do not change
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

void initXWindows(void)
{
    //do not change
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w=WINDOW_WIDTH, h=WINDOW_HEIGHT;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
	std::cout << "\n\tcannot connect to X server\n" << std::endl;
	exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
	std::cout << "\n\tno appropriate visual found\n" << std::endl;
	exit(EXIT_FAILURE);
    } 
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
	ButtonPress | ButtonReleaseMask | PointerMotionMask |
	StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
	    InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void init_opengl(void)
{
    //OpenGL initialization
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    //Initialize matrices
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //Set 2D mode (no perspective)
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    //Set the screen background color
    glClearColor(0.1, 0.1, 0.1, 1.0);
}

#define rnd() (float)rand() / (float)RAND_MAX
void makeParticle(Game *game, int x, int y)
{
    if (game->n >= MAX_PARTICLES)
	return;
    //std::cout << "makeParticle() " << x << " " << y << std::endl;
    //position of particle
    Particle *p = &game->particle[game->n];  // here
    p->s.center.x = x;
    p->s.center.y = y;
    p->velocity.y =  rnd() * 1.0 - 0.5;
    p->velocity.x =  rnd() * 1.0 - 0.5;
    game->n++;
}

void check_mouse(XEvent *e, Game *game)
{
    static int savex = 0;
    static int savey = 0;
    static int n = 0;

    if (e->type == ButtonRelease) {
	return;
    }
    if (e->type == ButtonPress) {
	if (e->xbutton.button==1) {
	    //Left button was pressed
	    int y = WINDOW_HEIGHT - e->xbutton.y;
	    makeParticle(game, e->xbutton.x, y);
	    makeParticle(game, e->xbutton.x, y);
	    makeParticle(game, e->xbutton.x, y);
	    makeParticle(game, e->xbutton.x, y);
	    makeParticle(game, e->xbutton.x, y);
	    return;
	}
	if (e->xbutton.button==3) {
	    //Right button was pressed
	    return;
	}
    }
    //Did the mouse move?
    if (savex != e->xbutton.x || savey != e->xbutton.y) {
	savex = e->xbutton.x;
	savey = e->xbutton.y;
	if (++n < 10)
	    return;
	int y = WINDOW_HEIGHT - e->xbutton.y;
	for (int i=0; i<10; i++)
	    makeParticle(game, e->xbutton.x, y);
    }
}

int check_keys(XEvent *e, Game *game)
{
    //Was there input from the keyboard?
    if (e->type == KeyPress) {
	int key = XLookupKeysym(&e->xkey, 0);
	if (key == XK_Escape) {
	    return 1;
	}
	//You may check other keys here.



    }
    return 0;
}

void movement(Game *game)
{
    Particle *p;
    //Particle *p2;

    if (game->n <= 0)
	return; 
    for(int i = 0; i < game->n; i++) {
	p = &game->particle[i];
	p->velocity.y -= GRAVITY;
	p->s.center.x += p->velocity.x;
	p->s.center.y += p->velocity.y;


	//check for collision with shapes...



	/*
	 *if particle's center y- axis is less than (shape y axis and shape height)
	 * 
	 *checking for left and right of shape and under the shape
	 * */





	//first
	Shape *s = &game->box;  //pink
	if(p->s.center.y < (s->center.y+300  + s->height) &&
		p->s.center.x > (s->center.x-350  - s->width) &&
		p->s.center.x < (s->center.x-350  + s->width) )
	{
	    //bounce   300 
	    p->s.center.y = s->center.y+300  + s->height;
	    p->velocity.y = -p->velocity.y;
	    p->velocity.y *= 0.5;

	}

	if(p->s.center.y < s->center.y + s->height &&
		p->s.center.x > s->center.x - s->width &&
		p->s.center.x < s->center.x + s->width)
	{

	    p->s.center.y = s->center.y + s->height;
	    p->velocity.y = -p->velocity.y;
	    p->velocity.y *= 0.5;

	}




	//second  orange
	if(p->s.center.y < s->center.y+100 + s->height &&
		p->s.center.x > s->center.x-75 - s->width &&
		p->s.center.x < s->center.x-75 + s->width)
	{
	    p->s.center.y = s->center.y+100 + s->height;
	    p->velocity.y = -p->velocity.y;
	    p->velocity.y *= 0.5;

	}

	if(p->s.center.y < s->center.y + s->height &&
		p->s.center.x > s->center.x - s->width &&
		p->s.center.x < s->center.x + s->width)
	{

	    p->s.center.y = s->center.y + s->height;
	    p->velocity.y = -p->velocity.y;
	    p->velocity.y *= 0.5;

	}



	//third  // 200   RED
	if(p->s.center.y < s->center.y+200 + s->height &&
		p->s.center.x > s->center.x-200 - s->width &&  // x is 200
		p->s.center.x < s->center.x-200 + s->width)
	{
	    p->s.center.y = s->center.y+200 + s->height;
	    p->velocity.y = -p->velocity.y;
	    p->velocity.y *= 0.5;

	}


	//green
	if(p->s.center.y < s->center.y + s->height &&
		p->s.center.x > s->center.x+35 - s->width &&
		p->s.center.x < s->center.x+35 + s->width)
	{

	    p->s.center.y = s->center.y + s->height;
	    p->velocity.y = -p->velocity.y;
	    p->velocity.y *= 0.5;

	}


	//fourth  GREY
	if(p->s.center.y < s->center.y-100 + s->height &&
		p->s.center.x > s->center.x+150 - s->width &&
		p->s.center.x < s->center.x+150 + s->width)
	{
	    p->s.center.y = s->center.y-100 + s->height;
	    p->velocity.y = -p->velocity.y;
	    p->velocity.y *= 0.5;

	}

	if(p->s.center.y < s->center.y + s->height &&
		p->s.center.x > s->center.x - s->width &&
		p->s.center.x < s->center.x + s->width)
	{

	    p->s.center.y = s->center.y + s->height;
	    p->velocity.y = -p->velocity.y;
	    p->velocity.y *= 0.5;

	}














	//check for off-screen
	if (p->s.center.y < 0.0 || p->s.center.y > WINDOW_HEIGHT) {
	    //  std::cout << "off screen" << std::endl;
	    game->particle[i] = game->particle[game->n-1];
	    game->n--;
	}



    }
    /*
    //second for loop
    for(int i = 0; i<game->n; i++) {
    p2 = &game->particle[i];
    p2->velocity.y -= GRAVITY;
    p2->s.center.x += p2->velocity.x;
    p2->s.center.y += p2->velocity.y;



    Shape *s = &game->box;
    if(p2->s.center.y < s->center.y + s->height &&
    p->s.center.x > s->center.x - s->width &&
    p->s.center.x < s->center.x + s->width)
    {

    p->s.center.y = s->center.y + s->height;
    p->velocity.y = -p->velocity.y;
    p->velocity.y *= 0.5;

    }





    if (p->s.center.y < 0.0 || p->s.center.y > WINDOW_HEIGHT) {
//  std::cout << "off screen" << std::endl;
game->particle[i] = game->particle[game->n-1];
game->n--;
}

}
*/

}





//display
void render(Game *game)      // render means to PRESENT
{
    float w, h;
    glClear(GL_COLOR_BUFFER_BIT);
    //Draw shapes...

    //draw box
    Shape *s;
    glColor3ub(200,90,5); //orange one
    s = &game->box;
    glPushMatrix();
    glTranslatef(s->center.x-75, s->center.y+100, s->center.z);
    w = s->width;
    h = s->height;
    glBegin(GL_QUADS);
    glVertex2i(-w,-h);
    glVertex2i(-w, h);
    glVertex2i( w, h);
    glVertex2i( w,-h);
    glEnd();
    glPopMatrix();


    //aqui


    Shape *s2;
    glColor3ub(90,140,90); //green
    s2 = &game->box;
    glPushMatrix();
    glTranslatef(s2->center.x+35, s2->center.y, s2->center.z);//this is WHERE it bounches off
    w = s2->width;
    h = s2->height;
    glBegin(GL_QUADS);
    glVertex2i(-w,-h);
    glVertex2i(-w, h);
    glVertex2i( w, h);
    glVertex2i( w,-h);
    glEnd();
    glPopMatrix();


    Shape *s3;
    glColor3ub(100,100,100); //grey
    s3 = &game->box;
    glPushMatrix();
    glTranslatef(s3->center.x+150, s3->center.y-100, s3->center.z);//this is WHERE it bounches off
    w = s3->width;
    h = s3->height;
    glBegin(GL_QUADS);
    glVertex2i(-w,-h);
    glVertex2i(-w, h);
    glVertex2i( w, h);
    glVertex2i( w,-h);
    glEnd();
    glPopMatrix();



    Shape *s4;
    glColor3ub(200,10,10);  //red
    s4 = &game->box;
    glPushMatrix();
    glTranslatef(s4->center.x-200, s4->center.y+200, s4->center.z);//this is WHERE it bounches off
    w = s4->width;
    h = s4->height;
    glBegin(GL_QUADS);
    glVertex2i(-w,-h);
    glVertex2i(-w, h);
    glVertex2i( w, h);
    glVertex2i( w,-h);
    glEnd();
    glPopMatrix();

    //last one

    Shape *s5;
    glColor3ub(200,20,100);  // pink
    s5 = &game->box;
    glPushMatrix();
    glTranslatef(s5->center.x-350, s5->center.y+300, s5->center.z);//this is WHERE it bounches off
    w = s5->width;
    h = s5->height;
    glBegin(GL_QUADS);
    glVertex2i(-w,-h);
    glVertex2i(-w, h);
    glVertex2i( w, h);
    glVertex2i( w,-h);
    glEnd();
    glPopMatrix();









    for (int i=0; i<game->n ; i++) {
	Vec *c = &game->particle[i].s.center;
	//draw all particles here
	glPushMatrix();
	glColor3ub(110,200,220);
	//red , green , blue

	w = 2;
	h = 2;
	glBegin(GL_QUADS);
	glVertex2i(c->x-w, c->y-h);
	glVertex2i(c->x-w, c->y+h);
	glVertex2i(c->x+w, c->y+h);
	glVertex2i(c->x+w, c->y-h);
	glEnd();
	glPopMatrix();
    }
}



