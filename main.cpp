/****************************
   Student Name: Xucheng Tang
   NetID: xt544
 ****************************/
#include "Angel-yjc.h"
#include "texmap.c"
#include <stdio.h>

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

static int shadowMenu;
static int lightMenu;
static int shadingMenu;
static int lightsourceMenu;
static int fogMenu;
static int blendingMenu;
static int textureMenu;
static int textureSphereMenu;
static int fireworkMenu;

static GLuint texName;

mat4 Transform_mv(mat4 mv);
mat4 Shadow_mv(mat4 mv);
GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;                         //shader buffer object id
GLuint program_firework;
GLuint sphere_shading_buffer_flat;      //vertex buffer object id for sphere
GLuint sphere_shading_buffer_smooth;
GLuint sphere_normal_buffer;
GLuint floor_shading_buffer;            //vertex buffer object id for floor
GLuint floor_normal_buffer;
GLuint line_buffer;                     //vertex buffer object id for axis lines
GLuint projection_buffer;               //vertex buffer object id for projection
GLuint particle_buffer;

float t = 0;
float t_sub = 0;
static float Tmax = 9000.0;
GLfloat fovy = 45.0;
GLfloat aspect;
GLfloat zNear = 0.5, zFar = 20.0;

GLfloat angle = 0.0;
vec4 init_eye(7.0, 3.0, -10.0, 1.0);    //VRP, initial viewer position
vec4 eye = init_eye;                    //current viewer position, can change with keyboard function
mat4 M = identity();                    //construct of accumulate rotation matrix

//set up Flags
int segmentFlag = 1;                    //flag for segment
int beginFlag = 0;                      //flag for begin animation
int animationFlag = 0;                  //flag for animation
int shadowFlag = 1;                     //flag for shadow
int wireFlag = 0;                       //flag for wire frame sphere
int lightFlag = 1;                      //flag for lighting
int shadingFlag = 1;                    //flag for shading method
int sourceFlag = 1;                     //flag for light source
int normalFlag = 0;                     //flag for shading method in shader
int fogFlag = 0;                        //flag for fog option
int blendFlag = 1;                      //flag for blending option
int textureFlag_floor = 1;              //flag for texture mapping for floor
int textureFlag_sphere = 1;             //flag for texture mapping for sphere
int verticalFlag = 0;                   //flag for texture vertical or slanted (initially vertical)
int coordinateFlag = 0;                 //flag for texture coordinate in object space or eye space (initially object)
int latticeFlag = 0;                    //flag for texture lattice
int uprightFlag = 1;                    //flag for texture lattice upright flag
int fireworkFlag = 0;

const int floor_NumVertices = 6;        //for a floor square
point4 floor_points[floor_NumVertices]; // positions for all vertices
color4 floor_colors[floor_NumVertices]; // colors for all vertices
vec3   floor_normals[floor_NumVertices];

const int particle_number = 300;
point4 particle_points[particle_number];
color4 particle_colors[particle_number];
vec3   velocity[particle_number];

const int line_NumVertices = 9;         //for three lines
point4 line_points[line_NumVertices];
color4 line_colors[line_NumVertices];

int sphere_total = 0;                   //initial sphere vertice number
int sphere_NumVertices = 0;
point4 sphere_points[4000];             //position for all sphere vertices. color = yellow
point4 sphere_colors[4000];
vec3   sphere_normals_flat[4000];
vec3   sphere_normals_smooth[4000];

point4 projection_points[4000];
color4 projection_colors[4000];


