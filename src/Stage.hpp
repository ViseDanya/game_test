#include <vector>

class player;
class platform;

class Stage
{
    public:
    Stage();
    ~Stage();

    void addPlayer(player* p);
    void addPlatform(platform* p);
    const std::vector<player*>& getPlayers() const;
    const std::vector<platform*>& getPlatforms() const;
    void CollidePlayers();

    private:
    void CollideDynamicWithStaticVertical();
    void CollideDynamicWithStaticHorizontal();
    void CollideDynamicWithDynamicVertical();
    void CollideDynamicWithDynamicHorizontal();

    std::vector<player*> players;
    std::vector<platform*> platforms;
};