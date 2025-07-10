#version 410
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoord;

layout(location = 0) uniform mat4 mvMatrix;
layout(location = 1) uniform mat4 projMatrix;

layout(location = 0) out vec3 texCoordOut;
layout(location = 1) out vec3 camPosOut;
layout(location = 2) out mat4 mvMatrixOut;

void main() {
	gl_Position = projMatrix * mvMatrix * vec4(position, 1.0);
	texCoordOut = texCoord;
	camPosOut = vec3(
		inverse(mvMatrix) * vec4(0.0, 0.0, 0.0, 1.0)
	);
	mvMatrixOut = mvMatrix;
}