point4 floor_vertex[4] = {              //position for all floor vertices. color = green
    point4( 5.0, 0.0,  8.0, 1.0),
    point4( 5.0, 0.0, -4.0, 1.0),
    point4(-5.0, 0.0, -4.0, 1.0),
    point4(-5.0, 0.0,  8.0, 1.0)
};
vec2 floor_textCoord[6] = {
    vec2(5.0, 6.0),
    vec2(5.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 6.0),
    vec2(5.0, 6.0)
};
//point
// RGBA colors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0),             // black
    color4( 1.0, 0.0, 0.0, 1.0),             // red
    color4( 1.0, 0.84, 0.0,1.0),             // yellow
    color4( 0.0, 1.0, 0.0, 1.0),             // green
    color4( 0.0, 0.0, 1.0, 1.0),             // blue
    color4( 1.0, 0.0 ,1.0, 1.0),             // magenta
    color4( 0.25,0.25,0.25,0.65),            // gray
    color4(0.529,0.807,0.92,1.0)             // sky blue
};
    //shader lighting setting
    color4 global_light_ambient( 1.0, 1.0, 1.0, 1.0);

    color4 directional_light_ambient( 0.0, 0.0, 0.0, 1.0);
    color4 directional_light_diffuse( 0.8, 0.8, 0.8, 1.0);
    color4 directional_light_specular( 0.2, 0.2, 0.2, 1.0);
    vec4   directional_light_direction( 0.1, 0.0, -1.0, 0.0);    //already in eye frame

    color4 positional_light_diffuse(1.0, 1.0, 1.0, 1.0);
    color4 positional_light_specular(1.0, 1.0, 1.0, 1.0);
    color4 positional_light_ambient(0.0, 0.0, 0.0, 1.0);
    point4 positional_light_position(-14.0, 12.0, -3.0, 1.0);  //in world frame

    float const_att = 2.0;
    float linear_att = 0.01;
    float quad_att = 0.001;
    point4 spotlightTowards(-6.0, 0.0, -4.5, 1.0); //in world frame
    float exponent = 15.0;
    float cut_off  = DegreesToRadians * 20.0;

    color4 floor_ambient(0.2, 0.2, 0.2, 1.0);
    color4 floor_diffuse(0.0, 1.0, 0.0, 1.0);
    color4 floor_specular(0.0, 0.0, 0.0, 1.0);

    color4 sphere_ambient(0.2, 0.2, 0.2, 1.0);
    color4 sphere_diffuse(1.0, 0.84, 0.0, 1.0);
    color4 sphere_specular(1.0, 0.84, 0.0, 1.0);
    float  sphere_shininess = 125.0;

    color4 global_ambient_product_floor = global_light_ambient * floor_ambient;
    color4 directional_ambient_product_floor = directional_light_ambient * floor_ambient;
    color4 directional_diffuse_product_floor = directional_light_diffuse * floor_diffuse;
    color4 directional_specular_product_floor= directional_light_specular * floor_specular;
    color4 positional_ambient_product_floor = positional_light_ambient * floor_ambient;
    color4 positional_diffuse_product_floor = positional_light_diffuse * floor_diffuse;
    color4 positional_specular_product_floor= positional_light_specular * floor_specular;

    color4 global_ambient_product_sphere = global_light_ambient * sphere_ambient;
    color4 directional_ambient_product_sphere = directional_light_ambient * sphere_ambient;
    color4 directional_diffuse_product_sphere = directional_light_diffuse * sphere_diffuse;
    color4 directional_specular_product_sphere= directional_light_specular * sphere_specular;
    color4 positional_ambient_product_sphere = positional_light_ambient * sphere_ambient;
    color4 positional_diffuse_product_sphere = positional_light_diffuse * sphere_diffuse;
    color4 positional_specular_product_sphere= positional_light_specular * sphere_specular;

float findMod(float a, float b) //to compute mode value for float inputs
{
    float mod = a;
    while (mod >= b)
        mod = mod - b;
    
    if (a < 0)
        return -mod;
    
    return mod;
}

void floor(){                           //use two triangles to build floor
    vec4 u = floor_vertex[1] - floor_vertex[0];
    vec4 v = floor_vertex[2] - floor_vertex[0];
    vec3 normal = normalize(cross(u, v));

    floor_colors[0] = floor_colors[1] =floor_colors[2]=floor_colors[3]=floor_colors[4]=floor_colors[5] = color4(0.0, 1.0, 0.0, 1.0);
    
    floor_normals[0] = floor_normals[1] = floor_normals[2] = floor_normals[3] = floor_normals[4] = floor_normals[5] = normal;
    floor_points[0] = floor_vertex[0];
    floor_points[1] = floor_vertex[1];
    floor_points[2] = floor_vertex[2];
    floor_points[3] = floor_vertex[2];
    floor_points[4] = floor_vertex[3];
    floor_points[5] = floor_vertex[0];
   
}
void particle(){                           //assign particles
    for (int i=0; i<particle_number; i++){
        velocity[i] = vec3(2.0 * ((rand() % 256) / 256.0 - 0.5),1.2 * 2.0 * ((rand() % 256)/ 256.0),2.0 * ((rand() % 256) / 256.0 - 0.5));
        particle_colors[i] = vec4(rand()%256/256.0,rand()%256/256.0,rand()%256/256.0, 1.0);
    }
}

void line(){                            //use three triangles to build three axes
    line_colors[0] = line_colors[1] = line_colors[2] = vertex_colors[1];    //x ais
    line_points[0] = point4(0,0.05, 0 ,1.0);
    line_points[1] = point4(10,0,0, 1.0);
    line_points[2] = point4(10,0,0, 1.0);
    
    line_colors[3] = line_colors[4] = line_colors[5] = vertex_colors[5];    //y axis
    line_points[3] = point4(0,0.05,0,1.0);
    line_points[4] = point4(0,10,0,1.0);
    line_points[5] = point4(0,10,0,1.0);
    
    line_colors[6] = line_colors[7] = line_colors[8] = vertex_colors[4];    //z axis
    line_points[6] = point4(0,0.05,0,1.0);
    line_points[7] = point4(0,0,10,1.0);
    line_points[8] = point4(0,0,10,1.0);
}

