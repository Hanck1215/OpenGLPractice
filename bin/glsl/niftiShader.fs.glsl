#version 410
#extension GL_ARB_explicit_uniform_location : require

layout(location = 0) in vec3 texCoord;
layout(location = 1) in vec3 camPos;
layout(location = 2) in mat4 mvMatrix;

layout(location = 2) uniform mat4 clipPlaneMatrix;

layout(location = 0) out vec4 color;
uniform sampler3D volume;

const int MAX_SAMPLES = 300;	
const vec3 texMin = vec3(0);	//最小紋理座標
const vec3 texMax = vec3(1);	//最大紋理座標

void main(void) {
	vec3 dataPos = texCoord; //光線投射起始點座標
	vec3 lightDir; //光線步進方向

	// 當相機也就是視點位於體渲染物件內部時，起始點應該改為相機視點的位置座標作為起始點
	bool isInTexture = false;
	if(abs(camPos.x) <= 0.5 && abs(camPos.y) <= 0.5 && abs(camPos.z) <= 0.5) {
		isInTexture = true;
	}

	if(isInTexture) {
		lightDir = normalize(camPos - (texCoord - vec3(0.5)));
	}else {
		lightDir = normalize((texCoord - vec3(0.5)) - camPos);
	}
	
	vec3 step = lightDir * 0.005;

	bool stop = false;
	vec4 cumulated = vec4(0);

	// 沿射線方向採樣累積顏色和不透明度
	for(int i = 0; i < MAX_SAMPLES; i++) {
		dataPos = dataPos + step;
		stop = dot(sign(dataPos - texMin), sign(texMax - dataPos)) < 3.0;
		if(stop) { break; }

		vec4 wDataPos = mvMatrix * vec4(dataPos - vec3(0.5), 1.0); // 得到此材質取樣點在世界座標中的位置
		vec4 cDataPos = inverse(clipPlaneMatrix) * wDataPos; // 取得材質採樣點在裁切平面坐標系中的位置

		vec4 vSample = texture(volume, dataPos); // 取得採樣點顏色值和不透明度

		if (cDataPos.z > 0 || wDataPos.z > 0) { // 如果採樣點在裁切平面或相機後方，就不顯示
			vSample.a = 0.0;
		}else {
			vSample.a = vSample.r;
		}

		cumulated[0] += vSample.r * vSample[3] * (1-cumulated[3]);
		cumulated[1] += vSample.g * vSample[3] * (1-cumulated[3]);
		cumulated[2] += vSample.b * vSample[3] * (1-cumulated[3]);
		cumulated[3] += vSample.a * (1-cumulated[3]);

		if(cumulated[3] > 0.99)
			break;
	}

	color = vec4(cumulated.r, cumulated.r, cumulated.r, cumulated.a);
}