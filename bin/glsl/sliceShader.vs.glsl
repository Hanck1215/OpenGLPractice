#version 410
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec3 iv3Vertex;

out VSOUT {
	vec4 color;
} vsout;

layout(location = 0) uniform mat4 mvMatrix;
layout(location = 1) uniform mat4 projMatrix;
layout(location = 2) uniform int selected;

void main() {
	gl_Position = projMatrix * mvMatrix * vec4(iv3Vertex, 1.0);
	if(selected == 1) {
		vsout.color = vec4(0.0, 0.0, 0.7, 0.2);
	}else {
		vsout.color = vec4(0.0, 0.0, 0.0, 0.2);
	}
	
}