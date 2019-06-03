/* 
File Name: "fshader53.glsl":
           Fragment Shader
*/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
                 //      due to different settings of the default GLSL version
vec4 fogcolor = vec4(0.7, 0.7, 0.7, 0.5);
uniform int fogFlag;
uniform int textureFlag_floor;
uniform int textureFlag_sphere;
uniform int latticeFlag;
float density = 0.09;
uniform sampler2D texture_2D;
uniform sampler1D texture_1D;

in  float z;
in  vec4 color;
in  vec2 texCoord;
in  vec2 discardCoord;
out vec4 fColor;

float f;
void main() 
{
    vec4 result_color = color;
    if (textureFlag_floor == 1)
    {
        result_color = color * texture(texture_2D, texCoord);
    }
    
    if (latticeFlag == 1){
        if (fract(4*discardCoord[0])<0.35&& fract(4*discardCoord[1])<0.35){
            discard;
        }
    }
    
    if (textureFlag_sphere == 1){
        result_color = color * texture(texture_1D, texCoord[0]);
    }else if (textureFlag_sphere == 2){
        vec4 textColor = texture(texture_2D, texCoord);
        if (textColor[0] == 0.0){
            textColor = vec4(0.9, 0.1, 0.1, 1.0);
        }
        result_color = color * textColor;
    }
    
    switch (fogFlag) {
        case 0:
            f = 1.0;
            break;
        case 1:
            f = (18.0-z)/(18.0-0.0);
            break;
        case 2:
            f =exp(-(density*z));
            break;
        case 3:
            f =exp(-(density*z)*(density*z));
            break;
        default:
            break;
    }
    f = clamp(f, 0, 1);
    fColor = mix(fogcolor, result_color, f);
} 

