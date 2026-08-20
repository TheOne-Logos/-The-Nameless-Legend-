// ============================================================
// 文明模拟器
// 版本：2.0.0（修正后）
// 语言：C++14
// ============================================================


// ============================================================
// 文明模拟器 - 第 1 部分：全局定义区与基础数据结构
// ============================================================
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <random>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <cfloat>
#include <climits>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <functional>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <map>

// ============================================================
// 全局常量
// ============================================================
constexpr int WORLD_WIDTH          = 600;
constexpr int WORLD_HEIGHT         = 600;
constexpr int BUFFER_SIZE          = 0;
constexpr int GEN_WIDTH            = WORLD_WIDTH  + 2 * BUFFER_SIZE;
constexpr int GEN_HEIGHT           = WORLD_HEIGHT + 2 * BUFFER_SIZE;
constexpr int WORLD_LAYERS         = 24;
constexpr int CULTURE_DIM          = 4;
constexpr int MAX_TURNS            = 2000;
constexpr int MONTHS_PER_TURN      = 3;
constexpr int INITIAL_KINGDOMS     = 1;
constexpr int MAX_KINGDOM_COUNT    = 30;
constexpr int MAX_SETTLEMENTS_PER_KINGDOM = 200;
constexpr double ARMY_POP_RATIO_LIMIT   = 0.2;
constexpr double ARMY_BASE_LIMIT        = 50.0;
constexpr double MAX_GOLD               = 100000.0;
constexpr double MAX_FOOD               = 100000.0;
constexpr double MAX_ARMY               = 50000.0;
constexpr double MAX_POPULATION         = 1000000.0;
constexpr double MAX_STABILITY          = 100.0;
constexpr double MIN_STABILITY          = 0.0;
constexpr double MAX_TECH_LEVEL         = 10.0;
constexpr int    MAX_CIVILIZATION_LEVEL = 10;
constexpr double ROAD_MAINTENANCE_PER_TILE   = 0.5;
constexpr double ROAD_BUILD_COST_BASE        = 5.0;
constexpr double ROAD_BUILD_COST_SLOPE_FACTOR= 20.0;
constexpr double ROAD_ASTAR_SLOPE_FACTOR     = 10.0;
constexpr double EXPANSION_COST_GOLD         = 2.0;
constexpr double EXPANSION_COST_FOOD         = 1.0;
constexpr double ARMY_FOOD_CONSUMPTION       = 0.5;
constexpr double NEW_SETTLEMENT_COST         = 60.0;
constexpr int    NEW_SETTLEMENT_INITIAL_POP  = 50;
constexpr double RESEARCH_BASE               = 0.05;
constexpr double CIVILIZATION_TECH_BONUS_PER_LEVEL = 0.2;
constexpr double DYNAMIC_KINGDOM_BASE_PROB   = 0.03;
constexpr double CONTROL_RECOVERY_RATE       = 0.05;
constexpr int    CULTURE_SPREAD_INTERVAL     = 5;
constexpr int    CIVILIZATION_CHECK_INTERVAL = 200;
constexpr double AREA_PER_TILE_KM2           = 2500.0;
constexpr int    OUTPUT_CHECKPOINT_INTERVAL  = 20;
constexpr int    ASTAR_MAX_NODES             = 5000;
constexpr int    MAX_EXPANSION_PER_TURN      = 10;
constexpr int    MAX_NEW_SETTLEMENT_PER_TURN = 1;
constexpr int    MAX_BUILDINGS_PER_TURN      = 2;
constexpr int    MAX_RESEARCH_TRIGGER_PER_TURN = 3;
constexpr int    MAX_ROAD_BUILD_PER_TURN     = 1;
constexpr int    MAX_DOMESTIC_EVENTS_PER_TURN = 3;
constexpr int    MAX_HIDDEN_EVENTS_PER_TURN  = 1;
constexpr int    MAX_DISASTERS_PER_TURN      = 1;
constexpr unsigned int SIMULATION_RANDOM_SEED = 12345;
constexpr double ALLIANCE_BREAK_BASE_PROB    = 0.015;
constexpr double ALLIANCE_SCORE_THRESHOLD    = 0.75;
constexpr double ALLY_JOIN_WAR_PROB          = 0.8;
constexpr int    WORLD_WAR_MIN_PARTICIPANTS  = 4;

// ============================================================
// 枚举定义
// ============================================================
enum class Race : int { HUMAN = 0, DWARF, ELF, ORC, HALFLING, NUM_RACES };
enum class Biome : int {
    CORAL_REEF = 0, SHALLOW_OCEAN, MID_OCEAN, DEEP_OCEAN, ABYSS, SEA_ICE,
    DESERT, SAVANNA, GRASSLAND, DECID_FOREST, MIXED_FOREST, BOREAL_FOREST,
    RAINFOREST, WETLAND, MOUNT_GRASS, MOUNT_TUNDRA, BARE_ROCK, GLACIER,
    SNOW_PLAINS, SNOW_FOREST, SNOW_BOREAL, TUNDRA, ICE_CAP, NUM_BIOMES
};
enum class TerrainClass : int {
    ABYSS = 0, DEEP_OCEAN, MID_OCEAN, SHALLOW_OCEAN, COASTAL_PLAIN,
    LOW_PLAIN, HILL, PLATEAU, MOUNTAIN, HIGH_MOUNTAIN, NUM_TERRAIN_CLASSES
};
enum class VegetationClass : int {
    NONE = 0, DESERT, GRASS, FOREST, RAINFOREST, BOREAL, TUNDRA, ICE, WETLAND,
    NUM_VEGETATION_CLASSES
};
enum class CoastType : int { NONE = -1, SAND = 0, CLIFF = 1, FJORD = 2, MARSH = 3 };
enum class VolcanicActivity : int { NONE = 0, DORMANT = 1, ACTIVE = 2 };
enum class WildlifePotential : int { NONE = 0, LOW = 1, MEDIUM = 2, HIGH = 3 };
enum class BedrockType : int {
    SEDIMENTARY = 0, GRANITE, BASALT, LIMESTONE, METAMORPHIC, VOLCANIC,
    SERPENTINE, NUM_BEDROCK_TYPES
};

// 建筑类型枚举
enum class BuildingType : int {
    FARM = 0, GRANARY, IRRIGATION_DITCH, WINDMILL, GREENHOUSE,
    MARKET, MARKET_SQUARE, TRADING_POST, MINT, GUILD_HALL, WAREHOUSE, TAVERN,
    BARRACKS, TRAINING_GROUND, ARCHERY_RANGE, STABLE, SIEGE_WORKSHOP,
    WALL, FORTRESS, WATCHTOWER, MOAT,
    ACADEMY, LIBRARY, UNIVERSITY, OBSERVATORY, SAGE_HALL,
    TEMPLE, CATHEDRAL, THEATER, ARENA, MONUMENT,
    HOSPITAL, ORPHANAGE, BATHHOUSE, AQUEDUCT, WELL,
    HARBOR, SHIPYARD, LIGHTHOUSE, FISHERY,
    ARCANE_LAB, MANA_WELL, ENCHANTED_TOWER,
    MINE,
    KNIGHT_ORDER, ROYAL_ACADEMY, CATHEDRAL_GRAND,
    DWARF_FORGE, DEEP_MINE, MOUNTAIN_FORTRESS, RUNE_HALL,
    TREE_HOUSE, STAR_ALTAR, FOREST_HUNT, MOON_POOL,
    WAR_TOTEM, BLOOD_ALTAR, RAID_CAMP, SHAMAN_HUT,
    HARVEST_INN, PIPE_HOUSE, TUNNEL_WAREHOUSE, GARDEN_DISTRICT,
    NUM_BUILDING_TYPES
};

enum class TechID : int {
    IRRIGATION = 0, CROP_ROTATION, ANIMAL_HUSBANDRY, FERTILIZER, TERRACE_FARMING,
    BRONZE_WORKING, IRON_WORKING, STEEL_WEAPONS, SIEGE_ENGINES, MILITARY_TACTICS,
    FORTIFICATION, MASONRY, ARCHITECTURE, MACHINERY, ADVANCED_TOOLS,
    SAILING, NAVIGATION, SHIPBUILDING, DEEP_SEA_FISHING,
    MAGIC_BASICS, ARCANE_RESEARCH, ELEMENTAL_MAGIC, ENCHANTMENT, RUNECRAFT,
    LAW_CODE, PHILOSOPHY, LITERATURE, MUSIC, RELIGION_INSTITUTIONS,
    CURRENCY, BANKING, TRADE_ROUTES, GUILDS,
    NUM_TECHS
};

enum class EventType : int {
    POLITICAL = 0, ECONOMIC, SOCIAL, DISASTER, MAGIC_DISASTER, INTERNATIONAL,
    WAR, FOUNDATION, EXTINCTION, ROAD, CULTURAL, TECHNOLOGICAL, RELIGIOUS,
    MIGRATION, NUM_EVENT_TYPES
};

enum class EffectStat : int {
    FOOD = 0, GOLD, STABILITY, POPULATION, ARMY, RESEARCH_POINTS,
    FOOD_MOD, GOLD_MOD, MILITARY_POWER_MOD, DEFENSE_BONUS, ATTACK_BONUS,
    TECH_MOD, GROWTH_MOD, TAX_MOD, BUILDING_COST_MOD, ROAD_SPEED_MOD,
    MAGIC_AFFINITY, REBELLION_RISK_MOD, WAR_EXHAUSTION_MOD, CULTURE_CHANGE_MOD,
    TRADE_MOD, WAR_LOOT_MOD, MAINTENANCE_MOD, POPULATION_CAP_MOD,
    ARMY_MAINTENANCE_MOD, STABILITY_REGEN_MOD, RESEARCH_RATE_MOD,
    EXPANSION_COST_MOD, SETTLEMENT_COST_MOD, NUM_EFFECT_STATS
};

enum class ConditionStat : int {
    FOOD = 0, GOLD, POPULATION, ARMY, STABILITY, TECH_LEVEL, FOOD_PER_POP,
    AT_PEACE, AT_WAR, HAS_BUILDING, HAS_TECH, RACE, CULTURE_SIMILARITY,
    CIVILIZATION_LEVEL, TURN, TAX_RATE,   // 已添加 TAX_RATE
    NUM_CONDITION_STATS
};

// ============================================================
// Balance 命名空间
// ============================================================
namespace Balance {
	constexpr double INITIAL_GOLD          = 500.0;
	constexpr double INITIAL_FOOD          = 500.0;
	constexpr double INITIAL_ARMY          = 50.0;
	constexpr double INITIAL_STABILITY     = 70.0;
	constexpr double INITIAL_TAX_RATE      = 0.2;
	constexpr double INITIAL_TECH_LEVEL    = 1.0;

	constexpr double NEW_KINGDOM_GOLD      = 200.0;
	constexpr double NEW_KINGDOM_FOOD      = 200.0;
	constexpr double NEW_KINGDOM_ARMY      = 30.0;
	constexpr double NEW_KINGDOM_STABILITY = 55.0;

	constexpr double BASE_POP_GROWTH_RATE  = 0.05;
	constexpr int    MIN_SETTLEMENT_POP    = 1;
	constexpr int    DESERTED_POP_THRESHOLD= 5;
	
	constexpr double CARRYING_CAPACITY_PER_TILE = 0.5;    // 每格领土支持人口数
    constexpr double CARRYING_CAPACITY_PER_TECH = 100.0;  // 每级科技增加承载力
    constexpr double CARRYING_FACTOR_SCALE       = 2.0;   // 承载力衰减速度（越大衰减越快）
    constexpr double MIN_GROWTH_RATE             = 0.001; // 最低增长率（0.1%）
    constexpr double MAX_FAMINE_RATIO            = 0.3;   // 最大饥荒死亡率

	constexpr double NPP_TO_FOOD_FACTOR    = 0.005;
	constexpr double MINERAL_GOLD_BASE     = 5.0;
	constexpr double HUNTING_GOLD_FACTOR   = 2.0;
	constexpr double TAX_BASE_RATE         = 0.15;

	constexpr double BASE_BUILDING_COST    = 30.0;
	constexpr double DEFAULT_BUILDING_MAINT= 0.5;

	constexpr double WAR_DECLARATION_BASE  = 0.03;
	constexpr double PEACE_BASE_PROB       = 0.05;
	constexpr double WAR_EXHAUSTION_FACTOR = 0.05;
	constexpr double ARMY_LOSS_MIN_DEFENDER= 8.0;
	constexpr double ARMY_LOSS_MAX_DEFENDER= 20.0;
	constexpr double ARMY_LOSS_MIN_ATTACKER= 3.0;
	constexpr double ARMY_LOSS_MAX_ATTACKER= 10.0;
	
	constexpr bool   WAR_LOSS_PERCENTAGE_ENABLED = true;   // 是否启用百分比损耗
    constexpr double WAR_LOSS_PCT_MIN = 0.01;               // 最小损失百分比（1%）
    constexpr double WAR_LOSS_PCT_MAX = 0.10;               // 最大损失百分比（10%）
    constexpr double WAR_LOSS_PCT_BASE_ATK_ADVANTAGE = 0.04; // 基础调节因子
    constexpr double WAR_LOSS_PCT_BASE_DEF_ADVANTAGE = 0.07;
    constexpr double WAR_LOSS_ABSOLUTE_SCALE = 0.3;         // 绝对值损失缩放因子（降低绝对值损失）

	constexpr double STABILITY_BASE        = 70.0;
	constexpr double STABILITY_FOOD_GOOD   = 1.5;
	constexpr double STABILITY_FOOD_BAD    = 0.5;
	constexpr double STABILITY_WAR_PENALTY = 8.0;
	constexpr double STABILITY_TAX_THRESHOLD = 0.3;
	constexpr double STABILITY_TAX_PENALTY = 50.0;

	constexpr double RESEARCH_BASE_POINTS  = 0.012;
	constexpr double TECH_DISCOVERY_BASE   = 0.02;
	constexpr double TECH_SPREAD_PROB      = 0.02;

	constexpr double CIV_LEVEL_TECH_MOD    = 0.2;
	constexpr double CIV_LEVEL_REBELLION_RED = 0.1;
	constexpr double CIV_LEVEL_WAR_DEC_RED = 0.03;

	constexpr double SOIL_DEGRADE_PROB     = 0.02;
	constexpr double MINERAL_DEPLETE_PROB  = 0.005;
	constexpr double FOREST_DEGRADE_PROB   = 0.01;
	constexpr int    SOIL_RECOVER_INTERVAL = 50;
	
	constexpr double CIV_LEVEL_UPGRADE_THRESHOLD_OFFSET = 1.0;    // 升级所需额外超出量（原0.5）
    constexpr double CIV_LEVEL_DOWNGRADE_THRESHOLD_OFFSET = 1.0;   // 降级所需低于量（原0.5）
    constexpr int    CIV_LEVEL_STABILIZE_TURNS = 10;               // 稳固回合数
    constexpr double CIV_LEVEL_BASE_UPGRADE_COST = 3.0;            // 每级基础需求（原3.0）
    constexpr double CIV_LEVEL_PER_LEVEL_INCREMENT = 1.5;          // 每级递增（原1.5）

	constexpr double ALLIANCE_SCORE_THRESH = 0.75;
	constexpr double ALLIANCE_BREAK_PROB   = 0.015;
	constexpr double ALLY_JOIN_WAR_PROB    = 0.8;

	constexpr double DOMESTIC_EVENT_BASE   = 0.01;
	constexpr double HIDDEN_EVENT_BASE     = 0.05;
	constexpr double DISASTER_BASE         = 0.002;
	constexpr double INTERNATIONAL_EVENT_BASE = 0.005;

	constexpr double ROAD_MAINTENANCE      = 0.1;
	constexpr double ROAD_COST_BASE        = 3.0;
	constexpr double ROAD_COST_SLOPE       = 5.0;
	constexpr double ROAD_ASTAR_SLOPE      = 2.0;

	constexpr double EXPAND_COST_GOLD      = 2.0;
	constexpr double EXPAND_COST_FOOD      = 1.0;
	constexpr double EXPAND_CONTROL_INIT   = 0.5;
	
	constexpr double EXPANSION_EFFICIENCY_DECAY_RATE = 500.0;
	constexpr double EXPANSION_COST_EXPONENT_BASE = 100.0;    // 基准领土规模（格）
    constexpr double EXPANSION_COST_EXPONENT      = 1.8;      // 指数幂次（1.8表示比平方略缓，但比线性增长快）
    constexpr double EXPANSION_COST_MIN_MULTIPLIER = 1.0;     // 最小倍率（不低于1）

	constexpr double NEW_SETTLEMENT_COST   = 60.0;
	constexpr int    NEW_SETTLEMENT_POP    = 50;
	constexpr double NEW_SETTLEMENT_SCORE_THRESH = 5.0;

	constexpr double AUTO_SETTLE_TERRITORY_PER_SETTLEMENT_THRESHOLD = 50.0;   // 原 30.0
    constexpr double AUTO_SETTLE_POP_PER_SETTLEMENT_THRESHOLD      = 250.0;   // 原 150.0
    constexpr int    AUTO_SETTLE_COOLDOWN_TURNS                    = 10;      // 冷却回合数
    constexpr double AUTO_SETTLE_EXTRA_COST_GOLD                   = 30.0;    // 额外消耗金币
    constexpr double AUTO_SETTLE_EXTRA_COST_FOOD                   = 20.0;    // 额外消耗食物

	constexpr double REBELLION_PROB        = 0.08;
	constexpr double REBELLION_STABILITY_THRESHOLD = 30.0;
	constexpr double REVOLUTION_PROB       = 0.1;
	constexpr double REVOLUTION_STABILITY_THRESHOLD = 10.0;

	constexpr double KING_DEATH_AGE_BASE   = 50.0;
	constexpr double KING_DEATH_AGE_PER_YEAR = 0.025;
	constexpr double KING_DEATH_WAR_BONUS  = 0.05;	
	constexpr int    KING_NAME_HISTORY_SIZE = 5;   // 记录最近几任国王名字用于去重
    constexpr double KING_NAME_USE_PROBABILITY = 0.3; // 使用称号/姓氏的概率（剩余为普通名）
}

namespace FeatureToggle {
	constexpr bool MULTI_RACE           = true;
	constexpr bool MAGIC                = true;
	constexpr bool FANTASY_DISASTERS    = true;
	constexpr bool RUINS                = true;
	constexpr bool ROADS                = true;
	constexpr bool ALLIANCES            = true;
	constexpr bool DYNAMIC_KINGDOMS     = true;
	constexpr bool RESOURCE_DEGRADATION = true;
}

// ============================================================
// 图层索引与名称
// ============================================================
enum LayerIndex {
    LAYER_ELEVATION = 0, LAYER_TEMP, LAYER_PRECIP, LAYER_BIOME,
    LAYER_LAND_OCEAN, LAYER_TERRAIN, LAYER_VEGETATION, LAYER_SOIL_FERT,
    LAYER_MINERAL, LAYER_BEDROCK, LAYER_GROUNDWATER, LAYER_COASTTYPE,
    LAYER_VOLCANIC, LAYER_WILDLIFE, LAYER_WIND_U, LAYER_WIND_V,
    LAYER_PET, LAYER_SLOPE, LAYER_ARIDITY, LAYER_NPP,
    LAYER_CRUST_THICK, LAYER_DIST_OCEAN, LAYER_CONTINENTALITY, LAYER_MOISTURE,
    TOTAL_LAYERS = 24
};

extern const char* const all_layer_names[TOTAL_LAYERS];

