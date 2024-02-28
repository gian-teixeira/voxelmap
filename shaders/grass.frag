#version 300 es
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_tex0;
uniform float u_time;

in vec2 v_texcoord;
out vec4 color;

#define PI 3.1415926538

vec2 bhaskara(float a, float b, float c)
{
    float delta = b*b - 4.0*a*c;
    return vec2(
        (-b - sqrt(delta)) / 2.0*a,
        (-b + sqrt(delta)) / 2.0*a
    );
}

vec2 f(float w, 
       float h,
       float x,
       float theta)
{
    vec2 center = vec2(
        0.5 + w*cos(theta),
        - w*sin(theta)
    );

    float a = h*h*pow(sin(theta), 2.0)
        + 50.0*w*w*pow(cos(theta), 2.0);
    
    float b = sin(theta)*cos(theta)*
        (x-center.x)*(100.0*w*w - 2.0*h*h);
    
    float c = pow(x-center.x,2.0)*
        (h*h*pow(cos(theta), 2.0) 
        + 50.0*w*w*pow(sin(theta), 2.0)) - w*w*h*h;

    vec2 ry = bhaskara(a,b,c);
    
    return vec2(
        min(ry.x,ry.y) + center.y, 
        max(ry.x,ry.y) + center.y
    );
}

float angle(float fac)
{
    return -PI/fac + sin(u_time)/10.0;
}

void main() {
    float w = 0.3;
    float h = -0.27;

    vec2 r[3];
    r[0] = f(w, h, v_texcoord.x, angle(4.0));
    r[1] = f(w, h, v_texcoord.x, angle(3.0));
    r[2] = f(w, h, v_texcoord.x, angle(1.5));

    bool found = false;
    for(int i = 0; i < 1; i++) {
        if (v_texcoord.y >= r[i].x && v_texcoord.y <= r[i].y) {
            color = vec4(0.0, 1.0, 0.5, 1.0);
            float dist = min(
                abs(v_texcoord.y - r[i].x),
                abs(v_texcoord.y - r[i].y)
            );
            if(dist < 0.002) color = vec4(0.0, 0.0, 0.0, 1.0);
            found = true;
        }
    }
    if(!found) color = vec4(1.0, 1.0, 1.0, 1.0);
}