 #version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in float inDepth;

layout(location = 0) out float outDepth;

void main() {
    outDepth = inDepth;
    float C = 0.001;
    float w = inDepth;
    float Far = 10000.0;
    gl_FragDepth = min(1.0, log(C*w + 1.0) / log(C*Far + 1.0));
}
