#version 330 core

layout(points) in; 
layout(points) out; 
layout (max_vertices = 40) out;

in vec3 vPositionPass[]; 

out vec3 vPositionOut; 

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
  // gl_Position doesn't matter now, as rendering is discarded, so I don't set it at all

  vPositionOut = vPositionPass[0]; 
  EmitVertex(); 
  EndPrimitive();  
}