void fileinput(){
    int n = 0;
    int index = 0;
    float x, y, z = 0.0;
    FILE *fp = NULL;
    printf("Please select file:\n");
    scanf("%i",&index);
    switch (index) {
        case 1:
            fp = fopen("sphere.8", "r");
            break;
        case 2:
            fp = fopen("sphere.128", "r");
            break;
        case 3:
            fp = fopen("sphere.256", "r");
            break;
        case 4:
            fp = fopen("sphere.1024", "r");
            break;
        default:
            break;
    }
    fscanf(fp, "%i", &sphere_total);                    //get total number of surfaces
    sphere_NumVertices = sphere_total * 3;              //3 vertices for each triangle
    for (int i = 0; i<sphere_total; i++){
        sphere_colors[i*3] = sphere_colors[i*3+1] = sphere_colors[i*3+2] = vertex_colors[2];
        projection_colors[i*3] = projection_colors[i*3+1] = projection_colors[i*3+2] = vertex_colors[6];
        fscanf(fp, "%i", &n);                           //get n number for three points to form a surface
        fscanf(fp, "%f %f %f", &x, &y, &z);
        sphere_points[i*n]  = point4(x, y, z, 1.0);         //store points in sphere_points array
        projection_points[i*n] = point4(x, y, z, 1.0);       //store points in projection_points array
        fscanf(fp, "%f %f %f", &x, &y, &z);
        sphere_points[i*n+1] = point4(x, y, z, 1.0);
        projection_points[i*n+1] = point4(x, y, z, 1.0);
        fscanf(fp, "%f %f %f", &x, &y, &z);
        sphere_points[i*n+2] = point4(x, y, z, 1.0);
        projection_points[i*n+2] = point4(x, y, z, 1.0);
        
        vec4 u = sphere_points[i * n + 1] - sphere_points[ i * n ];
        vec4 v = sphere_points[i * n + 2] - sphere_points[ i * n ];
        vec3 normal = normalize(cross(u, v));
        sphere_normals_flat[i*n] = sphere_normals_flat[i*n+1] =sphere_normals_flat[i*n + 2] = normal;
        
        point4 origin = point4(0.0, 0.0, 0.0, 1.0);  //needs improvement
        vec3 normal0 = vec3((origin - sphere_points[i*n]).x, (origin-sphere_points[i*n]).y , (origin-sphere_points[i*n]).z);
        sphere_normals_smooth[i*n] = normal0;
        vec3 normal1 = vec3((origin - sphere_points[i*n+1]).x, (origin-sphere_points[i*n+1]).y , (origin-sphere_points[i*n+1]).z);
        sphere_normals_smooth[i*n+1] = normal1;
        vec3 normal2 = vec3((origin - sphere_points[i*n+2]).x, (origin-sphere_points[i*n+2]).y , (origin-sphere_points[i*n+2]).z);
        sphere_normals_smooth[i*n+2] = normal2;
    }
}

void init()
{
    image_set_up();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    /*--- Create and Initialize a texture object ---*/
    glGenTextures(1, &texName);      // Generate texture obj name(s)
    //2D texture
    glActiveTexture( GL_TEXTURE0 );  // Set the active texture unit to be 0
    glBindTexture(GL_TEXTURE_2D, texName); // Bind the texture to this texture unit
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
    //1D texture
    glActiveTexture( GL_TEXTURE1 );  // Set the active texture unit to be 1
    glBindTexture(GL_TEXTURE_1D, texName); // Bind the texture to this texture unit
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth, 0, GL_RGBA, GL_UNSIGNED_BYTE, stripeImage);
    
    particle();
    glGenBuffers(1, &particle_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particle_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color4)*particle_number+sizeof(vec3)*particle_number, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color4)*particle_number, particle_colors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(color4)*particle_number,sizeof(vec3)*particle_number, velocity);
    
    fileinput();
    glGenBuffers(1, &sphere_normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_normal_buffer);       //set sphere buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*sphere_NumVertices+sizeof(color4)*sphere_NumVertices, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*sphere_NumVertices, sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*sphere_NumVertices,sizeof(color4)*sphere_NumVertices, sphere_colors);
    
    glGenBuffers(1, &sphere_shading_buffer_flat);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_shading_buffer_flat);       //set sphere buffer with flat shading
    glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*sphere_NumVertices+sizeof(vec3)*sphere_NumVertices, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*sphere_NumVertices, sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*sphere_NumVertices,sizeof(vec3)*sphere_NumVertices, sphere_normals_flat);
    
    glGenBuffers(1, &sphere_shading_buffer_smooth);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_shading_buffer_smooth);       //set sphere buffer with smooth shading
    glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*sphere_NumVertices+sizeof(vec3)*sphere_NumVertices, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*sphere_NumVertices, sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*sphere_NumVertices,sizeof(vec3)*sphere_NumVertices, sphere_normals_smooth);
    
    
    glGenBuffers(1, &projection_buffer);                //set shadow buffer
    glBindBuffer(GL_ARRAY_BUFFER, projection_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*sphere_NumVertices+sizeof(color4)*sphere_NumVertices, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*sphere_NumVertices, projection_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*sphere_NumVertices,sizeof(color4)*sphere_NumVertices, projection_colors);
    
    floor();
    glGenBuffers(1, &floor_shading_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_shading_buffer);        //set floor buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*floor_NumVertices+sizeof(vec3)*floor_NumVertices+sizeof(vec2)*floor_NumVertices, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*floor_NumVertices, floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*floor_NumVertices, sizeof(vec3)*floor_NumVertices, floor_normals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*floor_NumVertices+sizeof(vec3)*floor_NumVertices, sizeof(vec2)*floor_NumVertices, floor_textCoord);
    
    glGenBuffers(1, &floor_normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_normal_buffer);        //set floor buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*sphere_NumVertices+sizeof(color4)*sphere_NumVertices, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*floor_NumVertices, floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*floor_NumVertices,sizeof(color4)*floor_NumVertices, floor_colors);
    
    line();
    glGenBuffers(1, &line_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, line_buffer);         //set line buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(point4)*line_NumVertices+sizeof(color4)*line_NumVertices, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(point4)*line_NumVertices, line_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(point4)*line_NumVertices,sizeof(color4)*line_NumVertices, line_colors);
    
    program = InitShader("vshader53.glsl", "fshader53.glsl");
    program_firework = InitShader("vshaderparticle.glsl", "fshaderparticle.glsl");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.529,0.807,0.92, 1.0);                //set background color
    glLineWidth(1.0);
   
}