// ============================================================
// 基础结构体
// ============================================================
struct Position {
    int x, y;
    Position(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

namespace std {
template<> struct hash<Position> {
    size_t operator()(const Position& p) const {
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};
}

struct Effect {
    EffectStat stat;
    double value;
    int duration = 0;
    std::string target = "self";
    double chance = 1.0;
    Effect() = default;
    Effect(EffectStat s, double v, int d = 0, std::string t = "self", double c = 1.0)
        : stat(s), value(v), duration(d), target(t), chance(c) {}
};

struct Condition {
    ConditionStat stat;
    std::string op;
    double value_num = 0.0;
    bool value_bool = true;
    std::string value_str;
    bool has_value_num = false;
    bool has_value_bool = false;
    Condition() = default;
    Condition(ConditionStat s, const std::string& o, double v)
        : stat(s), op(o), value_num(v), has_value_num(true) {}
    Condition(ConditionStat s, const std::string& o, bool v)
        : stat(s), op(o), value_bool(v), has_value_bool(true) {}
    Condition(ConditionStat s, const std::string& o, const std::string& v)
        : stat(s), op(o), value_str(v), has_value_num(false), has_value_bool(false) {}
};

struct TechNode {
    TechID id;
    std::string name;
    std::string domain;
    std::vector<TechID> prerequisites;
    double weight = 1.0;
    std::vector<Effect> effects;
    bool magic_required = false;
    bool globally_discovered = false;
    int first_discovered_kingdom = -1;
    TechNode() = default;
    TechNode(TechID _id, const std::string& _name, const std::string& _domain,
             const std::vector<TechID>& _pre, double _weight,
             const std::vector<Effect>& _effects, bool _magic = false)
        : id(_id), name(_name), domain(_domain), prerequisites(_pre),
          weight(_weight), effects(_effects), magic_required(_magic) {}
};

struct EventTemplate {
    int id;
    std::string name;
    EventType type;
    double probability;
    std::string scope;
    int cooldown = 0;
    int last_triggered_turn = -1;
    std::vector<Condition> conditions;
    std::vector<Effect> effects;
    bool record = true;
    bool is_international = false;
    bool requires_magic = false;
    std::string message;
    EventTemplate() = default;
    EventTemplate(int _id, const std::string& _name, EventType _type,
                  double _prob, const std::string& _scope,
                  const std::vector<Condition>& _cond,
                  const std::vector<Effect>& _eff,
                  bool _record = true, bool _intl = false,
                  bool _magic = false, const std::string& _msg = "")
        : id(_id), name(_name), type(_type), probability(_prob),
          scope(_scope), conditions(_cond), effects(_eff),
          record(_record), is_international(_intl),
          requires_magic(_magic), message(_msg) {}
};

struct Commander {
    std::string name;
    double aggression;
    double caution;
    int wins = 0;
    int losses = 0;
    bool active = true;
    Commander() = default;
    Commander(const std::string& n, double agg, double cau)
        : name(n), aggression(agg), caution(cau) {}
};

struct Settlement {
    Position pos;
    int population;
    bool is_capital;
    std::string name;
    double political_weight;
    double military_weight;
    std::vector<BuildingType> buildings;
    Settlement() = default;
    Settlement(int x, int y, int pop, bool capital, const std::string& nm,
               double pol_w, double mil_w)
        : pos(x, y), population(pop), is_capital(capital), name(nm),
          political_weight(pol_w), military_weight(mil_w) {}
};

struct KingRecord {
    std::string name;
    int reign_start_turn;
    int reign_end_turn;
    KingRecord() = default;
    KingRecord(const std::string& n, int start)
        : name(n), reign_start_turn(start), reign_end_turn(-1) {}
};

struct WorldData {
    int width, height;
    std::vector<std::vector<std::vector<double>>> layers;
    WorldData() : width(0), height(0) {}
    WorldData(int w, int h) : width(w), height(h) {
        layers.assign(WORLD_LAYERS,
                      std::vector<std::vector<double>>(h, std::vector<double>(w, 0.0)));
    }
    double get(int layer, int x, int y) const {
        return layers[layer][y][x];
    }
    void set(int layer, int x, int y, double val) {
        layers[layer][y][x] = val;
    }
    bool inside(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
};

struct EventRecord {
    int turn;
    std::string timestamp;
    EventType type;
    int primary_kingdom_id;
    std::vector<int> involved_kingdom_ids;
    std::string description;
    bool is_international;
    int x = -1, y = -1;
    EventRecord() = default;
    EventRecord(int t, const std::string& ts, EventType ty, int k,
                const std::string& desc, bool intl = false, int _x = -1, int _y = -1)
        : turn(t), timestamp(ts), type(ty), primary_kingdom_id(k),
          description(desc), is_international(intl), x(_x), y(_y) {}
};

// 遗迹结构体（预留）
struct Ruin {
    int x, y;
    double danger;
    double wealth;
    std::string name;
    int origin_kingdom_id = -1;
    Ruin() = default;
    Ruin(int _x, int _y, double d, double w, const std::string& n, int oid = -1)
        : x(_x), y(_y), danger(d), wealth(w), name(n), origin_kingdom_id(oid) {}
};

// ============================================================
// 前置声明全局变量
// ============================================================
extern std::vector<TechNode> globalTechPool;  // 必须在 Kingdom 定义前声明

// ============================================================
// 王国结构体
// ============================================================
struct Kingdom {
    int id = -1;
    std::string name;
    std::string king_name;
    Race race = Race::HUMAN;
    std::array<double, CULTURE_DIM> culture;

    std::vector<Settlement> settlements;
    std::unordered_set<int> territory;

    double army = 0.0;
    double gold = 0.0;
    double food = 0.0;
    double stability = Balance::INITIAL_STABILITY;
    double tax_rate = Balance::INITIAL_TAX_RATE;

    double tech_level = Balance::INITIAL_TECH_LEVEL;
    std::vector<TechID> owned_techs;

    int king_age = 30;
    double aggression = 0.5;
    double greed = 0.5;
    double caution = 0.5;
    double industriousness = 0.5;
    double diplomacy = 0.5;

    bool alive = true;
    int establish_turn = 0;
    int death_turn = -1;
    int last_auto_settlement_turn = -999;

    int war_timer = 0;
    int peace_timer = 0;
    int total_wars = 0;
    int active_cmd_index = 0;

    int civilization_level = 0;
    double civ_points = 0.0;
    double culture_health = 1.0;   // 文化健康度，0~2，1为健康
	int civ_decline_timer = 0;     // 文明衰退计时器
	double civ_effect_mod = 1.0;   // 文明等级对各项修正的系数（低文明惩罚，高文明加成）
	int upgrade_stabilize_timer = 0;   // 升级后稳固回合数
    int downgrade_stabilize_timer = 0; // 降级后稳固回合数

    std::vector<Commander> commanders;
    std::vector<KingRecord> king_history;
    std::vector<std::string> recent_king_names;  // 存储最近 N 任国王名字（用于去重）
    std::vector<int> allies;

    // 缓存
    double cached_food_mod = 1.0;
    double cached_gold_mod = 1.0;
    double cached_military_mod = 1.0;
    double cached_research_mod = 1.0;
    double cached_growth_mod = 1.0;
    double cached_defense_bonus = 0.0;
    double cached_attack_bonus = 0.0;
    double cached_tax_mod = 1.0;
    double cached_building_cost_mod = 1.0;
    double cached_road_speed_mod = 1.0;
    double cached_magic_affinity = 0.0;
    double cached_rebellion_risk_mod = 1.0;
    double cached_maintenance_mod = 1.0;

    std::unordered_set<int> road_tiles;
    bool in_world_war = false;

    // 历史峰值数据（用于灭亡后输出）
    int max_territory_size = 0;
    int max_population = 0;

    int total_population() const {
        int total = 0;
        for (const auto& s : settlements) total += s.population;
        return total;
    }

    double growth_modifier() const {
        switch (race) {
        case Race::HUMAN:
            return 1.0;
        case Race::DWARF:
            return 0.9;
        case Race::ELF:
            return 0.8;
        case Race::ORC:
            return 1.1;
        case Race::HALFLING:
            return 1.05;
        default:
            return 1.0;
        }
    }

    double combat_modifier() const {
        switch (race) {
        case Race::HUMAN:
            return 1.0;
        case Race::DWARF:
            return 1.1;
        case Race::ELF:
            return 0.95;
        case Race::ORC:
            return 1.15;
        case Race::HALFLING:
            return 0.9;
        default:
            return 1.0;
        }
    }

    double tech_modifier() const {
        switch (race) {
        case Race::HUMAN:
            return 1.0;
        case Race::DWARF:
            return 1.1;
        case Race::ELF:
            return 1.2;
        case Race::ORC:
            return 0.9;
        case Race::HALFLING:
            return 1.0;
        default:
            return 1.0;
        }
    }

    double diplomacy_modifier() const {
        switch (race) {
        case Race::HUMAN:
            return 0.0;
        case Race::DWARF:
            return 0.1;
        case Race::ELF:
            return 0.2;
        case Race::ORC:
            return -0.15;
        case Race::HALFLING:
            return 0.1;
        default:
            return 0.0;
        }
    }

    double effective_tech() const {
        return tech_level * (1.0 + civilization_level * Balance::CIV_LEVEL_TECH_MOD);
    }

    double get_tech_effect(EffectStat stat) const {
        double sum = 0.0;
        for (TechID tech_id : owned_techs) {
            for (const auto& node : globalTechPool) {
                if (node.id == tech_id) {
                    for (const Effect& eff : node.effects) {
                        if (eff.stat == stat) sum += eff.value;
                    }
                    break;
                }
            }
        }
        return sum;
    }

    void update_cached_effects();
};

// ============================================================
// 全局变量声明
// ============================================================
extern std::vector<std::vector<int>> ownerMap;
extern std::vector<std::vector<double>> controlMap;
extern std::vector<std::vector<std::array<double, CULTURE_DIM>>> cultureMap;
extern std::vector<std::vector<int>> roadMap;

extern std::vector<std::vector<int>> warMatrix;
extern std::vector<std::vector<bool>> allianceMatrix;

extern std::vector<EventRecord> globalEventLog;
extern std::vector<EventTemplate> globalEventTemplates;
extern std::mt19937 global_rng;

struct BuildingData {
    BuildingType type;
    std::string name;
    std::string category;
    double cost;
    double maintenance;
    int max_per_settlement;
    bool race_restricted;
    Race race_required;
    std::vector<Effect> effects;
    std::vector<std::string> tags;

    BuildingData() = default;
    BuildingData(BuildingType t, const std::string& n, const std::string& cat,
                 double c, double m, int max, bool rr, Race rr_race,
                 const std::vector<Effect>& eff, const std::vector<std::string>& tg)
        : type(t), name(n), category(cat), cost(c), maintenance(m),
          max_per_settlement(max), race_restricted(rr), race_required(rr_race),
          effects(eff), tags(tg) {}
};

extern std::vector<BuildingData> globalBuildingData;

// ============================================================
// 辅助函数声明
// ============================================================
int linear_index(int x, int y, int width);
std::pair<int,int> unlinear_index(int idx, int width);
double cosine_similarity(const std::array<double, CULTURE_DIM>& a,
                         const std::array<double, CULTURE_DIM>& b);
void apply_effects(Kingdom& k, const std::vector<Effect>& effects,
                   const std::string& target_override = "");
bool evaluate_condition(const Kingdom& k, const Condition& cond);

// ============================================================
// 各模块函数声明
// ============================================================
bool load_world(const std::string& filename, WorldData& world);
void init_global_maps(int width, int height);
bool is_land(const WorldData& world, int x, int y);
bool is_habitable(const WorldData& world, int x, int y);

int create_kingdom(std::vector<Kingdom>& kingdoms, const WorldData& world,
                   int capital_x, int capital_y, Race race, int turn);
void initialize_first_kingdom(const WorldData& world, std::vector<Kingdom>& kingdoms);
void check_for_new_kingdoms(int turn, const WorldData& world, std::vector<Kingdom>& kingdoms);
std::string generate_settlement_name(Race race, bool is_capital);

void collect_resources(Kingdom& k, WorldData& world, int turn);
void apply_resource_degradation(Kingdom& k, WorldData& world, int turn);
std::vector<BuildingData> create_building_data();
bool is_settlement_connected_to_capital(const Kingdom& k, const Settlement& s);

void grow_population(Kingdom& k, int turn);
void remove_deserted_settlements(Kingdom& k);
void manage_settlements(Kingdom& k, const WorldData& world, int turn);

void expand_territory(Kingdom& k, const WorldData& world, int turn);
void build_new_settlement(Kingdom& k, const WorldData& world, int turn);

void init_tech_pool();
void update_technology(Kingdom& k, int turn);
void transfer_tech(Kingdom& from, Kingdom& to, int turn);
bool kingdoms_share_border(const Kingdom& a, const Kingdom& b);

void update_stability(Kingdom& k, const WorldData& world, int turn);
void handle_rebellion(Kingdom& k, int turn);
void check_revolution(Kingdom& k, int turn);
void check_king_death(Kingdom& k, int turn);
void update_civilization_level(Kingdom& k, int turn);

void update_diplomacy(std::vector<Kingdom>& kingdoms, const WorldData& world, int turn);
void process_wars(std::vector<Kingdom>& kingdoms, const WorldData& world, int turn);
void ally_join_wars(std::vector<Kingdom>& kingdoms, int turn);
void check_world_war(std::vector<Kingdom>& kingdoms, int turn);

void update_alliances(std::vector<Kingdom>& kingdoms, const WorldData& world, int turn);
void process_international_events(std::vector<Kingdom>& kingdoms, int turn);

void build_roads(Kingdom& k, const WorldData& world, int turn);
void maintain_roads(Kingdom& k, int turn);

void process_domestic_events(Kingdom& k, int turn);
void process_hidden_events(Kingdom& k, int turn);
void apply_disasters(Kingdom& k, const WorldData& world, int turn);
void apply_magic_disasters(Kingdom& k, int turn);

void spread_culture(std::vector<Kingdom>& kingdoms, const WorldData& world, int turn);
void recover_control(std::vector<Kingdom>& kingdoms, int turn);

void write_kingdom_file(const Kingdom& k, const std::string& dir);
void write_fallen_kingdoms_file(const std::vector<Kingdom>& all_kingdoms, const std::string& dir);
void write_international_events_file(const std::vector<EventRecord>& events, const std::string& dir);
void write_roads_file(const std::vector<Kingdom>& kingdoms, const std::string& dir);
void write_territory_file(const std::vector<Kingdom>& kingdoms, const std::string& dir);
void write_history_file(const std::vector<EventRecord>& events, const std::string& dir);
void append_world_stability_assessment(const std::vector<Kingdom>& kingdoms,
                                       const std::string& dir);

void run_simulation(const std::string& world_file, const std::string& output_dir);
std::vector<int> shuffle_kingdoms(const std::vector<Kingdom>& kingdoms);
void update_cooldowns(Kingdom& k);
void recruit_army(Kingdom& k, int turn);
void check_extinction(Kingdom& k, int turn);
void init_event_templates();

// 统一时间格式化函数（前置声明）
std::string format_time(int turn, int month = -1, int day = -1);
// ============================================================
// 第 1 部分结束
// ============================================================

// ============================================================
// 文明模拟器 - 第 2 部分：世界文件加载与地图状态初始化
// ============================================================

// ---------- 全局变量定义 ----------
std::vector<std::vector<int>> ownerMap;
std::vector<std::vector<double>> controlMap;
std::vector<std::vector<std::array<double, CULTURE_DIM>>> cultureMap;
std::vector<std::vector<int>> roadMap;

std::vector<std::vector<int>> warMatrix;
std::vector<std::vector<bool>> allianceMatrix;

std::vector<EventRecord> globalEventLog;
std::vector<TechNode> globalTechPool;
std::vector<EventTemplate> globalEventTemplates;
//std::vector<BuildingData> globalBuildingData;

std::mt19937 global_rng(SIMULATION_RANDOM_SEED);

// 定义世界图层名称数组
const char* const all_layer_names[TOTAL_LAYERS] = {
    "ELEVATION", "TEMP", "PRECIP", "BIOME", "LAND_OCEAN", "TERRAIN",
    "VEGETATION", "SOIL_FERT", "MINERAL", "BEDROCK", "GROUNDWATER",
    "COASTTYPE", "VOLCANIC", "WILDLIFE", "WIND_U", "WIND_V",
    "PET", "SLOPE", "ARIDITY", "NPP", "CRUST_THICK", "DIST_OCEAN",
    "CONTINENTALITY", "MOISTURE"
};

// ---------- 辅助函数实现 ----------
int linear_index(int x, int y, int width) {
    return y * width + x;
}

std::pair<int, int> unlinear_index(int idx, int width) {
    return {idx % width, idx / width};
}

double cosine_similarity(const std::array<double, CULTURE_DIM>& a,
                         const std::array<double, CULTURE_DIM>& b) {
    double dot = 0.0, norm_a = 0.0, norm_b = 0.0;
    for (int i = 0; i < CULTURE_DIM; ++i) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    if (norm_a < 1e-12 || norm_b < 1e-12) return 0.0;
    return dot / (std::sqrt(norm_a) * std::sqrt(norm_b));
}

// ---------- 效果应用函数 ----------
void apply_effects(Kingdom& k, const std::vector<Effect>& effects,
                   const std::string& target_override) {
    for (const Effect& eff : effects) {
        if (eff.chance < 1.0) {
            std::uniform_real_distribution<double> prob(0.0, 1.0);
            if (prob(global_rng) > eff.chance) continue;
        }
        if (!target_override.empty() && target_override != "self" && target_override != eff.target) continue;
        if (!target_override.empty() && target_override == "self" && eff.target != "self" && eff.target != "all") continue;

        switch (eff.stat) {
        case EffectStat::FOOD:
            k.food += eff.value;
            break;
        case EffectStat::GOLD:
            k.gold += eff.value;
            break;
        case EffectStat::STABILITY:
            k.stability = std::max(MIN_STABILITY, std::min(MAX_STABILITY, k.stability + eff.value));
            break;
        case EffectStat::POPULATION: {
            if (std::abs(eff.value) <= 1.0) {
                for (auto& s : k.settlements) {
                    int change = static_cast<int>(std::llround(s.population * eff.value));
                    s.population = std::max(Balance::MIN_SETTLEMENT_POP, s.population + change);
                }
            } else {
                int delta = static_cast<int>(eff.value);
                if (!k.settlements.empty()) {
                    int per = delta / static_cast<int>(k.settlements.size());
                    int rem = delta % static_cast<int>(k.settlements.size());
                    for (size_t i = 0; i < k.settlements.size(); ++i) {
                        k.settlements[i].population += per + (i == 0 ? rem : 0);
                        k.settlements[i].population = std::max(Balance::MIN_SETTLEMENT_POP, k.settlements[i].population);
                    }
                }
            }
            
            break;
        }
        case EffectStat::ARMY:
            k.army += eff.value;
            k.army = std::max(0.0, k.army); 
            break;
        case EffectStat::RESEARCH_POINTS:
            k.tech_level += eff.value;
            k.tech_level = std::max(0.0, std::min(MAX_TECH_LEVEL, k.tech_level));
            break;
        case EffectStat::FOOD_MOD:
            k.cached_food_mod += eff.value;
            break;
        case EffectStat::GOLD_MOD:
            k.cached_gold_mod += eff.value;
            break;
        case EffectStat::MILITARY_POWER_MOD:
            k.cached_military_mod += eff.value;
            break;
        case EffectStat::DEFENSE_BONUS:
            k.cached_defense_bonus += eff.value;
            break;
        case EffectStat::ATTACK_BONUS:
            k.cached_attack_bonus += eff.value;
            break;
        case EffectStat::TECH_MOD:
            k.cached_research_mod += eff.value;
            break;
        case EffectStat::GROWTH_MOD:
            k.cached_growth_mod += eff.value;
            break;
        case EffectStat::TAX_MOD:
            k.cached_tax_mod += eff.value;
            break;
        case EffectStat::BUILDING_COST_MOD:
            k.cached_building_cost_mod += eff.value;
            break;
        case EffectStat::ROAD_SPEED_MOD:
            k.cached_road_speed_mod += eff.value;
            break;
        case EffectStat::MAGIC_AFFINITY:
            k.cached_magic_affinity += eff.value;
            break;
        case EffectStat::REBELLION_RISK_MOD:
            k.cached_rebellion_risk_mod += eff.value;
            break;
        case EffectStat::WAR_EXHAUSTION_MOD:
            k.war_timer += static_cast<int>(eff.value * 10);
            break;
        case EffectStat::CULTURE_CHANGE_MOD:
            break; // 后续处理
        case EffectStat::TRADE_MOD:
            k.cached_gold_mod += eff.value;
            break;
        case EffectStat::WAR_LOOT_MOD:
            k.cached_military_mod += eff.value * 0.5;
            break;
        case EffectStat::MAINTENANCE_MOD:
            k.cached_maintenance_mod += eff.value;
            break;
        case EffectStat::POPULATION_CAP_MOD:
            break;
        case EffectStat::ARMY_MAINTENANCE_MOD:
            break;
        case EffectStat::STABILITY_REGEN_MOD:
            break;
        case EffectStat::RESEARCH_RATE_MOD:
            k.cached_research_mod += eff.value;
            break;
        case EffectStat::EXPANSION_COST_MOD:
            break;
        case EffectStat::SETTLEMENT_COST_MOD:
            break;
        default:
            break;
        }
    }
}

// ---------- 条件判断函数 ----------
bool evaluate_condition(const Kingdom& k, const Condition& cond) {
    double actual_value = 0.0;
    bool actual_bool = false;
    std::string actual_str = "";
    switch (cond.stat) {
    case ConditionStat::FOOD:
        actual_value = k.food;
        break;
    case ConditionStat::GOLD:
        actual_value = k.gold;
        break;
    case ConditionStat::POPULATION:
        actual_value = static_cast<double>(k.total_population());
        break;
    case ConditionStat::ARMY:
        actual_value = k.army;
        break;
    case ConditionStat::STABILITY:
        actual_value = k.stability;
        break;
    case ConditionStat::TECH_LEVEL:
        actual_value = k.tech_level;
        break;
    case ConditionStat::FOOD_PER_POP: {
        int pop = k.total_population();
        actual_value = pop > 0 ? (k.food / pop) : 0.0;
        break;
    }
    case ConditionStat::AT_PEACE:
        actual_bool = (k.war_timer == 0);
        break;
    case ConditionStat::AT_WAR:
        actual_bool = (k.war_timer > 0);
        break;
    case ConditionStat::HAS_BUILDING: {
        for (const auto& s : k.settlements) {
            for (BuildingType bt : s.buildings) {
                if (std::to_string(static_cast<int>(bt)) == cond.value_str) {
                    actual_bool = true;
                    break;
                }
            }
            if (actual_bool) break;
        }
        break;
    }
    case ConditionStat::HAS_TECH: {
        for (TechID tech : k.owned_techs) {
            if (std::to_string(static_cast<int>(tech)) == cond.value_str) {
                actual_bool = true;
                break;
            }
        }
        break;
    }
    case ConditionStat::RACE:
        actual_str = std::to_string(static_cast<int>(k.race));
        break;
    case ConditionStat::CULTURE_SIMILARITY:
        return false; // 需要上下文
    case ConditionStat::CIVILIZATION_LEVEL:
        actual_value = static_cast<double>(k.civilization_level);
        break;
    case ConditionStat::TAX_RATE:
        actual_value = k.tax_rate;
        break;
    case ConditionStat::TURN:
        return false;
    default:
        return false;
    }
    if (cond.has_value_num) {
        if (cond.op == ">")  return actual_value > cond.value_num;
        if (cond.op == "<")  return actual_value < cond.value_num;
        if (cond.op == ">=") return actual_value >= cond.value_num;
        if (cond.op == "<=") return actual_value <= cond.value_num;
        if (cond.op == "==") return std::abs(actual_value - cond.value_num) < 1e-9;
        if (cond.op == "!=") return std::abs(actual_value - cond.value_num) >= 1e-9;
    }
    if (cond.has_value_bool) {
        if (cond.op == "==") return actual_bool == cond.value_bool;
        if (cond.op == "!=") return actual_bool != cond.value_bool;
    }
    if (!cond.value_str.empty()) {
        if (cond.op == "==") return actual_str == cond.value_str;
        if (cond.op == "!=") return actual_str != cond.value_str;
    }
    return false;
}

// ---------- 世界加载 ----------
// load_world: 同时支持旧文本格式 "WORLDMAP" 和新版二进制格式 "WORLDMAPBIN"
// 兼容 Weather-Terrain.cpp 的二进制输出（layer 名称表 + 连续的 double 数据块）
// 如果不是二进制文件则回退为原来的文本解析逻辑。
bool load_world(const std::string& filename, WorldData& world) {
    // 先以二进制方式打开并检测文件头（这样可以同时识别二进制与文本格式）
    std::ifstream inbin(filename, std::ios::binary);
    if (!inbin.is_open()) {
        std::cerr << "无法打开文件 " << filename << std::endl;
        return false;
    }

    // 读前 11 字节尝试判定是否为 "WORLDMAPBIN"
    char magic_buf[12] = {0};
    inbin.read(magic_buf, 11);
    if (!inbin) {
        // 文件太短，尝试作为文本回退
        inbin.close();
    } else {
        std::string magic(magic_buf, 11);
        if (magic == "WORLDMAPBIN") {
            // 二进制格式解析
            int32_t gen_w = 0, gen_h = 0, num_layers = 0;
            inbin.read(reinterpret_cast<char*>(&gen_w), sizeof(gen_w));
            inbin.read(reinterpret_cast<char*>(&gen_h), sizeof(gen_h));
            inbin.read(reinterpret_cast<char*>(&num_layers), sizeof(num_layers));
            if (!inbin) {
                std::cerr << "读取二进制头失败: " << filename << std::endl;
                return false;
            }
            if (num_layers != WORLD_LAYERS) {
                std::cerr << "文件层数(" << num_layers << ")与程序期望(" << WORLD_LAYERS << ")不符" << std::endl;
                return false;
            }

            // 校验 buffer 大小是否与本程序设定匹配
            int buffer_calc_w = (gen_w - WORLD_WIDTH) / 2;
            int buffer_calc_h = (gen_h - WORLD_HEIGHT) / 2;
            if (buffer_calc_w != BUFFER_SIZE || (gen_w - WORLD_WIDTH) % 2 != 0 ||
                buffer_calc_h != BUFFER_SIZE || (gen_h - WORLD_HEIGHT) % 2 != 0) {
                std::cerr << "地图尺寸/边界与程序设定不匹配 (gen_w=" << gen_w << ", gen_h=" << gen_h << ", expected buffer=" << BUFFER_SIZE << ")" << std::endl;
                return false;
            }

            // 读取每层的名字（Weather 输出的是 uint16_t len + raw bytes）
            std::vector<std::string> layer_names;
            for (int l = 0; l < num_layers; ++l) {
                uint16_t len = 0;
                inbin.read(reinterpret_cast<char*>(&len), sizeof(len));
                if (!inbin) {
                    std::cerr << "读取层名称长度失败 (layer " << l << ")" << std::endl;
                    return false;
                }
                std::string name;
                name.resize(len);
                inbin.read(&name[0], len);
                if (!inbin) {
                    std::cerr << "读取层名称失败 (layer " << l << ")" << std::endl;
                    return false;
                }
                layer_names.push_back(name);
                // 可选：校验名字与 all_layer_names 一致（保持严格）
                if (l < TOTAL_LAYERS && name != all_layer_names[l]) {
                    std::cerr << "警告：文件第 " << l << " 层名为 '" << name << "'，期望 '" << all_layer_names[l] << "'（仍尝试继续解析）" << std::endl;
                    // 不直接返回错误，允许兼容性（但你可以改为返回 false）
                }
            }

            // 读取数据区（连续的 double）
            const size_t total_elems = static_cast<size_t>(num_layers) * static_cast<size_t>(gen_h) * static_cast<size_t>(gen_w);
            std::vector<double> rawdata;
            try {
                rawdata.resize(total_elems);
            } catch (...) {
                std::cerr << "内存分配失败: 无法为地图数据分配缓冲区" << std::endl;
                return false;
            }

            inbin.read(reinterpret_cast<char*>(rawdata.data()), static_cast<std::streamsize>(total_elems * sizeof(double)));
            if (!inbin) {
                std::cerr << "读取地图数据失败或数据不完整" << std::endl;
                return false;
            }
            inbin.close();

            // 把 rawdata 转成 full_layers 三维结构以按原逻辑提取中心区域
            std::vector<std::vector<std::vector<double>>> full_layers(
                WORLD_LAYERS,
                std::vector<std::vector<double>>(gen_h, std::vector<double>(gen_w, 0.0))
            );

            // rawdata 排列为： l major, 然后 y, x （与 Grid3::data 排列一致）
            for (int l = 0; l < num_layers; ++l) {
                size_t layer_offset = static_cast<size_t>(l) * static_cast<size_t>(gen_h) * static_cast<size_t>(gen_w);
                for (int y = 0; y < gen_h; ++y) {
                    size_t row_offset = layer_offset + static_cast<size_t>(y) * static_cast<size_t>(gen_w);
                    for (int x = 0; x < gen_w; ++x) {
                        full_layers[l][y][x] = rawdata[row_offset + static_cast<size_t>(x)];
                    }
                }
            }

            // 将中心区域拷贝到 world（裁剪掉 BUFFER）
            world = WorldData(WORLD_WIDTH, WORLD_HEIGHT);
            for (int l = 0; l < WORLD_LAYERS; ++l) {
                for (int y = 0; y < WORLD_HEIGHT; ++y) {
                    for (int x = 0; x < WORLD_WIDTH; ++x) {
                        world.layers[l][y][x] = full_layers[l][y + BUFFER_SIZE][x + BUFFER_SIZE];
                    }
                }
            }

            std::cout << "二进制地图加载成功: " << WORLD_WIDTH << "x" << WORLD_HEIGHT << " 层数 " << WORLD_LAYERS << std::endl;
            return true;
        }
    }

    // 如果不是二进制格式，回退为文本格式解析（与原实现兼容）
    // 关闭之前的二进制流并用文本模式重新打开
    inbin.close();
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "无法打开文件 " << filename << std::endl;
        return false;
    }

    std::string header;
    in >> header;
    if (header != "WORLDMAP") {
        std::cerr << "文件头不是文本 WORLDMAP，也不是二进制 WORLDMAPBIN，无法解析: " << header << std::endl;
        in.close();
        return false;
    }

    int gen_w, gen_h, num_layers;
    in >> gen_w >> gen_h >> num_layers;
    if (num_layers != WORLD_LAYERS) {
        std::cerr << "地图文件层数应为 " << WORLD_LAYERS << "，但文件中为 " << num_layers << std::endl;
        in.close();
        return false;
    }
    int buffer_calc = (gen_w - WORLD_WIDTH) / 2;
    if (buffer_calc != BUFFER_SIZE || (gen_w - WORLD_WIDTH) % 2 != 0 ||
        (gen_h - WORLD_HEIGHT) / 2 != BUFFER_SIZE || (gen_h - WORLD_HEIGHT) % 2 != 0) {
        std::cerr << "地图尺寸/缓冲区检查不通过，请确认地图与本程序的 WORLD_WIDTH/WORLD_HEIGHT/BUFFER_SIZE 匹配" << std::endl;
        in.close();
        return false;
    }

    std::vector<std::vector<std::vector<double>>> full_layers(
        WORLD_LAYERS,
        std::vector<std::vector<double>>(gen_h, std::vector<double>(gen_w, 0.0)));

    for (int l = 0; l < WORLD_LAYERS; ++l) {
        std::string layer_name;
        in >> layer_name;
        if (layer_name != all_layer_names[l]) {
            std::cerr << "文件第 " << l << " 层名应为 " << all_layer_names[l]
                      << "，但读到 " << layer_name << "（仍尝试继续解析）" << std::endl;
            // 不直接失败，尽量兼容旧文件
        }
        for (int y = 0; y < gen_h; ++y) {
            for (int x = 0; x < gen_w; ++x) {
                double val;
                in >> val;
                full_layers[l][y][x] = val;
            }
        }
    }
    in.close();

    world = WorldData(WORLD_WIDTH, WORLD_HEIGHT);
    for (int l = 0; l < WORLD_LAYERS; ++l) {
        for (int y = 0; y < WORLD_HEIGHT; ++y) {
            for (int x = 0; x < WORLD_WIDTH; ++x) {
                world.layers[l][y][x] = full_layers[l][y + BUFFER_SIZE][x + BUFFER_SIZE];
            }
        }
    }

    std::cout << "文本地图加载成功: " << WORLD_WIDTH << "x" << WORLD_HEIGHT << " 层数 " << WORLD_LAYERS << std::endl;
    return true;
}
void init_global_maps(int width, int height) {
    ownerMap.assign(height, std::vector<int>(width, -1));
    controlMap.assign(height, std::vector<double>(width, 0.0));
    std::array<double, CULTURE_DIM> zero_culture = {0.0, 0.0, 0.0, 0.0};
    cultureMap.assign(height, std::vector<std::array<double, CULTURE_DIM>>(width, zero_culture));
    roadMap.assign(height, std::vector<int>(width, 0));
    warMatrix.clear();
    allianceMatrix.clear();
}

bool is_land(const WorldData& world, int x, int y) {
    if (!world.inside(x, y)) return false;
    return world.get(LAYER_ELEVATION, x, y) > 0.0;
}

bool is_habitable(const WorldData& world, int x, int y) {
    if (!is_land(world, x, y)) return false;
    int biome = static_cast<int>(world.get(LAYER_BIOME, x, y));
    int terrain = static_cast<int>(world.get(LAYER_TERRAIN, x, y));
    if (biome == static_cast<int>(Biome::ICE_CAP) || biome == static_cast<int>(Biome::GLACIER)) return false;
    if (terrain == static_cast<int>(TerrainClass::HIGH_MOUNTAIN)) return false;
    return true;
}

// ============================================================
// 第 2 部分结束
// ============================================================

// ============================================================
// 第 3 部分：王国初始化与动态建国
// ============================================================

// ============================================================
// 名字池扩充（用于国王、指挥官、定居点）
// ============================================================

// 各种族名字（至少20个）
static const std::vector<std::string> HUMAN_NAMES = {
    "阿尔弗雷德", "爱德华", "威廉", "亨利", "理查德", "罗伯特", "威廉明娜", "伊莎贝拉",
    "乔治", "查理", "菲利普", "路易", "弗朗西斯", "约翰", "托马斯", "詹姆斯",
    "玛丽", "安妮", "凯瑟琳", "伊丽莎白", "维多利亚", "亚瑟", "埃德蒙", "奥利弗"
};
static const std::vector<std::string> DWARF_NAMES = {
    "托林", "巴林", "德瓦林", "欧因", "格罗因", "金雳", "索林", "丹恩",
    "铁足", "石锤", "铜须", "银臂", "钢盾", "熔炉", "深矿", "岩心",
    "锻炉", "铁砧", "锤手", "火炉", "霜须", "赤铜"
};
static const std::vector<std::string> ELF_NAMES = {
    "莱戈拉斯", "瑟兰迪尔", "埃尔德隆", "凯兰崔尔", "阿尔温", "费纳芬", "格洛芬德尔", "爱隆",
    "露西恩", "蒂努维尔", "贝伦", "欧尔", "加拉德", "凯勒布林博", "奇尔丹", "阿玛瑞尔",
    "芬罗德", "阿格诺尔", "埃克塞利昂", "吉尔加拉德", "凯勒布", "诺多"
};
static const std::vector<std::string> ORC_NAMES = {
    "格罗姆", "乌格鲁克", "鲁兹", "夏格拉特", "阿佐格", "博尔多", "纳兹格雷尔", "霍格",
    "血牙", "碎颅", "劈骨", "黑手", "铁颌", "食人魔", "战吼", "裂蹄",
    "刃风", "嗜血", "暗影", "死眼", "碎刃", "石拳"
};
static const std::vector<std::string> HALFLING_NAMES = {
    "比尔博", "弗罗多", "山姆", "皮平", "梅里", "罗索", "霍比特", "巴金斯",
    "图克", "布兰德", "河岸", "绿丘", "麦酒", "烟斗", "果园", "蜜糖",
    "暖炉", "啤酒", "橡木", "柳树", "蟾蜍", "蘑菇"
};

// 称号/姓氏（用于组合）
static const std::vector<std::string> KING_TITLES = {
    "大帝", "征服者", "智者", "公正者", "勇者", "铁腕", "仁王", "雄狮",
    "鹰眼", "圣王", "大公", "元帅", "炼金术师", "龙裔", "星行者", "暗影",
    "银手", "金鬃", "霜狼", "炎心", "风行者", "山岭之王"
};

static const std::vector<std::string> COMMANDER_TITLES = {
    "将军", "统领", "勇士", "领主", "队长", "铁卫", "先锋", "统帅",
    "元帅", "骑士", "剑圣", "弩手", "骑兵", "射手", "旗手", "战歌"
};

// ---------- 名称与性格生成辅助函数 ----------
// 注意：以下函数定义为全局函数，确保所有模块可调用。

std::string generate_settlement_name(Race race, bool is_capital) {
    // ===== 扩充前缀池（每个种族至少20个） =====
    static const std::vector<std::string> prefix_human = {
        "新", "旧", "北", "南", "东", "西", "中", "上", "下", "大", "小",
        "白", "黑", "红", "金", "银", "铁", "铜", "石", "木", "水", "火",
        "风", "云", "星", "月", "日", "光", "明", "暗", "深", "浅", "高",
        "低", "远", "近", "内", "外", "前", "后", "青", "翠", "碧", "苍",
        "霜", "雪", "雨", "雷", "电", "霞", "虹", "雾", "林", "山", "川",
        "河", "湖", "海", "泉", "溪", "涧", "渊", "泽", "港", "湾", "峡"
    };
    static const std::vector<std::string> prefix_dwarf = {
        "石", "铁", "金", "铜", "银", "深", "熔", "锻", "锤", "砧", "矿",
        "岩", "晶", "钢", "黑", "赤", "灰", "白", "山", "岭", "谷", "渊",
        "炉", "堡", "卫", "坚", "固", "厚", "硬", "锐", "棱", "柱", "壁",
        "穹", "岩", "穴", "洞", "坑", "脉", "砥", "砺", "锋", "芒"
    };
    static const std::vector<std::string> prefix_elf = {
        "翠", "银", "月", "星", "林", "叶", "花", "露", "晨", "暮", "风",
        "云", "虹", "白", "金", "晶", "灵", "树", "藤", "泉", "湖", "溪",
        "谷", "丘", "原", "森", "幽", "明", "辉", "光", "岚", "霞", "雾",
        "霜", "雪", "雨", "虹", "霓", "莲", "兰", "桂", "梅", "竹", "松"
    };
    static const std::vector<std::string> prefix_orc = {
        "血", "骨", "战", "暗", "炎", "刃", "裂", "碎", "狂", "暴", "铁",
        "黑", "赤", "灰", "怒", "吼", "锋", "利", "坚", "石", "岩", "渊",
        "坑", "疤", "伤", "影", "噬", "魂", "祭", "牲", "屠", "杀", "毁",
        "灭", "破", "败", "毒", "瘴", "朽", "枯", "棘", "刺"
    };
    static const std::vector<std::string> prefix_halfling = {
        "绿", "丘", "河", "花", "蜜", "甜", "暖", "阳", "春", "秋", "丰",
        "收", "果", "园", "田", "穗", "麦", "谷", "溪", "桥", "磨", "坊",
        "仓", "窖", "炉", "炊", "香", "醇", "悠", "闲", "适", "乐", "和",
        "睦", "宁", "静", "幽", "雅", "清", "澈", "净", "圆", "满", "福"
    };

    // 后缀池（扩充）
    static const std::vector<std::string> suffix = {
        "城", "堡", "镇", "村", "港", "要塞", "渡", "关", "驿", "亭", "台",
        "阁", "楼", "殿", "宫", "府", "邸", "庄", "园", "圃", "苑", "林",
        "野", "原", "洲", "岛", "湾", "峡", "口", "门", "津", "泊", "泽",
        "池", "塘", "渊", "涧", "泉", "溪", "河", "江", "湖", "海", "山",
        "岭", "峰", "崖", "谷", "壑", "丘", "岗", "坪", "坝", "堰"
    };

    const std::vector<std::string>* prefixes = nullptr;
    switch (race) {
        case Race::HUMAN:    prefixes = &prefix_human; break;
        case Race::DWARF:    prefixes = &prefix_dwarf; break;
        case Race::ELF:      prefixes = &prefix_elf; break;
        case Race::ORC:      prefixes = &prefix_orc; break;
        case Race::HALFLING: prefixes = &prefix_halfling; break;
        default:             prefixes = &prefix_human;
    }

    std::uniform_int_distribution<size_t> dist_p(0, prefixes->size() - 1);
    std::uniform_int_distribution<size_t> dist_s(0, suffix.size() - 1);
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    std::string name;
    // 双前缀概率 20%（原来是30%，适当降低）
    if (prob(global_rng) < 0.2) {
        std::string p1 = (*prefixes)[dist_p(global_rng)];
        std::string p2 = (*prefixes)[dist_p(global_rng)];
        name = p1 + p2 + suffix[dist_s(global_rng)];
    } else {
        name = (*prefixes)[dist_p(global_rng)] + suffix[dist_s(global_rng)];
    }

    // 首都特殊名称：在名称前加“首都”、“王都”等，或者改为更独特的修饰
    if (is_capital) {
        std::vector<std::string> capital_prefix = {"王都", "皇城", "京都", "圣城", "大城", "帝都", "王城"};
        std::uniform_int_distribution<size_t> dist_c(0, capital_prefix.size() - 1);
        // 将原有的前缀替换为首都前缀，或者追加？这里选择替换为“首都前缀+原名字”或“原名字+都”
        // 为保持多样性，随机选择是加前缀还是加后缀“都”
        if (prob(global_rng) < 0.5) {
            name = capital_prefix[dist_c(global_rng)] + name;
        } else {
            name = name + "都";
        }
    }

    return name;
}
std::string generate_king_name(Race race) {
    // 选择名字池
    const std::vector<std::string>* name_pool = nullptr;
    switch (race) {
        case Race::HUMAN:    name_pool = &HUMAN_NAMES; break;
        case Race::DWARF:    name_pool = &DWARF_NAMES; break;
        case Race::ELF:      name_pool = &ELF_NAMES; break;
        case Race::ORC:      name_pool = &ORC_NAMES; break;
        case Race::HALFLING: name_pool = &HALFLING_NAMES; break;
        default:             name_pool = &HUMAN_NAMES;
    }
    std::uniform_int_distribution<size_t> dist_name(0, name_pool->size() - 1);
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    // 基础名字
    std::string name = (*name_pool)[dist_name(global_rng)];

    // 有概率添加称号/姓氏（增加变体）
    if (prob(global_rng) < Balance::KING_NAME_USE_PROBABILITY) {
        std::uniform_int_distribution<size_t> dist_title(0, KING_TITLES.size() - 1);
        std::string title = KING_TITLES[dist_title(global_rng)];
        name = name + "·" + title;  // 用间隔符连接
    }
    return name;
}

std::string generate_commander_name(Race race) {
    // 先取一个国王名作为基础（复用名字池）
    std::string base_name = generate_king_name(race);  // 可能带称号
    // 但指挥官称号更偏向军事，所以重新随机一个军事称号
    std::uniform_int_distribution<size_t> dist_title(0, COMMANDER_TITLES.size() - 1);
    std::string title = COMMANDER_TITLES[dist_title(global_rng)];
    // 如果 base_name 已经包含 "·"，则替换原有称号，否则追加
    size_t pos = base_name.find("·");
    if (pos != std::string::npos) {
        base_name = base_name.substr(0, pos) + "·" + title;
    } else {
        base_name = base_name + "·" + title;
    }
    return base_name;
}

std::array<double, CULTURE_DIM> generate_initial_culture(Race race) {
    std::array<double, CULTURE_DIM> base;
    switch (race) {
    case Race::HUMAN:
        base = {{0.5, 0.5, 0.5, 0.5}};
        break;
    case Race::DWARF:
        base = {{0.4, 0.7, 0.6, 0.3}};
        break;
    case Race::ELF:
        base = {{0.3, 0.8, 0.5, 0.6}};
        break;
    case Race::ORC:
        base = {{0.7, 0.3, 0.4, 0.2}};
        break;
    case Race::HALFLING:
        base = {{0.5, 0.4, 0.6, 0.7}};
        break;
    default:
        base = {{0.5, 0.5, 0.5, 0.5}};
    }
    std::uniform_real_distribution<double> jitter(-0.1, 0.1);
    for (int i = 0; i < CULTURE_DIM; ++i) {
        base[i] = std::max(0.0, std::min(1.0, base[i] + jitter(global_rng)));
    }
    return base;
}

std::array<double, 5> generate_personality(Race race) {
    std::array<double, 5> ideal;
    switch (race) {
    case Race::HUMAN:
        ideal = {{0.5, 0.5, 0.5, 0.5, 0.5}};
        break;
    case Race::DWARF:
        ideal = {{0.4, 0.6, 0.7, 0.7, 0.4}};
        break;
    case Race::ELF:
        ideal = {{0.3, 0.4, 0.6, 0.5, 0.8}};
        break;
    case Race::ORC:
        ideal = {{0.8, 0.5, 0.3, 0.4, 0.2}};
        break;
    case Race::HALFLING:
        ideal = {{0.2, 0.5, 0.4, 0.6, 0.7}};
        break;
    default:
        ideal = {{0.5, 0.5, 0.5, 0.5, 0.5}};
    }
    std::array<double, 5> base = ideal;
    std::uniform_real_distribution<double> jitter(-0.15, 0.15);
    for (int i = 0; i < 5; ++i) {
        base[i] = std::max(0.0, std::min(1.0, base[i] + jitter(global_rng)));
        base[i] = base[i] * 0.7 + ideal[i] * 0.3; // 回归30%
    }
    return base;
}

void generate_new_king(Kingdom& k, const std::array<double, 5>& old_personality, bool is_initial) {
    if (is_initial) {
        std::uniform_int_distribution<int> age_dist(25, 55);
        k.king_age = age_dist(global_rng);
    } else {
        std::uniform_int_distribution<int> age_dist(18, 30);
        k.king_age = age_dist(global_rng);
    }

    // ---------- 生成国王名字（带去重） ----------
    // 1. 选择名字池
    const std::vector<std::string>* name_pool = nullptr;
    switch (k.race) {
        case Race::HUMAN:    name_pool = &HUMAN_NAMES; break;
        case Race::DWARF:    name_pool = &DWARF_NAMES; break;
        case Race::ELF:      name_pool = &ELF_NAMES; break;
        case Race::ORC:      name_pool = &ORC_NAMES; break;
        case Race::HALFLING: name_pool = &HALFLING_NAMES; break;
        default:             name_pool = &HUMAN_NAMES;
    }

    // 2. 建立排除名单（最近 N 任国王的基础名字，不含称号）
    std::unordered_set<std::string> exclude_set;
    // 从 recent_king_names 中提取基础名（去掉称号）
    for (const std::string& full_name : k.recent_king_names) {
        size_t pos = full_name.find("·");
        std::string base = (pos != std::string::npos) ? full_name.substr(0, pos) : full_name;
        exclude_set.insert(base);
    }

    // 3. 构建可用的名字列表（排除排除名单）
    std::vector<std::string> available_names;
    for (const std::string& name : *name_pool) {
        if (exclude_set.find(name) == exclude_set.end()) {
            available_names.push_back(name);
        }
    }
    // 如果可用列表为空（极端情况，所有名字都用过了），则回退到全部名字（允许重复）
    if (available_names.empty()) {
        available_names = *name_pool;
    }

    // 4. 随机选择基础名字
    std::uniform_int_distribution<size_t> dist_name(0, available_names.size() - 1);
    std::string base_name = available_names[dist_name(global_rng)];

    // 5. 添加称号/姓氏（概率）
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    if (prob(global_rng) < Balance::KING_NAME_USE_PROBABILITY) {
        std::uniform_int_distribution<size_t> dist_title(0, KING_TITLES.size() - 1);
        std::string title = KING_TITLES[dist_title(global_rng)];
        k.king_name = base_name + "·" + title;
    } else {
        k.king_name = base_name;
    }

    // ---------- 更新最近名字历史 ----------
    k.recent_king_names.push_back(k.king_name);
    if (k.recent_king_names.size() > Balance::KING_NAME_HISTORY_SIZE) {
        k.recent_king_names.erase(k.recent_king_names.begin());
    }

    // ---------- 原有性格生成逻辑不变 ----------
    std::array<double, 5> base_person = generate_personality(k.race);
    if (!is_initial && old_personality[0] != -1.0) {
        std::uniform_real_distribution<double> jitter(-0.1, 0.1);
        k.aggression = std::max(0.0, std::min(1.0, old_personality[0]*0.5 + base_person[0]*0.3 + jitter(global_rng)));
        k.greed     = std::max(0.0, std::min(1.0, old_personality[1]*0.5 + base_person[1]*0.3 + jitter(global_rng)));
        k.caution   = std::max(0.0, std::min(1.0, old_personality[2]*0.5 + base_person[2]*0.3 + jitter(global_rng)));
        k.industriousness = std::max(0.0, std::min(1.0, old_personality[3]*0.5 + base_person[3]*0.3 + jitter(global_rng)));
        k.diplomacy = std::max(0.0, std::min(1.0, old_personality[4]*0.5 + base_person[4]*0.3 + jitter(global_rng)));
    } else {
        k.aggression = base_person[0];
        k.greed = base_person[1];
        k.caution = base_person[2];
        k.industriousness = base_person[3];
        k.diplomacy = base_person[4];
    }
}

void add_initial_commander(Kingdom& k) {
    Commander cmd(generate_commander_name(k.race), 0.5, 0.5);
    k.commanders.push_back(cmd);
    k.active_cmd_index = 0;
}

// ---------- 王国创建函数 ----------
int create_kingdom(std::vector<Kingdom>& kingdoms, const WorldData& world,
                   int capital_x, int capital_y, Race race, int turn) {
    Kingdom new_kingdom;
    new_kingdom.id = static_cast<int>(kingdoms.size());
    new_kingdom.name = generate_settlement_name(race, true) + "王国";
    new_kingdom.race = race;
    new_kingdom.culture = generate_initial_culture(race);
    new_kingdom.establish_turn = turn;
    new_kingdom.alive = true;
    new_kingdom.king_history.clear();

    new_kingdom.gold = Balance::INITIAL_GOLD;
    new_kingdom.food = Balance::INITIAL_FOOD;
    new_kingdom.army = Balance::INITIAL_ARMY;
    new_kingdom.stability = Balance::INITIAL_STABILITY;
    new_kingdom.tax_rate = Balance::INITIAL_TAX_RATE;
    new_kingdom.tech_level = Balance::INITIAL_TECH_LEVEL;

    std::array<double, 5> dummy = {{-1.0, -1.0, -1.0, -1.0, -1.0}};
    generate_new_king(new_kingdom, dummy, true);
    KingRecord record(new_kingdom.king_name, turn);
    new_kingdom.king_history.push_back(record);
    add_initial_commander(new_kingdom);

    Settlement capital(capital_x, capital_y, NEW_SETTLEMENT_INITIAL_POP,
                       true, generate_settlement_name(race, true), 0.8, 0.5);
    new_kingdom.settlements.push_back(capital);

    int cap_idx = linear_index(capital_x, capital_y, WORLD_WIDTH);
    ownerMap[capital_y][capital_x] = new_kingdom.id;
    controlMap[capital_y][capital_x] = 1.0;
    cultureMap[capital_y][capital_x] = new_kingdom.culture;
    new_kingdom.territory.insert(cap_idx);

    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    for (int d = 0; d < 4; ++d) {
        int nx = capital_x + dx[d];
        int ny = capital_y + dy[d];
        if (nx >= 0 && nx < WORLD_WIDTH && ny >= 0 && ny < WORLD_HEIGHT &&
                is_habitable(world, nx, ny) && ownerMap[ny][nx] == -1) {
            ownerMap[ny][nx] = new_kingdom.id;
            controlMap[ny][nx] = 0.8;
            cultureMap[ny][nx] = new_kingdom.culture;
            new_kingdom.territory.insert(linear_index(nx, ny, WORLD_WIDTH));
        }
    }

    kingdoms.push_back(new_kingdom);

    size_t new_size = kingdoms.size();
    for (auto& row : warMatrix) row.resize(new_size, 0);
    warMatrix.resize(new_size, std::vector<int>(new_size, 0));
    for (auto& row : allianceMatrix) row.resize(new_size, false);
    allianceMatrix.resize(new_size, std::vector<bool>(new_size, false));

    std::string race_str = (race == Race::HUMAN ? "人类" : race == Race::DWARF ? "矮人" :
                            race == Race::ELF ? "精灵" : race == Race::ORC ? "兽人" : "半身人");
    std::ostringstream oss;
    oss << "新王国 \"" << new_kingdom.name << "\" 在 (" << capital_x << ", " << capital_y
        << ") 建立，种族：" << race_str;
    EventRecord event(turn, format_time(turn),
                      EventType::FOUNDATION, new_kingdom.id, oss.str(), false,
                      capital_x, capital_y);
    globalEventLog.push_back(event);
    std::cout << "[" << turn << "] " << oss.str() << std::endl;
    return new_kingdom.id;
}

// ---------- 初始王国 ----------
void initialize_first_kingdom(const WorldData& world, std::vector<Kingdom>& kingdoms) {
    std::vector<Position> candidates;
    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        for (int x = 0; x < WORLD_WIDTH; ++x) {
            if (is_habitable(world, x, y)) candidates.push_back({x, y});
        }
    }
    if (candidates.empty()) {
        std::cerr << "错误：世界中没有适合建国的位置！" << std::endl;
        return;
    }
    std::uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
    Position capital = candidates[dist(global_rng)];

    Race initial_race = Race::HUMAN;
    if (FeatureToggle::MULTI_RACE) {
        int terrain = static_cast<int>(world.get(LAYER_TERRAIN, capital.x, capital.y));
        int biome = static_cast<int>(world.get(LAYER_BIOME, capital.x, capital.y));
        if (terrain >= static_cast<int>(TerrainClass::MOUNTAIN)) initial_race = Race::DWARF;
        else if (biome == static_cast<int>(Biome::BOREAL_FOREST) ||
                 biome == static_cast<int>(Biome::MIXED_FOREST) ||
                 biome == static_cast<int>(Biome::DECID_FOREST)) initial_race = Race::ELF;
        else if (biome == static_cast<int>(Biome::TUNDRA) ||
                 biome == static_cast<int>(Biome::SNOW_PLAINS)) initial_race = Race::ORC;
        else if (terrain == static_cast<int>(TerrainClass::HILL)) initial_race = Race::HALFLING;
        else initial_race = Race::HUMAN;
    }
    create_kingdom(kingdoms, world, capital.x, capital.y, initial_race, 0);
}

// ---------- 动态建国 ----------
void check_for_new_kingdoms(int turn, const WorldData& world, std::vector<Kingdom>& kingdoms) {
    if (!FeatureToggle::DYNAMIC_KINGDOMS) return;
    if (static_cast<int>(kingdoms.size()) >= MAX_KINGDOM_COUNT) return;
    double prob = DYNAMIC_KINGDOM_BASE_PROB / (1.0 + 0.1 * static_cast<double>(kingdoms.size()));
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    if (dist(global_rng) > prob) return;
    std::vector<Position> capitals;
    for (const auto& k : kingdoms) {
        for (const auto& s : k.settlements) {
            if (s.is_capital) {
                capitals.push_back(s.pos);
                break;
            }
        }
    }
    std::vector<Position> candidates;
    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        for (int x = 0; x < WORLD_WIDTH; ++x) {
            if (ownerMap[y][x] != -1) continue;
            if (!is_habitable(world, x, y)) continue;
            bool far_enough = true;
            for (const auto& cap : capitals) {
                int dist_sq = (x - cap.x) * (x - cap.x) + (y - cap.y) * (y - cap.y);
                if (dist_sq < 20 * 20) {
                    far_enough = false;
                    break;
                }
            }
            if (far_enough) candidates.push_back({x, y});
        }
    }
    if (candidates.empty()) return;
    std::uniform_int_distribution<size_t> pos_dist(0, candidates.size() - 1);
    Position pos = candidates[pos_dist(global_rng)];
    Race new_race = Race::HUMAN;
    if (FeatureToggle::MULTI_RACE) {
        int terrain = static_cast<int>(world.get(LAYER_TERRAIN, pos.x, pos.y));
        int biome = static_cast<int>(world.get(LAYER_BIOME, pos.x, pos.y));
        if (terrain >= static_cast<int>(TerrainClass::MOUNTAIN)) new_race = Race::DWARF;
        else if (biome == static_cast<int>(Biome::BOREAL_FOREST) ||
                 biome == static_cast<int>(Biome::MIXED_FOREST) ||
                 biome == static_cast<int>(Biome::DECID_FOREST)) new_race = Race::ELF;
        else if (biome == static_cast<int>(Biome::TUNDRA) ||
                 biome == static_cast<int>(Biome::SNOW_PLAINS)) new_race = Race::ORC;
        else if (terrain == static_cast<int>(TerrainClass::HILL)) new_race = Race::HALFLING;
        else new_race = Race::HUMAN;
    }
    int new_id = create_kingdom(kingdoms, world, pos.x, pos.y, new_race, turn);
    kingdoms[new_id].gold = Balance::NEW_KINGDOM_GOLD;
    kingdoms[new_id].food = Balance::NEW_KINGDOM_FOOD;
    kingdoms[new_id].army = Balance::NEW_KINGDOM_ARMY;
    kingdoms[new_id].stability = Balance::NEW_KINGDOM_STABILITY;
    std::cout << "[" << turn << "] 动态建国：新王国 " << kingdoms[new_id].name
              << "（ID=" << new_id << "）" << std::endl;
}

// ============================================================
// 第 3 部分结束
// ============================================================


// ============================================================
// 文明模拟器 - 第 4 部分：资源采集与经济
// ============================================================

// ---------- 创建建筑数据表 ----------
std::vector<BuildingData> create_building_data() {
    std::vector<BuildingData> data;
    auto add_common = [&](BuildingType type, const std::string& name, const std::string& cat,
                          double cost, double maint, int max_per_sett,
    std::vector<Effect> effects) {
        data.emplace_back(type, name, cat, cost, maint, max_per_sett,
                          false, Race::HUMAN, effects, std::vector<std::string> {});
    };
    auto add_race = [&](BuildingType type, const std::string& name, const std::string& cat,
                        double cost, double maint, int max_per_sett,
    Race race, std::vector<Effect> effects) {
        data.emplace_back(type, name, cat, cost, maint, max_per_sett,
                          true, race, effects, std::vector<std::string> {});
    };

    // 农业
    add_common(BuildingType::FARM, "农场", "agriculture", 30.0, 0.2, 3,
    { Effect(EffectStat::FOOD, 4.0) });
    add_common(BuildingType::GRANARY, "谷仓", "agriculture", 40.0, 0.3, 1,
    { Effect(EffectStat::FOOD, 3.0), Effect(EffectStat::FOOD_MOD, 0.05) });
    add_common(BuildingType::IRRIGATION_DITCH, "灌溉渠", "agriculture", 50.0, 0.4, 1,
    { Effect(EffectStat::FOOD_MOD, 0.15) });
    add_common(BuildingType::WINDMILL, "风车磨坊", "agriculture", 60.0, 0.5, 1,
    { Effect(EffectStat::FOOD, 4.0), Effect(EffectStat::FOOD_MOD, 0.05) });
    add_common(BuildingType::GREENHOUSE, "温室", "agriculture", 70.0, 0.6, 1,
    {   Effect(EffectStat::FOOD, 5.0), Effect(EffectStat::FOOD_MOD, 0.05),
        Effect(EffectStat::STABILITY, 1.0)
    });

    // 经济
    add_common(BuildingType::MARKET, "市场", "economy", 35.0, 0.3, 2,
    { Effect(EffectStat::GOLD, 3.0) });
    add_common(BuildingType::MARKET_SQUARE, "市场广场", "economy", 50.0, 0.5, 1,
    { Effect(EffectStat::GOLD, 5.0), Effect(EffectStat::TRADE_MOD, 0.05) });
    add_common(BuildingType::TRADING_POST, "商栈", "economy", 45.0, 0.4, 1,
    { Effect(EffectStat::GOLD, 3.0), Effect(EffectStat::TRADE_MOD, 0.10) });
    add_common(BuildingType::MINT, "铸币厂", "economy", 60.0, 0.6, 1,
    { Effect(EffectStat::GOLD, 3.0), Effect(EffectStat::TAX_MOD, 0.10) });
    add_common(BuildingType::GUILD_HALL, "商会", "economy", 80.0, 0.8, 1,
    {   Effect(EffectStat::GOLD, 5.0), Effect(EffectStat::TRADE_MOD, 0.15),
        Effect(EffectStat::STABILITY, 1.0)
    });
    add_common(BuildingType::WAREHOUSE, "仓库", "economy", 30.0, 0.2, 2,
    { Effect(EffectStat::FOOD, 2.0), Effect(EffectStat::GOLD, 1.0) });
    add_common(BuildingType::TAVERN, "酒馆", "economy", 25.0, 0.2, 2,
    { Effect(EffectStat::GOLD, 3.0), Effect(EffectStat::STABILITY, 1.0) });

    // 军事
    add_common(BuildingType::BARRACKS, "兵营", "military", 40.0, 0.4, 2,
    {   Effect(EffectStat::MILITARY_POWER_MOD, 0.05),
        Effect(EffectStat::ARMY_MAINTENANCE_MOD, -0.05)
    });
    add_common(BuildingType::TRAINING_GROUND, "训练场", "military", 50.0, 0.5, 1,
    { Effect(EffectStat::MILITARY_POWER_MOD, 0.08) });
    add_common(BuildingType::ARCHERY_RANGE, "靶场", "military", 45.0, 0.4, 1,
    {   Effect(EffectStat::MILITARY_POWER_MOD, 0.05),
        Effect(EffectStat::ATTACK_BONUS, 0.1)
    });
    add_common(BuildingType::STABLE, "马厩", "military", 55.0, 0.5, 1,
    {   Effect(EffectStat::MILITARY_POWER_MOD, 0.05),
        Effect(EffectStat::ROAD_SPEED_MOD, 0.10)
    });
    add_common(BuildingType::SIEGE_WORKSHOP, "攻城工坊", "military", 70.0, 0.7, 1,
    {   Effect(EffectStat::MILITARY_POWER_MOD, 0.10),
        Effect(EffectStat::ATTACK_BONUS, 0.15)
    });

    // 防御
    add_common(BuildingType::WALL, "城墙", "defense", 50.0, 0.5, 1,
    { Effect(EffectStat::DEFENSE_BONUS, 0.5) });
    add_common(BuildingType::FORTRESS, "要塞", "defense", 80.0, 0.8, 1,
    {   Effect(EffectStat::DEFENSE_BONUS, 1.0),
        Effect(EffectStat::STABILITY, 2.0)
    });
    add_common(BuildingType::WATCHTOWER, "瞭望塔", "defense", 20.0, 0.2, 3,
    { Effect(EffectStat::DEFENSE_BONUS, 0.2) });
    add_common(BuildingType::MOAT, "护城河", "defense", 60.0, 0.6, 1,
    {   Effect(EffectStat::DEFENSE_BONUS, 0.4),
        Effect(EffectStat::STABILITY, 1.0)
    });

    // 科技
    add_common(BuildingType::ACADEMY, "学院", "tech", 60.0, 0.6, 2,
    { Effect(EffectStat::RESEARCH_POINTS, 0.02) });
    add_common(BuildingType::LIBRARY, "图书馆", "tech", 50.0, 0.5, 2,
    { Effect(EffectStat::RESEARCH_POINTS, 0.03) });
    add_common(BuildingType::UNIVERSITY, "大学", "tech", 100.0, 1.0, 1,
    {   Effect(EffectStat::RESEARCH_POINTS, 0.05),
        Effect(EffectStat::STABILITY, 2.0)
    });
    add_common(BuildingType::OBSERVATORY, "天文台", "tech", 80.0, 0.8, 1,
    {   Effect(EffectStat::RESEARCH_POINTS, 0.03),
        Effect(EffectStat::MAGIC_AFFINITY, 0.05)
    });
    add_common(BuildingType::SAGE_HALL, "贤者厅", "tech", 90.0, 0.9, 1,
    {   Effect(EffectStat::RESEARCH_POINTS, 0.03),
        Effect(EffectStat::STABILITY, 3.0)
    });

    // 文化
    add_common(BuildingType::TEMPLE, "神庙", "culture", 40.0, 0.4, 3,
    { Effect(EffectStat::STABILITY, 3.0) });
    add_common(BuildingType::CATHEDRAL, "大教堂", "culture", 80.0, 0.8, 1,
    {   Effect(EffectStat::STABILITY, 5.0),
        Effect(EffectStat::CULTURE_CHANGE_MOD, 0.05)
    });
    add_common(BuildingType::THEATER, "剧院", "culture", 70.0, 0.7, 1,
    {   Effect(EffectStat::STABILITY, 4.0),
        Effect(EffectStat::CULTURE_CHANGE_MOD, 0.03)
    });
    add_common(BuildingType::ARENA, "竞技场", "culture", 65.0, 0.6, 1,
    {   Effect(EffectStat::STABILITY, 3.0),
        Effect(EffectStat::GOLD, 5.0)
    });
    add_common(BuildingType::MONUMENT, "纪念碑", "culture", 55.0, 0.5, 2,
    {   Effect(EffectStat::STABILITY, 2.0),
        Effect(EffectStat::CULTURE_CHANGE_MOD, 0.02)
    });

    // 民生/福利
    add_common(BuildingType::HOSPITAL, "医院", "welfare", 70.0, 0.7, 1,
    {   Effect(EffectStat::GROWTH_MOD, 0.10),
        Effect(EffectStat::STABILITY, 2.0)
    });
    add_common(BuildingType::ORPHANAGE, "孤儿院", "welfare", 40.0, 0.4, 1,
    {   Effect(EffectStat::STABILITY, 2.0),
        Effect(EffectStat::GROWTH_MOD, 0.05)
    });
    add_common(BuildingType::BATHHOUSE, "浴场", "welfare", 45.0, 0.4, 1,
    {   Effect(EffectStat::STABILITY, 2.0),
        Effect(EffectStat::GOLD, 3.0)
    });
    add_common(BuildingType::AQUEDUCT, "水渠", "welfare", 60.0, 0.6, 1,
    {   Effect(EffectStat::FOOD_MOD, 0.05),
        Effect(EffectStat::GROWTH_MOD, 0.05)
    });
    add_common(BuildingType::WELL, "水井", "welfare", 15.0, 0.1, 5,
    {   Effect(EffectStat::FOOD_MOD, 0.02),
        Effect(EffectStat::GROWTH_MOD, 0.02)
    });

    // 航海
    add_common(BuildingType::HARBOR, "港口", "naval", 55.0, 0.5, 1,
    {   Effect(EffectStat::GOLD, 10.0),
        Effect(EffectStat::TRADE_MOD, 0.05)
    });
    add_common(BuildingType::SHIPYARD, "船坞", "naval", 60.0, 0.6, 1,
    {   Effect(EffectStat::GOLD, 5.0),
        Effect(EffectStat::TRADE_MOD, 0.10),
        Effect(EffectStat::ROAD_SPEED_MOD, 0.05)
    });
    add_common(BuildingType::LIGHTHOUSE, "灯塔", "naval", 50.0, 0.5, 1,
    {   Effect(EffectStat::GOLD, 4.0),
        Effect(EffectStat::STABILITY, 1.0)
    });
    add_common(BuildingType::FISHERY, "渔场", "naval", 30.0, 0.3, 2,
    { Effect(EffectStat::FOOD, 6.0) });

    // 魔法
    add_common(BuildingType::ARCANE_LAB, "奥术实验室", "magic", 80.0, 1.0, 1,
    {   Effect(EffectStat::RESEARCH_POINTS, 0.04),
        Effect(EffectStat::MAGIC_AFFINITY, 0.10)
    });
    add_common(BuildingType::MANA_WELL, "法力井", "magic", 70.0, 0.8, 1,
    {   Effect(EffectStat::MAGIC_AFFINITY, 0.15),
        Effect(EffectStat::STABILITY, 1.0)
    });
    add_common(BuildingType::ENCHANTED_TOWER, "附魔塔", "magic", 120.0, 1.2, 1,
    {   Effect(EffectStat::DEFENSE_BONUS, 0.3),
        Effect(EffectStat::MAGIC_AFFINITY, 0.20)
    });

    // 通用矿场
    add_common(BuildingType::MINE, "矿场", "economy", 40.0, 0.5, 1,
    { Effect(EffectStat::GOLD, 5.0) });

    // 人类特色
    add_race(BuildingType::KNIGHT_ORDER, "骑士团驻地", "military", 70.0, 0.8, 1, Race::HUMAN,
    {   Effect(EffectStat::MILITARY_POWER_MOD, 0.08),
        Effect(EffectStat::STABILITY, 2.0)
    });
    add_race(BuildingType::ROYAL_ACADEMY, "皇家学院", "tech", 100.0, 1.0, 1, Race::HUMAN,
    {   Effect(EffectStat::RESEARCH_POINTS, 0.05),
        Effect(EffectStat::STABILITY, 3.0)
    });
    add_race(BuildingType::CATHEDRAL_GRAND, "大教堂（人类）", "culture", 90.0, 0.9, 1, Race::HUMAN,
    {   Effect(EffectStat::STABILITY, 6.0),
        Effect(EffectStat::CULTURE_CHANGE_MOD, 0.08)
    });

    // 矮人特色
    add_race(BuildingType::DWARF_FORGE, "矮人锻造厂", "military", 80.0, 1.0, 1, Race::DWARF,
    {   Effect(EffectStat::MILITARY_POWER_MOD, 0.10),
        Effect(EffectStat::DEFENSE_BONUS, 0.4)
    });
    add_race(BuildingType::DEEP_MINE, "深井矿场", "economy", 70.0, 0.8, 1, Race::DWARF,
    { Effect(EffectStat::GOLD, 10.0) });
    add_race(BuildingType::MOUNTAIN_FORTRESS, "山腹要塞", "defense", 100.0, 1.2, 1, Race::DWARF,
    {   Effect(EffectStat::DEFENSE_BONUS, 1.2),
        Effect(EffectStat::STABILITY, 2.0)
    });
    add_race(BuildingType::RUNE_HALL, "符文大厅", "tech", 90.0, 1.0, 1, Race::DWARF,
    {   Effect(EffectStat::RESEARCH_POINTS, 0.04),
        Effect(EffectStat::MAGIC_AFFINITY, 0.10)
    });

    // 精灵特色
    add_race(BuildingType::TREE_HOUSE, "古树居所", "welfare", 60.0, 0.6, 1, Race::ELF,
    {   Effect(EffectStat::POPULATION, 20.0),
        Effect(EffectStat::STABILITY, 2.0)
    });
    add_race(BuildingType::STAR_ALTAR, "星辉祭坛", "tech", 80.0, 0.8, 1, Race::ELF,
    {   Effect(EffectStat::RESEARCH_POINTS, 0.04),
        Effect(EffectStat::MAGIC_AFFINITY, 0.20)
    });
    add_race(BuildingType::FOREST_HUNT, "森林猎场", "agriculture", 50.0, 0.5, 2, Race::ELF,
    {   Effect(EffectStat::FOOD, 6.0),
        Effect(EffectStat::MILITARY_POWER_MOD, 0.05)
    });
    add_race(BuildingType::MOON_POOL, "月池", "welfare", 70.0, 0.7, 1, Race::ELF,
    {   Effect(EffectStat::GROWTH_MOD, 0.05),
        Effect(EffectStat::STABILITY, 3.0)
    });

    // 兽人特色
    add_race(BuildingType::WAR_TOTEM, "战争图腾", "military", 60.0, 0.6, 1, Race::ORC,
    {   Effect(EffectStat::MILITARY_POWER_MOD, 0.15),
        Effect(EffectStat::STABILITY, 2.0)
    });
    add_race(BuildingType::BLOOD_ALTAR, "血祭坛", "military", 50.0, 0.5, 1, Race::ORC,
    {   Effect(EffectStat::ARMY, 10.0),
        Effect(EffectStat::STABILITY, -3.0)
    });
    add_race(BuildingType::RAID_CAMP, "掠夺营地", "economy", 55.0, 0.5, 1, Race::ORC,
    {   Effect(EffectStat::GOLD, 5.0),
        Effect(EffectStat::WAR_LOOT_MOD, 0.10)
    });
    add_race(BuildingType::SHAMAN_HUT, "萨满小屋", "magic", 65.0, 0.6, 1, Race::ORC,
    {   Effect(EffectStat::MAGIC_AFFINITY, 0.10),
        Effect(EffectStat::STABILITY, 1.0)
    });

    // 半身人特色
    add_race(BuildingType::HARVEST_INN, "丰收酒馆", "agriculture", 40.0, 0.4, 2, Race::HALFLING,
    {   Effect(EffectStat::FOOD, 8.0),
        Effect(EffectStat::STABILITY, 3.0)
    });
    add_race(BuildingType::PIPE_HOUSE, "烟斗屋", "welfare", 30.0, 0.3, 2, Race::HALFLING,
    {   Effect(EffectStat::STABILITY, 4.0),
        Effect(EffectStat::CULTURE_CHANGE_MOD, 0.03)
    });
    add_race(BuildingType::TUNNEL_WAREHOUSE, "隧道仓库", "economy", 50.0, 0.5, 1, Race::HALFLING,
    {   Effect(EffectStat::FOOD, 10.0),
        Effect(EffectStat::GOLD, 3.0)
    });
    add_race(BuildingType::GARDEN_DISTRICT, "花园街区", "welfare", 70.0, 0.7, 1, Race::HALFLING,
    {   Effect(EffectStat::GROWTH_MOD, 0.05),
        Effect(EffectStat::STABILITY, 5.0)
    });

    return data;
}

// ---------- 全局建筑数据表初始化 ----------
std::vector<BuildingData> globalBuildingData = create_building_data();

// ---------- 王国效果缓存更新 ----------
void Kingdom::update_cached_effects() {
    // ---------- 1. 重置所有缓存为基准值 ----------
    cached_food_mod = 1.0;
    cached_gold_mod = 1.0;
    cached_military_mod = 1.0;
    cached_research_mod = 1.0;
    cached_growth_mod = 1.0;
    cached_defense_bonus = 0.0;
    cached_attack_bonus = 0.0;
    cached_tax_mod = 1.0;
    cached_building_cost_mod = 1.0;
    cached_road_speed_mod = 1.0;
    cached_magic_affinity = 0.0;
    cached_rebellion_risk_mod = 1.0;
    cached_maintenance_mod = 1.0;

    // ---------- 2. 累加建筑效果 ----------
    for (const auto& s : settlements) {
        for (BuildingType bt : s.buildings) {
            for (const auto& bd : globalBuildingData) {
                if (bd.type == bt) {
                    for (const Effect& eff : bd.effects) {
                        switch (eff.stat) {
                        case EffectStat::FOOD_MOD:
                            cached_food_mod += eff.value;
                            break;
                        case EffectStat::GOLD_MOD:
                            cached_gold_mod += eff.value;
                            break;
                        case EffectStat::MILITARY_POWER_MOD:
                            cached_military_mod += eff.value;
                            break;
                        case EffectStat::DEFENSE_BONUS:
                            cached_defense_bonus += eff.value;
                            break;
                        case EffectStat::ATTACK_BONUS:
                            cached_attack_bonus += eff.value;
                            break;
                        case EffectStat::TECH_MOD:
                            cached_research_mod += eff.value;
                            break;
                        case EffectStat::GROWTH_MOD:
                            cached_growth_mod += eff.value;
                            break;
                        case EffectStat::TAX_MOD:
                            cached_tax_mod += eff.value;
                            break;
                        case EffectStat::BUILDING_COST_MOD:
                            cached_building_cost_mod += eff.value;
                            break;
                        case EffectStat::ROAD_SPEED_MOD:
                            cached_road_speed_mod += eff.value;
                            break;
                        case EffectStat::MAGIC_AFFINITY:
                            cached_magic_affinity += eff.value;
                            break;
                        case EffectStat::REBELLION_RISK_MOD:
                            cached_rebellion_risk_mod += eff.value;
                            break;
                        case EffectStat::MAINTENANCE_MOD:
                            cached_maintenance_mod += eff.value;
                            break;
                        case EffectStat::RESEARCH_RATE_MOD:
                            cached_research_mod += eff.value;
                            break;
                        default:
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    // ---------- 3. 累加科技效果 ----------
    for (TechID tech_id : owned_techs) {
        for (const TechNode& tech : globalTechPool) {
            if (tech.id == tech_id) {
                for (const Effect& eff : tech.effects) {
                    switch (eff.stat) {
                    case EffectStat::FOOD_MOD:
                        cached_food_mod += eff.value;
                        break;
                    case EffectStat::GOLD_MOD:
                        cached_gold_mod += eff.value;
                        break;
                    case EffectStat::MILITARY_POWER_MOD:
                        cached_military_mod += eff.value;
                        break;
                    case EffectStat::DEFENSE_BONUS:
                        cached_defense_bonus += eff.value;
                        break;
                    case EffectStat::ATTACK_BONUS:
                        cached_attack_bonus += eff.value;
                        break;
                    case EffectStat::TECH_MOD:
                        cached_research_mod += eff.value;
                        break;
                    case EffectStat::GROWTH_MOD:
                        cached_growth_mod += eff.value;
                        break;
                    case EffectStat::TAX_MOD:
                        cached_tax_mod += eff.value;
                        break;
                    case EffectStat::BUILDING_COST_MOD:
                        cached_building_cost_mod += eff.value;
                        break;
                    case EffectStat::ROAD_SPEED_MOD:
                        cached_road_speed_mod += eff.value;
                        break;
                    case EffectStat::MAGIC_AFFINITY:
                        cached_magic_affinity += eff.value;
                        break;
                    case EffectStat::REBELLION_RISK_MOD:
                        cached_rebellion_risk_mod += eff.value;
                        break;
                    case EffectStat::MAINTENANCE_MOD:
                        cached_maintenance_mod += eff.value;
                        break;
                    case EffectStat::RESEARCH_RATE_MOD:
                        cached_research_mod += eff.value;
                        break;
                    default:
                        break;
                    }
                }
                break;
            }
        }
    }

    // ---------- 4. 应用文明等级持久修正系数 ----------
    // civ_effect_mod 由 update_civilization_level() 维护，范围 0.3 ~ 2.0
    // 低文明惩罚（系数小于1），高文明加成（系数大于1）
    cached_growth_mod = std::max(0.6, cached_growth_mod);
    
    cached_food_mod *= civ_effect_mod;
    cached_gold_mod *= civ_effect_mod;
    cached_military_mod *= civ_effect_mod;
    cached_research_mod *= civ_effect_mod;
    cached_growth_mod *= civ_effect_mod;
    cached_defense_bonus *= civ_effect_mod;      // 防御加成也受文明影响
    cached_attack_bonus *= civ_effect_mod;
    cached_tax_mod *= civ_effect_mod;
    cached_building_cost_mod *= civ_effect_mod;   // 注意：成本系数，文明越高成本越低（但该值越大表示成本增加，所以乘以系数可能不合理，我们改为成本 = 基础 / civ_effect_mod？这里保持简单，直接相乘，后续可调整）
    cached_road_speed_mod *= civ_effect_mod;
    cached_magic_affinity *= civ_effect_mod;
    // 叛乱风险：文明越低，风险越高（系数大于1）
    cached_rebellion_risk_mod = 1.0 + (1.0 - civ_effect_mod) * 0.5;  // 当civ_effect_mod=0.5时，风险=1.25；=1.0时，风险=1.0；=1.5时，风险=0.75
    cached_maintenance_mod *= civ_effect_mod;    // 维护费受文明影响
}
// ---------- 检查居住区是否通过道路连通首都 ----------
bool is_settlement_connected_to_capital(const Kingdom& k, const Settlement& s) {
    if (s.is_capital) return true;
    
    Position capital_pos(-1, -1);
    for (const auto& cap : k.settlements) {
        if (cap.is_capital) {
            capital_pos = cap.pos;
            break;
        }
    }
    if (capital_pos.x == -1) return false;
    
    std::queue<Position> q;
    std::unordered_set<int> visited;
    q.push(s.pos);
    visited.insert(linear_index(s.pos.x, s.pos.y, WORLD_WIDTH));
    
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    
    while (!q.empty()) {
        Position cur = q.front();
        q.pop();
        if (cur.x == capital_pos.x && cur.y == capital_pos.y) return true;
        
        for (int d = 0; d < 4; ++d) {
            int nx = cur.x + dx[d];
            int ny = cur.y + dy[d];
            if (nx < 0 || nx >= WORLD_WIDTH || ny < 0 || ny >= WORLD_HEIGHT) continue;
            
            // ===== [修改] 道路归属权检查：只能使用本国道路或共有道路 =====
            // 原逻辑：if (roadMap[ny][nx] == 0) continue;  →  任何国家的道路都能用（“蹭网”）
            // 新逻辑：必须属于本王国（k.id）或共有道路（-1）才能通行
            if (roadMap[ny][nx] == 0) continue;  // 无道路
            if (roadMap[ny][nx] != k.id && roadMap[ny][nx] != -1) continue;  // 不是自己的路，也非共有
            
            int idx = linear_index(nx, ny, WORLD_WIDTH);
            if (visited.find(idx) != visited.end()) continue;
            visited.insert(idx);
            q.push({nx, ny});
        }
    }
    return false;
}


// ---------- 资源采集 ----------
void collect_resources(Kingdom& k, WorldData& world, int turn) {
    k.update_cached_effects();

    double base_food = 0.0;
    double base_gold = 0.0;

    // 领土格基础产出
    for (int idx : k.territory) {
        int x = idx % WORLD_WIDTH;
        int y = idx / WORLD_WIDTH;
        if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT) continue;
        double control = controlMap[y][x];
        if (control <= 0.0) continue;

        double npp = world.get(LAYER_NPP, x, y);
        base_food += npp * Balance::NPP_TO_FOOD_FACTOR * control;

        int mineral = static_cast<int>(world.get(LAYER_MINERAL, x, y));
        if (mineral > 0) {
            switch (mineral) {
            case 1:
                base_gold += 3.0 * control;
                break;
            case 2:
                base_gold += 8.0 * control;
                break;
            case 3:
                base_gold += 5.0 * control;
                break;
            case 4:
                base_gold += 4.0 * control;
                break;
            default:
                base_gold += 2.0 * control;
            }
        }

        int wildlife = static_cast<int>(world.get(LAYER_WILDLIFE, x, y));
        base_gold += wildlife * Balance::HUNTING_GOLD_FACTOR * control;
    }

    // 建筑直接产出（不受比例修正影响）
    double building_food = 0.0;
    double building_gold = 0.0;
    for (const auto& s : k.settlements) {
        for (BuildingType bt : s.buildings) {
            for (const auto& bd : globalBuildingData) {
                if (bd.type == bt) {
                    for (const Effect& eff : bd.effects) {
                        if (eff.stat == EffectStat::FOOD) building_food += eff.value;
                        else if (eff.stat == EffectStat::GOLD) building_gold += eff.value;
                    }
                    break;
                }
            }
        }
    }

    // 税收（包含腐败因子和稳定度因子）
    double corruption_factor = 1.0 / (1.0 + static_cast<double>(k.territory.size()) / 1000.0);
    double stability_tax_factor = 0.5 + (k.stability / 200.0);
    double tax_income = k.total_population() * k.tax_rate * k.cached_tax_mod 
                        * corruption_factor * stability_tax_factor;

    // 净资源（应用修正）
    double total_food = base_food * k.cached_food_mod + building_food;
    double total_gold = base_gold * k.cached_gold_mod + building_gold + tax_income;

    // ---------- 维护费 ----------
    double building_maintenance = 0.0;
    for (const auto& s : k.settlements) {
        for (BuildingType bt : s.buildings) {
            for (const auto& bd : globalBuildingData) {
                if (bd.type == bt) {
                    building_maintenance += bd.maintenance;
                    break;
                }
            }
        }
    }
    building_maintenance *= k.cached_maintenance_mod;
    
    double road_maintenance = 0.0;
    if (FeatureToggle::ROADS) {
        road_maintenance = k.road_tiles.size() * Balance::ROAD_MAINTENANCE;
    }
    
    double army_food_cost = k.army * ARMY_FOOD_CONSUMPTION;

    // ===== [新增] 人口基础食物消耗 =====
    // 每个居民每回合消耗 0.3 食物（维持基本生存）
    int total_pop = k.total_population();
    double population_food_consumption = total_pop * 0.3;

    // ===== [新增] 国王挥霍（基于贪婪度） =====
    // 贪婪度越高，消耗金币越多
    double king_squander = k.greed * 0.1 * total_pop;
    // 但是不能超过总金币的 10%
    king_squander = std::min(king_squander, k.gold * 0.1);

    // ===== [新增] 官僚腐败（基于领土大小） =====
    // 领土越大，官僚体系越臃肿，腐败越严重
    double corruption_cost = static_cast<double>(k.territory.size()) * 0.05;
    // 但不能超过总金币的 15%
    corruption_cost = std::min(corruption_cost, k.gold * 0.15);

    // ===== [新增] 食物腐败损耗 =====
    // 食物超过 5000 后，超出部分每回合衰减 5%
    constexpr double FOOD_SPOILAGE_THRESHOLD = 5000.0;
    constexpr double FOOD_SPOILAGE_RATE = 0.05;
    double food_spoilage = 0.0;
    if (k.food > FOOD_SPOILAGE_THRESHOLD) {
        double excess = k.food - FOOD_SPOILAGE_THRESHOLD;
        food_spoilage = excess * FOOD_SPOILAGE_RATE;
    }

    // ===== [新增] 减税事件（如果稳定度过低或发生叛乱） =====
    // 当稳定度 < 30 且税率 > 0.15 时，自动减税（模拟农民反抗）
    if (k.stability < 30.0 && k.tax_rate > 0.15) {
        double tax_reduction = std::min(0.02, (30.0 - k.stability) / 100.0 * 0.05);
        k.tax_rate = std::max(0.05, k.tax_rate - tax_reduction);
        // 减税会降低金币收入，但增加稳定度
        k.stability = std::min(MAX_STABILITY, k.stability + 1.0);
        // 记录减税事件（仅当减税幅度大于 0.005）
        if (tax_reduction > 0.005) {
            std::ostringstream oss;
            oss << k.name << " 因稳定度过低自动减税至 " << std::fixed << std::setprecision(2) << k.tax_rate;
            EventRecord event(turn, format_time(turn),
                              EventType::ECONOMIC, k.id, oss.str(), false);
            globalEventLog.push_back(event);
            std::cout << "[" << turn << "] " << oss.str() << std::endl;
        }
    }

    // ---------- 应用所有收入和消耗 ----------
    k.food += total_food - army_food_cost - population_food_consumption - food_spoilage;
    k.gold += total_gold - building_maintenance - road_maintenance - king_squander - corruption_cost;

    // ---------- 资源上限截断 ----------
    if (k.food > MAX_FOOD) k.food = MAX_FOOD;
    if (k.food < 0.0) k.food = 0.0;
    if (k.gold > MAX_GOLD) k.gold = MAX_GOLD;
    if (k.gold < 0.0) k.gold = 0.0;

    // ---------- 资源溢出损耗（保留原有的溢出衰减） ----------
    constexpr double SURPLUS_THRESHOLD = 90000.0;
    constexpr double DECAY_RATE = 0.05;
    if (k.food > SURPLUS_THRESHOLD) {
        double excess = k.food - SURPLUS_THRESHOLD;
        k.food -= excess * DECAY_RATE;
    }
    if (k.gold > SURPLUS_THRESHOLD) {
        double excess = k.gold - SURPLUS_THRESHOLD;
        k.gold -= excess * DECAY_RATE;
    }

    // 再次截断
    if (k.food > MAX_FOOD) k.food = MAX_FOOD;
    if (k.food < 0.0) k.food = 0.0;
    if (k.gold > MAX_GOLD) k.gold = MAX_GOLD;
    if (k.gold < 0.0) k.gold = 0.0;

    // 资源退化
    if (FeatureToggle::RESOURCE_DEGRADATION) {
        apply_resource_degradation(k, world, turn);
    }
}
// ---------- 资源退化 ----------
void apply_resource_degradation(Kingdom& k, WorldData& world, int turn) {
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    for (auto& s : k.settlements) {
        int x = s.pos.x;
        int y = s.pos.y;
        if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT) continue;

        int farm_count = 0;
        bool has_mine = false;
        bool has_shipyard = false;
        bool has_fortress = false;
        for (BuildingType bt : s.buildings) {
            if (bt == BuildingType::FARM) farm_count++;
            if (bt == BuildingType::MINE || bt == BuildingType::DEEP_MINE) has_mine = true;
            if (bt == BuildingType::SHIPYARD) has_shipyard = true;
            if (bt == BuildingType::FORTRESS) has_fortress = true;
        }

        // 土壤退化/恢复
        if (farm_count > 3 && world.get(LAYER_SOIL_FERT, x, y) > 0.0) {
            if (prob(global_rng) < Balance::SOIL_DEGRADE_PROB) {
                double fert = world.get(LAYER_SOIL_FERT, x, y);
                fert = std::max(0.0, fert - 1.0);
                world.set(LAYER_SOIL_FERT, x, y, fert);
            }
        } else if (farm_count == 0 && world.get(LAYER_SOIL_FERT, x, y) < 5.0) {
            if ((turn % Balance::SOIL_RECOVER_INTERVAL) == 0) {
                double fert = world.get(LAYER_SOIL_FERT, x, y);
                fert = std::min(5.0, fert + 1.0);
                world.set(LAYER_SOIL_FERT, x, y, fert);
            }
        }

        // 矿产枯竭
        if (has_mine && world.get(LAYER_MINERAL, x, y) > 0.0) {
            if (prob(global_rng) < Balance::MINERAL_DEPLETE_PROB) {
                world.set(LAYER_MINERAL, x, y, 0.0);
            }
        }

        // 森林砍伐
        if (has_shipyard || has_fortress) {
            const int dx[4] = {1, -1, 0, 0};
            const int dy[4] = {0, 0, 1, -1};
            for (int d = 0; d < 4; ++d) {
                int nx = x + dx[d];
                int ny = y + dy[d];
                if (nx < 0 || nx >= WORLD_WIDTH || ny < 0 || ny >= WORLD_HEIGHT) continue;
                int veg = static_cast<int>(world.get(LAYER_VEGETATION, nx, ny));
                if (veg == static_cast<int>(VegetationClass::FOREST) ||
                        veg == static_cast<int>(VegetationClass::RAINFOREST) ||
                        veg == static_cast<int>(VegetationClass::BOREAL)) {
                    if (prob(global_rng) < Balance::FOREST_DEGRADE_PROB) {
                        if (veg == static_cast<int>(VegetationClass::RAINFOREST)) {
                            world.set(LAYER_VEGETATION, nx, ny, static_cast<double>(VegetationClass::FOREST));
                        } else {
                            world.set(LAYER_VEGETATION, nx, ny, static_cast<double>(VegetationClass::GRASS));
                        }
                    }
                }
            }
        }
    }
}

// ============================================================
// 第 4 部分结束
// ============================================================

// ============================================================
// 第 5 部分：人口增长与居住区管理
// ============================================================

// ---------- 人口增长 ----------
void grow_population(Kingdom& k, int turn) {
    // ---------- 1. 计算基础增长率 ----------
    double base_rate = Balance::BASE_POP_GROWTH_RATE;
    double effective_growth_mod = std::max(0.6, k.cached_growth_mod);
    double total_rate_factor = k.industriousness * k.growth_modifier() * effective_growth_mod;

    // ---------- 2. 计算人口承载力 ----------
    // 承载力 = 领土格数 * 每格承载力 + 科技等级 * 每级科技承载力
    double carrying_capacity = static_cast<double>(k.territory.size()) * Balance::CARRYING_CAPACITY_PER_TILE
                               + k.tech_level * Balance::CARRYING_CAPACITY_PER_TECH;
    // 确保承载力至少为 10（避免初始时过小）
    carrying_capacity = std::max(10.0, carrying_capacity);

    int total_pop = k.total_population();
    double load_factor = static_cast<double>(total_pop) / carrying_capacity;

    // ---------- 3. 计算承载力衰减因子 ----------
    // load_factor = 0.5 时衰减到 0.67；=1.0 时衰减到 0.33；>1.5 时变为负数（人口减少）
    double carrying_factor = 1.0 / (1.0 + load_factor * Balance::CARRYING_FACTOR_SCALE);
    // 限制范围，避免极端负增长（但若超载严重，factor 可低至 0.1 以下）
    carrying_factor = std::max(0.1, std::min(1.0, carrying_factor));

    // ---------- 4. 最终增长率（至少保证 0.1% 的微增长，避免绝对停滞） ----------
    double growth_rate = base_rate * total_rate_factor * carrying_factor;
    growth_rate = std::max(Balance::MIN_GROWTH_RATE, growth_rate);

    // ---------- 5. 计算每个定居点的潜在增长 ----------
    std::vector<int> potential_growth(k.settlements.size(), 0);
    int total_potential_growth = 0;
    for (size_t i = 0; i < k.settlements.size(); ++i) {
        double growth = k.settlements[i].population * growth_rate;
        int int_growth = static_cast<int>(std::llround(growth));
        if (int_growth < 0) int_growth = 0;
        potential_growth[i] = int_growth;
        total_potential_growth += int_growth;
    }

    // ---------- 6. 食物消耗与增长 ----------
    double food_required = static_cast<double>(total_potential_growth);
    k.food -= food_required;

    if (k.food >= 0.0) {
        // 食物充足，正常增长
        for (size_t i = 0; i < k.settlements.size(); ++i) {
            k.settlements[i].population += potential_growth[i];
        }
    } else {
        // 食物不足，饥荒惩罚
        // 原惩罚为 0.2，现改为动态：基础 0.15，超载时加重
        double deficit = -k.food;
        int total_pop_now = k.total_population();
        if (total_pop_now <= 0) return;
        // 基础饥荒比例 = 食物缺口 / 总人口，再乘以承载力因子（承载力越高，饥荒影响越小）
        double famine_ratio = std::min(0.15, deficit / static_cast<double>(total_pop_now));
        // 超载时饥荒更严重：load_factor 越高，额外惩罚越大
        famine_ratio *= (1.0 + load_factor * 0.5);
        famine_ratio = std::min(Balance::MAX_FAMINE_RATIO, famine_ratio);
        for (auto& s : k.settlements) {
            int reduction = static_cast<int>(std::llround(s.population * famine_ratio));
            s.population = std::max(Balance::MIN_SETTLEMENT_POP, s.population - reduction);
        }
        k.food = 0.0;
    }

    // ---------- 7. 移除废弃定居点（人口低于阈值） ----------
    remove_deserted_settlements(k);

    // ---------- 8. 更新峰值记录 ----------
    int current_pop = k.total_population();
    if (current_pop > k.max_population) k.max_population = current_pop;
    if (static_cast<int>(k.territory.size()) > k.max_territory_size)
        k.max_territory_size = static_cast<int>(k.territory.size());
}
// ---------- 移除废弃居住区 ----------
void remove_deserted_settlements(Kingdom& k) {
    std::vector<Settlement> surviving;
    for (auto& s : k.settlements) {
        if (!s.is_capital && s.population <= Balance::DESERTED_POP_THRESHOLD) {
            int idx = linear_index(s.pos.x, s.pos.y, WORLD_WIDTH);
            if (idx >= 0 && idx < WORLD_WIDTH * WORLD_HEIGHT) {
                ownerMap[s.pos.y][s.pos.x] = -1;
                controlMap[s.pos.y][s.pos.x] = 0.0;
                k.territory.erase(idx);
            }
        } else {
            surviving.push_back(s);
        }
    }
    k.settlements = surviving;
}

// ---------- 管理居住区（建筑建造） ----------
void manage_settlements(Kingdom& k, const WorldData& world, int turn) {
    // ---------- 1. 前置计算 ----------
    double food_ratio = (k.total_population() > 0) ? (k.food / k.total_population()) : 1.5;
    bool gold_shortage = (k.gold < 50.0);
    bool food_shortage = (food_ratio < 1.0);
    bool army_shortage = (k.army < 30.0 && k.total_population() > 100);
    bool stability_low = (k.stability < 50.0);
    bool tech_low = (k.tech_level < 5.0);

    // ---------- 2. 生成候选建筑列表 ----------
    struct BuildCandidate {
        double score;
        int settlement_index;
        BuildingType type;
    };
    std::vector<BuildCandidate> candidates;

    for (size_t si = 0; si < k.settlements.size(); ++si) {
        Settlement& s = k.settlements[si];

        // 单城建筑上限检查（原有）
        int max_buildings = 5 + s.population / 10;
        max_buildings = std::min(20, max_buildings);
        if (static_cast<int>(s.buildings.size()) >= max_buildings) {
            continue;
        }

        int x = s.pos.x;
        int y = s.pos.y;

        bool coastal = false;
        const int dx[8] = {1,1,0,-1,-1,-1,0,1};
        const int dy[8] = {0,1,1,1,0,-1,-1,-1};
        for (int d = 0; d < 8; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx >= 0 && nx < WORLD_WIDTH && ny >= 0 && ny < WORLD_HEIGHT) {
                if (world.get(LAYER_ELEVATION, nx, ny) <= 0.0) {
                    coastal = true;
                    break;
                }
            }
        }

        for (const BuildingData& bd : globalBuildingData) {
            if (!FeatureToggle::MAGIC && bd.category == "magic") continue;
            if (bd.race_restricted && bd.race_required != k.race) continue;
            int existing_count = 0;
            for (BuildingType bt : s.buildings) {
                if (bt == bd.type) existing_count++;
            }
            if (bd.max_per_settlement > 0 && existing_count >= bd.max_per_settlement) continue;
            double cost = bd.cost * k.cached_building_cost_mod;
            if (k.gold < cost) continue;

            double desire = 0.0;
            if (bd.category == "agriculture" && food_shortage) desire += 3.0;
            if (bd.category == "economy" && gold_shortage) desire += 3.0;
            if (bd.category == "military" && army_shortage) desire += 3.0;
            if (bd.category == "defense" && k.war_timer > 0) desire += 2.0;
            if (bd.category == "tech" && tech_low) desire += 2.5;
            if (bd.category == "culture" && stability_low) desire += 2.5;
            if (bd.category == "welfare" && stability_low) desire += 2.0;
            if (bd.category == "naval" && coastal) desire += 2.5;
            desire += k.industriousness * 1.0;
            if (bd.race_restricted && bd.race_required == k.race) desire += 2.0;
            for (const Effect& eff : bd.effects) {
                if (eff.stat == EffectStat::STABILITY && stability_low) desire += 0.5;
                if (eff.stat == EffectStat::FOOD && food_shortage) desire += 0.5;
                if (eff.stat == EffectStat::GOLD && gold_shortage) desire += 0.5;
                if (eff.stat == EffectStat::MILITARY_POWER_MOD && army_shortage) desire += 0.5;
            }
            std::uniform_real_distribution<double> jitter(-0.5, 0.5);
            desire += jitter(global_rng);
            if (desire > 0.0) {
                candidates.push_back({desire, static_cast<int>(si), bd.type});
            }
        }
    }

    if (candidates.empty()) return;

    std::sort(candidates.begin(), candidates.end(),
              [](const BuildCandidate& a, const BuildCandidate& b) {
                  return a.score > b.score;
              });

    // ---------- 3. 批量人口检查（新增） ----------
    // 统计每个定居点本回合计划建造的建筑数量及人口消耗
    std::unordered_map<int, int> pop_cost_per_settlement;
    // 由于我们最多建造 MAX_BUILDINGS_PER_TURN（通常为2）个，所以不会过多，但为通用性保留分组
    int built = 0;
    // 先统计前 MAX_BUILDINGS_PER_TURN 个候选（因为最终最多建这么多）
    int max_to_consider = std::min(MAX_BUILDINGS_PER_TURN, static_cast<int>(candidates.size()));
    for (int i = 0; i < max_to_consider; ++i) {
        int idx = candidates[i].settlement_index;
        pop_cost_per_settlement[idx] += 1;  // 每建筑消耗2人口
    }

    // ---------- 4. 执行建造（带人口检查） ----------
    int buildings_built = 0;
    for (const BuildCandidate& cand : candidates) {
        if (buildings_built >= MAX_BUILDINGS_PER_TURN) break;

        int si = cand.settlement_index;
        Settlement& s = k.settlements[si];

        // 检查该定居点本回合所有计划建造的总人口消耗是否超出当前人口
        int total_pop_cost = pop_cost_per_settlement[si];
        if (s.population - total_pop_cost < Balance::MIN_SETTLEMENT_POP) {
            // 人口不足，跳过该定居点的所有建造（本回合不再为其建造）
            // 但为了简化，我们直接跳过当前这个，并且将该定居点的剩余计划建造计数置零，避免重复检查
            pop_cost_per_settlement[si] = 0;
            continue;
        }

        const BuildingData* bd_ptr = nullptr;
        for (const auto& bd : globalBuildingData) {
            if (bd.type == cand.type) {
                bd_ptr = &bd;
                break;
            }
        }
        if (!bd_ptr) continue;

        double gold_cost = bd_ptr->cost * k.cached_building_cost_mod;
        double food_cost = gold_cost * 0.8;          // 食物消耗为金币成本的80%
        if (k.gold < gold_cost || k.food < food_cost) continue;

        // 扣除资源
        k.gold -= gold_cost;
        k.food -= food_cost;
        s.population -= 1;      // 扣除人口（已通过批量检查确保安全）
        s.buildings.push_back(cand.type);
        buildings_built++;
        // 减少该定居点剩余计划建造计数
        pop_cost_per_settlement[si] -= 1;
    }

    // 最终更新缓存
    k.update_cached_effects();
}
// ============================================================
// 第 5 部分结束
// ============================================================

// ============================================================
// 文明模拟器 - 第 6 部分：领土扩张与新建居住区
// ============================================================

void expand_territory(Kingdom& k, const WorldData& world, int turn) {
    // ---------- 1. 计算基础扩张能力（基于军队） ----------
    double base_expand = k.army * 0.05;
    int base_max = static_cast<int>(std::floor(base_expand));
    if (base_max <= 0) return;

    // ---------- 2. 计算效率递减因子（方案A） ----------
    double territory_size = static_cast<double>(k.territory.size());
    double efficiency = 1.0 / (1.0 + territory_size / Balance::EXPANSION_EFFICIENCY_DECAY_RATE);
    efficiency = std::max(0.05, efficiency);

    // ---------- 3. 计算最终可扩张格数 ----------
    int max_expand = static_cast<int>(std::floor(base_max * efficiency));
    max_expand = std::max(1, max_expand);
    max_expand = std::min(max_expand, MAX_EXPANSION_PER_TURN);

    // ---------- 4. 构建候选格子列表 ----------
    std::vector<Position> candidates;
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    std::unordered_set<int> candidate_set;

    for (int idx : k.territory) {
        int x = idx % WORLD_WIDTH;
        int y = idx / WORLD_WIDTH;
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx < 0 || nx >= WORLD_WIDTH || ny < 0 || ny >= WORLD_HEIGHT) continue;
            if (!is_land(world, nx, ny)) continue;
            if (ownerMap[ny][nx] != -1) continue;
            int nidx = linear_index(nx, ny, WORLD_WIDTH);
            if (candidate_set.find(nidx) != candidate_set.end()) continue;
            candidate_set.insert(nidx);
            candidates.push_back({nx, ny});
        }
    }

    if (candidates.empty()) return;

    // ---------- 5. 随机打乱候选顺序 ----------
    std::shuffle(candidates.begin(), candidates.end(), global_rng);

    // ---------- 6. 执行扩张（使用指数成本） ----------
    int expanded = 0;
    for (const Position& pos : candidates) {
        if (expanded >= max_expand) break;

        // ===== 方案B：指数级成本计算 =====
        // 计算领土规模倍率： (territory_size / BASE) ^ EXPONENT
        double size_ratio = territory_size / Balance::EXPANSION_COST_EXPONENT_BASE;
        double size_multiplier = std::pow(size_ratio, Balance::EXPANSION_COST_EXPONENT);
        // 确保至少为1
        size_multiplier = std::max(Balance::EXPANSION_COST_MIN_MULTIPLIER, size_multiplier);

        // 如果当前格是沿海或特殊地形，可适当调整（可选，但暂不实现）
        // 基础成本 * 指数倍率
        double gold_cost = Balance::EXPAND_COST_GOLD * size_multiplier;
        double food_cost = Balance::EXPAND_COST_FOOD * size_multiplier;

        // 检查资源是否足够
        if (k.gold < gold_cost || k.food < food_cost) break;  // 资源不足停止扩张

        // 扣除资源
        k.gold -= gold_cost;
        k.food -= food_cost;

        // 占领格子
        int idx = linear_index(pos.x, pos.y, WORLD_WIDTH);
        ownerMap[pos.y][pos.x] = k.id;
        controlMap[pos.y][pos.x] = Balance::EXPAND_CONTROL_INIT;
        cultureMap[pos.y][pos.x] = k.culture;
        k.territory.insert(idx);

        expanded++;
        // 注意：每次扩张后领土大小变化，但为了性能，我们不在循环内更新 territory_size，因为成本基于扩张前的领土大小计算，这更简单且对玩家有利（不会因扩张而立刻涨价）。但更真实的应该是动态更新，我们可以考虑在每次扩张后更新 territory_size，但那样会显著增加计算复杂度。为了保持简洁，我们使用扩张前的领土大小作为基准。
        // 不过，如果希望更动态，可以在循环内每次更新 territory_size = k.territory.size();
        // 这里我们选择不更新，以保持稳定。
    }
}
void build_new_settlement(Kingdom& k, const WorldData& world, int turn) {
    // ---------- 基础条件检查 ----------
    if (k.settlements.size() >= MAX_SETTLEMENTS_PER_KINGDOM) return;
    if (k.gold < Balance::NEW_SETTLEMENT_COST) return;
    if (k.greed <= 0.05) return;

    // 找到首都位置
    Position capital_pos(-1, -1);
    for (const auto& s : k.settlements) {
        if (s.is_capital) {
            capital_pos = s.pos;
            break;
        }
    }
    if (capital_pos.x == -1) return;

    // ---------- 计算定居点相关指标 ----------
    double territory_per_settlement = k.territory.size() / std::max(1.0, static_cast<double>(k.settlements.size()));
    double pop_per_settlement = k.total_population() / std::max(1.0, static_cast<double>(k.settlements.size()));

    // ---------- 检查自动建城条件（已调整阈值） ----------
    bool auto_settle = (territory_per_settlement > Balance::AUTO_SETTLE_TERRITORY_PER_SETTLEMENT_THRESHOLD) ||
                       (pop_per_settlement > Balance::AUTO_SETTLE_POP_PER_SETTLEMENT_THRESHOLD);

    // ---------- 处理自动建城 ----------
    if (auto_settle) {
        // 1. 冷却检查
        if (turn - k.last_auto_settlement_turn < Balance::AUTO_SETTLE_COOLDOWN_TURNS) {
            return;  // 冷却中，禁止自动建城
        }

        // 2. 查找最佳候选位置（即使评分低也要建）
        Position best_pos(-1, -1);
        double best_score = -1000.0;
        std::unordered_set<int> settlement_positions;
        for (const auto& s : k.settlements) {
            settlement_positions.insert(linear_index(s.pos.x, s.pos.y, WORLD_WIDTH));
        }

        for (int idx : k.territory) {
            int x = idx % WORLD_WIDTH;
            int y = idx / WORLD_WIDTH;
            if (settlement_positions.find(idx) != settlement_positions.end()) continue;
            if (!is_habitable(world, x, y)) continue;

            // 评分：距离首都越近越好，土壤越肥沃越好，并考虑已有定居点密度
            double dist_to_cap = std::sqrt((x - capital_pos.x) * (x - capital_pos.x) +
                                           (y - capital_pos.y) * (y - capital_pos.y));
            double score = -dist_to_cap * 0.1 + world.get(LAYER_SOIL_FERT, x, y) * 2.0;
            // 额外惩罚：周围已有定居点过多则降低评分（避免挤在一起）
            int nearby_settlements = 0;
            for (const auto& s : k.settlements) {
                double d = std::sqrt((x - s.pos.x)*(x - s.pos.x) + (y - s.pos.y)*(y - s.pos.y));
                if (d < 10.0) nearby_settlements++;
            }
            score -= nearby_settlements * 1.5;
            if (score > best_score) {
                best_score = score;
                best_pos = {x, y};
            }
        }

        if (best_pos.x == -1) return;

        // 3. 计算建城成本（基础费用 + 额外消耗）
        double total_gold_cost = Balance::NEW_SETTLEMENT_COST + Balance::AUTO_SETTLE_EXTRA_COST_GOLD;
        double total_food_cost = Balance::NEW_SETTLEMENT_COST * 0.8 + Balance::AUTO_SETTLE_EXTRA_COST_FOOD;
        if (k.gold < total_gold_cost || k.food < total_food_cost) return;

        // 4. 扣除资源
        k.gold -= total_gold_cost;
        k.food -= total_food_cost;

        // 5. 计算新定居点的人口（动态：基础50 + 随机偏移0~50，且与领土密度正相关，但不超过100）
        int base_pop = Balance::NEW_SETTLEMENT_POP;
        int random_offset = std::uniform_int_distribution<int>(0, 50)(global_rng);
        double density_factor = std::min(1.0, territory_per_settlement / 100.0);  // 领土密度越高，人口越多
        int extra_pop = static_cast<int>(base_pop * 0.3 * density_factor);
        int final_pop = std::max(30, std::min(100, base_pop + random_offset + extra_pop));

        // 6. 创建定居点
        Settlement new_sett(best_pos.x, best_pos.y, final_pop,
                            false, generate_settlement_name(k.race, false), 0.3, 0.4);
        k.settlements.push_back(new_sett);
        k.update_cached_effects();

        // 7. 记录冷却回合
        k.last_auto_settlement_turn = turn;

        // 8. 记录事件
        std::string reason;
        if (territory_per_settlement > Balance::AUTO_SETTLE_TERRITORY_PER_SETTLEMENT_THRESHOLD &&
            pop_per_settlement > Balance::AUTO_SETTLE_POP_PER_SETTLEMENT_THRESHOLD) {
            reason = "领土过大且人口过多";
        } else if (territory_per_settlement > Balance::AUTO_SETTLE_TERRITORY_PER_SETTLEMENT_THRESHOLD) {
            reason = "领土过大（" + std::to_string((int)territory_per_settlement) + "格/定居点）";
        } else {
            reason = "人口过多（" + std::to_string((int)pop_per_settlement) + "人/定居点）";
        }

        std::ostringstream oss;
        oss << k.name << " 因" << reason << "在 (" << best_pos.x << ", " << best_pos.y 
            << ") 自动建立了新居住区 " << new_sett.name << "，人口 " << final_pop;
        EventRecord event(turn, format_time(turn),
                          EventType::SOCIAL, k.id, oss.str(), false, best_pos.x, best_pos.y);
        globalEventLog.push_back(event);
        std::cout << "[" << turn << "] " << oss.str() << std::endl;
        return;
    }

    // ---------- 原逻辑：正常评分建城（评分阈值仍为1.5，但增加人口动态） ----------
    struct CandidateSettlement {
        Position pos;
        double score;
    };
    std::vector<CandidateSettlement> candidates;
    std::unordered_set<int> settlement_positions;
    for (const auto& s : k.settlements) {
        settlement_positions.insert(linear_index(s.pos.x, s.pos.y, WORLD_WIDTH));
    }

    for (int idx : k.territory) {
        int x = idx % WORLD_WIDTH;
        int y = idx / WORLD_WIDTH;
        if (settlement_positions.find(idx) != settlement_positions.end()) continue;
        if (!is_habitable(world, x, y)) continue;

        double score = 0.0;
        double dist_to_cap = std::sqrt((x - capital_pos.x) * (x - capital_pos.x) +
                                       (y - capital_pos.y) * (y - capital_pos.y));
        score -= dist_to_cap * 0.3;

        double soil = world.get(LAYER_SOIL_FERT, x, y);
        score += soil * 2.0;

        int mineral = static_cast<int>(world.get(LAYER_MINERAL, x, y));
        if (mineral > 0) score += 3.0;

        bool coastal = false;
        const int dx[8] = {1,1,0,-1,-1,-1,0,1};
        const int dy[8] = {0,1,1,1,0,-1,-1,-1};
        for (int d = 0; d < 8; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx >= 0 && nx < WORLD_WIDTH && ny >= 0 && ny < WORLD_HEIGHT) {
                if (world.get(LAYER_ELEVATION, nx, ny) <= 0.0) {
                    coastal = true;
                    break;
                }
            }
        }
        if (coastal) score += 2.0;

        double groundwater = world.get(LAYER_GROUNDWATER, x, y);
        score += groundwater * 1.5;

        int wildlife = static_cast<int>(world.get(LAYER_WILDLIFE, x, y));
        score += wildlife * 0.5;

        bool border = false;
        const int dx4[4] = {1,-1,0,0};
        const int dy4[4] = {0,0,1,-1};
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx4[d];
            int ny = y + dy4[d];
            if (nx >= 0 && nx < WORLD_WIDTH && ny >= 0 && ny < WORLD_HEIGHT) {
                if (ownerMap[ny][nx] == -1) {
                    border = true;
                    break;
                }
            }
        }
        if (border) score += k.aggression * 1.5;

        double slope = world.get(LAYER_SLOPE, x, y);
        score -= slope * 0.5;

        candidates.push_back({ {x, y}, score });
    }

