class aabb;

class renderable
{
    public:
    virtual ~renderable() = default;
    virtual void render(aabb box) const = 0;
};