void drawobj_normal(GLuint buffer, int num_vertices){          //sent data to buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4) * num_vertices));
    
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);
}

void drawobj_particle(GLuint buffer, int num_vertices){
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    GLuint vColor = glGetAttribLocation(program_firework, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    GLuint velocity = glGetAttribLocation(program_firework, "velocity");
    glEnableVertexAttribArray(velocity);
    glVertexAttribPointer(velocity, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(color4) * num_vertices));
    
    glDrawArrays(GL_POINTS, 0, num_vertices);
    
    glDisableVertexAttribArray(vColor);
    glDisableVertexAttribArray(velocity);
}
void drawobj_shading(GLuint buffer, int num_vertices){          //sent data to buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4) * num_vertices));
 
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vNormal);
}

void drawobj_texture(GLuint buffer, int num_vertices){          //sent data to buffer
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4) * num_vertices));
    
    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4) * num_vertices+ sizeof(vec3) * num_vertices));
    
    glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vNormal);
    glDisableVertexAttribArray(vTexCoord);
}
void SetUp_Lighting_Uniform_Vars_floor(void){
    
    glUniform4fv( glGetUniformLocation(program, "global_AmbientProduct"),
                 1, global_ambient_product_floor );
    
    glUniform4fv( glGetUniformLocation(program, "Directional_AmbientProduct"),
                 1, directional_ambient_product_floor );
    glUniform4fv( glGetUniformLocation(program, "Directional_DiffuseProduct"),
                 1, directional_diffuse_product_floor );
    glUniform4fv( glGetUniformLocation(program, "Directional_SpecularProduct"),
                 1, directional_specular_product_floor );
    
    glUniform4fv( glGetUniformLocation(program, "Positional_SpecularProduct"),
                 1, positional_specular_product_floor );
    glUniform4fv( glGetUniformLocation(program, "Positional_DiffuseProduct"),
                 1, positional_diffuse_product_floor );
    glUniform4fv( glGetUniformLocation(program, "Positional_AmbientProduct"),
                 1, positional_ambient_product_floor );
}

void SetUp_Lighting_Uniform_Vars_sphere(mat4 mv){
    glUniform4fv( glGetUniformLocation(program, "global_AmbientProduct"),
                 1, global_ambient_product_sphere );
    
    glUniform4fv( glGetUniformLocation(program, "Directional_AmbientProduct"),
                 1, directional_ambient_product_sphere );
    glUniform4fv( glGetUniformLocation(program, "Directional_DiffuseProduct"),
                 1, directional_diffuse_product_sphere );
    glUniform4fv( glGetUniformLocation(program, "Directional_SpecularProduct"),
                 1, directional_specular_product_sphere );
    
    glUniform4fv( glGetUniformLocation(program, "Positional_AmbientProduct"),
                 1, positional_ambient_product_sphere );
    glUniform4fv( glGetUniformLocation(program, "Positional_DiffuseProduct"),
                 1, positional_diffuse_product_sphere );
    glUniform4fv( glGetUniformLocation(program, "Positional_SpecularProduct"),
                 1, positional_specular_product_sphere );
    
    vec4 light_position_eyeFrame = mv * positional_light_position;
    vec4 lightTowards_eyeframe = mv * spotlightTowards;
    
    vec4 Lf = normalize(lightTowards_eyeframe - light_position_eyeFrame);
    glUniform4fv( glGetUniformLocation(program, "Directional_Light_Direction"),
                 1, directional_light_direction);
    glUniform4fv( glGetUniformLocation(program, "Positional_Light_Position"),
                 1, light_position_eyeFrame);
    glUniform4fv(glGetUniformLocation(program, "Lf"),
                 1, Lf );
    
    glUniform1f(glGetUniformLocation(program, "Shininess"),
                sphere_shininess);
    glUniform1f(glGetUniformLocation(program, "ConstAtt"),
                const_att);
    glUniform1f(glGetUniformLocation(program, "LinearAtt"),
                linear_att);
    glUniform1f(glGetUniformLocation(program, "QuadAtt"),
                quad_att);
    glUniform1f(glGetUniformLocation(program, "Cutoff"),
                cut_off );
    glUniform1f(glGetUniformLocation(program, "SpotLightExponent"),
                exponent );
}