    if (candidates.empty()) return;

    std::sort(candidates.begin(), candidates.end(),
              [](const CandidateSettlement& a, const CandidateSettlement& b) {
                  return a.score > b.score;
              });

    CandidateSettlement best = candidates.front();

    // 强制建城条件（保留但放宽）
    bool force_settle = false;
    if (k.territory.size() > 50 && k.settlements.size() < 3) force_settle = true;
    if (k.territory.size() > 80 && k.settlements.size() < 5) force_settle = true;
    if (k.total_population() > 300 && k.settlements.size() < 3) force_settle = true;

    if (!force_settle && best.score <= 1.5) {
        return;
    }
    if (force_settle && best.score < -20.0) {
        return;
    }

    // 扣除基础费用
    k.gold -= Balance::NEW_SETTLEMENT_COST;

    // 计算评分建城的人口（同样动态，但比自动建城略低）
    int base_pop = Balance::NEW_SETTLEMENT_POP;
    int random_offset = std::uniform_int_distribution<int>(-10, 30)(global_rng);
    int final_pop = std::max(30, std::min(80, base_pop + random_offset));

    Settlement new_sett(best.pos.x, best.pos.y, final_pop,
                        false, generate_settlement_name(k.race, false), 0.3, 0.4);
    k.settlements.push_back(new_sett);
    k.update_cached_effects();

