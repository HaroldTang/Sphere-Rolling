/* 
File Name: "vshader53.glsl":
Vertex shader:
  - Per vertex shading for a single point light source;
    distance attenuation is Yet To Be Completed.
  - Entire shading computation is done in the Eye Frame.
*/

#version 150  // YJC: Comment/un-comment this line to resolve compilation errors
                 //      due to different settings of the default GLSL version

in  vec4 vPosition;
in  vec3 vNormal;
in  vec4 vColor;
in  vec2 vTexCoord;

out vec4 color;
out float z;
out vec2 texCoord;
out vec2 discardCoord;

uniform int  normalFlag;    //indicate original color or shading
uniform int  lightFlag;     //indicate lighting is enable or not (original color or shading)
uniform int  sourceFlag;    //indicate light source (spot light & point source)
uniform int  verticalFlag;  //indicate vertical & slanted
uniform int  coordinateFlag;//indicate coordinate system (object & eye space)
uniform int  textureFlag_sphere;
uniform int  textureFlag_floor;
uniform int  uprightFlag;
uniform int  latticeFlag;
uniform vec4 Eye;

uniform vec4 global_AmbientProduct;
uniform vec4 Directional_AmbientProduct, Directional_DiffuseProduct, Directional_SpecularProduct;
uniform vec4 Directional_Light_Direction; //already in eye frame
uniform vec4 Positional_AmbientProduct, Positional_DiffuseProduct, Positional_SpecularProduct;
uniform vec4 Positional_Light_Position; // already transform to eye frame

uniform mat4 ModelView;
uniform mat4 Projection;
uniform mat3 Normal_Matrix; //input normal vector for each outside vertex
uniform float Shininess;

uniform float ConstAtt;     // Constant Attenuation
uniform float LinearAtt;    // Linear Attenuation
uniform float QuadAtt;      // Quadratic Attenuation

uniform float Cutoff;       // spot light Cutooff angle
uniform float SpotLightExponent;//spot light exponent
uniform vec4  Lf;


void main()
{
    vec3 pos = (ModelView * vPosition).xyz;
    vec3 eye = (ModelView * Eye).xyz;
    z = length(pos-eye);
    //for light flag = 0, use original setting
    if ((lightFlag == 0) || (lightFlag == 1 && normalFlag == 0)){
    vec4 vPosition4 = vPosition;
    vec4 vColor4 = vColor;
    gl_Position = Projection * ModelView * vPosition4;
    color = vColor4;
    }
    else{
    // Transform vertex position into eye coordinates
        
    
        
    vec3 L_directional = normalize( - Directional_Light_Direction.xyz );
    vec3 L_positional = normalize( Positional_Light_Position.xyz - pos );
    vec3 E = normalize( -pos );
    vec3 H_directional = normalize( L_directional + E );
    vec3 H_positional = normalize( L_positional + E );
    
    // Transform vertex normal into eye coordinates
    vec3 N = normalize(Normal_Matrix * vNormal);
        
    if ( dot(N, E) < 0 ) N = - N;

    float attenuation_d = 1.0;//
    
 // Compute terms in the illumination equation
    vec4  ambient_d = Directional_AmbientProduct;

    float d = max( dot(L_directional, N), 0.0 );
    vec4  diffuse_d = d * Directional_DiffuseProduct;

    float s = pow( max(dot(N, H_directional), 0.0), Shininess );
    vec4  specular_d = s * Directional_SpecularProduct;
    
    if( dot(L_directional, N) < 0.0 ) {
	specular_d = vec4(0.0, 0.0, 0.0, 1.0);
    }
        
    float distance = length(Positional_Light_Position.xyz - pos);
        
    //point source attenuation
    float attenuation_p = 1.0  / (ConstAtt + LinearAtt * distance + QuadAtt * (distance * distance));
        
    //out of spot light range
    float attenuation_spot = 0.0;
    if (dot(Lf.xyz, normalize(-L_positional)) >= cos(Cutoff))
          //attenuation for spot light
          attenuation_spot = attenuation_p * pow(dot(Lf.xyz, normalize( -L_positional )), SpotLightExponent) / (ConstAtt + LinearAtt * distance + QuadAtt * (distance * distance));

        
    vec4 ambient_p = Positional_AmbientProduct;

    float d_p = max( dot(L_positional, N), 0.0 );
    vec4 diffuse_p = d_p * Positional_DiffuseProduct;
    
    float s_p = pow( max(dot(N, H_positional), 0.0), Shininess );
    vec4 specular_p = s_p * Positional_SpecularProduct;
    
    if( dot(L_positional, N) < 0.0 ) {
        specular_p = vec4(0.0, 0.0, 0.0, 1.0);
    }
    gl_Position = Projection * ModelView * vPosition;

/*--- attenuation below must be computed properly ---*/
        
    if (sourceFlag == 1)
        //ambient light + distant light + spot light
        color = global_AmbientProduct + attenuation_d * (ambient_d + diffuse_d + specular_d) + attenuation_p * (ambient_p + diffuse_p + specular_p );
    else if (sourceFlag == 2)
        //only ambient light and
        color = global_AmbientProduct + attenuation_d * (ambient_d + diffuse_d + specular_d) + attenuation_spot * (ambient_p + diffuse_p + specular_p );
    else
        //only ambient light and distant light
        color = global_AmbientProduct + attenuation_d * (ambient_d + diffuse_d + specular_d);
    }
    if (latticeFlag == 1){
        if (uprightFlag == 0){
            discardCoord[0] = 0.5 * (vPosition[0] + 1);
            discardCoord[1] = 0.5 * (vPosition[1] + 1);
        }else{
            discardCoord[0] = 0.3 * (vPosition[0] + vPosition[1] + vPosition[2]);
            discardCoord[1] = 0.3 * (vPosition[0] - vPosition[1] + vPosition[2]);
        }
    }
    if (textureFlag_sphere != 0){
        vec3 vPosition1 = vPosition.xyz;
        
        if (coordinateFlag == 0){   //object frame
            vPosition1 = vPosition1;
        }else {                     //eye frame
            vPosition1 = (ModelView * vPosition).xyz;
        }
        
        if (textureFlag_sphere == 1){ //1D
            if (verticalFlag == 0){
                texCoord[0] = 2.5 * vPosition1.x;
            }else {
                texCoord[0] = 1.5 * (vPosition1.x + vPosition1.y + vPosition1.z);
            }
        } else if (textureFlag_sphere == 2){ // 2D
            if (verticalFlag == 0){
                texCoord[0] = 0.75 * (vPosition1.x + 1);
                texCoord[1] = 0.75 * (vPosition1.y + 1);
            }else if (verticalFlag == 1){
                texCoord[0] = 0.45 * (vPosition1.x + vPosition1.y + vPosition1.z);
                texCoord[1] = 0.45 * (vPosition1.x - vPosition1.y + vPosition1.z);
            }
        }
        
    }else{
        texCoord = vTexCoord;
    }
}
