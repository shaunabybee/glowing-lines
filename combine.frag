#version 330 core
out vec4 FragColor;

in vec2 vST;

uniform sampler2D sharpTex;
uniform sampler2D blueLinesTex;
uniform sampler2D blurredTex;

void main()
{
    vec3 color = vec3(0.);

    vec3 sharp = texture(sharpTex, vST).rgb;
    vec3 blueLines = texture(blueLinesTex, vST).rgb;
    vec3 blurred = texture(blurredTex, vST).rgb;

    vec3 background;

    // Add the colors together
    background = blueLines + blurred;
    
    if (sharp.r > 0.f)
    {
        // If there are white lines, render solid white
        color = sharp;
    }
    else
    {
        // Otherwise, render the background
        color = background;
    }

    FragColor = vec4(color, 1.0);
}