    std::ostringstream oss;
    oss << k.name << " 在 (" << best.pos.x << ", " << best.pos.y << ") 建立了新居住区 "
        << new_sett.name << "，人口 " << final_pop;
    EventRecord event(turn, format_time(turn),
                      EventType::SOCIAL, k.id, oss.str(), false, best.pos.x, best.pos.y);
    globalEventLog.push_back(event);
    std::cout << "[" << turn << "] " << oss.str() << std::endl;
}
// ============================================================
// 第 6 部分结束
// ============================================================

// ============================================================
// 第 7 部分：科技系统
// ============================================================

// ---------- 科技池初始化 ----------
void init_tech_pool() {
    globalTechPool.clear();

    // 农业
    globalTechPool.emplace_back(TechID::IRRIGATION, "灌溉", std::string("agriculture"),
                                std::vector<TechID> {}, 1.0,
                                std::vector<Effect> { Effect(EffectStat::FOOD_MOD, 0.10) });
    globalTechPool.emplace_back(TechID::CROP_ROTATION, "轮作", std::string("agriculture"),
                                std::vector<TechID> {TechID::IRRIGATION}, 0.9,
                                std::vector<Effect> { Effect(EffectStat::FOOD_MOD, 0.05), Effect(EffectStat::GROWTH_MOD, 0.02) });
    globalTechPool.emplace_back(TechID::ANIMAL_HUSBANDRY, "畜牧", std::string("agriculture"),
                                std::vector<TechID> {}, 1.1,
                                std::vector<Effect> { Effect(EffectStat::FOOD_MOD, 0.08), Effect(EffectStat::GOLD_MOD, 0.03) });
    globalTechPool.emplace_back(TechID::FERTILIZER, "肥料", std::string("agriculture"),
                                std::vector<TechID> {TechID::CROP_ROTATION, TechID::ANIMAL_HUSBANDRY}, 0.8,
                                std::vector<Effect> { Effect(EffectStat::FOOD_MOD, 0.10) });
    globalTechPool.emplace_back(TechID::TERRACE_FARMING, "梯田", std::string("agriculture"),
                                std::vector<TechID> {TechID::IRRIGATION}, 0.7,
                                std::vector<Effect> { Effect(EffectStat::FOOD_MOD, 0.06), Effect(EffectStat::DEFENSE_BONUS, 0.1) });

    // 军事
    globalTechPool.emplace_back(TechID::BRONZE_WORKING, "青铜器", std::string("military"),
                                std::vector<TechID> {}, 1.0,
                                std::vector<Effect> { Effect(EffectStat::MILITARY_POWER_MOD, 0.05) });
    globalTechPool.emplace_back(TechID::IRON_WORKING, "铁器", std::string("military"),
                                std::vector<TechID> {TechID::BRONZE_WORKING}, 0.9,
                                std::vector<Effect> { Effect(EffectStat::MILITARY_POWER_MOD, 0.10) });
    globalTechPool.emplace_back(TechID::STEEL_WEAPONS, "钢制武器", std::string("military"),
                                std::vector<TechID> {TechID::IRON_WORKING}, 0.8,
                                std::vector<Effect> { Effect(EffectStat::MILITARY_POWER_MOD, 0.12) });
    globalTechPool.emplace_back(TechID::SIEGE_ENGINES, "攻城器械", std::string("military"),
                                std::vector<TechID> {TechID::IRON_WORKING}, 0.7,
                                std::vector<Effect> { Effect(EffectStat::ATTACK_BONUS, 0.15) });
    globalTechPool.emplace_back(TechID::MILITARY_TACTICS, "军事战术", std::string("military"),
                                std::vector<TechID> {TechID::BRONZE_WORKING}, 1.0,
                                std::vector<Effect> { Effect(EffectStat::MILITARY_POWER_MOD, 0.05), Effect(EffectStat::ATTACK_BONUS, 0.05) });
    globalTechPool.emplace_back(TechID::FORTIFICATION, "防御工事", std::string("military"),
                                std::vector<TechID> {}, 0.9,
                                std::vector<Effect> { Effect(EffectStat::DEFENSE_BONUS, 0.1) });

    // 工业
    globalTechPool.emplace_back(TechID::MASONRY, "石工", std::string("industry"),
                                std::vector<TechID> {}, 1.0,
                                std::vector<Effect> { Effect(EffectStat::BUILDING_COST_MOD, -0.05) });
    globalTechPool.emplace_back(TechID::ARCHITECTURE, "建筑学", std::string("industry"),
                                std::vector<TechID> {TechID::MASONRY}, 0.8,
                                std::vector<Effect> { Effect(EffectStat::BUILDING_COST_MOD, -0.10), Effect(EffectStat::STABILITY, 1.0) });
    globalTechPool.emplace_back(TechID::MACHINERY, "机械", std::string("industry"),
                                std::vector<TechID> {TechID::MASONRY, TechID::BRONZE_WORKING}, 0.6,
                                std::vector<Effect> { Effect(EffectStat::FOOD_MOD, 0.03), Effect(EffectStat::GOLD_MOD, 0.03) });
    globalTechPool.emplace_back(TechID::ADVANCED_TOOLS, "高级工具", std::string("industry"),
                                std::vector<TechID> {TechID::IRON_WORKING}, 0.7,
                                std::vector<Effect> { Effect(EffectStat::BUILDING_COST_MOD, -0.05), Effect(EffectStat::GOLD_MOD, 0.05) });

    // 航海
    globalTechPool.emplace_back(TechID::SAILING, "帆船", std::string("naval"),
                                std::vector<TechID> {}, 1.0,
                                std::vector<Effect> { Effect(EffectStat::TRADE_MOD, 0.05), Effect(EffectStat::GOLD_MOD, 0.02) });
    globalTechPool.emplace_back(TechID::NAVIGATION, "导航", std::string("naval"),
                                std::vector<TechID> {TechID::SAILING}, 0.8,
                                std::vector<Effect> { Effect(EffectStat::TRADE_MOD, 0.10), Effect(EffectStat::GOLD_MOD, 0.03) });
    globalTechPool.emplace_back(TechID::SHIPBUILDING, "造船", std::string("naval"),
                                std::vector<TechID> {TechID::SAILING}, 0.7,
                                std::vector<Effect> { Effect(EffectStat::ROAD_SPEED_MOD, 0.05), Effect(EffectStat::TRADE_MOD, 0.05) });
    globalTechPool.emplace_back(TechID::DEEP_SEA_FISHING, "远洋渔业", std::string("naval"),
                                std::vector<TechID> {TechID::SAILING}, 0.9,
                                std::vector<Effect> { Effect(EffectStat::FOOD_MOD, 0.05) });

    // 魔法
    globalTechPool.emplace_back(TechID::MAGIC_BASICS, "魔法基础", std::string("magic"),
                                std::vector<TechID> {}, 1.0,
                                std::vector<Effect> { Effect(EffectStat::MAGIC_AFFINITY, 0.10), Effect(EffectStat::RESEARCH_RATE_MOD, 0.02) },
                                true);
    globalTechPool.emplace_back(TechID::ARCANE_RESEARCH, "奥术研究", std::string("magic"),
                                std::vector<TechID> {TechID::MAGIC_BASICS}, 0.8,
                                std::vector<Effect> { Effect(EffectStat::MAGIC_AFFINITY, 0.15), Effect(EffectStat::RESEARCH_RATE_MOD, 0.03) },
                                true);
    globalTechPool.emplace_back(TechID::ELEMENTAL_MAGIC, "元素魔法", std::string("magic"),
                                std::vector<TechID> {TechID::MAGIC_BASICS}, 0.7,
                                std::vector<Effect> { Effect(EffectStat::ATTACK_BONUS, 0.1), Effect(EffectStat::MAGIC_AFFINITY, 0.10) },
                                true);
    globalTechPool.emplace_back(TechID::ENCHANTMENT, "附魔", std::string("magic"),
                                std::vector<TechID> {TechID::ARCANE_RESEARCH}, 0.6,
                                std::vector<Effect> { Effect(EffectStat::MILITARY_POWER_MOD, 0.05), Effect(EffectStat::MAGIC_AFFINITY, 0.10) },
                                true);
    globalTechPool.emplace_back(TechID::RUNECRAFT, "符文工艺", std::string("magic"),
                                std::vector<TechID> {TechID::ARCANE_RESEARCH, TechID::IRON_WORKING}, 0.5,
                                std::vector<Effect> { Effect(EffectStat::DEFENSE_BONUS, 0.2), Effect(EffectStat::MAGIC_AFFINITY, 0.10) },
                                true);

    // 文化
    globalTechPool.emplace_back(TechID::LAW_CODE, "法典", std::string("culture"),
                                std::vector<TechID> {}, 1.0,
                                std::vector<Effect> { Effect(EffectStat::STABILITY, 2.0), Effect(EffectStat::REBELLION_RISK_MOD, -0.05) });
    globalTechPool.emplace_back(TechID::PHILOSOPHY, "哲学", std::string("culture"),
                                std::vector<TechID> {TechID::LAW_CODE}, 0.8,
                                std::vector<Effect> { Effect(EffectStat::RESEARCH_RATE_MOD, 0.03), Effect(EffectStat::STABILITY, 1.0) });
    globalTechPool.emplace_back(TechID::LITERATURE, "文学", std::string("culture"),
                                std::vector<TechID> {}, 0.9,
                                std::vector<Effect> { Effect(EffectStat::CULTURE_CHANGE_MOD, 0.05), Effect(EffectStat::STABILITY, 1.0) });
    globalTechPool.emplace_back(TechID::MUSIC, "音乐", std::string("culture"),
                                std::vector<TechID> {}, 0.9,
                                std::vector<Effect> { Effect(EffectStat::CULTURE_CHANGE_MOD, 0.03), Effect(EffectStat::STABILITY, 2.0) });
    globalTechPool.emplace_back(TechID::RELIGION_INSTITUTIONS, "宗教制度", std::string("culture"),
                                std::vector<TechID> {TechID::LAW_CODE}, 0.7,
                                std::vector<Effect> { Effect(EffectStat::STABILITY, 3.0), Effect(EffectStat::REBELLION_RISK_MOD, -0.03) });

    // 经济
    globalTechPool.emplace_back(TechID::CURRENCY, "货币", std::string("economy"),
                                std::vector<TechID> {}, 1.0,
                                std::vector<Effect> { Effect(EffectStat::TAX_MOD, 0.05), Effect(EffectStat::GOLD_MOD, 0.03) });
    globalTechPool.emplace_back(TechID::BANKING, "银行业", std::string("economy"),
                                std::vector<TechID> {TechID::CURRENCY}, 0.7,
                                std::vector<Effect> { Effect(EffectStat::TAX_MOD, 0.10), Effect(EffectStat::GOLD_MOD, 0.05) });
    globalTechPool.emplace_back(TechID::TRADE_ROUTES, "贸易路线", std::string("economy"),
                                std::vector<TechID> {TechID::CURRENCY, TechID::SAILING}, 0.8,
                                std::vector<Effect> { Effect(EffectStat::TRADE_MOD, 0.15) });
    globalTechPool.emplace_back(TechID::GUILDS, "行会", std::string("economy"),
                                std::vector<TechID> {TechID::CURRENCY, TechID::MASONRY}, 0.9,
                                std::vector<Effect> { Effect(EffectStat::GOLD_MOD, 0.05), Effect(EffectStat::STABILITY, 1.0) });
}
// ---------- 判断两个王国是否接壤 ----------
bool kingdoms_share_border(const Kingdom& a, const Kingdom& b) {
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    for (int idx_a : a.territory) {
        int x = idx_a % WORLD_WIDTH;
        int y = idx_a / WORLD_WIDTH;
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx < 0 || nx >= WORLD_WIDTH || ny < 0 || ny >= WORLD_HEIGHT) continue;
            if (ownerMap[ny][nx] == b.id) return true;
        }
    }
    return false;
}