void display(void){
    GLuint model_view;
    GLuint projection;
    GLuint model_view_firework;
    GLuint projection_firework;
    
    t = (float)glutGet(GLUT_ELAPSED_TIME);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    
    glUniform1i(glGetUniformLocation(program,  "lightFlag"), lightFlag);
    glUniform1i(glGetUniformLocation(program,  "sourceFlag"), sourceFlag);
    glUniform1i(glGetUniformLocation(program,  "fogFlag"),  0);
    glUniform1i(glGetUniformLocation(program,  "verticalFlag"),  verticalFlag);
    glUniform1i(glGetUniformLocation(program,  "coordinateFlag"),  coordinateFlag);
    glUniform1i(glGetUniformLocation(program,  "textureFlag_sphere"),  textureFlag_sphere);
    glUniform1i(glGetUniformLocation(program,  "latticeFlag"),  latticeFlag);
    glUniform1i(glGetUniformLocation(program,  "uprightFlag"),  uprightFlag);
    
    model_view = glGetUniformLocation(program, "ModelView");
    projection = glGetUniformLocation(program, "Projection");
    
    mat4 p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p);
    
    glUniform1i( glGetUniformLocation(program, "texture_2D"), 0 );
    glUniform1i( glGetUniformLocation(program, "texture_1D"), 1 );
    
    vec4   at(0.0, 0.0, 0.0, 1.0);
    vec4   up(0.0, 1.0, 0.0, 0.0);

    mat4 mv = LookAt(eye, at, up);                          //set lookAt function
    
    SetUp_Lighting_Uniform_Vars_sphere(mv);                 //set up uniform variable
    glUniform4fv(glGetUniformLocation(program, "Eye"), 1, eye);
    glUniform1i(glGetUniformLocation(program, "textureFlag_sphere"),  0);
    glUniform1i(glGetUniformLocation(program, "textureFlag_floor"),  0);
    glUniform1i(glGetUniformLocation(program, "latticeFlag"), 0);
    glUniform1i(glGetUniformLocation(program, "normalFlag"), 0);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);         //pass axis parameter to buffer
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawobj_normal(line_buffer, line_NumVertices);          //draw axis
    
    mat4 mv_sphere; mat4 mv_shadow;

    float distance = 2 * M_PI * angle / 360.0;              //compute the total rolling distance
    
    vec4 a(12,14,0,0);vec4 b(0,0,0,0); vec4 c(0,3,12,0); vec4 d(0,-1,0,12);     //set projection matrix
    mat4 projection_matrix = mat4(a,b,c,d);
    
    //switch case for three segments
    switch (segmentFlag) {
        case 1:         //segment AB
            mv_sphere = mv * Translate(-4.0 + 3 * distance / sqrt(73), 1.0, 4.0 - 8 * distance / sqrt(73)) * Rotate(angle, -8, 0, -3) * M;
            mv_shadow = mv * projection_matrix * Translate(-4.0 + 3 * distance / sqrt(73), 1.0, 4.0 - 8 * distance / sqrt(73)) * Rotate(angle, -8, 0, -3) * M;
            break;
            
        case 2:         //segment BC
            mv_sphere = mv * Translate(-1.0 + 4 * distance / sqrt(97), 1.0, -4.0 + 9 * distance / sqrt(97)) * Rotate(angle, 9, 0, -4) * M;
            mv_shadow = mv * projection_matrix *Translate(-1.0 + 4 * distance / sqrt(97), 1.0, -4.0 + 9 * distance / sqrt(97)) * Rotate(angle, 9, 0, -4) * M;
            break;
            
        case 3:         //segment CA
            mv_sphere = mv * Translate( 3.0 - 7 * distance / sqrt(50), 1.0,  5.0 - 1 * distance / sqrt(50)) * Rotate(angle, -1, 0, 7) * M;
            mv_shadow = mv * projection_matrix *Translate( 3.0 - 7 * distance / sqrt(50), 1.0,  5.0 - 1 * distance / sqrt(50)) * Rotate(angle, -1, 0, 7) * M;
            break;
    }
    if (distance >= sqrt(73) and segmentFlag == 1){         //rolling to the end of AB
        M = Rotate(angle, -8, 0, -3) * M;
        angle = 0.0;
        segmentFlag = 2;
    }
    else if (distance >= sqrt(97) and segmentFlag == 2 ){   //rolling to the end of BC
        M = Rotate(angle, 9, 0, -4) * M;
        angle = 0.0;
        segmentFlag = 3;
    }
    else if (distance >= sqrt(50) and segmentFlag == 3 ){   //rolling to the end of CA
        M = Rotate(angle, -1, 0, 7) * M;
        angle = 0.0;
        segmentFlag = 1;
    }
    glUniform1i(glGetUniformLocation(program,  "fogFlag"),  fogFlag);
    glUniform1i(glGetUniformLocation(program, "latticeFlag"), latticeFlag);
    glUniform1i(glGetUniformLocation(program, "textureFlag_sphere"),  textureFlag_sphere);
    mat3 normal_matrix_sphere = NormalMatrix(mv_sphere, 0);
    glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"), 1, GL_TRUE, normal_matrix_sphere);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv_sphere);  //pass sphere parameter to buffer
    if (wireFlag == 1){                                     //control the wire frame tranformation
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glUniform1i(glGetUniformLocation(program, "textureFlag_sphere"),  0);
    }else{
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (lightFlag == 1 && wireFlag == 0){                   //if enable lighting and solid sphere
        normalFlag = 1;
        glUniform1i(glGetUniformLocation(program, "normalFlag"), normalFlag);
        if (shadingFlag == 0)                               //if flat shading
            drawobj_texture(sphere_shading_buffer_flat, sphere_NumVertices);
        else                                                //if smooth shading
            drawobj_texture(sphere_shading_buffer_smooth, sphere_NumVertices);
    } else {
        glUniform1i(glGetUniformLocation(program, "normalFlag"), 0);
        drawobj_normal(sphere_normal_buffer, sphere_NumVertices);   //draw the original sphere with color
    }
    glUniform1i(glGetUniformLocation(program, "textureFlag_sphere"),  0);
    glUniform1i(glGetUniformLocation(program,  "latticeFlag"),  0);
    
    /* enable blending */
    if (blendFlag == 1)
        glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);                                  //disable writing to z buffer
    if (textureFlag_floor == 1)
        glUniform1i(glGetUniformLocation(program, "textureFlag_floor"),  textureFlag_floor);
    SetUp_Lighting_Uniform_Vars_floor();
    mat3 normal_matrix_floor = NormalMatrix(mv, 0);         //pass normal matrix
    glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"), 1, GL_TRUE, normal_matrix_floor);
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);         //draw ground without z-buffer writing
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (lightFlag == 1){                                    //use shading
        glUniform1i(glGetUniformLocation(program, "normalFlag"), 1);
        drawobj_texture(floor_shading_buffer, floor_NumVertices);
    }else{                                                  //original setting
        glUniform1i(glGetUniformLocation(program, "normalFlag"), 0);
        drawobj_normal(floor_normal_buffer, floor_NumVertices);
    }
    glUniform1i(glGetUniformLocation(program,  "latticeFlag"),  latticeFlag);
    glUniform1i(glGetUniformLocation(program, "textureFlag_floor"),  0);
