class aabb;

class Renderer
{
    public:
    virtual ~Renderer() = default;
    virtual void render(const aabb& box) const = 0;
};