// ---------- 科技研究 ----------
void update_technology(Kingdom& k, int turn) {
    // --- 基础研究点 ---
    double research_points = Balance::RESEARCH_BASE_POINTS * k.industriousness * k.tech_modifier();
    research_points *= k.cached_research_mod;
    if (k.war_timer > 0) research_points *= 0.5;

    // --- 新增：科技瓶颈递减 ---
    if (k.tech_level > 5.0) {
        double penalty = 1.0 - (k.tech_level - 5.0) * 0.15;
        penalty = std::max(0.2, penalty);
        research_points *= penalty;
    }

    // --- 累积科技点 ---
    k.tech_level += research_points;
    k.tech_level = std::max(0.0, std::min(MAX_TECH_LEVEL, k.tech_level));

    // --- 科技发现（不变） ---
    int attempts = MAX_RESEARCH_TRIGGER_PER_TURN;
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    for (const TechNode& tech : globalTechPool) {
        if (attempts <= 0) break;
        if (tech.magic_required && !FeatureToggle::MAGIC) continue;
        bool owned = false;
        for (TechID owned_id : k.owned_techs) {
            if (owned_id == tech.id) { owned = true; break; }
        }
        if (owned) continue;
        bool prerequisites_met = true;
        for (TechID pre : tech.prerequisites) {
            bool has_pre = false;
            for (TechID owned_id : k.owned_techs) {
                if (owned_id == pre) { has_pre = true; break; }
            }
            if (!has_pre) { prerequisites_met = false; break; }
        }
        if (!prerequisites_met) continue;

        double discovery_prob = tech.weight * Balance::TECH_DISCOVERY_BASE * k.industriousness;
        if (tech.globally_discovered) discovery_prob *= 0.5;
        if (prob(global_rng) < discovery_prob) {
            k.owned_techs.push_back(tech.id);
            for (TechNode& t : globalTechPool) {
                if (t.id == tech.id) {
                    if (!t.globally_discovered) {
                        t.globally_discovered = true;
                        t.first_discovered_kingdom = k.id;
                    }
                    break;
                }
            }
            attempts--;
            std::ostringstream oss;
            oss << k.name << " 发现了新科技：「" << tech.name << "」";
            EventRecord event(turn, format_time(turn),
                              EventType::TECHNOLOGICAL, k.id, oss.str(), false);
            globalEventLog.push_back(event);
            std::cout << "[" << turn << "] " << oss.str() << std::endl;
        }
    }
    k.update_cached_effects();
}
// ---------- 科技传播 ----------
void transfer_tech(Kingdom& from, Kingdom& to, int turn) {
    // ---------- 1. 前置条件检查 ----------
    if (warMatrix[from.id][to.id] != 0 || warMatrix[to.id][from.id] != 0) return;
    if (!kingdoms_share_border(from, to)) return;

    // ---------- 2. 找出可传播的科技（从方拥有、到方未拥有） ----------
    std::vector<TechID> transferable;
    for (TechID tech_id : from.owned_techs) {
        bool to_has = false;
        for (TechID to_id : to.owned_techs) {
            if (to_id == tech_id) {
                to_has = true;
                break;
            }
        }
        if (!to_has) {
            // 检查是否为魔法科技且未启用
            for (const TechNode& tech : globalTechPool) {
                if (tech.id == tech_id) {
                    if (tech.magic_required && !FeatureToggle::MAGIC) continue;
                    transferable.push_back(tech_id);
                    break;
                }
            }
        }
    }
    if (transferable.empty()) return;

    // ---------- 3. 决定传播数量（1~3个） ----------
    int max_num = std::min(3, static_cast<int>(transferable.size()));
    int num = std::uniform_int_distribution<int>(1, max_num)(global_rng);

    // 随机打乱传播列表
    std::shuffle(transferable.begin(), transferable.end(), global_rng);

    // ---------- 4. 计算传播基础概率 ----------
    double base_prob = Balance::TECH_SPREAD_PROB * (0.5 + from.diplomacy * 0.5) *
                       (1.0 + from.civilization_level * 0.1);
    // 限制概率上限，避免过度传播
    base_prob = std::min(0.5, base_prob);

    std::uniform_real_distribution<double> prob(0.0, 1.0);
    int transferred = 0;

    // ---------- 5. 依次尝试传播每个科技 ----------
    for (int i = 0; i < num && i < static_cast<int>(transferable.size()); ++i) {
        TechID selected = transferable[i];

        // 对每个科技独立判定传播成功与否（概率相同）
        if (prob(global_rng) < base_prob) {
            to.owned_techs.push_back(selected);
            // 更新全球科技状态（如果尚未全球发现）
            for (TechNode& t : globalTechPool) {
                if (t.id == selected && !t.globally_discovered) {
                    t.globally_discovered = true;
                    t.first_discovered_kingdom = from.id;
                    break;
                }
            }
            transferred++;
        }
    }

    // ---------- 6. 若有科技被传播，更新目标王国缓存 ----------
    if (transferred > 0) {
        to.update_cached_effects();

        // 记录事件（时间格式将在第7步统一替换）
        std::ostringstream oss;
        oss << from.name << " 的科技传播至 " << to.name << "，共 " << transferred << " 项。";
        EventRecord event(turn, format_time(turn),
                          EventType::TECHNOLOGICAL, from.id, oss.str(), true);
        event.involved_kingdom_ids = {from.id, to.id};
        globalEventLog.push_back(event);
    }
}
// ============================================================
// 第 7 部分结束
// ============================================================

// ============================================================
// 文明模拟器 - 第 8 部分：稳定度与内部政治
// ============================================================

// ---------- 稳定度更新（增量式） ----------
void update_stability(Kingdom& k, const WorldData& world, int turn) {
    double adjustment = 0.0;

    // 1. 食物/人口比
    int total_pop = k.total_population();
    if (total_pop > 0) {
        double food_per_pop = k.food / total_pop;
        if (food_per_pop > Balance::STABILITY_FOOD_GOOD) {
            adjustment += 10.0;
        } else if (food_per_pop < Balance::STABILITY_FOOD_BAD) {
            adjustment -= 20.0;
        } else {
            adjustment += (food_per_pop - Balance::STABILITY_FOOD_BAD) /
                          (Balance::STABILITY_FOOD_GOOD - Balance::STABILITY_FOOD_BAD) * 30.0 - 20.0;
        }
    }

    // 2. 战争数量惩罚（原）
    int active_wars = 0;
    for (size_t i = 0; i < warMatrix.size(); ++i) {
        if (i == static_cast<size_t>(k.id)) continue;
        if (warMatrix[k.id][i] != 0 || warMatrix[i][k.id] != 0) {
            active_wars++;
        }
    }
    adjustment -= active_wars * Balance::STABILITY_WAR_PENALTY;

    // ===== 新增：战争持续时间疲劳 =====
    if (k.war_timer > 0) {
        double war_exhaustion = std::min(20.0, k.war_timer * 0.5);
        adjustment -= war_exhaustion;
    }

    // 3. 税率惩罚
    if (k.tax_rate > Balance::STABILITY_TAX_THRESHOLD) {
        double excess = k.tax_rate - Balance::STABILITY_TAX_THRESHOLD;
        adjustment -= excess * Balance::STABILITY_TAX_PENALTY;
    }

    // 4. 人口规模压力
    if (total_pop > 500) {
        adjustment -= 5.0;
    }

    // 5. 文明等级加成
    adjustment += k.civilization_level * 2.0;

    // 6. 道路连通加成（若启用道路）
    if (FeatureToggle::ROADS) {
        int connected = 0;
        for (const auto& s : k.settlements) {
            if (s.is_capital) continue;
            if (is_settlement_connected_to_capital(k, s)) {
                connected++;
            }
        }
        adjustment += std::min(5.0, static_cast<double>(connected));
    }

    // 7. 建筑和科技直接稳定性效果
    for (const auto& s : k.settlements) {
        for (BuildingType bt : s.buildings) {
            for (const auto& bd : globalBuildingData) {
                if (bd.type == bt) {
                    for (const Effect& eff : bd.effects) {
                        if (eff.stat == EffectStat::STABILITY) {
                            adjustment += eff.value;
                        }
                    }
                    break;
                }
            }
        }
    }
    for (TechID tech_id : k.owned_techs) {
        for (const auto& tech : globalTechPool) {
            if (tech.id == tech_id) {
                for (const Effect& eff : tech.effects) {
                    if (eff.stat == EffectStat::STABILITY) {
                        adjustment += eff.value;
                    }
                }
                break;
            }
        }
    }

    // 8. 叛乱风险修正
    adjustment += (1.0 - k.cached_rebellion_risk_mod) * 10.0;

    // 应用调整量
    k.stability = std::max(MIN_STABILITY, std::min(MAX_STABILITY, k.stability + adjustment));
    
    /*// ===== 新增：稳定度向50靠拢，避免长期满值 =====
    const double TARGET_STABILITY = 50.0;
    const double REGEN_RATE = 0.005;
    k.stability += (TARGET_STABILITY - k.stability) * REGEN_RATE;
    k.stability = std::max(MIN_STABILITY, std::min(MAX_STABILITY, k.stability));*/
}
// ---------- 叛乱 ----------
void handle_rebellion(Kingdom& k, int turn) {
    if (k.stability >= Balance::REBELLION_STABILITY_THRESHOLD) return;

    std::uniform_real_distribution<double> prob(0.0, 1.0);
    if (prob(global_rng) >= Balance::REBELLION_PROB) return;

    // 收集非首都居住区
    std::vector<int> non_capital_indices;
    for (size_t i = 0; i < k.settlements.size(); ++i) {
        if (!k.settlements[i].is_capital) {
            non_capital_indices.push_back(static_cast<int>(i));
        }
    }
    if (non_capital_indices.empty()) return;

    std::uniform_int_distribution<size_t> dist(0, non_capital_indices.size() - 1);
    int idx = non_capital_indices[dist(global_rng)];
    Settlement& rebel_sett = k.settlements[idx];

    // 释放居住区中心格
    int center_idx = linear_index(rebel_sett.pos.x, rebel_sett.pos.y, WORLD_WIDTH);
    ownerMap[rebel_sett.pos.y][rebel_sett.pos.x] = -1;
    controlMap[rebel_sett.pos.y][rebel_sett.pos.x] = 0.0;
    k.territory.erase(center_idx);

    // 移除居住区
    k.settlements.erase(k.settlements.begin() + idx);

    // 军队损失、稳定度回升
    k.army = std::max(0.0, k.army - 10.0);
    k.stability = std::min(MAX_STABILITY, k.stability + 15.0);

    // 记录事件
    std::ostringstream oss;
    oss << k.name << " 的居住区 " << rebel_sett.name << " 发生叛乱，宣布独立！";
    EventRecord event(turn, format_time(turn),
                      EventType::POLITICAL, k.id, oss.str(), false,
                      rebel_sett.pos.x, rebel_sett.pos.y);
    globalEventLog.push_back(event);
    std::cout << "[" << turn << "] " << oss.str() << std::endl;
}

// ---------- 革命 ----------
void check_revolution(Kingdom& k, int turn) {
    if (k.stability >= Balance::REVOLUTION_STABILITY_THRESHOLD) return;

    std::uniform_real_distribution<double> prob(0.0, 1.0);
    if (prob(global_rng) >= Balance::REVOLUTION_PROB) return;

    // ===== 方案一：生成全新的国名（重置） =====
    // 使用与建国时相同的命名风格：种族前缀 + "共和国"
    k.name = generate_settlement_name(k.race, true) + "共和国";

    // 旧国王退位
    if (!k.king_history.empty()) {
        k.king_history.back().reign_end_turn = turn;
    }

    // 生成新国王
    std::array<double,5> old_personality = {{
            k.aggression, k.greed, k.caution,
            k.industriousness, k.diplomacy
        }
    };
    generate_new_king(k, old_personality, false);
    KingRecord new_record(k.king_name, turn);
    k.king_history.push_back(new_record);

    // 稳定度重置、军队损失
    k.stability = 60.0;
    k.army *= 0.8;

    // 记录事件
    std::ostringstream oss;
    oss << k.name << " 爆发革命！旧王被推翻，新王 " << k.king_name << " 即位。";
    EventRecord event(turn, format_time(turn),
                      EventType::POLITICAL, k.id, oss.str(), false);
    globalEventLog.push_back(event);
    std::cout << "[" << turn << "] " << oss.str() << std::endl;
}
// ---------- 国王死亡与继承 ----------
void check_king_death(Kingdom& k, int turn) {
    // ===== [修改] 死亡概率计算：更敏感，且增加年龄上限 =====
    double death_prob = 0.0;
    
    // 基础死亡概率随年龄增长（超过 50 岁后加速）
    if (k.king_age > Balance::KING_DEATH_AGE_BASE) {
        int age_over = k.king_age - Balance::KING_DEATH_AGE_BASE;
        death_prob += age_over * Balance::KING_DEATH_AGE_PER_YEAR;
    }
    
    // 年迈加成：80 岁后额外增加死亡概率
    if (k.king_age > 80) {
        death_prob += (k.king_age - 80) * 0.01; // 每超 80 岁 +1%
    }
    
    // 战争加成：战争期间死亡概率增加
    if (k.war_timer > 0) {
        death_prob += Balance::KING_DEATH_WAR_BONUS;
    }
    
    // 稳定度影响：稳定度低于 30 时，国王死亡风险增加（被暗杀/政变）
    if (k.stability < 30.0) {
        death_prob += (30.0 - k.stability) / 100.0 * 0.1;
    }
    
    // 限制最大死亡概率为 95%（避免必死）
    death_prob = std::min(0.95, death_prob);
    
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    if (prob(global_rng) >= death_prob) return;

    int old_age = k.king_age;
    std::string old_name = k.king_name;

    // 旧王退位
    if (!k.king_history.empty()) {
        k.king_history.back().reign_end_turn = turn;
    }

    // 生成新王（年龄重置为 18-40 岁，比原来稍大，更真实）
    std::array<double,5> old_personality = {{
        k.aggression, k.greed, k.caution,
        k.industriousness, k.diplomacy
    }};
    generate_new_king(k, old_personality, false);
    
    // 新王年龄设置在 18-40 岁之间
    std::uniform_int_distribution<int> age_dist(18, 40);
    k.king_age = age_dist(global_rng);
    
    KingRecord new_record(k.king_name, turn);
    k.king_history.push_back(new_record);

    // 可能产生新指挥官（30% 概率，与原来一致）
    std::uniform_real_distribution<double> cmd_prob(0.0, 1.0);
    if (cmd_prob(global_rng) < 0.3) {
        Commander new_cmd(generate_commander_name(k.race), 0.5, 0.5);
        k.commanders.push_back(new_cmd);
    }

    // 记录事件
    std::ostringstream oss;
    oss << k.name << " 国王 " << old_name << "（" << old_age << "岁）驾崩，"
        << k.king_name << " 继位。";
    EventRecord event(turn, format_time(turn),
                      EventType::POLITICAL, k.id, oss.str(), false);
    globalEventLog.push_back(event);
    std::cout << "[" << turn << "] " << oss.str() << std::endl;
}
// ---------- 文明等级更新（综合评定 + 文化挂钩 + 可倒退） ----------
void update_civilization_level(Kingdom& k, int turn) {
    // ---------- 1. 计算文化健康度 ----------
    // 改用更稳定的指标：文化建筑数量、科技水平、稳定度、人口规模等
    int cultural_buildings = 0;
    for (const auto& s : k.settlements) {
        for (BuildingType bt : s.buildings) {
            for (const auto& bd : globalBuildingData) {
                if (bd.type == bt && bd.category == "culture") {
                    cultural_buildings++;
                    break;
                }
            }
        }
    }
    // 文化健康度 = 文化建筑数 / (定居点数+1) 的贡献 + 稳定度贡献 + 科技贡献
    double building_factor = std::min(1.0, cultural_buildings / (k.settlements.size() * 2.0 + 1.0));
    double stability_factor = k.stability / 100.0;  // 0~1
    double tech_factor = k.tech_level / MAX_TECH_LEVEL; // 0~1
    // 文化健康度在 0.2 ~ 1.8 之间
    double target_culture_health = 0.2 + 0.6 * building_factor + 0.4 * stability_factor + 0.3 * tech_factor;
    target_culture_health = std::max(0.2, std::min(1.8, target_culture_health));
    
    // 平滑变化（但仍然会变化，不会恒为0.5）
    k.culture_health += (target_culture_health - k.culture_health) * 0.05;
    k.culture_health = std::max(0.1, std::min(2.0, k.culture_health));

    // ---------- 2. 计算文明等级的基础分数 ----------
    double base_score = 0.0;
    base_score += k.culture_health * 2.0;          // 文化贡献
    base_score += k.tech_level * 0.3;              // 科技贡献
    base_score += (k.stability / 100.0) * 1.5;      // 稳定度贡献
    int pop = k.total_population();
    base_score += std::min(1.0, pop / 200.0) * 0.8; // 人口贡献
    base_score += std::min(1.0, k.territory.size() / 100.0) * 0.3; // 领土贡献

    // ---------- 3. 计算升级/降级阈值 ----------
    double upgrade_threshold = Balance::CIV_LEVEL_BASE_UPGRADE_COST + 
                                k.civilization_level * Balance::CIV_LEVEL_PER_LEVEL_INCREMENT;
    double downgrade_threshold = upgrade_threshold; // 降级阈值与升级阈值相同，但比较方式不同

    // ---------- 4. 处理升级 ----------
    if (k.civilization_level < MAX_CIVILIZATION_LEVEL) {
        // 只有稳固期结束且超出阈值足够多才升级
        if (k.downgrade_stabilize_timer == 0 && base_score > upgrade_threshold + Balance::CIV_LEVEL_UPGRADE_THRESHOLD_OFFSET) {
            k.civilization_level++;
            k.civ_decline_timer = 0;
            k.upgrade_stabilize_timer = Balance::CIV_LEVEL_STABILIZE_TURNS; // 进入升级稳固期
            k.downgrade_stabilize_timer = 0; // 清除降级稳固期
            std::ostringstream oss;
            oss << k.name << " 的文明进入新时代！文明等级提升至 " << k.civilization_level 
                << "（文化健康度：" << std::fixed << std::setprecision(2) << k.culture_health << "）";
            EventRecord event(turn, format_time(turn),
                              EventType::CULTURAL, k.id, oss.str(), false);
            globalEventLog.push_back(event);
            std::cout << "[" << turn << "] " << oss.str() << std::endl;
        }
    }

    // ---------- 5. 处理降级 ----------
    if (k.civilization_level > 0) {
        // 只有稳固期结束且低于阈值足够多才降级（需要连续低于阈值）
        if (k.upgrade_stabilize_timer == 0 && base_score < downgrade_threshold - Balance::CIV_LEVEL_DOWNGRADE_THRESHOLD_OFFSET) {
            k.civ_decline_timer++;
            if (k.civ_decline_timer > 5) { // 连续5回合低于阈值才降级
                k.civilization_level--;
                k.civ_decline_timer = 0;
                k.downgrade_stabilize_timer = Balance::CIV_LEVEL_STABILIZE_TURNS; // 进入降级稳固期
                k.upgrade_stabilize_timer = 0; // 清除升级稳固期
                std::ostringstream oss;
                oss << k.name << " 的文明陷入衰退！文明等级下降至 " << k.civilization_level 
                    << "（文化健康度：" << std::fixed << std::setprecision(2) << k.culture_health << "）";
                EventRecord event(turn, format_time(turn),
                                  EventType::POLITICAL, k.id, oss.str(), false);
                globalEventLog.push_back(event);
                std::cout << "[" << turn << "] " << oss.str() << std::endl;
            }
        } else {
            // 不满足降级条件时重置衰退计时器
            k.civ_decline_timer = std::max(0, k.civ_decline_timer - 1);
        }
    } else {
        // 等级为0时不需要降级
        k.civ_decline_timer = 0;
    }

    // ---------- 6. 更新稳固计时器 ----------
    if (k.upgrade_stabilize_timer > 0) k.upgrade_stabilize_timer--;
    if (k.downgrade_stabilize_timer > 0) k.downgrade_stabilize_timer--;

    // ---------- 7. 根据文明等级计算 civ_effect_mod ----------
    double min_mod = 0.7;
    double max_mod = 1.5;
    double ratio = static_cast<double>(k.civilization_level) / MAX_CIVILIZATION_LEVEL;
    k.civ_effect_mod = min_mod + (max_mod - min_mod) * ratio;
    k.civ_effect_mod = std::max(0.3, std::min(2.0, k.civ_effect_mod));
}

// ============================================================
// 第 8 部分结束
// ============================================================

// ============================================================
// 第 9 部分：战争系统
// ============================================================

// ---------- 辅助函数：判断是否处于战争 ----------
bool at_war(const Kingdom& a, const Kingdom& b) {
    if (a.id < 0 || b.id < 0) return false;
    if (warMatrix[a.id][b.id] != 0 || warMatrix[b.id][a.id] != 0) return true;
    return false;
}

// ---------- 辅助函数：获取攻击方边境格（攻击方领土中与防守方相邻的格） ----------
std::vector<int> get_border_tiles(const Kingdom& attacker, const Kingdom& defender) {
    std::vector<int> border_tiles;
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    for (int idx : attacker.territory) {
        int x = idx % WORLD_WIDTH;
        int y = idx / WORLD_WIDTH;
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx < 0 || nx >= WORLD_WIDTH || ny < 0 || ny >= WORLD_HEIGHT) continue;
            if (ownerMap[ny][nx] == defender.id) {
                border_tiles.push_back(idx);
                break;
            }
        }
    }
    return border_tiles;
}

// ---------- 辅助函数：获取防守方边境格（防守方领土中与攻击方相邻的格） ----------
std::vector<int> get_enemy_border_tiles(const Kingdom& attacker, const Kingdom& defender) {
    std::vector<int> enemy_tiles;
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    for (int idx : defender.territory) {
        int x = idx % WORLD_WIDTH;
        int y = idx / WORLD_WIDTH;
        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx < 0 || nx >= WORLD_WIDTH || ny < 0 || ny >= WORLD_HEIGHT) continue;
            if (ownerMap[ny][nx] == attacker.id) {
                enemy_tiles.push_back(idx);
                break;
            }
        }
    }
    return enemy_tiles;
}

