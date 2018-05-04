#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D texGameStageRenderResult;
layout(set = 0, binding = 1) uniform sampler2D texGameStageUI;

void main() {
    vec4 rendered = texture(texGameStageRenderResult, UV);
    vec4 ui = texture(texGameStageUI, UV);

    outColor = vec4(mix(rendered.rgb, ui.rgb, ui.a), 1.0);
}