//    glUniform1i(glGetUniformLocation(program,  "fogFlag"),  0);
    /* blending function before drawing the shadow */
    if (blendFlag == 1)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (shadowFlag == 1 && eye[1] >= 0.0){
        glUniformMatrix4fv(model_view, 1, GL_TRUE, mv_shadow);  //pass projection parameter to buffer
        if (wireFlag == 1){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }else{
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        glUniform1i(glGetUniformLocation(program, "normalFlag"), 0);
        drawobj_normal(projection_buffer, sphere_NumVertices);  //draw sphere projection
    }
    glUniform1i(glGetUniformLocation(program,  "latticeFlag"),  0);
    /* disable belndding after drawing shadow */
    if (blendFlag == 1)
        glDisable(GL_BLEND);
    
    glDepthMask(GL_TRUE);                                       //enable wrting to z buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);        //disable writing to frame buffer
    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);             //pass floor parameter to buffer
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (lightFlag == 1){
        normalFlag = 1;
        glUniform1i(glGetUniformLocation(program, "normalFlag"), normalFlag);
        drawobj_texture(floor_shading_buffer, floor_NumVertices);//draw floor with shading
    }else{
        normalFlag = 0;
        glUniform1i(glGetUniformLocation(program, "normalFlag"), normalFlag);
        drawobj_normal(floor_normal_buffer, floor_NumVertices);  //draw floor with original color
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);             //enable writing to frame buffer
    if (fireworkFlag == 1){
    glUseProgram(program_firework);
    model_view_firework = glGetUniformLocation( program_firework, "ModelView" );
    projection_firework = glGetUniformLocation( program_firework, "Projection" );
    p = Perspective(fovy, aspect, zNear, zFar);
    mv = LookAt(eye, at, up);
    glUniformMatrix4fv(projection_firework, 1, GL_TRUE, p);
    glUniformMatrix4fv(model_view_firework, 1, GL_TRUE, mv);
    glUniform1f(glGetUniformLocation(program_firework, "t"), findMod(t-t_sub, Tmax));
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glPointSize(3.0);
    drawobj_particle(particle_buffer, particle_number);
    }
    glutSwapBuffers();
}