// ---------- 计算攻击力 ----------
double calc_attack_power(const Kingdom& attacker, const WorldData& world, int tile_x, int tile_y) {
    double base = attacker.army;
    double commander_agg = 0.5;
    if (!attacker.commanders.empty() && attacker.active_cmd_index >= 0 &&
            attacker.active_cmd_index < static_cast<int>(attacker.commanders.size())) {
        commander_agg = attacker.commanders[attacker.active_cmd_index].aggression;
    }
    double attack_factor = 0.8 + commander_agg * 0.4;
    double attack = base * attack_factor * attacker.combat_modifier() * attacker.cached_military_mod;

    // 地形攻击修正（基于目标格地形）
    int terrain = static_cast<int>(world.get(LAYER_TERRAIN, tile_x, tile_y));
    double terrain_atk_mod = 0.0;
    switch (terrain) {
    case static_cast<int>(TerrainClass::MOUNTAIN):
    case static_cast<int>(TerrainClass::HIGH_MOUNTAIN):
        terrain_atk_mod -= 0.4;
        break;
    case static_cast<int>(TerrainClass::HILL):
        terrain_atk_mod -= 0.2;
        break;
    case static_cast<int>(TerrainClass::LOW_PLAIN):
    case static_cast<int>(TerrainClass::COASTAL_PLAIN):
        terrain_atk_mod += 0.2;
        break;
    default:
        break;
    }
    double slope = world.get(LAYER_SLOPE, tile_x, tile_y);
    if (slope > 0.3) terrain_atk_mod -= 0.1;
    attack *= (1.0 + terrain_atk_mod);

    // 攻击方建筑加成
    attack *= (1.0 + attacker.cached_attack_bonus);

    return std::max(1.0, attack);
}

// ---------- 计算防御力 ----------
double calc_defense_power(const Kingdom& defender, const WorldData& world, int tile_x, int tile_y) {
    double base = defender.army;
    double commander_caution = 0.5;
    if (!defender.commanders.empty() && defender.active_cmd_index >= 0 &&
            defender.active_cmd_index < static_cast<int>(defender.commanders.size())) {
        commander_caution = defender.commanders[defender.active_cmd_index].caution;
    }
    double defense_factor = 1.0 + commander_caution * 0.4;
    double defense = base * defense_factor * defender.combat_modifier() * defender.cached_military_mod;

    // 地形防御修正（基于目标格地形）
    int terrain = static_cast<int>(world.get(LAYER_TERRAIN, tile_x, tile_y));
    double terrain_def_mod = 0.0;
    switch (terrain) {
    case static_cast<int>(TerrainClass::MOUNTAIN):
    case static_cast<int>(TerrainClass::HIGH_MOUNTAIN):
        terrain_def_mod += 0.6;
        break;
    case static_cast<int>(TerrainClass::HILL):
        terrain_def_mod += 0.3;
        break;
    default:
        break;
    }
    // 沿海悬崖额外防御
    int coast = static_cast<int>(world.get(LAYER_COASTTYPE, tile_x, tile_y));
    if (coast == static_cast<int>(CoastType::CLIFF) || coast == static_cast<int>(CoastType::FJORD)) {
        terrain_def_mod += 0.3;
    }
    double slope = world.get(LAYER_SLOPE, tile_x, tile_y);
    if (slope > 0.3) terrain_def_mod += 0.1;
    defense *= (1.0 + terrain_def_mod);

    // 建筑防御加成（比例）
    defense *= (1.0 + defender.cached_defense_bonus);

    return std::max(1.0, defense);
}

// ---------- 宣战与和平判定 ----------
void update_diplomacy(std::vector<Kingdom>& kingdoms, const WorldData& world, int turn) {
    size_t n = kingdoms.size();
    if (n < 2) return;

    std::uniform_real_distribution<double> prob(0.0, 1.0);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            Kingdom& a = kingdoms[i];
            Kingdom& b = kingdoms[j];
            if (!a.alive || !b.alive) continue;
            if (!kingdoms_share_border(a, b)) continue;

            bool in_war = at_war(a, b);

            if (!in_war) {
                if (a.peace_timer > 0 || b.peace_timer > 0) continue;
                if (FeatureToggle::ALLIANCES && allianceMatrix[i][j]) continue;

                double p = Balance::WAR_DECLARATION_BASE;
                double max_aggr = std::max(a.aggression, b.aggression);
                p += max_aggr * 0.25;
                double sim = cosine_similarity(a.culture, b.culture);
                p += (1.0 - sim) * 0.15;
                p -= (a.diplomacy_modifier() + b.diplomacy_modifier()) * 0.1;
                double army_ratio = (a.army + 1.0) / (b.army + 1.0);
                if (army_ratio > 1.2) p += std::min(0.3, (army_ratio - 1.0) * 0.1);
                else if (army_ratio < 0.8) p -= std::min(0.2, (1.0 - army_ratio) * 0.1);
                p -= (a.civilization_level + b.civilization_level) * Balance::CIV_LEVEL_WAR_DEC_RED;
                p = std::max(0.0, std::min(0.9, p));

                if (prob(global_rng) < p) {
                    warMatrix[i][j] = 1;
                    warMatrix[j][i] = 1;
                    a.war_timer = 0;
                    b.war_timer = 0;
                    a.peace_timer = 0;
                    b.peace_timer = 0;
                    a.total_wars++;
                    b.total_wars++;

                    // ===== 新增：宣战初期资源损耗 =====
                    a.gold = std::max(0.0, a.gold - 50.0);
                    b.gold = std::max(0.0, b.gold - 50.0);
                    a.food = std::max(0.0, a.food - 30.0);
                    b.food = std::max(0.0, b.food - 30.0);

                    std::ostringstream oss;
                    oss << a.name << " 向 " << b.name << " 宣战！";
                    EventRecord event(turn, format_time(turn),
                                      EventType::WAR, a.id, oss.str(), true);
                    event.involved_kingdom_ids = {a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
            } else {
                // 和平判定
                double p_peace = Balance::PEACE_BASE_PROB;
                p_peace += (a.diplomacy + b.diplomacy) * 0.2;
                int max_war_timer = std::max(a.war_timer, b.war_timer);
                p_peace += std::min(0.5, max_war_timer * Balance::WAR_EXHAUSTION_FACTOR);
                p_peace += (a.diplomacy_modifier() + b.diplomacy_modifier()) * 0.1;
                p_peace = std::max(0.0, std::min(0.9, p_peace));

                if (prob(global_rng) < p_peace) {
                    warMatrix[i][j] = 0;
                    warMatrix[j][i] = 0;
                    a.war_timer = 0;
                    b.war_timer = 0;

                    // ===== 修改：和平冷却期延长至40回合 =====
                    a.peace_timer = 40;
                    b.peace_timer = 40;

                    // ===== 新增：战后稳定度惩罚（厌战） =====
                    a.stability = std::max(MIN_STABILITY, a.stability - 10.0);
                    b.stability = std::max(MIN_STABILITY, b.stability - 10.0);

                    std::ostringstream oss;
                    oss << a.name << " 与 " << b.name << " 达成和平协议。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::WAR, a.id, oss.str(), true);
                    event.involved_kingdom_ids = {a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
            }
        }
    }
}

// ---------- 战斗处理 ----------
void process_wars(std::vector<Kingdom>& kingdoms, const WorldData& world, int turn) {
    size_t n = kingdoms.size();
    if (n < 2) return;

    // 收集所有战争对
    std::vector<std::pair<int,int>> war_pairs;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (warMatrix[i][j] != 0) {
                war_pairs.push_back({static_cast<int>(i), static_cast<int>(j)});
            }
        }
    }

    for (auto& wp : war_pairs) {
        // 随机决定进攻方与防守方，模拟双向战争
        Kingdom* attacker_ptr = &kingdoms[wp.first];
        Kingdom* defender_ptr = &kingdoms[wp.second];
        std::uniform_real_distribution<double> swap_prob(0.0, 1.0);
        if (swap_prob(global_rng) < 0.5) {
            std::swap(attacker_ptr, defender_ptr);
        }
        Kingdom& attacker = *attacker_ptr;
        Kingdom& defender = *defender_ptr;
        if (!attacker.alive || !defender.alive) continue;

        // 获取双方边境格
        std::vector<int> attacker_border = get_border_tiles(attacker, defender);
        std::vector<int> defender_border = get_enemy_border_tiles(attacker, defender);
        if (attacker_border.empty() || defender_border.empty()) continue;

        // 随机选择最多5个目标格进行攻击
        int max_attacks = std::min(5, static_cast<int>(defender_border.size()));
        std::shuffle(defender_border.begin(), defender_border.end(), global_rng);

        for (int k = 0; k < max_attacks; ++k) {
            int target_idx = defender_border[k];
            int tx = target_idx % WORLD_WIDTH;
            int ty = target_idx / WORLD_WIDTH;

            double atk_power = calc_attack_power(attacker, world, tx, ty);
            double def_power = calc_defense_power(defender, world, tx, ty);
            if (atk_power <= 0 || def_power <= 0) continue;

            // 道路加成
            double control_loss_modifier = 1.0;
            if (FeatureToggle::ROADS && roadMap[ty][tx] != 0) {
                control_loss_modifier = 1.2;
            }

            std::uniform_real_distribution<double> rand_factor(0.9, 1.1);
            double effective_atk = atk_power;
            double effective_def = def_power * rand_factor(global_rng);

            // ===== 战斗结果分支 =====
            if (effective_atk > effective_def) {
                // ---- 攻击成功：占领或降低控制 ----
                double control_change = (effective_atk - effective_def) / std::max(1.0, effective_def);
                control_change = std::min(0.3, control_change);
                control_change *= control_loss_modifier;
                double new_control = controlMap[ty][tx] - control_change;
                if (new_control <= 0.0) {
                    // 格子易主
                    new_control = 0.0;
                    ownerMap[ty][tx] = attacker.id;
                    controlMap[ty][tx] = 0.5;
                    cultureMap[ty][tx] = attacker.culture;
                    defender.territory.erase(target_idx);
                    attacker.territory.insert(target_idx);

                    // 检查易主的格子是否存在防守方的定居点，若有则强制移除
                    auto it_settle = std::find_if(defender.settlements.begin(), defender.settlements.end(),
                        [&](const Settlement& s) { return s.pos.x == tx && s.pos.y == ty; });
                    if (it_settle != defender.settlements.end()) {
                        defender.settlements.erase(it_settle);
                        defender.update_cached_effects();
                    }

                    // ---- 计算兵力损失（新逻辑） ----
                    // 绝对值损失（原逻辑，但缩放）
                    std::uniform_real_distribution<double> loss_def(Balance::ARMY_LOSS_MIN_DEFENDER * Balance::WAR_LOSS_ABSOLUTE_SCALE,
                                                                   Balance::ARMY_LOSS_MAX_DEFENDER * Balance::WAR_LOSS_ABSOLUTE_SCALE);
                    std::uniform_real_distribution<double> loss_atk(Balance::ARMY_LOSS_MIN_ATTACKER * Balance::WAR_LOSS_ABSOLUTE_SCALE,
                                                                   Balance::ARMY_LOSS_MAX_ATTACKER * Balance::WAR_LOSS_ABSOLUTE_SCALE);
                    double abs_loss_def = loss_def(global_rng);
                    double abs_loss_atk = loss_atk(global_rng);

                    // 百分比损失
                    double ratio = effective_atk / effective_def; // >= 1
                    double atk_pct = 0.0, def_pct = 0.0;
                    if (Balance::WAR_LOSS_PERCENTAGE_ENABLED) {
                        // 攻击方损失：优势越大损失越小，最低1%，最高约5%（当ratio=1时）
                        atk_pct = Balance::WAR_LOSS_PCT_MIN + Balance::WAR_LOSS_PCT_BASE_ATK_ADVANTAGE * (1.0 / ratio);
                        atk_pct = std::min(Balance::WAR_LOSS_PCT_MAX, atk_pct);
                        // 防守方损失：劣势越大损失越大，最低3%，最高10%
                        def_pct = Balance::WAR_LOSS_PCT_MIN + Balance::WAR_LOSS_PCT_BASE_DEF_ADVANTAGE * ratio;
                        def_pct = std::min(Balance::WAR_LOSS_PCT_MAX, def_pct);
                    }

                    // 应用损失（百分比和绝对值相加）
                    double def_total_loss = abs_loss_def + defender.army * def_pct;
                    double atk_total_loss = abs_loss_atk + attacker.army * atk_pct;
                    defender.army = std::max(0.0, defender.army - def_total_loss);
                    attacker.army = std::max(0.0, attacker.army - atk_total_loss);

                    // 人口损耗（征兵损耗） - 保持原逻辑，但基于百分比损耗调整
                    int pop_loss_def = static_cast<int>(def_total_loss * 0.3);
                    int pop_loss_atk = static_cast<int>(atk_total_loss * 0.3);
                    if (!defender.settlements.empty()) {
                        int total_pop = defender.total_population();
                        if (total_pop > 0) {
                            double ratio_pop = static_cast<double>(pop_loss_def) / total_pop;
                            for (auto& s : defender.settlements) {
                                s.population = std::max(Balance::MIN_SETTLEMENT_POP,
                                                        s.population - static_cast<int>(s.population * ratio_pop));
                            }
                        }
                    }
                    if (!attacker.settlements.empty()) {
                        int total_pop = attacker.total_population();
                        if (total_pop > 0) {
                            double ratio_pop = static_cast<double>(pop_loss_atk) / total_pop;
                            for (auto& s : attacker.settlements) {
                                s.population = std::max(Balance::MIN_SETTLEMENT_POP,
                                                        s.population - static_cast<int>(s.population * ratio_pop));
                            }
                        }
                    }

                    // 战争额外金币消耗（军需）
                    defender.gold = std::max(0.0, defender.gold - 20.0);
                    attacker.gold = std::max(0.0, attacker.gold - 20.0);

                } else {
                    // 未占领，降低控制
                    controlMap[ty][tx] = new_control;
                    // 未占领，但仍消耗一些资源（减少绝对值损失）
                    defender.gold = std::max(0.0, defender.gold - 10.0);
                    attacker.gold = std::max(0.0, attacker.gold - 10.0);
                    // 也损失少量兵力（战斗接触）
                    std::uniform_real_distribution<double> loss_fail(1.0, 4.0);
                    double abs_loss = loss_fail(global_rng) * Balance::WAR_LOSS_ABSOLUTE_SCALE;
                    // 百分比损失（较小，约1~3%）
                    double pct_loss = 0.01 + 0.02 * (effective_def / (effective_atk + effective_def)); // 守方略占优时损失稍大
                    if (Balance::WAR_LOSS_PERCENTAGE_ENABLED) {
                        attacker.army = std::max(0.0, attacker.army - abs_loss - attacker.army * pct_loss);
                        defender.army = std::max(0.0, defender.army - abs_loss * 0.5 - defender.army * pct_loss * 0.5);
                    } else {
                        attacker.army = std::max(0.0, attacker.army - abs_loss);
                        defender.army = std::max(0.0, defender.army - abs_loss * 0.5);
                    }
                }
            } else {
                // ---- 攻击失败 ----
                std::uniform_real_distribution<double> loss_atk_fail(1.0, 4.0);
                std::uniform_real_distribution<double> loss_def_fail(1.0, 3.0);
                double abs_loss_atk = loss_atk_fail(global_rng) * Balance::WAR_LOSS_ABSOLUTE_SCALE;
                double abs_loss_def = loss_def_fail(global_rng) * Balance::WAR_LOSS_ABSOLUTE_SCALE;

                // 百分比损失（攻击方损失更大）
                double ratio_def = effective_def / effective_atk; // > 1
                double atk_pct_fail = 0.02 + 0.05 * ratio_def; // 攻击方劣势越大损失越大
                double def_pct_fail = 0.01 + 0.02 * (1.0 / ratio_def); // 防守方优势越大损失越小
                atk_pct_fail = std::min(Balance::WAR_LOSS_PCT_MAX, atk_pct_fail);
                def_pct_fail = std::min(Balance::WAR_LOSS_PCT_MAX, def_pct_fail);

                if (Balance::WAR_LOSS_PERCENTAGE_ENABLED) {
                    attacker.army = std::max(0.0, attacker.army - abs_loss_atk - attacker.army * atk_pct_fail);
                    defender.army = std::max(0.0, defender.army - abs_loss_def - defender.army * def_pct_fail);
                } else {
                    attacker.army = std::max(0.0, attacker.army - abs_loss_atk);
                    defender.army = std::max(0.0, defender.army - abs_loss_def);
                }

                attacker.gold = std::max(0.0, attacker.gold - 5.0);
                defender.gold = std::max(0.0, defender.gold - 5.0);
            }

            // 确保兵力非负
            attacker.army = std::max(0.0, attacker.army);
            defender.army = std::max(0.0, defender.army);
        }

        // 更新战争持续时间
        attacker.war_timer++;
        defender.war_timer++;
    }
}

