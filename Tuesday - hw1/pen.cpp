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
//Display *dpy;
//Window win;
//GLXContext glc;

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

