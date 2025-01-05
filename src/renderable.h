class aabb;

class renderable
{
    public:
    virtual ~renderable() = default;
    virtual void render(aabb box) = 0;
};