// ---------- 盟友参战 ----------
void ally_join_wars(std::vector<Kingdom>& kingdoms, int turn) {
    if (!FeatureToggle::ALLIANCES) return;
    size_t n = kingdoms.size();
    if (n < 2) return;

    std::vector<std::pair<int,int>> war_pairs;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (warMatrix[i][j] != 0) war_pairs.push_back({static_cast<int>(i), static_cast<int>(j)});
        }
    }

    for (auto& wp : war_pairs) {
        int attacker_id = wp.first;
        int defender_id = wp.second;
        // 防守方盟友
        for (int ally_id : kingdoms[defender_id].allies) {
            if (ally_id == attacker_id || ally_id == defender_id) continue;
            if (!kingdoms[ally_id].alive) continue;
            if (warMatrix[ally_id][attacker_id] == 0 && warMatrix[attacker_id][ally_id] == 0) {
                std::uniform_real_distribution<double> prob(0.0, 1.0);
                if (prob(global_rng) < Balance::ALLY_JOIN_WAR_PROB) {
                    warMatrix[ally_id][attacker_id] = 1;
                    warMatrix[attacker_id][ally_id] = 1;
                    kingdoms[ally_id].war_timer = 0;
                    kingdoms[attacker_id].war_timer = 0;
                    std::ostringstream oss;
                    oss << kingdoms[ally_id].name << " 作为盟友加入战争，向 "
                        << kingdoms[attacker_id].name << " 宣战！";
                    EventRecord event(turn, format_time(turn),
                                      EventType::WAR, ally_id, oss.str(), true);
                    event.involved_kingdom_ids = {ally_id, attacker_id, defender_id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
            }
        }
        // 攻击方盟友
        for (int ally_id : kingdoms[attacker_id].allies) {
            if (ally_id == attacker_id || ally_id == defender_id) continue;
            if (!kingdoms[ally_id].alive) continue;
            if (warMatrix[ally_id][defender_id] == 0 && warMatrix[defender_id][ally_id] == 0) {
                std::uniform_real_distribution<double> prob(0.0, 1.0);
                if (prob(global_rng) < Balance::ALLY_JOIN_WAR_PROB) {
                    warMatrix[ally_id][defender_id] = 1;
                    warMatrix[defender_id][ally_id] = 1;
                    kingdoms[ally_id].war_timer = 0;
                    kingdoms[defender_id].war_timer = 0;
                    std::ostringstream oss;
                    oss << kingdoms[ally_id].name << " 作为盟友加入战争，向 "
                        << kingdoms[defender_id].name << " 宣战！";
                    EventRecord event(turn, format_time(turn),
                                      EventType::WAR, ally_id, oss.str(), true);
                    event.involved_kingdom_ids = {ally_id, attacker_id, defender_id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
            }
        }
    }
}

// ---------- 世界大战检测 ----------
void check_world_war(std::vector<Kingdom>& kingdoms, int turn) {
    size_t n = kingdoms.size();
    if (n < WORLD_WAR_MIN_PARTICIPANTS) return;

    // 收集所有参与战争的王国
    std::vector<int> participants;
    for (size_t i = 0; i < n; ++i) {
        if (!kingdoms[i].alive) continue;
        bool at_war_flag = false;
        for (size_t j = 0; j < n; ++j) {
            if (i != j && warMatrix[i][j] != 0) {
                at_war_flag = true;
                break;
            }
        }
        if (at_war_flag) participants.push_back(static_cast<int>(i));
    }

    // ===== [修改] 如果参与国数量不足，世界大战结束 =====
    if (static_cast<int>(participants.size()) < WORLD_WAR_MIN_PARTICIPANTS) {
        for (auto& k : kingdoms) {
            if (k.in_world_war) k.in_world_war = false;
        }
        return;
    }

    // ===== [修改] 检查是否有新王国加入世界大战 =====
    // 只有当有王国从非世界大战状态变为世界大战状态时，才记录事件
    bool world_war_started = false;
    for (auto& k : kingdoms) {
        if (!k.alive) continue;
        
        bool is_participant = false;
        for (int pid : participants) {
            if (pid == k.id) {
                is_participant = true;
                break;
            }
        }
        
        // 如果该王国是参战国，但尚未标记为世界大战状态
        if (is_participant && !k.in_world_war) {
            k.in_world_war = true;
            world_war_started = true;  // 标记有新的世界大战触发
        }
    }

    // ===== [修改] 只在首次触发世界大战时记录一次事件 =====
    if (world_war_started) {
        std::ostringstream oss;
        oss << "世界大战爆发！参与国数量：" << participants.size();
        EventRecord event(turn, format_time(turn),
                          EventType::WAR, -1, oss.str(), true);
        event.involved_kingdom_ids = participants;
        globalEventLog.push_back(event);
        std::cout << "[" << turn << "] " << oss.str() << std::endl;
    }
}
// ============================================================
// 第 9 部分结束
// ============================================================

// ============================================================
// 文明模拟器 - 第 10 部分：外交与联盟
// ============================================================

// 全局计数器：每回合国际事件触发总数
static int global_international_event_counter = 0;
// 每王国每回合国际事件触发上限
constexpr int MAX_INTERNATIONAL_EVENTS_PER_TURN = 2;

// ---------- 辅助函数：检查两个王国是否有共同敌人 ----------
bool has_common_enemy(const Kingdom& a, const Kingdom& b) {
    size_t n = warMatrix.size();
    for (size_t i = 0; i < n; ++i) {
        if (i == static_cast<size_t>(a.id) || i == static_cast<size_t>(b.id)) continue;
        if (warMatrix[a.id][i] != 0 && warMatrix[b.id][i] != 0) {
            return true;
        }
    }
    return false;
}

// ---------- 联盟更新 ----------
void update_alliances(std::vector<Kingdom>& kingdoms, const WorldData& world, int turn) {
    if (!FeatureToggle::ALLIANCES) return;
    size_t n = kingdoms.size();
    if (n < 2) return;

    std::uniform_real_distribution<double> prob(0.0, 1.0);

    // 先根据 allianceMatrix 同步盟友列表
    for (auto& k : kingdoms) {
        k.allies.clear();
        for (size_t j = 0; j < n; ++j) {
            if (j == static_cast<size_t>(k.id)) continue;
            if (allianceMatrix[k.id][j]) {
                k.allies.push_back(static_cast<int>(j));
            }
        }
    }

    // 遍历所有王国对
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            Kingdom& a = kingdoms[i];
            Kingdom& b = kingdoms[j];
            if (!a.alive || !b.alive) continue;

            bool currently_allied = allianceMatrix[i][j];

            // 若处于战争状态，强制解除联盟
            if (warMatrix[i][j] != 0 || warMatrix[j][i] != 0) {
                if (currently_allied) {
                    allianceMatrix[i][j] = false;
                    allianceMatrix[j][i] = false;
                    std::ostringstream oss;
                    oss << a.name << " 与 " << b.name << " 因战争联盟破裂。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::INTERNATIONAL, a.id, oss.str(), true);
                    event.involved_kingdom_ids = {a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
                continue;
            }

            if (currently_allied) {
                // 联盟破裂判定
                double break_prob = Balance::ALLIANCE_BREAK_PROB;
                if (!has_common_enemy(a, b)) {
                    break_prob += 0.01;
                }
                double sim = cosine_similarity(a.culture, b.culture);
                if (sim < 0.5) break_prob += 0.01;

                if (prob(global_rng) < break_prob) {
                    allianceMatrix[i][j] = false;
                    allianceMatrix[j][i] = false;
                    a.allies.erase(std::remove(a.allies.begin(), a.allies.end(), static_cast<int>(j)), a.allies.end());
                    b.allies.erase(std::remove(b.allies.begin(), b.allies.end(), static_cast<int>(i)), b.allies.end());
                    std::ostringstream oss;
                    oss << a.name << " 与 " << b.name << " 的联盟破裂。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::INTERNATIONAL, a.id, oss.str(), true);
                    event.involved_kingdom_ids = {a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
            } else {
                // 结盟判定
                if (!kingdoms_share_border(a, b)) continue;
                double sim = cosine_similarity(a.culture, b.culture);
                double culture_score = sim * 0.6;
                double enemy_score = has_common_enemy(a, b) ? 0.3 : 0.0;
                double diplomacy_score = (a.diplomacy_modifier() + b.diplomacy_modifier()) * 0.1;
                double total_score = culture_score + enemy_score + diplomacy_score;

                if (total_score > Balance::ALLIANCE_SCORE_THRESH) {
                    double alliance_prob = std::min(1.0, total_score);
                    if (prob(global_rng) < alliance_prob) {
                        allianceMatrix[i][j] = true;
                        allianceMatrix[j][i] = true;
                        a.allies.push_back(static_cast<int>(j));
                        b.allies.push_back(static_cast<int>(i));
                        std::ostringstream oss;
                        oss << a.name << " 与 " << b.name << " 缔结同盟。";
                        EventRecord event(turn, format_time(turn),
                                          EventType::INTERNATIONAL, a.id, oss.str(), true);
                        event.involved_kingdom_ids = {a.id, b.id};
                        globalEventLog.push_back(event);
                        std::cout << "[" << turn << "] " << oss.str() << std::endl;
                    }
                }
            }
        }
    }
}

// ---------- 国际事件处理 ----------
void process_international_events(std::vector<Kingdom>& kingdoms, int turn) {
    size_t n = kingdoms.size();
    if (n < 2) return;

    std::uniform_real_distribution<double> prob(0.0, 1.0);
    global_international_event_counter = 0; // 每回合重置

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (global_international_event_counter >= MAX_INTERNATIONAL_EVENTS_PER_TURN * static_cast<int>(n)) {
                return;
            }
            Kingdom& a = kingdoms[i];
            Kingdom& b = kingdoms[j];
            if (!a.alive || !b.alive) continue;

            bool at_war_pair = at_war(a, b);

            double base_prob = Balance::INTERNATIONAL_EVENT_BASE;
            if (allianceMatrix[i][j]) base_prob *= 1.5;
            if (at_war_pair) base_prob *= 0.5;

            if (prob(global_rng) < base_prob) {
                // 选择事件类型
                struct EventWeight {
                    std::string type;
                    double weight;
                };
                std::vector<EventWeight> pool;
                if (!at_war_pair) {
                    pool = {
                        {"heqin", 0.15},
                        {"huimeng", 0.10},
                        {"trade_agreement", 0.30},
                        {"cultural_exchange", 0.25},
                        {"spy", 0.10},
                        {"border_friction", 0.10},
                        {"religious_revival", 0.06},
                        {"great_plague", 0.05},
                        {"golden_age", 0.05},
                        {"migration_wave", 0.04},
                        {"alliance_formation", 0.04},
                        {"diplomatic_marriage", 0.02}
                    };
                } else {
                    pool = {
                        {"spy", 0.30},
                        {"border_friction", 0.70},
                        {"religious_revival", 0.10},
                        {"great_plague", 0.15},
                        {"migration_wave", 0.10},
                        {"alliance_formation", 0.15}
                    };
                }

                double total_weight = 0.0;
                for (const auto& ew : pool) total_weight += ew.weight;
                std::uniform_real_distribution<double> weight_dist(0.0, total_weight);
                double r = weight_dist(global_rng);
                double cumulative = 0.0;
                std::string chosen;
                for (const auto& ew : pool) {
                    cumulative += ew.weight;
                    if (r <= cumulative) {
                        chosen = ew.type;
                        break;
                    }
                }

                // ---------- 执行事件 ----------
                if (chosen == "heqin") {
                    a.stability = std::min(MAX_STABILITY, a.stability + 10.0);
                    b.stability = std::min(MAX_STABILITY, b.stability + 10.0);
                    if (at_war_pair) {
                        warMatrix[i][j] = 0;
                        warMatrix[j][i] = 0;
                    }
                    a.peace_timer = 20;
                    b.peace_timer = 20;
                    std::ostringstream oss;
                    oss << a.name << " 与 " << b.name << " 通过和亲达成和平。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::INTERNATIONAL, a.id, oss.str(), true);
                    event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
                else if (chosen == "huimeng") {
                    if (at_war_pair) {
                        warMatrix[i][j] = 0;
                        warMatrix[j][i] = 0;
                    }
                    a.peace_timer = 20;
                    b.peace_timer = 20;
                    a.stability += 5.0;
                    b.stability += 5.0;
                    std::ostringstream oss;
                    oss << a.name << " 与 " << b.name << " 会盟，互不侵犯。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::INTERNATIONAL, a.id, oss.str(), true);
                    event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
                else if (chosen == "trade_agreement") {
                    a.gold += 20.0;
                    b.gold += 20.0;
                    std::ostringstream oss;
                    oss << a.name << " 与 " << b.name << " 签订贸易协定。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::INTERNATIONAL, a.id, oss.str(), true);
                    event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
                else if (chosen == "cultural_exchange") {
                    a.tech_level = std::min(MAX_TECH_LEVEL, a.tech_level + 0.05);
                    b.tech_level = std::min(MAX_TECH_LEVEL, b.tech_level + 0.05);
                    for (int c = 0; c < CULTURE_DIM; ++c) {
                        double avg = (a.culture[c] + b.culture[c]) / 2.0;
                        a.culture[c] = a.culture[c] * 0.95 + avg * 0.05;
                        b.culture[c] = b.culture[c] * 0.95 + avg * 0.05;
                    }
                    std::ostringstream oss;
                    oss << a.name << " 与 " << b.name << " 进行文化交流。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::INTERNATIONAL, a.id, oss.str(), true);
                    event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
                else if (chosen == "spy") {
                    bool target_a = (prob(global_rng) < 0.5);
                    Kingdom& target = target_a ? a : b;
                    Kingdom& source = target_a ? b : a;
                    target.stability = std::max(MIN_STABILITY, target.stability - 5.0);
                    if (prob(global_rng) < 0.3 && !source.owned_techs.empty()) {
                        std::uniform_int_distribution<size_t> tech_dist(0, source.owned_techs.size() - 1);
                        TechID stolen = source.owned_techs[tech_dist(global_rng)];
                        bool already_has = false;
                        for (TechID tid : target.owned_techs) {
                            if (tid == stolen) {
                                already_has = true;
                                break;
                            }
                        }
                        if (!already_has) {
                            target.owned_techs.push_back(stolen);
                            target.update_cached_effects();
                        }
                    }
                    std::ostringstream oss;
                    oss << source.name << " 对 " << target.name << " 发动间谍活动。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::INTERNATIONAL, source.id, oss.str(), true);
                    event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
                else if (chosen == "border_friction") {
                    std::uniform_real_distribution<double> loss(2.0, 8.0);
                    a.army = std::max(0.0, a.army - loss(global_rng));
                    b.army = std::max(0.0, b.army - loss(global_rng));
                    if (!at_war_pair && prob(global_rng) < 0.3) {
                        warMatrix[i][j] = 1;
                        warMatrix[j][i] = 1;
                        a.war_timer = 0;
                        b.war_timer = 0;
                        std::ostringstream oss;
                        oss << a.name << " 与 " << b.name << " 边境摩擦升级为战争！";
                        EventRecord event(turn, format_time(turn),
                                          EventType::WAR, a.id, oss.str(), true);
                        event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                        globalEventLog.push_back(event);
                        std::cout << "[" << turn << "] " << oss.str() << std::endl;
                    } else {
                        std::ostringstream oss;
                        oss << a.name << " 与 " << b.name << " 发生边境摩擦。";
                        EventRecord event(turn, format_time(turn),
                                          EventType::INTERNATIONAL, a.id, oss.str(), true);
                        event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                        globalEventLog.push_back(event);
                        std::cout << "[" << turn << "] " << oss.str() << std::endl;
                    }
                }
                // ===== 新增事件 =====
                else if (chosen == "religious_revival") {
                    a.stability = std::min(MAX_STABILITY, a.stability + 15.0);
                    b.stability = std::min(MAX_STABILITY, b.stability + 15.0);
                    for (int c = 0; c < CULTURE_DIM; ++c) {
                        double avg = (a.culture[c] + b.culture[c]) / 2.0;
                        a.culture[c] = a.culture[c] * 0.8 + avg * 0.2;
                        b.culture[c] = b.culture[c] * 0.8 + avg * 0.2;
                    }
                    std::ostringstream oss;
                    oss << a.name << " 与 " << b.name << " 地区宗教复兴，民心凝聚。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::RELIGIOUS, a.id, oss.str(), true);
                    event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
                else if (chosen == "great_plague") {
                    for (auto& s : a.settlements) {
                        s.population = std::max(Balance::MIN_SETTLEMENT_POP,
                                                static_cast<int>(s.population * 0.85));
                    }
                    for (auto& s : b.settlements) {
                        s.population = std::max(Balance::MIN_SETTLEMENT_POP,
                                                static_cast<int>(s.population * 0.85));
                    }
                    a.stability = std::max(MIN_STABILITY, a.stability - 10.0);
                    b.stability = std::max(MIN_STABILITY, b.stability - 10.0);
                    a.gold = std::max(0.0, a.gold - 50.0);
                    b.gold = std::max(0.0, b.gold - 50.0);
                    std::ostringstream oss;
                    oss << "大瘟疫席卷 " << a.name << " 与 " << b.name << "，人口锐减。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::DISASTER, -1, oss.str(), true);
                    event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
                else if (chosen == "golden_age") {
                    a.tech_level = std::min(MAX_TECH_LEVEL, a.tech_level + 0.2);
                    b.tech_level = std::min(MAX_TECH_LEVEL, b.tech_level + 0.2);
                    a.gold += 100.0;
                    b.gold += 100.0;
                    a.stability = std::min(MAX_STABILITY, a.stability + 10.0);
                    b.stability = std::min(MAX_STABILITY, b.stability + 10.0);
                    std::ostringstream oss;
                    oss << a.name << " 与 " << b.name << " 迎来黄金时代，繁荣昌盛。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::ECONOMIC, a.id, oss.str(), true);
                    event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
                else if (chosen == "migration_wave") {
                    int migrants_a = 20 + std::uniform_int_distribution<int>(0, 30)(global_rng);
                    int migrants_b = 20 + std::uniform_int_distribution<int>(0, 30)(global_rng);
                    if (!a.settlements.empty()) {
                        int per = migrants_a / a.settlements.size();
                        for (auto& s : a.settlements) s.population += per;
                    }
                    if (!b.settlements.empty()) {
                        int per = migrants_b / b.settlements.size();
                        for (auto& s : b.settlements) s.population += per;
                    }
                    a.stability = std::max(MIN_STABILITY, a.stability - 5.0);
                    b.stability = std::max(MIN_STABILITY, b.stability - 5.0);
                    std::ostringstream oss;
                    oss << "移民潮涌入 " << a.name << " 与 " << b.name << "，人口增加但社会动荡。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::MIGRATION, a.id, oss.str(), true);
                    event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }
                else if (chosen == "alliance_formation") {
                    if (!allianceMatrix[a.id][b.id]) {
                        allianceMatrix[a.id][b.id] = true;
                        allianceMatrix[b.id][a.id] = true;
                        a.allies.push_back(b.id);
                        b.allies.push_back(a.id);
                        std::ostringstream oss;
                        oss << a.name << " 与 " << b.name << " 响应国际倡议，缔结同盟。";
                        EventRecord event(turn, format_time(turn),
                                          EventType::INTERNATIONAL, a.id, oss.str(), true);
                        event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                        globalEventLog.push_back(event);
                        std::cout << "[" << turn << "] " << oss.str() << std::endl;
                    }
                }
                else if (chosen == "diplomatic_marriage") {
                    a.stability = std::min(MAX_STABILITY, a.stability + 15.0);
                    b.stability = std::min(MAX_STABILITY, b.stability + 15.0);
                    if (at_war_pair) {
                        warMatrix[i][j] = 0;
                        warMatrix[j][i] = 0;
                        a.peace_timer = 30;
                        b.peace_timer = 30;
                    }
                    std::ostringstream oss;
                    oss << a.name << " 与 " << b.name << " 通过政治联姻巩固关系。";
                    EventRecord event(turn, format_time(turn),
                                      EventType::POLITICAL, a.id, oss.str(), true);
                    event.involved_kingdom_ids = std::vector<int>{a.id, b.id};
                    globalEventLog.push_back(event);
                    std::cout << "[" << turn << "] " << oss.str() << std::endl;
                }

                global_international_event_counter++;
            }
        }
    }
}

// ============================================================
// 第 10 部分结束
// ============================================================

// ============================================================
// 第 11 部分：道路系统
// ============================================================

// ---------- A* 寻路辅助结构 ----------
struct AStarNode {
    int x, y;
    double g_cost;
    double f_cost;
    int parent_idx;
    bool in_closed;
    bool in_open;

    AStarNode() : x(0), y(0), g_cost(0.0), f_cost(0.0), parent_idx(-1),
        in_closed(false), in_open(false) {}
};

// ---------- 启发式函数（曼哈顿距离） ----------
double heuristic(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

// ---------- A* 寻路 ----------
// ---------- A* 寻路 ----------
bool astar_find_path(int start_x, int start_y, int goal_x, int goal_y,
                     const WorldData& world, std::vector<Position>& path) {
    path.clear();
    if (start_x == goal_x && start_y == goal_y) return true;
    if (!world.inside(start_x, start_y) || !world.inside(goal_x, goal_y)) return false;

    int total_cells = WORLD_WIDTH * WORLD_HEIGHT;
    std::vector<AStarNode> nodes(total_cells);
    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        for (int x = 0; x < WORLD_WIDTH; ++x) {
            int idx = linear_index(x, y, WORLD_WIDTH);
            nodes[idx].x = x;
            nodes[idx].y = y;
            nodes[idx].g_cost = std::numeric_limits<double>::infinity();
            nodes[idx].f_cost = std::numeric_limits<double>::infinity();
            nodes[idx].parent_idx = -1;
            nodes[idx].in_closed = false;
            nodes[idx].in_open = false;
        }
    }

    using PQElement = std::pair<double, int>;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> open_list;
    std::unordered_set<int> open_set;

    int start_idx = linear_index(start_x, start_y, WORLD_WIDTH);
    int goal_idx = linear_index(goal_x, goal_y, WORLD_WIDTH);

    nodes[start_idx].g_cost = 0.0;
    nodes[start_idx].f_cost = heuristic(start_x, start_y, goal_x, goal_y);
    nodes[start_idx].parent_idx = -1;
    nodes[start_idx].in_open = true;
    open_list.push({nodes[start_idx].f_cost, start_idx});
    open_set.insert(start_idx);

    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};

    int explored = 0;
    while (!open_list.empty() && explored < ASTAR_MAX_NODES) {
        explored++;
        auto current = open_list.top();
        open_list.pop();
        int cur_idx = current.second;
        if (nodes[cur_idx].in_closed) continue;
        open_set.erase(cur_idx);
        nodes[cur_idx].in_open = false;
        nodes[cur_idx].in_closed = true;

        if (cur_idx == goal_idx) {
            int idx = goal_idx;
            while (idx != start_idx) {
                path.push_back({nodes[idx].x, nodes[idx].y});
                idx = nodes[idx].parent_idx;
                if (idx == -1) {
                    path.clear();
                    return false;
                }
            }
            std::reverse(path.begin(), path.end());
            return true;
        }

        int cx = nodes[cur_idx].x;
        int cy = nodes[cur_idx].y;

        for (int d = 0; d < 4; ++d) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];
            if (nx < 0 || nx >= WORLD_WIDTH || ny < 0 || ny >= WORLD_HEIGHT) continue;
            int nidx = linear_index(nx, ny, WORLD_WIDTH);
            if (nodes[nidx].in_closed) continue;

            double slope = world.get(LAYER_SLOPE, nx, ny);
            double move_cost = 1.0 + slope * Balance::ROAD_ASTAR_SLOPE;

            double elev = world.get(LAYER_ELEVATION, nx, ny);
            if (elev <= 0.0 && roadMap[ny][nx] == 0) continue;

            double new_g = nodes[cur_idx].g_cost + move_cost;
            if (!nodes[nidx].in_open || new_g < nodes[nidx].g_cost) {
                nodes[nidx].g_cost = new_g;
                nodes[nidx].f_cost = new_g + heuristic(nx, ny, goal_x, goal_y);
                nodes[nidx].parent_idx = cur_idx;
                if (!nodes[nidx].in_open) {
                    nodes[nidx].in_open = true;
                    open_list.push({nodes[nidx].f_cost, nidx});
                    open_set.insert(nidx);
                }
            }
        }
    }
    return false;
}
// ---------- 道路修建 ----------
void build_roads(Kingdom& k, const WorldData& world, int turn) {
    if (!FeatureToggle::ROADS) return;
    if (k.gold < Balance::ROAD_COST_BASE) return;
    if (k.industriousness <= 0.3) return;

    // 找到首都
    Position capital_pos(-1, -1);
    for (const auto& s : k.settlements) {
        if (s.is_capital) {
            capital_pos = s.pos;
            break;
        }
    }
    if (capital_pos.x == -1) return;

    // 优先连接未连通定居点
    std::vector<const Settlement*> unconnected;
    for (const auto& s : k.settlements) {
        if (s.is_capital) continue;
        if (!is_settlement_connected_to_capital(k, s)) {
            unconnected.push_back(&s);
        }
    }

    if (!unconnected.empty()) {
        // 有未连通定居点，优先修建连接道路
        std::uniform_int_distribution<size_t> dist(0, unconnected.size() - 1);
        const Settlement* target = unconnected[dist(global_rng)];

        std::vector<Position> path;
        bool found = astar_find_path(capital_pos.x, capital_pos.y,
                                     target->pos.x, target->pos.y, world, path);
        if (!found || path.empty()) return;

        // 从路径中找第一个未修建道路的格子
        Position build_pos(-1, -1);
        for (const Position& p : path) {
            if (roadMap[p.y][p.x] == 0) {
                build_pos = p;
                break;
            }
        }
        if (build_pos.x == -1) return;

        double slope = world.get(LAYER_SLOPE, build_pos.x, build_pos.y);
        double cost = Balance::ROAD_COST_BASE + slope * Balance::ROAD_COST_SLOPE;
        if (k.gold < cost) return;

        k.gold -= cost;
        roadMap[build_pos.y][build_pos.x] = k.id;
        k.road_tiles.insert(linear_index(build_pos.x, build_pos.y, WORLD_WIDTH));

        std::ostringstream oss;
        oss << k.name << " 在 (" << build_pos.x << ", " << build_pos.y << ") 修建了官道。";
        EventRecord event(turn, format_time(turn),
                          EventType::ROAD, k.id, oss.str(), false,
                          build_pos.x, build_pos.y);
        globalEventLog.push_back(event);
        return;
    }

    // ===== 新增：所有定居点已连通，在领土内随机修建道路（主干道） =====
    // 收集领土中未修路的格子
    std::vector<int> unroaded;
    for (int idx : k.territory) {
        if (roadMap[idx / WORLD_WIDTH][idx % WORLD_WIDTH] == 0) {
            unroaded.push_back(idx);
        }
    }
    if (unroaded.empty()) return;

    std::uniform_int_distribution<size_t> dist(0, unroaded.size() - 1);
    int idx = unroaded[dist(global_rng)];
    int x = idx % WORLD_WIDTH;
    int y = idx / WORLD_WIDTH;

    double slope = world.get(LAYER_SLOPE, x, y);
    double cost = Balance::ROAD_COST_BASE + slope * Balance::ROAD_COST_SLOPE;
    if (k.gold < cost) return;

    k.gold -= cost;
    roadMap[y][x] = k.id;
    k.road_tiles.insert(idx);

    std::ostringstream oss;
    oss << k.name << " 在 (" << x << ", " << y << ") 修建了主干道。";
    EventRecord event(turn, format_time(turn),
                      EventType::ROAD, k.id, oss.str(), false, x, y);
    globalEventLog.push_back(event);
}
// ---------- 道路维护与拆除 ----------
void maintain_roads(Kingdom& k, int turn) {
    if (!FeatureToggle::ROADS) return;
    if (k.road_tiles.empty()) return;

    double total_maintenance = k.road_tiles.size() * Balance::ROAD_MAINTENANCE;
    if (k.gold >= total_maintenance) return; // 金币足够，无需拆除

    std::vector<int> road_list(k.road_tiles.begin(), k.road_tiles.end());
    std::shuffle(road_list.begin(), road_list.end(), global_rng);

    for (int idx : road_list) {
        if (k.gold >= k.road_tiles.size() * Balance::ROAD_MAINTENANCE) break; // 已经够付
        int x = idx % WORLD_WIDTH;
        int y = idx / WORLD_WIDTH;
        roadMap[y][x] = 0;
        k.road_tiles.erase(idx);
    }
}

// ============================================================
// 第 11 部分结束
// ============================================================


// ============================================================
// 文明模拟器 - 第 12 部分：事件系统
// ============================================================

// ---------- 事件冷却表：event_id -> (kingdom_id -> last_triggered_turn) ----------
static std::unordered_map<int, std::unordered_map<int, int>> event_cooldowns;

// ---------- 辅助函数：检查事件是否处于冷却（按王国独立） ----------
bool event_on_cooldown(int event_id, int kingdom_id, int current_turn, int cooldown) {
    auto it_kingdom = event_cooldowns.find(event_id);
    if (it_kingdom == event_cooldowns.end()) return false;
    auto it_turn = it_kingdom->second.find(kingdom_id);
    if (it_turn == it_kingdom->second.end()) return false;
    return (current_turn - it_turn->second) < cooldown;
}

// ---------- 辅助函数：记录事件触发回合（按王国独立） ----------
void mark_event_triggered(int event_id, int kingdom_id, int turn) {
    event_cooldowns[event_id][kingdom_id] = turn;
}

// ---------- 事件模板初始化 ----------
void init_event_templates() {
    globalEventTemplates.clear();

    // ==================== 国内显性事件 ====================
    // 政治类
    globalEventTemplates.emplace_back(1, "贵族内斗", EventType::POLITICAL, 0.008, "domestic",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::STABILITY, -8.0}, {EffectStat::GOLD, -30.0} },
    true, false, false, "{kingdom} 爆发贵族内斗，朝堂陷入混乱。");
    globalEventTemplates.emplace_back(2, "国王改革", EventType::POLITICAL, 0.005, "domestic",
    std::vector<Condition> { {ConditionStat::STABILITY, ">", 60.0} },
    std::vector<Effect> { {EffectStat::STABILITY, 5.0}, {EffectStat::TAX_MOD, 0.05} },
    true, false, false, "{kingdom} 的国王推行改革，国力有所增强。");
    globalEventTemplates.emplace_back(3, "宫廷政变", EventType::POLITICAL, 0.003, "domestic",
    std::vector<Condition> { {ConditionStat::STABILITY, "<", 40.0} },
    std::vector<Effect> { {EffectStat::STABILITY, -15.0}, {EffectStat::ARMY, -10.0} },
    true, false, false, "{kingdom} 发生宫廷政变，王位易主。");
    globalEventTemplates.emplace_back(4, "地方自治", EventType::POLITICAL, 0.006, "domestic",
    std::vector<Condition> { {ConditionStat::STABILITY, "<", 50.0} },
    std::vector<Effect> { {EffectStat::STABILITY, -5.0}, {EffectStat::REBELLION_RISK_MOD, 0.1} },
    true, false, false, "{kingdom} 的地方贵族要求更多自治权。");

    // 经济类
    globalEventTemplates.emplace_back(5, "贸易繁荣", EventType::ECONOMIC, 0.01, "domestic",
    std::vector<Condition> { {ConditionStat::GOLD, ">", 100.0} },
    std::vector<Effect> { {EffectStat::GOLD, 80.0} },
    true, false, false, "{kingdom} 贸易繁荣，国库充盈。");
    globalEventTemplates.emplace_back(6, "商人捐献", EventType::ECONOMIC, 0.008, "domestic",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::GOLD, 40.0}, {EffectStat::STABILITY, 2.0} },
    true, false, false, "{kingdom} 的商人向王国捐献巨款。");
    globalEventTemplates.emplace_back(7, "虫害", EventType::ECONOMIC, 0.006, "domestic",
    std::vector<Condition> { {ConditionStat::FOOD_PER_POP, "<", 1.2} },
    std::vector<Effect> { {EffectStat::FOOD, -60.0} },
    true, false, false, "{kingdom} 遭遇大规模虫害，粮食歉收。");
    globalEventTemplates.emplace_back(8, "经济危机", EventType::ECONOMIC, 0.004, "domestic",
    std::vector<Condition> { {ConditionStat::GOLD, "<", 200.0} },
    std::vector<Effect> { {EffectStat::GOLD, -100.0}, {EffectStat::STABILITY, -10.0} },
    true, false, false, "{kingdom} 陷入经济危机，民生凋敝。");
    globalEventTemplates.emplace_back(9, "大丰收", EventType::ECONOMIC, 0.012, "domestic",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::FOOD, 100.0}, {EffectStat::STABILITY, 5.0} },
    true, false, false, "{kingdom} 迎来前所未有的大丰收。");

    // 社会类
    globalEventTemplates.emplace_back(10, "农民请愿", EventType::SOCIAL, 0.01, "domestic",
    std::vector<Condition> { {ConditionStat::TAX_RATE, ">", 0.3} },
    std::vector<Effect> { {EffectStat::STABILITY, -5.0} },
    true, false, false, "{kingdom} 农民请愿减税，否则将暴动。");
    globalEventTemplates.emplace_back(11, "商人请愿减税", EventType::SOCIAL, 0.008, "domestic",
    std::vector<Condition> { {ConditionStat::TAX_RATE, ">", 0.25} },
    std::vector<Effect> { {EffectStat::STABILITY, -3.0} },
    true, false, false, "{kingdom} 商人团体请求降低税率。");
    globalEventTemplates.emplace_back(12, "民间建神庙", EventType::SOCIAL, 0.006, "domestic",
    std::vector<Condition> { {ConditionStat::STABILITY, ">", 50.0} },
    std::vector<Effect> { {EffectStat::STABILITY, 3.0} },
    true, false, false, "{kingdom} 民间自发修建神庙，信仰更加虔诚。");
    globalEventTemplates.emplace_back(13, "医院救治", EventType::SOCIAL, 0.005, "domestic",
    std::vector<Condition> { {ConditionStat::POPULATION, ">", 500.0} },
    std::vector<Effect> { {EffectStat::POPULATION, 0.01}, {EffectStat::STABILITY, 2.0} },
    true, false, false, "{kingdom} 医院救治了许多病患，人口增长。");

    // ==================== 隐性事件（不记录） ====================
    // 农业
    globalEventTemplates.emplace_back(100, "土壤自然改良", EventType::ECONOMIC, 0.02, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::FOOD_MOD, 0.02} },
    false, false, false, "");
    globalEventTemplates.emplace_back(101, "秘密灌溉", EventType::ECONOMIC, 0.015, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::FOOD_MOD, 0.05} },
    false, false, false, "");
    globalEventTemplates.emplace_back(102, "猎人丰收", EventType::ECONOMIC, 0.02, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::FOOD, 20.0} },
    false, false, false, "");
    globalEventTemplates.emplace_back(103, "作物病害", EventType::ECONOMIC, 0.01, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::FOOD, -30.0} },
    false, false, false, "");

    // 经济
    globalEventTemplates.emplace_back(104, "黑市交易", EventType::ECONOMIC, 0.02, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::GOLD, 15.0}, {EffectStat::STABILITY, -2.0} },
    false, false, false, "");
    globalEventTemplates.emplace_back(105, "资源浪费", EventType::ECONOMIC, 0.015, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::GOLD, -20.0} },
    false, false, false, "");
    globalEventTemplates.emplace_back(106, "税收调整", EventType::ECONOMIC, 0.02, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::TAX_MOD, 0.02} },
    false, false, false, "");

    // 军事
    globalEventTemplates.emplace_back(107, "边境巡逻加强", EventType::WAR, 0.02, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::DEFENSE_BONUS, 0.05} },
    false, false, false, "");
    globalEventTemplates.emplace_back(108, "士兵逃役", EventType::WAR, 0.015, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::ARMY, -5.0} },
    false, false, false, "");
    globalEventTemplates.emplace_back(109, "武器改进", EventType::WAR, 0.01, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::MILITARY_POWER_MOD, 0.03} },
    false, false, false, "");

    // 科技
    globalEventTemplates.emplace_back(110, "意外发现", EventType::TECHNOLOGICAL, 0.02, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::RESEARCH_POINTS, 0.02} },
    false, false, false, "");
    globalEventTemplates.emplace_back(111, "学者争论", EventType::TECHNOLOGICAL, 0.015, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::RESEARCH_POINTS, -0.01} },
    false, false, false, "");

    // 文化
    globalEventTemplates.emplace_back(112, "民间艺术繁荣", EventType::CULTURAL, 0.02, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::STABILITY, 2.0} },
    false, false, false, "");
    globalEventTemplates.emplace_back(113, "传统节日", EventType::CULTURAL, 0.03, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::STABILITY, 3.0} },
    false, false, false, "");

    // 人口
    globalEventTemplates.emplace_back(114, "婴儿潮", EventType::SOCIAL, 0.02, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::POPULATION, 0.03} },
    false, false, false, "");
    globalEventTemplates.emplace_back(115, "瘟疫苗头", EventType::SOCIAL, 0.01, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::POPULATION, -0.02} },
    false, false, false, "");

    // 环境
    globalEventTemplates.emplace_back(116, "河流改道", EventType::DISASTER, 0.005, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::FOOD, -20.0} },
    false, false, false, "");
    globalEventTemplates.emplace_back(117, "森林恢复", EventType::DISASTER, 0.01, "hidden",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::FOOD_MOD, 0.02} },
    false, false, false, "");

    // ==================== 自然灾害（显性） ====================
    globalEventTemplates.emplace_back(200, "火山爆发", EventType::DISASTER, 0.002, "domestic",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::STABILITY, -20.0}, {EffectStat::ARMY, -15.0},
        {EffectStat::POPULATION, -0.1}, {EffectStat::GOLD, -100.0}
    },
    true, false, false, "{kingdom} 发生火山爆发，造成巨大灾难！");
    globalEventTemplates.emplace_back(201, "地震", EventType::DISASTER, 0.004, "domestic",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::STABILITY, -15.0}, {EffectStat::GOLD, -80.0},
        {EffectStat::POPULATION, -0.05}
    },
    true, false, false, "{kingdom} 遭遇强烈地震，房屋倒塌。");
    globalEventTemplates.emplace_back(202, "洪水", EventType::DISASTER, 0.006, "domestic",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::FOOD, -50.0}, {EffectStat::STABILITY, -10.0},
        {EffectStat::POPULATION, -0.03}
    },
    true, false, false, "{kingdom} 洪水泛滥，农田被淹。");
    globalEventTemplates.emplace_back(203, "干旱", EventType::DISASTER, 0.008, "domestic",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::FOOD, -80.0}, {EffectStat::STABILITY, -12.0} },
    true, false, false, "{kingdom} 长期干旱，庄稼枯死。");
    globalEventTemplates.emplace_back(204, "瘟疫", EventType::DISASTER, 0.005, "domestic",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::POPULATION, -0.15}, {EffectStat::STABILITY, -15.0} },
    true, false, false, "{kingdom} 爆发瘟疫，人口锐减。");
    globalEventTemplates.emplace_back(205, "暴风雪", EventType::DISASTER, 0.004, "domestic",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::FOOD, -30.0}, {EffectStat::STABILITY, -8.0} },
    true, false, false, "{kingdom} 遭遇罕见暴风雪，交通中断。");

    // ==================== 魔法灾害（显性，需魔法） ====================
    globalEventTemplates.emplace_back(300, "魔法风暴", EventType::MAGIC_DISASTER, 0.003, "domestic",
                                      std::vector<Condition> {},
    std::vector<Effect> { {EffectStat::STABILITY, -25.0}, {EffectStat::ARMY, -20.0},
        {EffectStat::MAGIC_AFFINITY, -0.1}
    },
    true, false, true, "{kingdom} 被狂暴的魔法风暴席卷，魔力失控！");
    globalEventTemplates.emplace_back(301, "魔力枯竭", EventType::MAGIC_DISASTER, 0.004, "domestic",
    std::vector<Condition> { {ConditionStat::HAS_TECH, "==", std::to_string(static_cast<int>(TechID::MAGIC_BASICS))} },
    std::vector<Effect> { {EffectStat::MAGIC_AFFINITY, -0.2}, {EffectStat::STABILITY, -10.0} },
    true, false, true, "{kingdom} 的魔力源泉枯竭，魔法师们陷入恐慌。");
    globalEventTemplates.emplace_back(302, "奥术爆炸", EventType::MAGIC_DISASTER, 0.002, "domestic",
    std::vector<Condition> { {ConditionStat::HAS_BUILDING, "==", std::to_string(static_cast<int>(BuildingType::ARCANE_LAB))} },
    std::vector<Effect> { {EffectStat::GOLD, -150.0}, {EffectStat::STABILITY, -20.0},
        {EffectStat::ARMY, -30.0}
    },
    true, false, true, "{kingdom} 的奥术实验室发生爆炸，损失惨重！");
}

// ---------- 国内显性事件处理 ----------
void process_domestic_events(Kingdom& k, int turn) {
    int triggered = 0;
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    for (EventTemplate& ev : globalEventTemplates) {
        if (triggered >= MAX_DOMESTIC_EVENTS_PER_TURN) break;
        if (ev.scope != "domestic") continue;
        if (!ev.record) continue;
        if (ev.requires_magic && !FeatureToggle::MAGIC) continue;
        if (event_on_cooldown(ev.id, k.id, turn, ev.cooldown)) continue;

        bool conditions_met = true;
        for (const Condition& cond : ev.conditions) {
            if (!evaluate_condition(k, cond)) {
                conditions_met = false;
                break;
            }
        }
        if (!conditions_met) continue;

        if (prob(global_rng) < ev.probability) {
            apply_effects(k, ev.effects);
            mark_event_triggered(ev.id, k.id, turn);
            std::string desc = ev.message;
            size_t pos = desc.find("{kingdom}");
            if (pos != std::string::npos) desc.replace(pos, 9, k.name);
            EventRecord event(turn, format_time(turn),
                              ev.type, k.id, desc, false);
            globalEventLog.push_back(event);
            std::cout << "[" << turn << "] " << desc << std::endl;
            triggered++;
        }
    }
}

// ---------- 隐性事件处理 ----------
void process_hidden_events(Kingdom& k, int turn) {
    int triggered = 0;
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    for (EventTemplate& ev : globalEventTemplates) {
        if (triggered >= MAX_HIDDEN_EVENTS_PER_TURN) break;
        if (ev.scope != "hidden") continue;
        if (ev.requires_magic && !FeatureToggle::MAGIC) continue;
        if (event_on_cooldown(ev.id, k.id, turn, ev.cooldown)) continue;

        bool conditions_met = true;
        for (const Condition& cond : ev.conditions) {
            if (!evaluate_condition(k, cond)) {
                conditions_met = false;
                break;
            }
        }
        if (!conditions_met) continue;

        if (prob(global_rng) < ev.probability) {
            apply_effects(k, ev.effects);
            mark_event_triggered(ev.id, k.id, turn);
            triggered++;
        }
    }
}

// ---------- 自然灾害处理 ----------
void apply_disasters(Kingdom& k, const WorldData& world, int turn) {
    int triggered = 0;
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    for (EventTemplate& ev : globalEventTemplates) {
        if (triggered >= MAX_DISASTERS_PER_TURN) break;
        if (ev.type != EventType::DISASTER) continue;
        if (ev.scope != "domestic") continue;
        if (ev.requires_magic && !FeatureToggle::MAGIC) continue;
        if (event_on_cooldown(ev.id, k.id, turn, ev.cooldown)) continue;

        double adjusted_prob = ev.probability;

        // 根据世界环境调整概率
        if (ev.name == "火山爆发") {
            bool has_volcanic = false;
            for (int idx : k.territory) {
                int x = idx % WORLD_WIDTH;
                int y = idx / WORLD_WIDTH;
                if (world.get(LAYER_VOLCANIC, x, y) > 0.0) {
                    has_volcanic = true;
                    break;
                }
            }
            if (!has_volcanic) continue;
            adjusted_prob *= 2.0;
        }
        else if (ev.name == "洪水") {
            bool has_flood_risk = false;
            for (int idx : k.territory) {
                int x = idx % WORLD_WIDTH;
                int y = idx / WORLD_WIDTH;
                if (world.get(LAYER_ELEVATION, x, y) < 10.0 ||
                        world.get(LAYER_PRECIP, x, y) > 1500.0) {
                    has_flood_risk = true;
                    break;
                }
            }
            if (!has_flood_risk) continue;
            adjusted_prob *= 1.5;
        }
        else if (ev.name == "干旱") {
            bool has_drought_risk = false;
            for (int idx : k.territory) {
                int x = idx % WORLD_WIDTH;
                int y = idx / WORLD_WIDTH;
                if (world.get(LAYER_ARIDITY, x, y) > 2.0) {
                    has_drought_risk = true;
                    break;
                }
            }
            if (!has_drought_risk) continue;
            adjusted_prob *= 1.8;
        }

        bool conditions_met = true;
        for (const Condition& cond : ev.conditions) {
            if (!evaluate_condition(k, cond)) {
                conditions_met = false;
                break;
            }
        }
        if (!conditions_met) continue;

        if (prob(global_rng) < adjusted_prob) {
            apply_effects(k, ev.effects);
            mark_event_triggered(ev.id, k.id, turn);
            std::string desc = ev.message;
            size_t pos = desc.find("{kingdom}");
            if (pos != std::string::npos) desc.replace(pos, 9, k.name);
            EventRecord event(turn, format_time(turn),
                              ev.type, k.id, desc, false);
            globalEventLog.push_back(event);
            std::cout << "[" << turn << "] " << desc << std::endl;
            triggered++;
        }
    }
}

// ---------- 魔法灾害处理 ----------
void apply_magic_disasters(Kingdom& k, int turn) {
    if (!FeatureToggle::MAGIC || !FeatureToggle::FANTASY_DISASTERS) return;

    int triggered = 0;
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    for (EventTemplate& ev : globalEventTemplates) {
        if (triggered >= MAX_DISASTERS_PER_TURN) break;
        if (ev.type != EventType::MAGIC_DISASTER) continue;
        if (ev.scope != "domestic") continue;
        if (ev.requires_magic && !FeatureToggle::MAGIC) continue;
        if (event_on_cooldown(ev.id, k.id, turn, ev.cooldown)) continue;

        bool conditions_met = true;
        for (const Condition& cond : ev.conditions) {
            if (!evaluate_condition(k, cond)) {
                conditions_met = false;
                break;
            }
        }
        if (!conditions_met) continue;

        if (prob(global_rng) < ev.probability) {
            apply_effects(k, ev.effects);
            mark_event_triggered(ev.id, k.id, turn);
            std::string desc = ev.message;
            size_t pos = desc.find("{kingdom}");
            if (pos != std::string::npos) desc.replace(pos, 9, k.name);
            EventRecord event(turn, format_time(turn),
                              ev.type, k.id, desc, false);
            globalEventLog.push_back(event);
            std::cout << "[" << turn << "] " << desc << std::endl;
            triggered++;
        }
    }
}

// ============================================================
// 第 12 部分结束
// ============================================================

// ============================================================
// 第 13 部分：文化扩散与控制度恢复
// ============================================================

// ---------- 文化扩散（每5回合） ----------
void spread_culture(std::vector<Kingdom>& kingdoms, const WorldData& world, int turn) {
    if (turn % CULTURE_SPREAD_INTERVAL != 0) return;

    constexpr double INTERNAL_COHESION_RATE = 0.02;
    constexpr double FOREIGN_INFLUENCE_RATE = 0.05;
    constexpr double REVERSE_INFLUENCE_RATE = 0.025;

    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};

    // 第一步：内部文化统一
    for (auto& k : kingdoms) {
        if (!k.alive) continue;
        for (int idx : k.territory) {
            int x = idx % WORLD_WIDTH;
            int y = idx / WORLD_WIDTH;
            if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT) continue;
            auto& cell_culture = cultureMap[y][x];
            for (int c = 0; c < CULTURE_DIM; ++c) {
                cell_culture[c] += (k.culture[c] - cell_culture[c]) * INTERNAL_COHESION_RATE;
            }
        }
    }

    // 第二步：边境文化渗透
    for (auto& k : kingdoms) {
        if (!k.alive) continue;
        for (int idx : k.territory) {
            int x = idx % WORLD_WIDTH;
            int y = idx / WORLD_WIDTH;
            if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT) continue;

            for (int d = 0; d < 4; ++d) {
                int nx = x + dx[d];
                int ny = y + dy[d];
                if (nx < 0 || nx >= WORLD_WIDTH || ny < 0 || ny >= WORLD_HEIGHT) continue;
                if (!is_land(world, nx, ny)) continue;

                int neighbor_owner = ownerMap[ny][nx];
                if (neighbor_owner == k.id) continue;

                // 邻格文化受当前王国影响
                auto& neighbor_culture = cultureMap[ny][nx];
                for (int c = 0; c < CULTURE_DIM; ++c) {
                    neighbor_culture[c] += (k.culture[c] - neighbor_culture[c]) * FOREIGN_INFLUENCE_RATE;
                }

                // 若邻格属于其他王国，反向影响
                if (neighbor_owner >= 0 && neighbor_owner < static_cast<int>(kingdoms.size())) {
                    Kingdom& neighbor_kingdom = kingdoms[neighbor_owner];
                    if (neighbor_kingdom.alive) {
                        auto& cur_culture = cultureMap[y][x];
                        for (int c = 0; c < CULTURE_DIM; ++c) {
                            cur_culture[c] += (neighbor_kingdom.culture[c] - cur_culture[c]) * REVERSE_INFLUENCE_RATE;
                        }
                    }
                }
            }
        }
    }
}

// ---------- 控制度恢复（每回合） ----------
void recover_control(std::vector<Kingdom>& kingdoms, int turn) {
    // 王国领土格控制度恢复
    for (auto& k : kingdoms) {
        if (!k.alive) continue;
        for (int idx : k.territory) {
            int x = idx % WORLD_WIDTH;
            int y = idx / WORLD_WIDTH;
            if (x < 0 || x >= WORLD_WIDTH || y < 0 || y >= WORLD_HEIGHT) continue;
            if (ownerMap[y][x] == k.id && controlMap[y][x] < 1.0) {
                controlMap[y][x] = std::min(1.0, controlMap[y][x] + CONTROL_RECOVERY_RATE);
            }
        }
    }

    // 无主格控制度衰减
    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        for (int x = 0; x < WORLD_WIDTH; ++x) {
            if (ownerMap[y][x] == -1 && controlMap[y][x] > 0.0) {
                controlMap[y][x] = std::max(0.0, controlMap[y][x] - CONTROL_RECOVERY_RATE);
            }
        }
    }
}

