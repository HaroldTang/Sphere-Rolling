#include <stdio.h>
#include <math.h>

#ifdef __APPLE__  // include Mac OS X verions of headers
#include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#include <GL/glut.h>
#endif

#define XOFF          50
#define YOFF          50
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600
#define GL_SILENCE_DEPRECATION True

void display(void);
void myinit(void);

/* Function to draw circles. */
void draw_circle(int x, int y, int r);
void circlePaint(int x, int y, int x_circle, int y_circle);
void draw_circle_static(void);
void draw_circle_animation(void);
/* Function to handle file input; modification may be needed */
void file_in(void);
void select(void);

void idle(void);

int circle[10][3]= {0};     //create matrix from file or input
int print[10][3]= {0};      //create coordinate
int max, n = 0;
int timer = 0;              //set timer for animation
int K = 1000;               //set frame rate K
int part;                   //indicate the part of problem

/*-----------------
 The main function
 ------------------*/
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    
    /* Use both double buffering and Z buffer */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
    glutInitWindowPosition(XOFF, YOFF);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("CS6533/CS4533 Assignment 1");
    
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    
    select();
    
    myinit();
    glutMainLoop();
    
    return 0;
}

/*----------
 file_in(): file input function. Modify here.
 ------------*/
void file_in(void)                           //read from the file and get information of circles
{
    int x, y, r = 0;
    FILE *fp = NULL;
    fp = fopen("input_circles","r");
    fscanf(fp, "%d", &n);                    //read number of circles
    int scale[n];
    for(int i = 0; i <= n-1; i++){
        fscanf(fp, "%d %d %d", &x, &y, &r);  //read input value from file
        circle[i][0] = x;
        circle[i][1] = y;
        circle[i][2] = r;
        scale[i] = fmax(fmax(fabs(x-r), fabs(x+r)), fmax(fabs(y-r), fabs(y+r))); //compare the most region boundary of one circle
    }
    
    for(int i = 0; i<= n-1; i++){
        if(scale[i]>max){
            max = scale[i];                  //get maximum value of scale to be the region boundary
        }
    }
}

/*---------------------------------------------------------------------
 display(): This function is called once for _every_ frame.
 ---------------------------------------------------------------------*/
void display(void)
{
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glColor3f(1.0, 0.84, 0);                /* draw in golden yellow */
    glPointSize(1.0);                       /* size of each point */
    glBegin(GL_POINTS);
    
    switch (part) {                         //select corresponding part and create circle for every frame
        case 1:
            draw_circle(print[0][0], print[0][1], print[0][2]);
            break;
        case 2:
            draw_circle_static();
            break;
        case 3:
            draw_circle_animation();
            break;
        default:
            break;
    }
    glEnd();
    
    glFlush();                              /* render graphics */
    
    glutSwapBuffers();                      /* swap buffers */
}

void select(void)
{
    printf("please input the select fucntion:\n1 for part(c) 2 for part(d) and 3 for part(e):\n");
    scanf("%d", &part);
    switch (part) {                            //show parts c,d,e as options for user to choose
        case 1:{
            int x, y, r = 0;
            printf("The window size is (%d, %d)\n", WINDOW_WIDTH,WINDOW_HEIGHT);
            printf("please input the value of x, y and radius r (separate by space):\n");
            scanf("%d %d %d", &x, &y, &r);                       //pass parameters of input circle
            print[0][0] = x;                                     //store value in list to be painted
            print[0][1] = y;
            print[0][2] = r;
            printf("The circle is displayed.\n");
            break;
        }
        case 2:{
            file_in();
            printf("The max window size is %d.\n", max);
            for(int j = 0; j<= n-1; j++){                       //transform the coordinate according to the maximum region of one circle
                print[j][0] = (circle[j][0] + max) * WINDOW_WIDTH / (2 * max);
                print[j][1] = (circle[j][1] + max) * WINDOW_HEIGHT / (2 * max);
                print[j][2] =  WINDOW_HEIGHT * circle[j][2] / (2 * max);  //get x, y, r for circles and set coordinates for each circle
            }
            printf("Circles are displayed.\n");
            break;
        }
        case 3:{
            file_in();
            for(int j = 0; j<= n-1; j++){                       //transform the coordinate according to the maximum region of one circle
                        print[j][0] = (circle[j][0] + max) * WINDOW_WIDTH / (2 * max);
                        print[j][1] = (circle[j][1] + max) * WINDOW_HEIGHT / (2 * max);
                        print[j][2] =  WINDOW_HEIGHT * circle[j][2] / (2 * max);
                    }
            printf("K value = %d.\nCircles are displayed.\n", K);
            break;
        }
        default:{
            break;}
    }
}
/*---------------------------------------------------------------------
 myinit(): Set up attributes and viewing
 ---------------------------------------------------------------------*/
void myinit()
{
    glClearColor(0.0, 0.0, 0.92, 0.0);    /* blue background*/
    /* set up viewing */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
}

void draw_circle_static(void){
    for (int i=0; i<n;i++){
        draw_circle(print[i][0], print[i][1], print[i][2]);      //draw circles stored in matrix
    }
}

void draw_circle_animation(void){
    for (int i=0; i<n;i++){
        draw_circle(print[i][0], print[i][1], print[i][2]*timer/K); //change the radius of circle based on timer value
    }
}

void draw_circle(int x, int y, int r)
{
    int x_circle = r;
    int y_circle = 0;
    int d = 1 - r;                                              //set first D
    circlePaint(x, y, x_circle, y_circle);                      //draw the initial point
    while(x_circle >= y_circle){
        y_circle++;                                             //increase y value by 1
        if(d <= 0){                                             //if pick N as previous p
            d = d + 2 * y_circle + 1;
        }
        else{
            x_circle--;                                         //first deduct the x coordinate by 1
            d = d + 2 * (y_circle - x_circle) + 1 ;              //if pick NW as previous p 
        }
        circlePaint(x, y, x_circle, y_circle);                  //deliver pixels coordinate that chosen to draw
    }
}
void circlePaint(int x, int y, int x_circle, int y_circle)      // create eight regions simultaneously
{
    
    glVertex2i( x_circle + x,  y_circle + y);
    glVertex2i(-x_circle + x,  y_circle + y);
    glVertex2i( x_circle + x, -y_circle + y);
    glVertex2i(-x_circle + x, -y_circle + y);
    glVertex2i( y_circle + x,  x_circle + y);
    glVertex2i(-y_circle + x,  x_circle + y);
    glVertex2i( y_circle + x, -x_circle + y);
    glVertex2i(-y_circle + x, -x_circle + y);
   
}

void idle(void)
{
    timer += 1;                                                 //change timer in idle function
    if(timer > K)
        timer = 0;
    glutPostRedisplay();                                        //recall the display function
}
