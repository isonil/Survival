#ifndef APP_ELECTRICITY_SYSTEM_HPP
#define APP_ELECTRICITY_SYSTEM_HPP

#include <vector>
#include <memory>
#include <unordered_map>

namespace engine { namespace app3D { class Model; } }

namespace app
{

class Structure;

class ElectricitySystem
{
public:
    enum class Overlay
    {
        None,
        GraySymbols,
        GraySymbolsAndLines,
        NormalSymbolsAndLines,
        RedSymbolsAndLines
    };

    // we need create() method, because this class needs to know its memory owner
    static std::shared_ptr <ElectricitySystem> create();

    void update();
    void draw3D();

    void setOverlay(Overlay overlay);

    int getRequiredPower() const;
    int getGeneratedPower() const;
    int getUnusedPower() const;
    bool isWorking() const;

    bool isStructurePresent(const Structure &structure);
    void removeStructure(const Structure &structure);
    void addStructure(std::shared_ptr <Structure> structure, std::weak_ptr <Structure> optionalConnection = {});
    bool isEmpty() const;

    ~ElectricitySystem();

    static const float k_maxConnectionLength;

private:
    struct Member
    {
        std::shared_ptr <Structure> structure;
        std::shared_ptr <engine::app3D::Model> symbol;

        std::vector <int> connections; // contains entityIDs

        // working vars
        int group_workingVar{};
    };

    ElectricitySystem();

    static Overlay combineOverlays(Overlay oldOverlay, Overlay newOverlay);
    static int getOverlayPriority(Overlay overlay);

    std::unordered_map <int, Member>::iterator removeMember(std::unordered_map <int, Member>::iterator it);
    void recache();
    void divideGraphIfBrokeConnection();
    void divideGraphIfBrokeConnection_setGroup(Member &member, int group);

    std::weak_ptr <ElectricitySystem> m_ptr;

    Overlay m_overlay_forUpdate;
    Overlay m_overlay_forRender;
    int m_cachedRequiredPower;
    int m_cachedGeneratedPower;
    std::unordered_map <int, Member> m_members; // key: entityID
};

} // namespace app

#endif // APP_ELECTRICITY_SYSTEM_HPP