void reshape(int width, int height){
    glViewport(0, 0, width, height);
    aspect = (GLfloat)width / (GLfloat)height;
    glutPostRedisplay();
}

void idle(void){
    angle += 4.0;                                          //change to control the speed
    
    glutPostRedisplay();
}

void demo_menu(int id){                                     //menu function
    switch (id) {
        case 1:
            eye = init_eye;                                 //set default eye
            animationFlag = 1;                              //resume the rolling animation
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            else
                glutIdleFunc(NULL);
            break;
        case 2:                                             //select wire frame sphere
            animationFlag = 1;
            wireFlag = 1;
            normalFlag = 0;
            glUniform1i(glGetUniformLocation(program, "normalFlag"), normalFlag);
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 3:
            exit( EXIT_SUCCESS );                           //exit from the program
            break;
    }
}

void shadow_menu(int id){                                   //shadow menu function
    switch (id) {
        case 1:
            animationFlag = 1;
            shadowFlag = 1;                                 //enable shadow display
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 2:
            animationFlag = 1;
            shadowFlag = 0;                                 //disable shadow display
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
    }
}

void fog_menu(int id){                                   //shadow menu function
    switch (id) {
        case 1:
            fogFlag = 0;
            animationFlag = 1;
            shadowFlag = 1;                                 //enable shadow display
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 2:
            fogFlag = 1;
            animationFlag = 1;
            shadowFlag = 1;                                 //disable shadow display
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 3:
            fogFlag = 2;
            animationFlag = 1;
            shadowFlag = 1;                                 //disable shadow display
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 4:
            fogFlag = 3;
            animationFlag = 1;
            shadowFlag = 1;                                 //disable shadow display
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
    }
}

void blending_menu(int id){                                  //shading option menu function
    switch (id) {
        case 1:
            animationFlag = 1;
            blendFlag = 0;                                   //use solid sphere
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 2:
            animationFlag = 1;
            blendFlag = 1;
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
    }
}
void firework_menu(int id){
    switch (id) {
        case 1:
            animationFlag = 1;
            fireworkFlag = 0;                                   //use solid sphere
            t_sub = (float)glutGet(GLUT_ELAPSED_TIME);
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 2:
            animationFlag = 1;
            fireworkFlag = 1;
            t_sub = (float)glutGet(GLUT_ELAPSED_TIME);
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
    }
}

void texture_menu(int id){                                  //shading option menu function
    switch (id) {
        case 1:
            animationFlag = 1;
            textureFlag_floor = 0;                                   //use solid sphere
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 2:
            animationFlag = 1;
            textureFlag_floor = 1;
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
    }
}

void texture_sphere_menu(int id){                                  //shading option menu function
    switch (id) {
        case 1:
            animationFlag = 1;
            textureFlag_sphere = 0;                                   //use solid sphere
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 2:
            animationFlag = 1;
            textureFlag_sphere = 1;

            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 3:
            animationFlag = 1;
            textureFlag_sphere = 2;

            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
    }
}

void lightsource_menu(int id){                              //shadow menu function
    switch (id) {
        case 1:
            animationFlag = 1;
            sourceFlag = 2;                                 //set up light source to spot light (2)
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 2:
            animationFlag = 1;
            sourceFlag = 1;                                 //set up light source to point source (1)
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
    }
}

void lighting_menu(int id){                                 //enable lighting menu function
    switch (id) {
        case 1:
            animationFlag = 1;
            lightFlag = 1;                                  //enable lighting
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 2:
            animationFlag = 1;
            lightFlag = 0;                                  //disable lighting
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
    }
}