// ============================================================
// 第 13 部分结束
// ============================================================

// ============================================================
// 文明模拟器 - 第 14 部分：输出系统
// ============================================================

// ---------- 辅助：时间格式化 ----------
/*std::string format_time(int turn) {
    int year = turn / 4;
    int month = (turn % 4) * 3 + 1;
    std::ostringstream oss;
    oss << "第" << year << "年" << month << "月";
    return oss.str();
}*/
// ============================================================
// 统一时间格式化函数（替代原 format_time）
// 用法：
//   format_time(turn)                -> "第X年"
//   format_time(turn, month, day)    -> "第X年(Y月Z日)"
// ============================================================
std::string format_time(int turn, int month, int day) {
    int total_months = turn * 3;          // 每回合3个月
    int year = total_months / 12;         // 整除，向下取整
    std::ostringstream oss;
    oss << "第" << year << "年";
    // 若月份和日期在有效范围内，追加括号内的月日
    if (month >= 1 && month <= 12 && day >= 1 && day <= 31) {
        oss << "(" << month << "月" << day << "日)";
    }
    return oss.str();
}

// ---------- 辅助：种族名称 ----------
std::string race_name(Race race) {
    switch (race) {
    case Race::HUMAN:
        return "人类";
    case Race::DWARF:
        return "矮人";
    case Race::ELF:
        return "精灵";
    case Race::ORC:
        return "兽人";
    case Race::HALFLING:
        return "半身人";
    default:
        return "未知";
    }
}

// ---------- 辅助：建筑名称 ----------
std::string building_name(BuildingType type) {
    for (const auto& bd : globalBuildingData) {
        if (bd.type == type) return bd.name;
    }
    return "未知建筑";
}

// ---------- 评定函数 ----------
double economy_rating(const Kingdom& k) {
    int pop = k.total_population();
    if (pop <= 0) return 0.0;
    return std::min(1.0, (k.gold / pop) / 10.0);
}
double tech_rating(const Kingdom& k) {
    return std::min(1.0, k.effective_tech() / MAX_TECH_LEVEL);
}
double military_rating(const Kingdom& k) {
    int pop = k.total_population();
    if (pop <= 0) return 0.0;
    return std::min(1.0, (k.army / pop) / 0.1);
}
double political_rating(const Kingdom& k) {
    return k.stability / MAX_STABILITY;
}

// ---------- 写入单个王国文件 ----------
void write_kingdom_file(const Kingdom& k, const std::string& dir) {
    std::string filename = dir + "/kingdom_" + std::to_string(k.id) + ".txt";
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "无法创建文件: " << filename << std::endl;
        return;
    }

    out << "========================================\n";
    out << "王国 ID: " << k.id << "\n";
    out << "王国名称: " << k.name << "\n";
    out << "种族: " << race_name(k.race) << "\n";
    out << "国王: " << k.king_name << " (年龄 " << k.king_age << ")\n";
    out << "建立回合: " << k.establish_turn << " (" << format_time(k.establish_turn) << ")\n";
    if (!k.alive) out << "灭亡回合: " << k.death_turn << " (" << format_time(k.death_turn) << ")\n";
    out << "----------------------------------------\n";
    out << "国王性格: 侵略性=" << k.aggression << ", 贪婪=" << k.greed
        << ", 谨慎=" << k.caution << ", 勤劳=" << k.industriousness
        << ", 外交=" << k.diplomacy << "\n";
    out << "文化向量: [";
    for (int c = 0; c < CULTURE_DIM; ++c) {
        if (c) out << ", ";
        out << k.culture[c];
    }
    out << "]\n";
    out << "科技评定: " << std::fixed << std::setprecision(2) << tech_rating(k) << "\n";
    out << "经济评定: " << economy_rating(k) << "\n";
    out << "政治评定: " << political_rating(k) << "\n";
    out << "军事评定: " << military_rating(k) << "\n";
    out << "文明等级: " << k.civilization_level << "\n";
    out << "资源: 金币=" << k.gold << ", 食物=" << k.food << ", 军队=" << k.army
        << ", 稳定度=" << k.stability << ", 税率=" << k.tax_rate
        << ", 科技等级=" << k.tech_level << "\n";
    out << "----------------------------------------\n";
    out << "领土面积: " << k.territory.size() << " 格 ("
        << k.territory.size() * AREA_PER_TILE_KM2 << " km2)\n";
    out << "人口总数: " << k.total_population() << "\n";
    out << "当前交战: ";
    bool first = true;
    for (size_t i = 0; i < warMatrix.size(); ++i) {
        if (i == static_cast<size_t>(k.id)) continue;
        if (warMatrix[k.id][i] != 0 || warMatrix[i][k.id] != 0) {
            if (!first) out << ", ";
            out << i;
            first = false;
        }
    }
    if (first) out << "无";
    out << "\n盟友: ";
    first = true;
    for (int ally : k.allies) {
        if (!first) out << ", ";
        out << ally;
        first = false;
    }
    if (first) out << "无";
    out << "\n历史战争总数: " << k.total_wars << "\n";
    out << "----------------------------------------\n";
    out << "居住区数量: " << k.settlements.size() << "\n";
    for (const auto& s : k.settlements) {
        out << "  " << s.name << " (" << s.pos.x << ", " << s.pos.y << ")";
        if (s.is_capital) out << " [首都]";
        out << "\n    人口: " << s.population << ", 政治权重: " << s.political_weight
            << ", 军事权重: " << s.military_weight << "\n";
        std::map<BuildingType, int> bcount;
        for (BuildingType bt : s.buildings) bcount[bt]++;
        if (!bcount.empty()) {
            out << "    建筑: ";
            first = true;
            for (const auto& e : bcount) {
                if (!first) out << ", ";
                out << building_name(e.first) << "×" << e.second;
                first = false;
            }
            out << "\n";
        } else out << "    建筑: 无\n";
    }
    out << "----------------------------------------\n";
    out << "国王历史:\n";
    for (const auto& r : k.king_history) {
        out << "  " << r.name << " (即位: " << r.reign_start_turn;
        if (r.reign_end_turn != -1) out << ", 退位: " << r.reign_end_turn;
        else out << ", 在位中";
        out << ")\n";
    }
    out << "========================================\n";
    out.close();
}

// ---------- 灭亡王国汇总 ----------
void write_fallen_kingdoms_file(const std::vector<Kingdom>& all_kingdoms, const std::string& dir) {
    std::string filename = dir + "/fallen_kingdoms.txt";
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "无法创建文件: " << filename << std::endl;
        return;
    }

    out << "========================================\n[灭亡国家汇总]\n========================================\n";
    int count = 0;
    for (const auto& k : all_kingdoms) {
        if (k.alive) continue;
        count++;
        out << count << ". 王国名称: " << k.name << "\n";
        out << "   种族: " << race_name(k.race) << "\n";
        out << "   建立回合: " << k.establish_turn << " (" << format_time(k.establish_turn) << ")\n";
        out << "   灭亡回合: " << k.death_turn << " (" << format_time(k.death_turn) << ")\n";
        out << "   灭亡原因: ";
        if (k.territory.empty()) out << "领土丧失殆尽";
        else if (k.total_population() == 0) out << "人口消亡";
        else out << "政权崩溃";
        out << "\n";
        out << "   领土峰值: " << k.max_territory_size << " 格 ("
            << k.max_territory_size * AREA_PER_TILE_KM2 << " km2)\n";
        out << "   人口峰值: " << k.max_population << "\n";
        out << "   国王序列:\n";
        for (const auto& r : k.king_history) {
            out << "       " << r.name << " (" << r.reign_start_turn;
            if (r.reign_end_turn != -1) out << "-" << r.reign_end_turn;
            else out << "-灭亡";
            out << ")\n";
        }
        out << "   备注: ";
        if (!k.allies.empty()) {
            out << "曾与王国 ";
            for (int a : k.allies) out << a << " ";
            out << "结盟。";
        } else out << "无联盟记录。";
        out << "\n----------------------------------------\n";
    }
    if (count == 0) out << "暂无灭亡王国。\n";
    out << "========================================\n";
    out.close();
}

// ---------- 国际事件文件 ----------
void write_international_events_file(const std::vector<EventRecord>& events, const std::string& dir) {
    std::string filename = dir + "/international.txt";
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "无法创建文件: " << filename << std::endl;
        return;
    }
    out << "========================================\n[国际事件记录]\n========================================\n";
    for (const auto& ev : events) {
        if (!ev.is_international) continue;
        out << "[" << ev.turn << "] " << ev.timestamp << " 类型: ";
        switch (ev.type) {
        case EventType::WAR:
            out << "战争";
            break;
        case EventType::INTERNATIONAL:
            out << "外交";
            break;
        default:
            out << "其他";
        }
        out << "\n  参与王国: ";
        for (size_t i = 0; i < ev.involved_kingdom_ids.size(); ++i) {
            if (i) out << ", ";
            out << ev.involved_kingdom_ids[i];
        }
        out << "\n  描述: " << ev.description << "\n----------------------------------------\n";
    }
    out << "\n[世界稳定度评估]\n";
    out << "（详见文件末尾）\n";
    out << "========================================\n";
    out.close();
}

// ---------- 道路文件 ----------
void write_roads_file(const std::vector<Kingdom>& kingdoms, const std::string& dir) {
    std::string filename = dir + "/roads.txt";
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "无法创建文件: " << filename << std::endl;
        return;
    }
    out << "========================================\n[道路系统]\n========================================\n";
    int total = 0;
    std::vector<int> cnt(kingdoms.size(), 0);
    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        for (int x = 0; x < WORLD_WIDTH; ++x) {
            if (roadMap[y][x] != 0) {
                total++;
                out << "道路格: (" << x << ", " << y << ") 归属: ";
                if (roadMap[y][x] == -1) out << "跨国共有\n";
                else {
                    out << "王国 " << roadMap[y][x];
                    if (roadMap[y][x] >= 0 && roadMap[y][x] < static_cast<int>(kingdoms.size()))
                        cnt[roadMap[y][x]]++;
                    out << "\n";
                }
            }
        }
    }
    out << "总道路里程: " << total << " 格\n";
    for (size_t i = 0; i < kingdoms.size(); ++i) {
        if (kingdoms[i].alive)
            out << "  王国 " << i << " (" << kingdoms[i].name << "): "
                << cnt[i] << " 格，维护费 " << cnt[i] * ROAD_MAINTENANCE_PER_TILE << " 金币/回合\n";
    }
    out << "========================================\n";
    out.close();
}

// ---------- 领土文件 ----------
void write_territory_file(const std::vector<Kingdom>& kingdoms, const std::string& dir) {
    std::string filename = dir + "/territory.txt";
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "无法创建文件: " << filename << std::endl;
        return;
    }
    out << "========================================\n[领土分布]\n========================================\n";
    for (const auto& k : kingdoms) {
        if (!k.alive) continue;
        out << "王国 " << k.id << " (" << k.name << "):\n";
        out << "  领土格数: " << k.territory.size() << "\n  领土坐标: ";
        bool first = true;
        for (int idx : k.territory) {
            int x = idx % WORLD_WIDTH, y = idx / WORLD_WIDTH;
            if (!first) out << ", ";
            out << "(" << x << "," << y << ")";
            first = false;
        }
        out << "\n  居住区:\n";
        for (const auto& s : k.settlements) {
            out << "    " << s.name << " (" << s.pos.x << ", " << s.pos.y << ")";
            if (s.is_capital) out << " [首都]";
            out << "\n";
        }
        out << "----------------------------------------\n";
    }
    out << "\n[命名地理区域]\n（需额外算法识别，此版本省略）\n";
    out << "========================================\n";
    out.close();
}

// ---------- 历史事件文件 ----------
void write_history_file(const std::vector<EventRecord>& events, const std::string& dir) {
    std::string filename = dir + "/history.txt";
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "无法创建文件: " << filename << std::endl;
        return;
    }
    out << "========================================\n[完整历史事件日志]\n========================================\n";
    for (const auto& ev : events) {
        out << "[" << ev.turn << "] " << ev.timestamp << " 类型: ";
        switch (ev.type) {
        case EventType::POLITICAL:
            out << "政治";
            break;
        case EventType::ECONOMIC:
            out << "经济";
            break;
        case EventType::SOCIAL:
            out << "社会";
            break;
        case EventType::DISASTER:
            out << "灾害";
            break;
        case EventType::MAGIC_DISASTER:
            out << "魔法灾害";
            break;
        case EventType::INTERNATIONAL:
            out << "国际";
            break;
        case EventType::WAR:
            out << "战争";
            break;
        case EventType::FOUNDATION:
            out << "建国";
            break;
        case EventType::EXTINCTION:
            out << "灭亡";
            break;
        case EventType::ROAD:
            out << "道路";
            break;
        case EventType::CULTURAL:
            out << "文化";
            break;
        case EventType::TECHNOLOGICAL:
            out << "科技";
            break;
        case EventType::RELIGIOUS:
            out << "宗教";
            break;
        case EventType::MIGRATION:
            out << "移民";
            break;
        default:
            out << "其他";
        }
        out << "\n  主要王国: " << ev.primary_kingdom_id << "\n";
        if (!ev.involved_kingdom_ids.empty()) {
            out << "  涉及王国: ";
            for (size_t i = 0; i < ev.involved_kingdom_ids.size(); ++i) {
                if (i) out << ", ";
                out << ev.involved_kingdom_ids[i];
            }
            out << "\n";
        }
        out << "  描述: " << ev.description << "\n----------------------------------------\n";
    }
    out << "========================================\n";
    out.close();
}

// ---------- 世界稳定度评估（追加到国际事件文件） ----------
void append_world_stability_assessment(const std::vector<Kingdom>& kingdoms,
                                       const std::string& dir) {
    std::string filename = dir + "/international.txt";
    std::ofstream out(filename, std::ios::app);
    if (!out) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }

    double avg_stab = 0.0;
    int alive = 0, war_count = 0;
    for (const auto& k : kingdoms) {
        if (!k.alive) continue;
        alive++;
        avg_stab += k.stability;
        for (size_t j = 0; j < warMatrix.size(); ++j) {
            if (j == static_cast<size_t>(k.id)) continue;
            if (warMatrix[k.id][j] != 0) war_count++;
        }
    }
    if (alive > 0) avg_stab /= alive;
    double war_density = alive > 0 ? static_cast<double>(war_count) / alive : 0.0;
    double var = 0.0;
    if (alive > 1) {
        double mean_civ = 0.0;
        for (const auto& k : kingdoms) if (k.alive) mean_civ += k.civilization_level;
        mean_civ /= alive;
        for (const auto& k : kingdoms) {
            if (k.alive) {
                double d = k.civilization_level - mean_civ;
                var += d * d;
            }
        }
        var /= alive;
    }
    out << "\n[世界稳定度评估]\n";
    out << "存活王国数: " << alive << "\n";
    out << "平均稳定度: " << std::fixed << std::setprecision(2) << avg_stab << "\n";
    out << "战争密度: " << war_density << "\n";
    out << "文明等级方差: " << var << "\n";
    if (avg_stab >= 70.0 && war_density < 0.2) out << "整体评价: 和平繁荣\n";
    else if (avg_stab < 40.0 || war_density > 0.8) out << "整体评价: 动荡不安\n";
    else out << "整体评价: 相对平衡\n";
    out << "========================================\n";
    out.close();
}

// ============================================================
// 第 14 部分结束
// ============================================================

// ============================================================
// 第 15 部分：主循环与辅助函数
// ============================================================

// ---------- 军队招募 ----------
void recruit_army(Kingdom& k, int turn) {
    double pop = static_cast<double>(k.total_population());
    double max_army = pop * ARMY_POP_RATIO_LIMIT + ARMY_BASE_LIMIT;
    if (k.army >= max_army) return;

    // 计算兵力缺口比例
    double shortage = (max_army - k.army) / max_army;
    double willingness = k.industriousness * shortage;
    if (willingness < 0.1) return;

    // ===== [修改] 基础招募数量至少为 1，且不超过人口 2% =====
    // 原逻辑：max_recruitable = min(max_army - k.army, pop * 0.05)
    // 新逻辑：至少招募 1 人，同时限制为人口 2%（避免人口锐减）
    double base_recruit = std::max(1.0, pop * 0.02);
    double max_recruitable = std::min(max_army - k.army, base_recruit);
    
    // 金币限制：可招募数量受金币一半的限制，但至少 1 人
    double gold_limit = k.gold / 2.0;
    double possible = std::min(max_recruitable, std::max(1.0, gold_limit));
    if (possible < 1.0) return;

    // 随机抖动和意愿调整
    std::uniform_real_distribution<double> jitter(0.8, 1.2);
    double recruited = possible * willingness * jitter(global_rng);
    recruited = std::max(1.0, std::min(recruited, possible));
    int count = static_cast<int>(std::floor(recruited));
    if (count <= 0) count = 1;

    // 确保金币足够支付招募费用（每兵 2 金币）
    double gold_cost = count * 2.0;
    if (k.gold < gold_cost) {
        count = static_cast<int>(k.gold / 2.0);
        if (count <= 0) return;
    }

    // 扣除金币，增加军队
    k.gold -= count * 2.0;
    k.army += count;

    // 从人口中扣除征兵损耗（限制不超过总人口的 10%，防止人口崩溃）
    int total_pop = k.total_population();
    if (total_pop > 0 && !k.settlements.empty()) {
        double ratio = std::min(0.1, static_cast<double>(count) / total_pop);
        for (auto& s : k.settlements) {
            int loss = static_cast<int>(std::llround(s.population * ratio));
            s.population = std::max(Balance::MIN_SETTLEMENT_POP, s.population - loss);
        }
    }

    // 确保军队非负
    k.army = std::max(0.0, k.army);
}
// ---------- 冷却更新 ----------
void update_cooldowns(Kingdom& k) {
    if (k.peace_timer > 0) k.peace_timer--;
    // 战争持续由战争系统增加，此处不重复
}

// ---------- 灭绝检查 ----------
void check_extinction(Kingdom& k, int turn) {
    if (!k.alive) return;
    bool extinct = k.territory.empty() || k.total_population() <= 0 ||
                   (k.stability <= 0.0 && k.army <= 0.0);
    if (!extinct) return;

    k.alive = false;
    k.death_turn = turn;
    std::ostringstream oss;
    oss << k.name << " 灭亡！";
    EventRecord event(turn, format_time(turn), EventType::EXTINCTION, k.id, oss.str(), false);
    globalEventLog.push_back(event);
    std::cout << "[" << turn << "] " << oss.str() << std::endl;

    for (int idx : k.territory) {
        int x = idx % WORLD_WIDTH;
        int y = idx / WORLD_WIDTH;
        ownerMap[y][x] = -1;
        controlMap[y][x] = 0.0;
    }
    k.territory.clear();
    k.settlements.clear();
}

// ---------- 随机打乱王国行动顺序 ----------
std::vector<int> shuffle_kingdoms(const std::vector<Kingdom>& kingdoms) {
    std::vector<int> indices;
    for (size_t i = 0; i < kingdoms.size(); ++i) if (kingdoms[i].alive) indices.push_back(static_cast<int>(i));
    std::shuffle(indices.begin(), indices.end(), global_rng);
    return indices;
}

// ---------- 遗迹生成（简化实现） ----------
std::vector<Ruin> global_ruins;
void generate_ruins(const WorldData& world, int count) {
    if (!FeatureToggle::RUINS) return;
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    for (int i = 0; i < count; ++i) {
        int x = std::uniform_int_distribution<int>(0, WORLD_WIDTH - 1)(global_rng);
        int y = std::uniform_int_distribution<int>(0, WORLD_HEIGHT - 1)(global_rng);
        if (is_habitable(world, x, y) && ownerMap[y][x] == -1) {
            double danger = prob(global_rng);
            double wealth = 50.0 + prob(global_rng) * 200.0;
            global_ruins.emplace_back(x, y, danger, wealth, "远古遗迹");
        }
    }
}

// ---------- 遗迹探索（简化实现） ----------
void explore_ruins(Kingdom& k, int turn) {
    if (!FeatureToggle::RUINS) return;
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    for (size_t i = 0; i < global_ruins.size(); ++i) {
        Ruin& r = global_ruins[i];
        if (ownerMap[r.y][r.x] != k.id) continue;
        if (prob(global_rng) < 0.05) { // 每回合5%探索概率
            double success = 1.0 - r.danger;
            if (prob(global_rng) < success) {
                k.gold += r.wealth;
                if (FeatureToggle::MAGIC) k.cached_magic_affinity += 0.05;
                std::ostringstream oss;
                oss << k.name << " 探索遗迹，获得财富 " << r.wealth << " 金币！";
                EventRecord event(turn, format_time(turn), EventType::CULTURAL, k.id, oss.str(), false, r.x, r.y);
                globalEventLog.push_back(event);
                std::cout << "[" << turn << "] " << oss.str() << std::endl;
            }
            global_ruins.erase(global_ruins.begin() + i);
            break;
        }
    }
}

// ---------- 科技传播调用 ----------
void propagate_tech_globally(std::vector<Kingdom>& kingdoms, int turn) {
    for (size_t i = 0; i < kingdoms.size(); ++i) {
        for (size_t j = i + 1; j < kingdoms.size(); ++j) {
            if (kingdoms[i].alive && kingdoms[j].alive) {
                transfer_tech(kingdoms[i], kingdoms[j], turn);
                transfer_tech(kingdoms[j], kingdoms[i], turn);
            }
        }
    }
}

// ---------- 主模拟循环（基于回合，每回合3个月） ----------
void run_simulation(const std::string& world_file, const std::string& output_dir) {
    // ---------- 1. 初始化世界与全局数据 ----------
    WorldData world;
    if (!load_world(world_file, world)) {
        std::cerr << "世界加载失败，模拟终止。" << std::endl;
        return;
    }
    init_global_maps(WORLD_WIDTH, WORLD_HEIGHT);
    init_tech_pool();
    init_event_templates();

    std::vector<Kingdom> kingdoms;
    initialize_first_kingdom(world, kingdoms);
    generate_ruins(world, 20);

    // ---------- 2. 时间与事件控制变量 ----------
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    std::uniform_int_distribution<int> month_dist(1, 12);
    std::uniform_int_distribution<int> day_dist(1, 28);  // 所有月份最多28天，避免非法日期

    // 事件间隔（以月为单位），每次触发后重新随机生成
    int months_until_next_event = 0;
    bool event_this_cycle = false;
    int global_events_this_year = 0;   // 当年已触发事件数（用于“平安无事”判定）

    // 季节判定（0=冬,1=春,2=夏,3=秋）
    auto get_season = [](int month) -> int {
        if (month >= 11 || month <= 1) return 0; // 冬季（12,1,2）
        if (month >= 2 && month <= 4) return 1;  // 春季（3,4,5）
        if (month >= 5 && month <= 7) return 2;  // 夏季（6,7,8）
        return 3; // 秋季（9,10,11）
    };

    // ---------- 3. 主循环：按回合推进 ----------
    for (int turn = 0; turn < MAX_TURNS; ++turn) {
        // 计算当前月份（0~11）
        int total_months = turn * 3;
        int current_month = total_months % 12;
        int current_year = total_months / 12;

        // ---------- 3.1 王国行动（每回合必执行） ----------
        // 冷却更新
        for (auto& k : kingdoms) if (k.alive) update_cooldowns(k);

        // 打乱行动顺序
        std::vector<int> order = shuffle_kingdoms(kingdoms);

        // 每个王国行动（使用 turn 作为当前回合）
        for (int kid : order) {
            Kingdom& k = kingdoms[kid];
            if (!k.alive) continue;

			k.king_age++;

            k.update_cached_effects();
            collect_resources(k, world, turn);
            grow_population(k, turn);
            manage_settlements(k, world, turn);
            expand_territory(k, world, turn);
            build_new_settlement(k, world, turn);
            recruit_army(k, turn);
            update_technology(k, turn);
            update_stability(k, world, turn);
            handle_rebellion(k, turn);
            check_king_death(k, turn);
            check_revolution(k, turn);
            update_civilization_level(k, turn);
            build_roads(k, world, turn);
            maintain_roads(k, turn);
            explore_ruins(k, turn);
            process_domestic_events(k, turn);
            process_hidden_events(k, turn);
            apply_disasters(k, world, turn);
            apply_magic_disasters(k, turn);
            check_extinction(k, turn);
        }

        // ---------- 3.2 全局处理（每回合必执行） ----------
        if (turn % CULTURE_SPREAD_INTERVAL == 0) {
            spread_culture(kingdoms, world, turn);
        }
        recover_control(kingdoms, turn);
        update_alliances(kingdoms, world, turn);
        process_international_events(kingdoms, turn);
        propagate_tech_globally(kingdoms, turn);
        ally_join_wars(kingdoms, turn);
        process_wars(kingdoms, world, turn);
        update_diplomacy(kingdoms, world, turn);
        check_world_war(kingdoms, turn);
        check_for_new_kingdoms(turn, world, kingdoms);

        // ---------- 3.3 年度隐性调整（每年1月，即 current_month == 0） ----------
        if (current_month == 0) {
            for (auto& k : kingdoms) {
                if (!k.alive) continue;
                // 文化缓慢自然演化
                for (int c = 0; c < CULTURE_DIM; ++c) {
                    k.culture[c] += (0.5 - k.culture[c]) * 0.01;
                }
                // 人口自然微调
                double pop_adjust = 0.005 * (k.stability / 100.0 - 0.5);
                for (auto& s : k.settlements) {
                    s.population += static_cast<int>(s.population * pop_adjust);
                    s.population = std::max(Balance::MIN_SETTLEMENT_POP, s.population);
                }
                // 稳定度缓慢向60靠拢
                k.stability += (60.0 - k.stability) * 0.005;
            }
            // 重置年度事件计数器（仅在1月重置，避免跨年累积）
            global_events_this_year = 0;
        }

        // ---------- 3.4 事件触发（独立于王国行动，基于随机间隔） ----------
        // 每回合递减事件间隔（每回合=3个月）
        if (months_until_next_event <= 0) {
            // 生成新的间隔（1~12个月）
            months_until_next_event = std::uniform_int_distribution<int>(1, 12)(global_rng);
            event_this_cycle = false;
        }
        months_until_next_event -= 3;  // 每回合减少3个月

        // 当间隔归零且本周期尚未触发事件时，触发事件
        if (months_until_next_event <= 0 && !event_this_cycle) {
            event_this_cycle = true;
            global_events_this_year++;

            // 全年无事件判定（15%概率，且本年度事件数不超过2）
            bool year_has_events = true;
            if (global_events_this_year <= 2 && prob(global_rng) < 0.15) {
                year_has_events = false;
                std::cout << "[" << format_time(turn) << "] 这一年平安无事，风调雨顺。" << std::endl;
            }

            if (year_has_events) {
                // 生成随机月日（用于精确日期的事件）
                int event_month = month_dist(global_rng);
                int event_day = day_dist(global_rng);
                std::string event_date = format_time(turn, event_month, event_day);

                // 季节修正（用于事件概率调整）
                int season = get_season(current_month);
                double season_mod = 1.0;
                switch (season) {
                    case 0: season_mod = 1.2; break;  // 冬
                    case 1: season_mod = 1.5; break;  // 春
                    case 2: season_mod = 1.3; break;  // 夏
                    case 3: season_mod = 1.1; break;  // 秋
                }

                // ---------- 遍历所有王国生成“额外事件”（原 run_simulation 中的事件） ----------
                for (auto& k : kingdoms) {
                    if (!k.alive) continue;
                    if (prob(global_rng) > 0.3) continue;   // 每王国约30%概率触发

                    double roll = prob(global_rng) * season_mod;

                    // 不稳定 -> 叛乱/内战（冬季更易）
                    if (k.stability < 30.0 && roll < 0.4) {
                        if (season == 0) roll *= 1.3;
                        handle_rebellion(k, turn);  // 该函数内会记录事件
                    }
                    // 军力强+好战 -> 扩张/战争（夏季更易）
                    else if (k.army > 200 && k.aggression > 0.7 && roll < 0.6) {
                        if (season == 2) roll *= 1.2;
                        // 战争由外交系统自动处理，这里仅记录提示（可选）
                        // std::cout << "[" << event_date << "] " << k.name << " 积极备战。" << std::endl;
                    }
                    // 富裕 -> 经济繁荣（春季更易）
                    else if (k.gold > 1000 && roll < 0.5) {
                        if (season == 1) roll *= 1.3;
                        k.gold += 50 * (1.0 + 0.2 * (season == 1 ? 1 : 0));
                        std::cout << "[" << event_date << "] " << k.name << " 贸易繁荣，国库增加。" << std::endl;
                    }
                    // 其他事件（秋季丰收等）
                    else if (roll < 0.3) {
                        if (season == 3) {
                            k.food += 100;
                            std::cout << "[" << event_date << "] " << k.name << " 秋收大丰收，粮仓充实！" << std::endl;
                        } else {
                            for (int c = 0; c < CULTURE_DIM; ++c) {
                                k.culture[c] += (0.5 - k.culture[c]) * 0.05;
                            }
                            std::cout << "[" << event_date << "] " << k.name << " 文化繁荣，艺术兴盛。" << std::endl;
                        }
                    }
                }

                // ---------- 全球性事件（每年5%概率） ----------
                if (prob(global_rng) < 0.05) {
                    std::ostringstream oss;
                    oss << "[" << event_date << "] 全球性事件爆发！";
                    if (prob(global_rng) < 0.3) {
                        // 全球瘟疫
                        for (auto& k : kingdoms) {
                            if (!k.alive) continue;
                            for (auto& s : k.settlements) {
                                int reduction = static_cast<int>(s.population * 0.1);
                                s.population = std::max(Balance::MIN_SETTLEMENT_POP, s.population - reduction);
                            }
                            k.stability -= 15.0;
                            if (k.stability < 0) k.stability = 0;
                            k.food -= k.food * 0.2;
                            if (k.food < 0) k.food = 0;
                            k.army -= k.army * 0.05;
                            if (k.army < 0) k.army = 0;
                        }
                        oss << "全球瘟疫肆虐，人口锐减！";
                    } else if (prob(global_rng) < 0.6) {
                        for (auto& k : kingdoms) {
                            if (!k.alive) continue;
                            k.gold *= 1.1;
                        }
                        oss << "全球贸易路线打通，各国财富增长！";
                    } else {
                        oss << "世界局势紧张，大战一触即发！";
                        for (auto& k : kingdoms) {
                            if (!k.alive) continue;
                            k.aggression = std::min(1.0, k.aggression + 0.1);
                        }
                    }
                    EventRecord event(turn, event_date, EventType::INTERNATIONAL, -1, oss.str(), true);
                    globalEventLog.push_back(event);
                    std::cout << "[" << event_date << "] " << oss.str() << std::endl;
                }

                // ---------- 邻国波及事件（每年20%概率） ----------
                if (prob(global_rng) < 0.2 && kingdoms.size() > 1) {
                    std::uniform_int_distribution<size_t> kdist(0, kingdoms.size() - 1);
                    size_t src_idx = kdist(global_rng);
                    if (kingdoms[src_idx].alive) {
                        Kingdom& src = kingdoms[src_idx];
                        for (auto& dst : kingdoms) {
                            if (!dst.alive || dst.id == src.id) continue;
                            if (kingdoms_share_border(src, dst)) {
                                if (prob(global_rng) < 0.5) {
                                    for (int c = 0; c < CULTURE_DIM; ++c) {
                                        dst.culture[c] += (src.culture[c] - dst.culture[c]) * 0.05;
                                    }
                                    std::cout << "[" << event_date << "] " << src.name
                                              << " 的文化影响了邻国 " << dst.name << std::endl;
                                } else {
                                    dst.army = std::max(0.0, dst.army - 5.0);
                                    src.army = std::max(0.0, src.army - 5.0);
                                    std::cout << "[" << event_date << "] " << src.name
                                              << " 与 " << dst.name << " 发生边境摩擦。" << std::endl;
                                }
                                break;
                            }
                        }
                    }
                }
            } // end if (year_has_events)
        } // end if (months_until_next_event <= 0 && !event_this_cycle)

        // ---------- 3.5 每20回合输出快照 ----------
        if (turn % OUTPUT_CHECKPOINT_INTERVAL == 0) {
            int alive = 0;
            for (const auto& k : kingdoms) if (k.alive) alive++;
            std::cout << "===== 回合 " << turn << " (" << format_time(turn) << ") | 存活王国: "
                      << alive << "/" << kingdoms.size() << " =====" << std::endl;
        }

    } // end for turn

    // ---------- 4. 模拟结束，输出文件 ----------
    std::cout << "\n模拟结束，开始输出文件...\n";
    for (const auto& k : kingdoms) if (k.alive) write_kingdom_file(k, output_dir);
    write_fallen_kingdoms_file(kingdoms, output_dir);
    write_international_events_file(globalEventLog, output_dir);
    append_world_stability_assessment(kingdoms, output_dir);
    write_roads_file(kingdoms, output_dir);
    write_territory_file(kingdoms, output_dir);
    write_history_file(globalEventLog, output_dir);
    std::cout << "所有输出文件已生成。" << std::endl;
}
// ---------- 主函数 ----------
int main() {
    global_rng.seed(SIMULATION_RANDOM_SEED);
    // 新版 Weather-Terrain 默认输出为二进制 worldmap.bin，默认使用该文件名
    std::string world_file = "worldmap.bin";
    std::string output_dir = "output";
    run_simulation(world_file, output_dir);
    return 0;
}

// ============================================================
// 第 15 部分结束 - 全部代码结束
// ============================================================
