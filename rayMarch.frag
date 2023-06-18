#version 440 core

uniform vec2 u_mouse_delta;
uniform vec2 u_resolution;
uniform vec3 origin;
uniform float u_mouse_sensitivity;
uniform float u_Time;
uniform int u_steps;

out vec4 fs_color;

const float FOV = 2.0;
uniform int MAX_STEPS = 500;
const float MAX_DIST = 500*2;
const float HIT_DIST = 0.001;

//--------------------------------------------------------SDFs-------------------------------------------------------------------------------
float sdSphere(vec3 p, float r){
    return length(p) - r;
}

float sdCylinder(vec3 p, vec2 dim){
    vec2 d = abs(vec2(length(p.xz),p.y)) - dim;
    return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float sdOctahedron(vec3 p, float s)
{
    p = abs(p);
    float m = p.x+p.y+p.z-s;
    vec3 q;
    if( 3.0*p.x < m ) q = p.xyz;
    else if( 3.0*p.y < m ) q = p.yzx;
    else if( 3.0*p.z < m ) q = p.zxy;
    else return m*0.57735027;

    float k = clamp(0.5*(q.z-q.y+s),0.0,s);
    return length(vec3(q.x,q.y-s+k,q.z-k));
}

float mandelBuilbSDF(vec3 p, int iterations, float power){
    vec3 z = p;
    float dr = 1.0;
    float r = 0;
    for(int i =0; i<iterations; i++){
        r = length(z);
        if(r>2) break;
        float theta = acos(z.z/r) * sin(u_Time * 0.1f) ;
        float phi = atan(z.y/z.x);
        dr = pow(r, power - 1.0) * power * dr + 1.0;
        float zr = pow(r, power);
        theta = theta * power;
        phi = phi * power;
        z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
        z += p;
    }
    return 0.5 * log(r) * r / dr;
}

float smoothMin(float distA, float distB, float blendStrength){
    float h = max(blendStrength-abs(distA-distB), 0)/blendStrength;
    return min(distA, distB) - h*h*h*blendStrength*1/6.0;
}

//--------------------------------------------------------SDF Scene Map-------------------------------------------------------------------------------
float map(vec3 p){
//    p = mod(p, 3.f) - 4.0*0.5;
    //sphere
    float sphere = sdSphere(p,1);
    float sphere2 = sdSphere(p-10.f, 2);
    //mandelbulb
    float frac = mandelBuilbSDF(p, 15, 6);
    //octahedron
    float octa = sdOctahedron(p, 6);
    //cylinder
    float cylinder = sdCylinder(p, vec2(0.2,1));
    //result
//    float res = min(sphere, sphere2);
//    float res = frac * sin(u_Time * pow(p.x,2));
    float res = min(sphere, sphere2);
    return sphere2;
}

//--------------------------------------------------------Calculating normals for light-------------------------------------------------------------------------------
vec3 calcNorm(vec3 pos){
    vec2 e = vec2(0.001f, 0.f);
    return normalize(
        vec3(map(pos+e.xyy)-map(pos-e.xyy),
            map(pos+e.yxy)-map(pos-e.yxy),
            map(pos+e.yyx)-map(pos-e.yyx)
        )
    );
}

//--------------------------------------------------------Shadows-------------------------------------------------------------------------------
float softShadows(in vec3 p, in vec3 lightPos){
    float res = 1.0;
    float dist = 0.01;
    float lightSize = 0.03;
    for (int i = 0; i < MAX_STEPS; i++) {
        float hit = map(p + lightPos * dist).x;
        res = min(res, hit / (dist * lightSize));
        dist += hit;
        if (hit < 0.0001 || dist > 60.0) break;
    }
    return clamp(res, 0.0, 1.0);
}

//----------------------------------------------------------AO----------------------------------------------------------------------------------
float ambientOcclusion(in vec3 p, in vec3 norm){
    float occ = 0.0;
    float weight = 1.0;
    for (int i = 0; i < 8; i++) {
        float len = 0.01 + 0.02 * float(i * i);
        float dist = map(p + norm * len).x;
        occ += (len - dist) * weight;
        weight *= 0.85;
    }
    return 1.0 - clamp(0.6 * occ, 0.0, 1.0);
}

//--------------------------------------------------------Lights data-------------------------------------------------------------------------------
vec3 getLight(in vec3 p, in vec3 rd){
    vec3 lightPos = vec3(0,0,0);
    vec3 L = normalize(lightPos-p);
    vec3 N = calcNorm(p);
    vec3 V = -rd;
    vec3 R = reflect(-L, N);

//    vec3 color = 0.8 * vec3(abs(sin(u_Time*0.1)), 0,abs(cos(u_Time*0.1)));
    vec3 color = vec3(0.2,0.4,0.2);

    vec3 specColor= color + vec3(0.2f);
    vec3 specular = 1.3 * specColor * pow(clamp(dot(R, V), 0.0, 1.0), 10.0);
    vec3 diffuse = 1.9 * color * clamp(dot(L, N), 0.0, 1.0);
    vec3 ambient = 0.15 * color;
    vec3 fresnel = 0.15 * color * pow(1.0 + dot(rd, N), 3.0);

    float shadow = softShadows(p + N * 0.02, normalize(lightPos)) + 0.02;
    // occ
    float occ = ambientOcclusion(p, N);
    // back
    vec3 back = 0.05 * color * clamp(dot(N, -L), 0.0, 1.0);

    return  (back + ambient + fresnel) * occ + (specular * occ + diffuse) * shadow;
}

//--------------------------------------------------------Marching-------------------------------------------------------------------------------
vec4 rayMarch(vec3 ro, vec3 rd){
    float hit, object=0;
    vec3 albedo = vec3(1,0,0);
    vec3 background = vec3(0.3f,0.36f,0.6f);
    int stepThreshold = 40;
    float minDist = 1.f;
    for(int i=0;i<MAX_STEPS;i++){
        vec3 p = ro + object.x * rd;
        hit = map(p);
        object += hit;
        if(abs(hit) < HIT_DIST){
            vec3 colo = getLight(p, rd);
            colo = mix(colo, background, 1.0 - exp(-1e-7 * object.x * object.x * object.x));
            return vec4(colo, 1);
        }
        if(i>stepThreshold){
            return vec4(1);
        }

        if(abs(hit)>MAX_DIST)   break;
    }
    return vec4(0.3f, 0.36f, 0.6f, rd.y+1.f);
}



//-----------------------------------------------------Cam rot and rendering-----------------------------------------------------------
mat3 camRot(vec2 angle){
    vec2 c = cos(angle);
    vec2 s = sin(angle);

    return mat3(
        c.y      ,  0.f,      -s.y,
        s.y * s.x,  c.x, c.y * s.x,
        s.y * c.x, -s.x, c.y * c.x
    );
}

vec3 rayDirection(in vec2 uv){
    vec3 direction = normalize(vec3(uv, 1.5f));

    return direction;
}

void render(inout vec4 col, in vec2 uv){
    vec3 ro = vec3(0,0,0);
    vec3 rd = rayDirection(uv);

    mat3 rot = camRot((u_mouse_delta - u_resolution * 0.5).yx * vec2(u_mouse_sensitivity));
    if(u_mouse_delta.x!=0 && u_mouse_delta.y!=0){
        rd = rot * rd;
    }
    ro = origin;

    col = rayMarch(ro, rd);
}

void main(){
    vec2 uv = (2.0 * gl_FragCoord.xy - u_resolution.xy) / u_resolution.y;

    vec4 col;
    render(col, uv);

    fs_color = col;
}