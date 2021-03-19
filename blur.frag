#version 330 core
out vec4 FragColor;

// Blur shader from: https://learnopengl.com/Advanced-Lighting/Bloom

in vec2 vST;

uniform sampler2D screenTexture;

uniform bool horizontal;
uniform float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 texelSize = 1.0 / textureSize(screenTexture, 0);   
    vec3 color = texture(screenTexture, vST).rgb * weight[0];   // Contribution of this particular fragment
    if (horizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            // Add contributions from neighboring texels
            color += texture(screenTexture, vST + vec2(texelSize.x * i, 0.)).rgb * weight[i];
            color += texture(screenTexture, vST - vec2(texelSize.x * i, 0.)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            // Add contributions from neighboring texels
            color += texture(screenTexture, vST + vec2(0., texelSize.y * i)).rgb * weight[i];
            color += texture(screenTexture, vST - vec2(0., texelSize.y * i)).rgb * weight[i];
        }
    }

    FragColor = vec4(color, 1.0);
}