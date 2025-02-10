#include <vector>

class Player;
class platform;

class Stage
{
    public:
    Stage();
    ~Stage();

    void addPlayer(Player* p);
    void addPlatform(platform* p);
    const std::vector<Player*>& getPlayers() const;
    const std::vector<platform*>& getPlatforms() const;
    void CollidePlayers();

    private:
    void CollideDynamicWithStaticVertical();
    void CollideDynamicWithStaticHorizontal();
    void CollideDynamicWithDynamicVertical();
    void CollideDynamicWithDynamicHorizontal();

    std::vector<Player*> players;
    std::vector<platform*> platforms;
};