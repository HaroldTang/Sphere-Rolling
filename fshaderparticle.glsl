#version 150

in  vec4 color;
in float position_y;
out vec4 fColor;

void main(){
    if (position_y < 0.1){
        discard;
    }
    fColor = color;
}
