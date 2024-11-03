//
// ================================================
// | Grafica pe calculator                        |
#version 330 core

//  Variabile de intrare (dinspre programul principal);
layout (location = 0) in vec4 in_Position;
layout (location = 1) in vec4 texCoord;       
 

//  Variabile de iesire;
out vec2 tex_Coord;     //  Transmite textura (de modificat in Shader.frag); 


//  Variabile uniforme;
uniform mat4 myMatrix;

void main(void)
  {
    gl_Position = myMatrix*in_Position;
    tex_Coord = vec2(texCoord.x, 1-texCoord.y);
   } 
 