void shading_menu(int id){                                  //shading option menu function
    switch (id) {
        case 1:
            animationFlag = 1;
            shadingFlag = 0;                                //flat shading
            wireFlag = 0;                                   //use solid sphere
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
        case 2:
            animationFlag = 1;
            shadingFlag = 1;                                //smooth shading
            wireFlag = 0;
            if(beginFlag == 1 && animationFlag == 1)
                glutIdleFunc(idle);
            break;
    }
}
void myMouse(int button, int state, int x, int y){          //mouse function
    //when B has been pressed and right button is pressed, rolling
    if (beginFlag == 1 && button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
        animationFlag = 1 - animationFlag;                  //control animation with animationFlag
        if (beginFlag == 1 && animationFlag == 1) glutIdleFunc(idle); //when two conditions are satisfied, idle will be called
        else                                      glutIdleFunc(NULL);
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){ //left mouse button function
        shadowMenu = glutCreateMenu(shadow_menu);
        glutAddMenuEntry("Yes", 1);
        glutAddMenuEntry("No", 2);
        lightMenu = glutCreateMenu(lighting_menu);
        glutAddMenuEntry("Yes", 1);
        glutAddMenuEntry("No", 2);
        shadingMenu = glutCreateMenu(shading_menu);
        glutAddMenuEntry("Flat Shading", 1);
        glutAddMenuEntry("Smooth Shading", 2);
        lightsourceMenu = glutCreateMenu(lightsource_menu);
        glutAddMenuEntry("Spot Light", 1);
        glutAddMenuEntry("Point Source", 2);
        fogMenu = glutCreateMenu(fog_menu);
        glutAddMenuEntry("No fog", 1);
        glutAddMenuEntry("Linear", 2);
        glutAddMenuEntry("Exponential", 3);
        glutAddMenuEntry("Exponential square", 4);
        blendingMenu = glutCreateMenu(blending_menu);
        glutAddMenuEntry("No", 1);
        glutAddMenuEntry("Yes", 2);
        textureMenu = glutCreateMenu(texture_menu);
        glutAddMenuEntry("No", 1);
        glutAddMenuEntry("Yes", 2);
        textureSphereMenu = glutCreateMenu(texture_sphere_menu);
        glutAddMenuEntry("No", 1);
        glutAddMenuEntry("Yes - Contour Lines", 2);
        glutAddMenuEntry("Yes - Checkerboard", 3);
        fireworkMenu = glutCreateMenu(firework_menu);
        glutAddMenuEntry("No", 1);
        glutAddMenuEntry("Yes", 2);
        glutCreateMenu(demo_menu);                          //set up pop-up keyboard function
        glutAddMenuEntry("Default View Point", 1);
        glutAddSubMenu("Shadow", shadowMenu);
        glutAddSubMenu("Enable lighting",  lightMenu);
        glutAddMenuEntry("Wire Frame Sphere", 2);
        glutAddSubMenu("Shading", shadingMenu);
        glutAddSubMenu("Light Source", lightsourceMenu);
        glutAddSubMenu("Blending Shadow", blendingMenu);
        glutAddSubMenu("Texture Mapped Ground", textureMenu);
        glutAddSubMenu("Texture Mapped Sphere", textureSphereMenu);
        glutAddSubMenu("Fog Option", fogMenu);
        glutAddSubMenu("Firework", fireworkMenu);
        glutAddMenuEntry("Quit", 3);
        glutAttachMenu(GLUT_LEFT_BUTTON);                   //bind menu to left button
    }
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){             //keyboard function
    switch (key) {
        case 'B': case 'b':                                 //press B to start
            beginFlag = 1;                                  //change the begin flag to prevent the right button start rolling
            animationFlag = 1;                              //set animation flag to control rolling
            if (beginFlag == 1 && animationFlag == 1) glutIdleFunc(idle);   //only two conditions satisfied, the sphere start rolling
            else                                      glutIdleFunc(NULL);
            break;
        case 'v': case 'V':
            verticalFlag = 0;
            break;
        case 's': case 'S':
            verticalFlag = 1;
            break;
            
        case 'o': case 'O':
            coordinateFlag = 0;
            break;
        case 'e': case 'E':
            coordinateFlag = 1;
            break;
        
        case 't': case 'T':
            uprightFlag = 0;
            break;
        case 'u': case 'U':
            uprightFlag = 1;
            break;
            
        case 'l': case 'L':
            latticeFlag = 1 - latticeFlag;
            break;

        case 'X': eye[0] += 1.0; break;                     //press the keyboard to control the viewer position
        case 'x': eye[0] -= 1.0; break;
        case 'Y': eye[1] += 1.0; break;
        case 'y': eye[1] -= 1.0; break;
        case 'Z': eye[2] += 1.0; break;
        case 'z': eye[2] -= 1.0; break;
    }
    glutPostRedisplay();
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_3_2_CORE_PROFILE);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Sphere Rolling Final");
    
#ifdef __APPLE__ // on macOS
    // Core profile requires to create a Vertex Array Object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
#else           // on Linux or Windows, we still need glew
    /* Call glewInit() and error checking */
    int err = glewInit();
    if (GLEW_OK != err)
    {
        printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err));
        exit(1);
    }
#endif
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMouseFunc(myMouse);                                 //set up mouse and keyboard function
    glutKeyboardFunc(keyboard);
    
    init();
    
    glutMainLoop();
    return 0;
}
