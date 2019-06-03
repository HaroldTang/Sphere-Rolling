#version 150

in vec4 vColor;
in vec3 velocity;

uniform float t;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform int fireworkFlag;

out vec4 color;
out float position_y;



void main(){
    
     float x = 0.001 * velocity[0] * t;
     float y = 0.1 + 0.001 * velocity[1] * t - 0.5 * 0.00000049 * t * t;
     float z = 0.001 * velocity[2] * t;
    
    vec4 Position = vec4(x, y, z, 1.0);
    
    position_y = y;
    gl_Position = Projection * ModelView * Position;

    color = vColor;
}
