#version 440 core

uniform vec2 u_mouse_delta;
uniform vec2 u_resolution;
uniform vec3 origin;
uniform float u_mouse_sensitivity;

out vec4 fs_color;

const float FOV = 2.0;
const int MAX_STEPS = 100;
const float MAX_DIST = 500*2;
const float HIT_DIST = 0.001;

float Voronesque( in vec3 p )
{

    vec3 i  = floor(p+dot(p, vec3(0.333333)) );  p -= i - dot(i, vec3(0.166666)) ;
    vec3 i1 = step(0., p-p.yzx), i2 = max(i1, 1.0-i1.zxy); i1 = min(i1, 1.0-i1.zxy);
    vec3 p1 = p - i1 + 0.166666, p2 = p - i2 + 0.333333, p3 = p - 0.5;
    vec3 rnd = vec3(5.46,62.8,164.98); // my tuning
    vec4 v = max(0.5 - vec4(dot(p, p), dot(p1, p1), dot(p2, p2), dot(p3, p3)), 0.);
    vec4 d = vec4( dot(i, rnd), dot(i + i1, rnd), dot(i + i2, rnd), dot(i + 1., rnd) );
    d = fract(sin(d)*1000.)*v*2.;
    v.x = max(d.x, d.y), v.y = max(d.z, d.w);
    return max(v.x, v.y);
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

float fractal(in vec3 p){
    p = cos(p*2.);
    float m = length(p) - 1.6;
    return m + sqrt(Voronesque(p)*0.5);
}

float map(vec3 p){
    p = mod(p, 3.f) - 4.0*0.5;
    //sphere
    float sphereDist = length(p) - 1.0;
    float sphereID = 1.0;
    float sphere = sphereDist;
    //mandelbrot
    float frac = fractal(p);
    //octahedron
    float octa = sdOctahedron(p, 1);
    //result
    float res = octa;
    return res;
}

vec3 calcNorm(vec3 pos){
    vec2 e = vec2(0.001f, 0.f);
    return normalize(
        vec3(map(pos+e.xyy)-map(pos-e.xyy),
            map(pos+e.yxy)-map(pos-e.yxy),
            map(pos+e.yyx)-map(pos-e.yyx)
        )
    );
}

float diffuse_intensity(in vec3 pos, in vec3 normal_dir){
    const vec3 l1 = vec3(0.f, -10.f, 0.f);
    const vec3 l2 = vec3(-5.f, -5.f, -5.f);

    float temp = max(dot(normal_dir,normalize(pos-l1)), dot(normal_dir,normalize(pos-l2)));
    return max(0.1f, temp);
}

vec4 rayMarch(vec3 ro, vec3 rd){
    float hit, object=0;
    for(int i=0;i<MAX_STEPS;i++){
        vec3 p = ro + object.x * rd;
        hit = map(p);
        object += hit;
        if(abs(hit) < HIT_DIST){
            vec3 norm = calcNorm(p);
            return vec4(vec3(2.f, 1.f, 2.f) * diffuse_intensity(p, norm), 1.f);
        }

        if(abs(hit)>MAX_DIST)   break;
    }
    return vec4(0.3f, 0.36f, 0.6f, rd.y+1.f);
}

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