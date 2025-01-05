class vec2
{
    public:
    vec2() = default;
    vec2(float x, float y) : x(x), y(y){};

    vec2 operator+(vec2 const& other) const { return vec2(x + other.x, y + other.y); };
    vec2 operator-(vec2 const& other) const { return vec2(x - other.x, y - other.y); };
    vec2& operator+=(vec2 const& other) { x+=other.x; y+=other.y; return *this; };
    vec2& operator-=(vec2 const& other) { x-=other.x; y-=other.y; return *this; };
    vec2 operator*(float s) const {return vec2(s*x, s*y); }
    friend vec2 operator*(float s, const vec2& obj) { return vec2(s*obj.x, s*obj.y); };



    float x;
    float y;

    static const vec2 right;
    static const vec2 left;
    static const vec2 up;
    static const vec2 down;
    static const vec2 zero;
};