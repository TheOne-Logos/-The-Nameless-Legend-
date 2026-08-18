// Weather.cpp
// Full refactor of original "weather" world generator
// - Preserves original algorithms and iteration counts (no simplifications).
// - Uses contiguous Grid3 storage for layers (performance, but identical semantics).
// - Binary output by default; text output available via --text.
// - Fixes correctness bugs: PET divide-by-zero guard, clamped bilinear sampling.
// - Uses original integer BFS for distance-to-ocean (preserves original semantics).
// - Erosion disabled (per request).
//
// Build: g++ -std=c++14 Weather.cpp -O2 -o weather
// Usage: ./weather [--seed N] [--out filename] [--text]
//
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <random>
#include <string>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cfloat>
#include <climits>
#include <queue>
#include <chrono>

constexpr double PI = 3.14159265358979323846;

// ------------------ Configuration (unchanged semantics) ------------------
const int OUT_WIDTH   = 300;
const int OUT_HEIGHT  = 300;
const int BUFFER      = 150;
const int GEN_WIDTH   = OUT_WIDTH  + 2 * BUFFER;
const int GEN_HEIGHT  = OUT_HEIGHT + 2 * BUFFER;
const int BASE_LAYERS = 16;

const unsigned int RANDOM_SEED = 42;

const int    OCTAVES           = 10;
const double PERSISTENCE       = 0.25;
const double LACUNARITY        = 2.2;
const double NOISE_SCALE_BASE  = 0.015;

const int    NUM_PLATES                = 9;
const double PLATE_SPEED_MEAN          = 0.02;
const double PLATE_SPEED_STD           = 0.008;
const double CRUST_THICKNESS_OCEANIC   = 6.0;
const double CRUST_THICKNESS_CONTINENT = 38.0;
const double OROGENY_FACTOR            = 5200.0;
const double SUBDUCTION_ANGLE_DEG      = 25.0;
const double RIFT_ELEVATION            = -2500.0;
const double TRENCH_ELEVATION          = -4500.0;
const double HOTSPOT_ANOMALY_HEIGHT    = 1800.0;

const double SEA_LEVEL           = 0.0;
const double COASTAL_PLAIN_MAX   = 150.0;
const double HILL_MAX            = 600.0;
const double PLATEAU_MAX         = 1500.0;
const double MOUNTAIN_MAX        = 2800.0;
const double HIGH_MOUNTAIN_MIN   = 2800.0;
const double SHALLOW_OCEAN_LIMIT = -30.0;
const double MID_OCEAN_LIMIT     = -200.0;
const double DEEP_OCEAN_LIMIT    = -3000.0;
const double ABYSS_LIMIT         = -5000.0;

const double LAPSE_RATE              = 0.0065;
const double BASE_EQUATOR_TEMP       = 30.0;
const double POLE_TEMP               = -18.0;
const double LATITUDE_TEMP_RANGE     = BASE_EQUATOR_TEMP - POLE_TEMP;
const double SEASONAL_AMPLITUDE      = 12.0;
const double CONTINENTALITY_FACTOR   = 0.4;
const double OCEANIC_MODERATION      = 2.0;
const double WIND_BASE_SPEED         = 10.0;
const double CORIOLIS_PARAM_BASE     = 1.0e-4;
const double SURFACE_FRICTION        = 0.05;

const double EVAPORATION_RATE_OCEAN  = 0.85;
const double EVAPORATION_RATE_LAND   = 0.25;
const double MOISTURE_CAPACITY_BASE  = 1.0;
const double OROGRAPHIC_LIFT_FACTOR  = 0.08;
const double CONVECTIVE_PRECIP_FACTOR= 0.02;
const double RAIN_SHADOW_FACTOR      = 0.25;
const double UPLIFT_PRECIP_FACTOR    = 0.0015;
const double CONVECTIVE_PRECIP_BASE  = 0.02;
const double PRECIP_THRESHOLD_SNOW   = -1.0;

const double THORNTHWAITE_EXP_A      = 6.75e-7;
const double THORNTHWAITE_EXP_B      = 7.07e-7;
const double THORNTHWAITE_EXP_C      = 1.79e-5;
const double THORNTHWAITE_EXP_D      = 0.49239;

const double TEMP_TROPICAL_COLDEST    = 18.0;
const double TEMP_ARID                = 18.0;
const double TEMP_MILD_WINTER         = -3.0;
const double TEMP_COLD_WARMEST        = 10.0;
const double PRECIP_DRY_SUMMER        = 30.0;
const double PRECIP_DRY_WINTER        = 30.0;
const double ARIDITY_HYPER_ARID       = 0.03;
const double ARIDITY_ARID             = 0.2;
const double ARIDITY_SEMI_ARID        = 0.5;
const double ARIDITY_DRY_SUBHUMID     = 0.65;
const double PRECIP_RAINFOREST_MIN    = 2000.0;

const double BASE_FERTILITY           = 2.5;
const double PRECIP_FERTILITY_WEIGHT  = 0.001;
const double TEMP_FERTILITY_WEIGHT    = 0.02;
const double MAX_FERTILITY            = 5.0;

const double GRANITE_DENSITY_THRESHOLD  = 0.55;
const double LIMESTONE_FAVOR_ELEV       = 300.0;
const double METAMORPHIC_ELEV_THRESHOLD = 1500.0;

const double MINERAL_CHANCE_BASIN      = 0.2;
const double MINERAL_CHANCE_OROGEN     = 0.6;
const double MINERAL_CHANCE_SHIELD     = 0.15;
const int    NUM_ORE_TYPES             = 4;

const double VOLCANO_NOISE_THRESHOLD_ACTIVE  = 0.78;
const double VOLCANO_NOISE_THRESHOLD_DORMANT = 0.68;
const double VOLCANIC_ANOMALY_RADIUS         = 4.0;

const double FJORD_LATITUDE_FACTOR    = 0.6;
const double CLIFF_ELEV_THRESHOLD     = 180.0;
const double SAND_BEACH_WAVE_ENERGY   = 0.5;

const double WILDLIFE_PRECIP_MIN      = 250.0;
const double NPP_SCALING_FACTOR       = 0.001;

const double GW_RECHARGE_RATE         = 0.3;
const double GW_LIMESTONE_BONUS       = 1.25;
const double GW_GRANITE_PENALTY       = 0.85;

// ------------------ Enums ------------------
enum Biome {
    CORAL_REEF = 0,
    SHALLOW_OCEAN,
    MID_OCEAN,
    DEEP_OCEAN,
    ABYSS,
    SEA_ICE,
    DESERT,
    SAVANNA,
    GRASSLAND,
    DECID_FOREST,
    MIXED_FOREST,
    BOREAL_FOREST,
    RAINFOREST,
    WETLAND,
    MOUNT_GRASS,
    MOUNT_TUNDRA,
    BARE_ROCK,
    GLACIER,
    SNOW_PLAINS,
    SNOW_FOREST,
    SNOW_BOREAL,
    TUNDRA,
    ICE_CAP,
    NUM_BIOMES
};

enum TerrainClass {
    TER_ABYSS = 0,
    TER_DEEP_OCEAN,
    TER_MID_OCEAN,
    TER_SHALLOW_OCEAN,
    TER_COASTAL_PLAIN,
    TER_LOW_PLAIN,
    TER_HILL,
    TER_PLATEAU,
    TER_MOUNTAIN,
    TER_HIGH_MOUNTAIN
};

enum VegetationClass {
    VEG_NONE = 0,
    VEG_DESERT,
    VEG_GRASS,
    VEG_FOREST,
    VEG_RAINFOREST,
    VEG_BOREAL,
    VEG_TUNDRA,
    VEG_ICE,
    VEG_WETLAND
};

enum Bedrock {
    BEDROCK_SEDIMENTARY = 0,
    BEDROCK_GRANITE,
    BEDROCK_BASALT,
    BEDROCK_LIMESTONE,
    BEDROCK_METAMORPHIC,
    BEDROCK_VOLCANIC,
    BEDROCK_SERPENTINE,
    NUM_BEDROCK_TYPES
};

enum CoastType {
    COAST_NONE = -1,
    COAST_SAND,
    COAST_CLIFF,
    COAST_FJORD,
    COAST_MARSH
};

enum VolcanicActivity {
    VOLCANIC_NONE = 0,
    VOLCANIC_DORMANT,
    VOLCANIC_ACTIVE
};

enum WildlifePotential {
    WILDLIFE_NONE = 0,
    WILDLIFE_LOW,
    WILDLIFE_MEDIUM,
    WILDLIFE_HIGH
};

enum LayerIndex {
    LAYER_ELEVATION = 0,
    LAYER_TEMP,
    LAYER_PRECIP,
    LAYER_BIOME,
    LAYER_LAND_OCEAN,
    LAYER_TERRAIN,
    LAYER_VEGETATION,
    LAYER_SOIL_FERT,
    LAYER_MINERAL,
    LAYER_BEDROCK,
    LAYER_GROUNDWATER,
    LAYER_COASTTYPE,
    LAYER_VOLCANIC,
    LAYER_WILDLIFE,
    LAYER_WIND_U,
    LAYER_WIND_V,
    LAYER_PET,
    LAYER_SLOPE,
    LAYER_ARIDITY,
    LAYER_NPP,
    LAYER_CRUST_THICK,
    LAYER_DIST_OCEAN,
    LAYER_CONTINENTALITY,
    LAYER_MOISTURE,
    TOTAL_LAYERS   // = 24
};

const char* const all_layer_names[TOTAL_LAYERS] = {
    "ELEVATION",
    "TEMP",
    "PRECIP",
    "BIOME",
    "LAND_OCEAN",
    "TERRAIN",
    "VEGETATION",
    "SOIL_FERT",
    "MINERAL",
    "BEDROCK",
    "GROUNDWATER",
    "COASTTYPE",
    "VOLCANIC",
    "WILDLIFE",
    "WIND_U",
    "WIND_V",
    "PET",
    "SLOPE",
    "ARIDITY",
    "NPP",
    "CRUST_THICK",
    "DIST_OCEAN",
    "CONTINENTALITY",
    "MOISTURE"
};

// ------------------ Simplex noise (unchanged) ------------------
namespace {

constexpr int SIMPLEX_GRAD_COUNT = 12;
constexpr double grad2[SIMPLEX_GRAD_COUNT][2] = {
    { 1.0,  1.0}, {-1.0,  1.0}, { 1.0, -1.0}, {-1.0, -1.0},
    { 1.0,  0.0}, {-1.0,  0.0}, { 0.0,  1.0}, { 0.0, -1.0},
    { 0.7071067811865476,  0.7071067811865476},
    {-0.7071067811865476,  0.7071067811865476},
    { 0.7071067811865476, -0.7071067811865476},
    {-0.7071067811865476, -0.7071067811865476}
};

constexpr unsigned char perm[256] = {
    151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,
    140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,
    247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,
    57,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,
    74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
    60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,
    65, 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
    200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,
    52,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,
    207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
    119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,
    129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
    218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
    81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
    184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
    222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180
};

const double F2 = 0.5 * (std::sqrt(3.0) - 1.0);
const double G2 = (3.0 - std::sqrt(3.0)) / 6.0;

inline int fastfloor(double x) {
    int i = static_cast<int>(x);
    return (x < i) ? i - 1 : i;
}

double simplex_noise_2d(double xin, double yin) {
    double n0, n1, n2;

    double s = (xin + yin) * F2;
    int i = fastfloor(xin + s);
    int j = fastfloor(yin + s);

    double t = (i + j) * G2;
    double X0 = i - t;
    double Y0 = j - t;
    double x0 = xin - X0;
    double y0 = yin - Y0;

    int i1, j1;
    if (x0 > y0) {
        i1 = 1;
        j1 = 0;
    } else {
        i1 = 0;
        j1 = 1;
    }

    double x1 = x0 - i1 + G2;
    double y1 = y0 - j1 + G2;
    double x2 = x0 - 1.0 + 2.0 * G2;
    double y2 = y0 - 1.0 + 2.0 * G2;

    int ii = i & 255;
    int jj = j & 255;
    int gi0 = perm[(ii + perm[jj]) % 256] % SIMPLEX_GRAD_COUNT;
    int gi1 = perm[(ii + i1 + perm[(jj + j1) % 256]) % 256] % SIMPLEX_GRAD_COUNT;
    int gi2 = perm[(ii + 1 + perm[(jj + 1) % 256]) % 256] % SIMPLEX_GRAD_COUNT;

    double t0 = 0.5 - x0*x0 - y0*y0;
    if (t0 < 0.0) n0 = 0.0;
    else {
        t0 *= t0;
        n0 = t0*t0 * (grad2[gi0][0]*x0 + grad2[gi0][1]*y0);
    }

    double t1 = 0.5 - x1*x1 - y1*y1;
    if (t1 < 0.0) n1 = 0.0;
    else {
        t1 *= t1;
        n1 = t1*t1 * (grad2[gi1][0]*x1 + grad2[gi1][1]*y1);
    }

    double t2 = 0.5 - x2*x2 - y2*y2;
    if (t2 < 0.0) n2 = 0.0;
    else {
        t2 *= t2;
        n2 = t2*t2 * (grad2[gi2][0]*x2 + grad2[gi2][1]*y2);
    }

    return 70.0 * (n0 + n1 + n2);
}

} // namespace

// ---------- FBM ----------
double fbm_noise(double x, double y, const std::vector<double>& phases) {
    double val = 0.0;
    double amp = 1.0;
    double freq = 1.0;
    double maxv = 0.0;

    for (int i = 0; i < OCTAVES; ++i) {
        double offset_x = (i < static_cast<int>(phases.size()) / 2) ? phases[i] : 0.0;
        double offset_y = (i + OCTAVES < static_cast<int>(phases.size())) ? phases[i + OCTAVES] : 0.0;
        double nx = freq * x + offset_x;
        double ny = freq * y + offset_y;
        val += amp * simplex_noise_2d(nx, ny);
        maxv += amp;
        amp *= PERSISTENCE;
        freq *= LACUNARITY;
    }
    return val / maxv;
}

// ---------- Helpers ----------
template<class T> inline T clamp_t(T v, T lo, T hi) { return v < lo ? lo : (v > hi ? hi : v); }

// ---------- Grid3 contiguous storage ----------
struct Grid3 {
    int L, H, W;
    std::vector<double> data;
    Grid3() : L(0), H(0), W(0) {}
    Grid3(int layers, int height, int width) { init(layers, height, width); }
    void init(int layers, int height, int width) {
        L = layers; H = height; W = width;
        data.assign(static_cast<size_t>(L) * H * W, 0.0);
    }
    inline double& at(int l, int y, int x) {
        return data[(size_t)l * H * W + (size_t)y * W + (size_t)x];
    }
    inline const double& at(int l, int y, int x) const {
        return data[(size_t)l * H * W + (size_t)y * W + (size_t)x];
    }
};

// ---------- WorldGenerator ----------
class WorldGenerator {
public:
    unsigned int seed;
    int width;
    int height;

    Grid3 layers; // TOTAL_LAYERS x H x W

    std::vector<std::vector<int>>    plate_id;
    std::vector<std::vector<double>> crust_thickness;
    std::vector<std::vector<double>> pet;
    std::vector<std::vector<double>> slope;
    std::vector<std::vector<double>> air_moisture;
    std::vector<std::vector<int>>    dist_to_ocean;

    std::mt19937 rng;

    std::vector<double> phases_elev;
    std::vector<double> phases_temp;
    std::vector<double> phases_precip;
    std::vector<double> phases_bedrock;
    std::vector<double> phases_volcanic;
    std::vector<double> phases_mineral;
    std::vector<double> phases_coast;
    std::vector<double> phases_wildlife;

    WorldGenerator(int w, int h, unsigned int seed = RANDOM_SEED);

    void generate_all();
    void populate_extra_layers();   // populate derived layers

private:
    // core stages
    void generate_plate_tectonics();
    void compute_initial_elevation_from_crust();
    void apply_erosion(int iterations); // not used (kept for completeness)
    void compute_slope();
    void compute_distance_to_ocean();

    // climate
    void generate_wind_field();
    void generate_temperature();
    void generate_precipitation();
    void advect_moisture_field();
    void compute_pet();

    // biomes & vegetation
    void classify_biomes();
    void generate_land_ocean_layer();
    void generate_terrain_layer();
    void generate_vegetation_layer();

    // geology & resources
    void generate_bedrock();
    void generate_volcanic();
    void generate_soil_fertility();
    void generate_minerals();
    void generate_groundwater();
    void generate_coastline_type();

    // wildlife
    void generate_wildlife();

    // helpers
    double continental_index(int x, int y) const;
    double latitude_factor(int y) const;
    void   estimate_seasonal_temps(int x, int y, double& t_ann, double& t_cold, double& t_warm) const;
    double estimate_precip_seasonality(int x, int y) const;
    int select_koppen_biome(double temp_annual, double temp_coldest, double temp_warmest,
                        	double precip_annual, double precip_seasonality, double aridity) const;

    double noise_elev(double x, double y)    { return fbm_noise(x, y, phases_elev); }
    double noise_temp(double x, double y)    { return fbm_noise(x, y, phases_temp); }
    double noise_precip(double x, double y)  { return fbm_noise(x, y, phases_precip); }
    double noise_bedrock(double x, double y) { return fbm_noise(x, y, phases_bedrock); }
    double noise_volc(double x, double y)    { return fbm_noise(x, y, phases_volcanic); }
    double noise_mineral(double x, double y) { return fbm_noise(x, y, phases_mineral); }
    double noise_coast(double x, double y)   { return fbm_noise(x, y, phases_coast); }
    double noise_wild(double x, double y)    { return fbm_noise(x, y, phases_wildlife); }

    inline bool inside(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
};

// constructor
WorldGenerator::WorldGenerator(int w, int h, unsigned int seed_in)
    : width(w), height(h), rng(seed_in), seed(seed_in)
{
    layers.init(TOTAL_LAYERS, height, width);

    plate_id.assign(height, std::vector<int>(width, 0));
    crust_thickness.assign(height, std::vector<double>(width, 0.0));
    pet.assign(height, std::vector<double>(width, 0.0));
    slope.assign(height, std::vector<double>(width, 0.0));
    air_moisture.assign(height, std::vector<double>(width, 0.0));
    dist_to_ocean.assign(height, std::vector<int>(width, 0));

    // phases
    std::uniform_real_distribution<double> dist(0.0, 2.0 * PI);
    auto fill_phases = [&](std::vector<double>& vec) {
        vec.clear();
        for (int i = 0; i < 2 * OCTAVES; ++i)
            vec.push_back(dist(rng));
    };
    fill_phases(phases_elev);
    fill_phases(phases_temp);
    fill_phases(phases_precip);
    fill_phases(phases_bedrock);
    fill_phases(phases_volcanic);
    fill_phases(phases_mineral);
    fill_phases(phases_coast);
    fill_phases(phases_wildlife);

    std::cout << "World generation random seed: " << seed << std::endl;
}

// generate_all orchestrates stages in original order
void WorldGenerator::generate_all() {
    generate_plate_tectonics();
    compute_initial_elevation_from_crust();
    compute_slope();
    //apply_erosion(EROSION_ITERATIONS); // disabled per user request
    compute_slope();
    compute_distance_to_ocean();

    generate_wind_field();
    generate_temperature();      // restored (original)
    generate_precipitation();
    compute_pet();

    classify_biomes();
    generate_land_ocean_layer();
    generate_terrain_layer();
    generate_vegetation_layer();

    generate_bedrock();
    generate_volcanic();
    generate_soil_fertility();
    generate_minerals();
    generate_groundwater();
    generate_coastline_type();

    generate_wildlife();
}

// Plate tectonics (Voronoi) - preserved semantics
void WorldGenerator::generate_plate_tectonics() {
    struct Plate {
        double cx, cy;
        double vx, vy;
        bool   is_oceanic;
    };
    std::vector<Plate> plates(NUM_PLATES);
    std::uniform_real_distribution<double> pos_x(0.0, width-1.0);
    std::uniform_real_distribution<double> pos_y(0.0, height-1.0);
    std::normal_distribution<double> speed(PLATE_SPEED_MEAN, PLATE_SPEED_STD);
    std::uniform_real_distribution<double> angle(0.0, 2.0*PI);
    std::uniform_real_distribution<double> prob(0.0, 1.0);

    for (int p = 0; p < NUM_PLATES; ++p) {
        plates[p].cx = pos_x(rng);
        plates[p].cy = pos_y(rng);
        double spd = std::abs(speed(rng));
        double ang = angle(rng);
        plates[p].vx = spd * std::cos(ang);
        plates[p].vy = spd * std::sin(ang);
        plates[p].is_oceanic = (prob(rng) < 0.6);
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double dx_noise = noise_elev(x * 0.07 + 100.0, y * 0.07) * 4.0;
            double dy_noise = noise_elev(x * 0.07, y * 0.07 + 100.0) * 4.0;
            double xx = x + dx_noise;
            double yy = y + dy_noise;

            double best_dist = 1e30;
            int best_plate = 0;
            for (int p = 0; p < NUM_PLATES; ++p) {
                double d = (xx - plates[p].cx) * (xx - plates[p].cx) +
                           (yy - plates[p].cy) * (yy - plates[p].cy);
                if (d < best_dist) {
                    best_dist = d;
                    best_plate = p;
                }
            }
            plate_id[y][x] = best_plate;
        }
    }

    // boundary dist not used further in original code; compute crust thickness
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int pid = plate_id[y][x];
            double thick = plates[pid].is_oceanic ? CRUST_THICKNESS_OCEANIC : CRUST_THICKNESS_CONTINENT;
            thick += noise_elev(x * 0.02, y * 0.02) * 2.0;
            thick = std::max(2.0, std::min(thick, 60.0));
            crust_thickness[y][x] = thick;
        }
    }

    // EXACT original: smooth crust_thickness by 5x5 average repeated 15 times
    for (int iter = 0; iter < 15; ++iter) {
        auto smoothed = crust_thickness;
        for (int y = 2; y < height-2; ++y) {
            for (int x = 2; x < width-2; ++x) {
                double sum = 0.0;
                int count = 0;
                for (int dy = -2; dy <= 2; ++dy)
                    for (int dx = -2; dx <= 2; ++dx) {
                        sum += crust_thickness[y+dy][x+dx];
                        ++count;
                    }
                smoothed[y][x] = sum / count;
            }
        }
        crust_thickness = smoothed;
    }
}

// Compute elevation (Airy isostasy + multi-scale noise) — preserve original smoothing exactly
void WorldGenerator::compute_initial_elevation_from_crust() {
    const double rho_c = 2.7;
    const double rho_m = 3.3;
    const double ref_thickness = CRUST_THICKNESS_OCEANIC;
    const double ref_elev = -4800.0;
    const double elev_scale = 1000.0;

    // 1. base elevation
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double thick = crust_thickness[y][x];
            double elev = ref_elev + (thick - ref_thickness) * ((rho_m - rho_c) / rho_m) * elev_scale;

            double n_large = noise_elev(x * 0.005, y * 0.005) * 1800.0;
            double n_mid   = noise_elev(x * 0.02,  y * 0.02)  * 700.0;
            double n_small = noise_elev(x * 0.08,  y * 0.08)  * 250.0;
            elev += n_large + n_mid + n_small;

            layers.at(LAYER_ELEVATION, y, x) = elev;
        }
    }

    // 2. EXACT original smoothing: 15 iterations of 5x5 average, with counts and edges handled
    for (int iter = 0; iter < 15; ++iter) {
        auto temp = layers; // copy all layers but we'll only touch ELEVATION channel
        for (int y = 2; y < height - 2; ++y) {
            for (int x = 2; x < width - 2; ++x) {
                double sum = 0.0;
                int count = 0;
                for (int dy = -2; dy <= 2; ++dy) {
                    for (int dx = -2; dx <= 2; ++dx) {
                        sum += layers.at(LAYER_ELEVATION, y + dy, x + dx);
                        ++count;
                    }
                }
                temp.at(LAYER_ELEVATION, y, x) = sum / count;
            }
        }
        layers = temp;
    }

    // 3. taper near edges exactly as original
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int dist_to_edge = std::min({x, y, width - 1 - x, height - 1 - y});
            if (dist_to_edge < BUFFER) {
                double t = (double)dist_to_edge / BUFFER;
                if (layers.at(LAYER_ELEVATION, y, x) > SEA_LEVEL) {
                    double target_elev = -500.0 + t * (layers.at(LAYER_ELEVATION, y, x) + 500.0);
                    layers.at(LAYER_ELEVATION, y, x) = std::min(layers.at(LAYER_ELEVATION, y, x), target_elev);
                }
                if (dist_to_edge < 5) {
                    layers.at(LAYER_ELEVATION, y, x) = std::min(layers.at(LAYER_ELEVATION, y, x), -100.0);
                }
            }
        }
    }

    // clamp extremes
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x) {
            if (layers.at(LAYER_ELEVATION, y, x) > 8500.0) layers.at(LAYER_ELEVATION, y, x) = 8500.0;
            if (layers.at(LAYER_ELEVATION, y, x) < -11000.0) layers.at(LAYER_ELEVATION, y, x) = -11000.0;
        }
}

// compute slope (unchanged)
void WorldGenerator::compute_slope() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double dz_dx = 0.0, dz_dy = 0.0;
            if (x > 0 && x < width-1) dz_dx = (layers.at(LAYER_ELEVATION,y,x+1) - layers.at(LAYER_ELEVATION,y,x-1)) / 2.0;
            else if (x == 0) dz_dx = layers.at(LAYER_ELEVATION,y,1) - layers.at(LAYER_ELEVATION,y,0);
            else dz_dx = layers.at(LAYER_ELEVATION,y,width-1) - layers.at(LAYER_ELEVATION,y,width-2);
            if (y > 0 && y < height-1) dz_dy = (layers.at(LAYER_ELEVATION,y+1,x) - layers.at(LAYER_ELEVATION,y-1,x)) / 2.0;
            else if (y == 0) dz_dy = layers.at(LAYER_ELEVATION,1,x) - layers.at(LAYER_ELEVATION,0,x);
            else dz_dy = layers.at(LAYER_ELEVATION,height-1,x) - layers.at(LAYER_ELEVATION,height-2,x);
            slope[y][x] = std::sqrt(dz_dx*dz_dx + dz_dy*dz_dy);
        }
    }
}

// apply_erosion left as a function body (not used)
void WorldGenerator::apply_erosion(int iterations) {
    // (kept for completeness — not invoked per request)
    auto &elev = layers;
    auto elev_copy = elev;

    const int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
    const int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};

    for (int iter = 0; iter < iterations; ++iter) {
        std::vector<std::vector<double>> water(height, std::vector<double>(width, 1.0));
        std::vector<std::vector<int>> flow_dir(height, std::vector<int>(width, -1));

        // D8 flow
        for (int y = 1; y < height-1; ++y) {
            for (int x = 1; x < width-1; ++x) {
                double min_h = elev.at(LAYER_ELEVATION,y,x);
                int best = -1;
                for (int d = 0; d < 8; ++d) {
                    int nx = x + dx[d], ny = y + dy[d];
                    if (inside(nx, ny) && elev.at(LAYER_ELEVATION,ny,nx) < min_h) {
                        min_h = elev.at(LAYER_ELEVATION,ny,nx);
                        best = d;
                    }
                }
                if (best >= 0 && min_h < elev.at(LAYER_ELEVATION,y,x) - 1e-6)
                    flow_dir[y][x] = best;
            }
        }

        // hydraulic transport
        for (int pass = 0; pass < 3; ++pass) {
            auto water_new = water;
            for (int y = 1; y < height-1; ++y) {
                for (int x = 1; x < width-1; ++x) {
                    int dir = flow_dir[y][x];
                    if (dir >= 0) {
                        int nx = x + dx[dir], ny = y + dy[dir];
                        double transfer = water[y][x] * 0.8;
                        water_new[y][x] -= transfer;
                        water_new[ny][nx] += transfer;
                    }
                }
            }
            water = water_new;
        }

        // erosion / deposition
        for (int y = 1; y < height-1; ++y) {
            for (int x = 1; x < width-1; ++x) {
                double w = water[y][x];
                double s = slope[y][x];
                double erosion = 0.12 * w * s;
                double deposition = 0.1 * w * (1.0 - std::min(1.0, s / 0.5));
                elev_copy.at(LAYER_ELEVATION,y,x) -= erosion;
                int dir = flow_dir[y][x];
                if (dir >= 0) {
                    int nx = x + dx[dir], ny = y + dy[dir];
                    elev_copy.at(LAYER_ELEVATION,ny,nx) += deposition * 0.5;
                    elev_copy.at(LAYER_ELEVATION,y,x) += deposition * 0.5;
                }
            }
        }
        layers = elev_copy;
        compute_slope();
    }
}

// compute_distance_to_ocean: ORIGINAL integer BFS semantics (keeps behavior)
void WorldGenerator::compute_distance_to_ocean() {
    std::queue<std::pair<int,int>> q;

    // init
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (layers.at(LAYER_ELEVATION,y,x) <= SEA_LEVEL) {
                dist_to_ocean[y][x] = 0;
                q.push({x, y});
            } else {
                dist_to_ocean[y][x] = INT_MAX / 2;
            }
        }
    }

    int dx[8] = {1, 1, 0, -1, -1, -1, 0, 1};
    int dy[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    double weights[8] = {1.0, 1.414, 1.0, 1.414, 1.0, 1.414, 1.0, 1.414};

    while (!q.empty()) {
        auto p = q.front(); q.pop();
        int x = p.first, y = p.second;
        for (int d = 0; d < 8; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (inside(nx, ny)) {
                int nd = dist_to_ocean[y][x] + (int)(weights[d] * 1.0);
                if (nd < dist_to_ocean[ny][nx]) {
                    dist_to_ocean[ny][nx] = nd;
                    q.push({nx, ny});
                }
            }
        }
    }
}

// ---------- Climate / atmospheric ----------
// generate_wind_field: same as original
void WorldGenerator::generate_wind_field() {
    for (int y = 0; y < height; ++y) {
        double lat_ratio = (double)y / (height - 1);
        double lat_deg = (lat_ratio - 0.5) * 180.0;
        double abs_lat = std::abs(lat_deg);

        double u_wind = 0.0;
        if (abs_lat < 30.0) {
            u_wind = -10.0 * (1.0 - abs_lat / 30.0);
        } else if (abs_lat < 60.0) {
            u_wind = 15.0 * ((abs_lat - 30.0) / 30.0);
        } else {
            u_wind = -5.0 * ((abs_lat - 60.0) / 30.0);
        }

        double v_wind = 0.0;
        if (abs_lat < 15.0) v_wind = 0.0;
        else if (abs_lat < 30.0) v_wind = (lat_deg > 0) ? 2.0 : -2.0;
        else if (abs_lat < 60.0) v_wind = (lat_deg > 0) ? -3.0 : 3.0;
        else v_wind = (lat_deg > 0) ? 2.0 : -2.0;

        for (int x = 0; x < width; ++x) {
            double u = u_wind, v = v_wind;
            double elev = layers.at(LAYER_ELEVATION,y,x);
            if (elev > 0.0) {
                double dz_dx = (x > 0) ? (layers.at(LAYER_ELEVATION,y,x) - layers.at(LAYER_ELEVATION,y,x-1)) : 0.0;
                double dz_dy = (y > 0) ? (layers.at(LAYER_ELEVATION,y,x) - layers.at(LAYER_ELEVATION,y-1,x)) : 0.0;
                u += -dz_dy * 0.1;
                v += dz_dx * 0.1;
            }
            layers.at(LAYER_WIND_U,y,x) = u;
            layers.at(LAYER_WIND_V,y,x) = v;
        }
    }
}

// generate_temperature: RESTORED EXACTLY as original
void WorldGenerator::generate_temperature() {
    for (int y = 0; y < height; ++y) {
        double lat_factor = latitude_factor(y);
        double base_temp = POLE_TEMP + LATITUDE_TEMP_RANGE * lat_factor;
        for (int x = 0; x < width; ++x) {
            double elev = layers.at(LAYER_ELEVATION,y,x);
            double temp = base_temp;
            if (elev > SEA_LEVEL) {
                temp -= LAPSE_RATE * elev;
            }
            double cont = continental_index(x, y);
            double seasonal_amp = SEASONAL_AMPLITUDE *
                                  (cont * CONTINENTALITY_FACTOR + (1.0 - cont) / OCEANIC_MODERATION);
            temp += seasonal_amp * 0.2;
            layers.at(LAYER_TEMP,y,x) = temp;
        }
    }
}

// Bilinear sampling clamped to valid indices (fixes out-of-bounds)
static inline double bilinear_sample_clamped(const std::vector<std::vector<double>>& arr, double sx, double sy) {
    int H = (int)arr.size();
    if (H == 0) return 0.0;
    int W = (int)arr[0].size();
    if (W == 0) return 0.0;
    sx = clamp_t(sx, 0.0, (double)(W - 1));
    sy = clamp_t(sy, 0.0, (double)(H - 1));
    int ix = static_cast<int>(std::floor(sx));
    int iy = static_cast<int>(std::floor(sy));
    double fx = sx - ix;
    double fy = sy - iy;
    ix = std::max(0, std::min(W - 2, ix));
    iy = std::max(0, std::min(H - 2, iy));
    auto get = [&](int xx, int yy)->double {
        if (xx < 0 || yy < 0 || xx >= W || yy >= H) return 0.0;
        return arr[yy][xx];
    };
    return (1-fx)*(1-fy)*get(ix,iy) + fx*(1-fy)*get(ix+1,iy) + (1-fx)*fy*get(ix,iy+1) + fx*fy*get(ix+1,iy+1);
}

// advect_moisture_field: same approach, safer sampling
void WorldGenerator::advect_moisture_field() {
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            air_moisture[y][x] = (layers.at(LAYER_ELEVATION,y,x) <= SEA_LEVEL) ? 1.0 : 0.0;

    auto moist = air_moisture;
    const int WIND_ADVECTION_STEPS = 10;
    const double MOISTURE_ADVECTION_FACTOR = 0.7;

    for (int iter = 0; iter < WIND_ADVECTION_STEPS; ++iter) {
        auto moist_temp = moist;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double u = layers.at(LAYER_WIND_U,y,x);
                double v = layers.at(LAYER_WIND_V,y,x);
                double src_x = x - u * 0.3;
                double src_y = y - v * 0.3;
                double val = bilinear_sample_clamped(moist, src_x, src_y);
                moist_temp[y][x] = val * MOISTURE_ADVECTION_FACTOR + moist[y][x] * (1.0 - MOISTURE_ADVECTION_FACTOR);
            }
        }
        moist = moist_temp;
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x)
                if (layers.at(LAYER_ELEVATION,y,x) <= SEA_LEVEL)
                    moist[y][x] = 1.0;
    }
    air_moisture = moist;
}

// generate_precipitation (same logic)
void WorldGenerator::generate_precipitation() {
    advect_moisture_field();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (layers.at(LAYER_ELEVATION,y,x) <= SEA_LEVEL) {
                layers.at(LAYER_PRECIP,y,x) = 0.0;
                continue;
            }

            double moist = air_moisture[y][x];
            double elev = layers.at(LAYER_ELEVATION,y,x);

            double uplift = 0.0;
            if (x > 0) {
                double prev_elev = layers.at(LAYER_ELEVATION,y,x-1);
                uplift = std::max(0.0, elev - prev_elev);
            }
            double orographic_precip = UPLIFT_PRECIP_FACTOR * uplift * moist * 1000.0;

            double temp = layers.at(LAYER_TEMP,y,x);
            double convective_precip = CONVECTIVE_PRECIP_BASE * std::max(0.0, temp - 5.0) * moist * 500.0;

            double precip = orographic_precip + convective_precip;

            if (x > 0) {
                double prev_elev = layers.at(LAYER_ELEVATION,y,x-1);
                if (elev < prev_elev) {
                    precip *= RAIN_SHADOW_FACTOR;
                }
            }

            double base_precip = 200.0 * moist;
            precip += base_precip;

            precip = std::max(0.0, precip);
            layers.at(LAYER_PRECIP,y,x) = precip;
        }
    }
}

// compute_pet (Thornthwaite) with I guard to avoid division by zero
void WorldGenerator::compute_pet() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double t_annual = layers.at(LAYER_TEMP,y,x);
            if (t_annual <= 0.0) {
                pet[y][x] = 0.0;
                layers.at(LAYER_PET,y,x) = 0.0;
                continue;
            }

            double t_cold, t_warm;
            estimate_seasonal_temps(x, y, t_annual, t_cold, t_warm);

            double I = 0.0;
            for (int m = 0; m < 12; ++m) {
                double t_month = t_annual + (t_warm - t_cold) / 2.0 *
                                 std::sin(2.0 * PI * (m + 0.5) / 12.0);
                if (t_month > 0.0) {
                    I += std::pow(t_month / 5.0, 1.514);
                }
            }

            if (I <= 1e-12) {
                pet[y][x] = 0.0;
                layers.at(LAYER_PET,y,x) = 0.0;
                continue;
            }

            double a = THORNTHWAITE_EXP_A * I * I * I +
                       THORNTHWAITE_EXP_B * I * I +
                       THORNTHWAITE_EXP_C * I +
                       THORNTHWAITE_EXP_D;

            double pet_annual = 0.0;
            for (int m = 0; m < 12; ++m) {
                double t_month = t_annual + (t_warm - t_cold) / 2.0 *
                                 std::sin(2.0 * PI * (m + 0.5) / 12.0);
                if (t_month > 0.0) {
                    pet_annual += 16.0 * std::pow(10.0 * t_month / I, a);
                }
            }
            pet[y][x] = pet_annual;
            layers.at(LAYER_PET,y,x) = pet_annual;
        }
    }
}

// ---------- Biomes ----------
// estimate seasonal temps and precip seasonality (unchanged)
double WorldGenerator::latitude_factor(int y) const {
    double lat_ratio = (double)y / (height - 1);
    double lat_angle = PI * (lat_ratio - 0.5);
    return std::abs(std::cos(lat_angle));
}

double WorldGenerator::continental_index(int x, int y) const {
    int d = dist_to_ocean[y][x];
    double maxd = (double)(std::min(width, height)) / 2.0;
    return std::min(1.0, d / maxd);
}

void WorldGenerator::estimate_seasonal_temps(int x, int y,
        double& temp_annual, double& temp_coldest_month,
        double& temp_warmest_month) const
{
    temp_annual = layers.at(LAYER_TEMP,y,x);
    double lat = latitude_factor(y);
    double cont = continental_index(x, y);

    double annual_range = SEASONAL_AMPLITUDE *
                          ((1.0 - cont) / OCEANIC_MODERATION +
                           cont * CONTINENTALITY_FACTOR);
    annual_range *= (0.5 + 0.5 * (1.0 - lat));

    temp_warmest_month = temp_annual + annual_range / 2.0;
    temp_coldest_month = temp_annual - annual_range / 2.0;
}

double WorldGenerator::estimate_precip_seasonality(int x, int y) const {
    double lat_deg = (y / (double)(height - 1) - 0.5) * 180.0;
    double season = std::sin(lat_deg * PI / 180.0 * 2.5);
    double elev = layers.at(LAYER_ELEVATION,y,x);
    if (elev > 2000.0) season += 0.2;
    double cont = continental_index(x, y);
    season += cont * 0.3;
    return std::max(-1.0, std::min(1.0, season));
}

// select_koppen_biome exactly preserved
int WorldGenerator::select_koppen_biome(double temp_annual,
                                        double temp_coldest, double temp_warmest,
                                        double precip_annual, double precip_seasonality,
                                        double aridity) const
{
    if (temp_warmest < TEMP_COLD_WARMEST) {
        if (temp_warmest < 0.0) return ICE_CAP;
        return TUNDRA;
    }

    double pth;
    if (precip_seasonality > 0.3)
        pth = 20.0 * temp_annual + 280.0;
    else if (precip_seasonality < -0.3)
        pth = 20.0 * temp_annual;
    else
        pth = 20.0 * temp_annual + 140.0;

    if (precip_annual < pth * 0.5) {
        return DESERT;
    }
    if (precip_annual < pth) {
        return GRASSLAND;
    }

    if (temp_coldest >= TEMP_TROPICAL_COLDEST) {
        if (precip_annual >= PRECIP_RAINFOREST_MIN) {
            return RAINFOREST;
        }
        if (std::abs(precip_seasonality) > 0.4)
            return SAVANNA;
        else
            return RAINFOREST;
    }

    if (temp_coldest > TEMP_MILD_WINTER && temp_coldest <= TEMP_TROPICAL_COLDEST) {
        if (precip_annual > 1200.0)
            return DECID_FOREST;
        if (precip_seasonality < -0.2)
            return MIXED_FOREST;
        return DECID_FOREST;
    }

    if (temp_warmest >= TEMP_COLD_WARMEST && temp_coldest <= TEMP_MILD_WINTER) {
        if (precip_annual > 800.0)
            return BOREAL_FOREST;
        else
            return MIXED_FOREST;
    }

    return GRASSLAND;
}

// classify_biomes preserved
void WorldGenerator::classify_biomes() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double elev   = layers.at(LAYER_ELEVATION,y,x);
            double temp   = layers.at(LAYER_TEMP,y,x);
            double precip = layers.at(LAYER_PRECIP,y,x);

            int biome;
            if (elev <= SEA_LEVEL) {
                if (temp < 0.0 && elev > DEEP_OCEAN_LIMIT)
                    biome = SEA_ICE;
                else if (elev > SHALLOW_OCEAN_LIMIT && temp > 20.0)
                    biome = CORAL_REEF;
                else if (elev > SHALLOW_OCEAN_LIMIT)
                    biome = SHALLOW_OCEAN;
                else if (elev > MID_OCEAN_LIMIT)
                    biome = MID_OCEAN;
                else if (elev > DEEP_OCEAN_LIMIT)
                    biome = DEEP_OCEAN;
                else
                    biome = ABYSS;
            } else {
                if (elev > HIGH_MOUNTAIN_MIN) {
                    if (temp < -5.0)       biome = GLACIER;
                    else if (temp < 0.0)   biome = MOUNT_TUNDRA;
                    else if (temp < 5.0)   biome = MOUNT_GRASS;
                    else                   biome = BARE_ROCK;
                }
                else if (elev > PLATEAU_MAX) {
                    if (temp < 0.0)        biome = MOUNT_TUNDRA;
                    else if (temp < 5.0)   biome = MOUNT_GRASS;
                    else                   biome = GRASSLAND;
                }
                else {
                    double t_ann, t_cold, t_warm;
                    estimate_seasonal_temps(x, y, t_ann, t_cold, t_warm);
                    double p_season = estimate_precip_seasonality(x, y);
                    double aridity = (pet[y][x] > 0.0) ? precip / pet[y][x] : 1.0;
                    biome = select_koppen_biome(t_ann, t_cold, t_warm,
                                                precip, p_season, aridity);

                    if (elev < COASTAL_PLAIN_MAX && precip > 800.0 && t_ann > 0.0) {
                        biome = WETLAND;
                    }

                    if (t_ann < 0.0 && biome != DESERT && biome != TUNDRA && biome != ICE_CAP) {
                        switch (biome) {
                        case SAVANNA:
                        case GRASSLAND:
                            biome = SNOW_PLAINS;
                            break;
                        case DECID_FOREST:
                        case MIXED_FOREST:
                        case RAINFOREST:
                            biome = SNOW_FOREST;
                            break;
                        case BOREAL_FOREST:
                            biome = SNOW_BOREAL;
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
            layers.at(LAYER_BIOME,y,x) = static_cast<double>(biome);
        }
    }
}

// generate_land_ocean_layer preserved
void WorldGenerator::generate_land_ocean_layer() {
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            layers.at(LAYER_LAND_OCEAN,y,x) =
                (layers.at(LAYER_ELEVATION,y,x) > SEA_LEVEL) ? 1.0 : 0.0;
}

// generate_terrain_layer preserved
void WorldGenerator::generate_terrain_layer() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double elev = layers.at(LAYER_ELEVATION,y,x);
            TerrainClass ter;
            if (elev <= ABYSS_LIMIT)                ter = TER_ABYSS;
            else if (elev <= DEEP_OCEAN_LIMIT)      ter = TER_DEEP_OCEAN;
            else if (elev <= MID_OCEAN_LIMIT)       ter = TER_MID_OCEAN;
            else if (elev <= SHALLOW_OCEAN_LIMIT)   ter = TER_SHALLOW_OCEAN;
            else if (elev <= COASTAL_PLAIN_MAX)     ter = TER_COASTAL_PLAIN;
            else if (elev <= 300.0)                 ter = TER_LOW_PLAIN;   
            else if (elev <= HILL_MAX)              ter = TER_HILL;   
            else if (elev <= PLATEAU_MAX)           ter = TER_PLATEAU;
            else if (elev <= MOUNTAIN_MAX)          ter = TER_MOUNTAIN;
            else                                    ter = TER_HIGH_MOUNTAIN;
            layers.at(LAYER_TERRAIN,y,x) = static_cast<double>(ter);
        }
    }
}

// generate_vegetation_layer preserved
void WorldGenerator::generate_vegetation_layer() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Biome b = static_cast<Biome>(static_cast<int>(layers.at(LAYER_BIOME,y,x)));
            VegetationClass veg = VEG_NONE;
            switch (b) {
            case DESERT:
            case BARE_ROCK:
                veg = VEG_DESERT;
                break;
            case SAVANNA:
            case GRASSLAND:
            case MOUNT_GRASS:
                veg = VEG_GRASS;
                break;
            case DECID_FOREST:
            case MIXED_FOREST:
                veg = VEG_FOREST;
                break;
            case RAINFOREST:
                veg = VEG_RAINFOREST;
                break;
            case BOREAL_FOREST:
            case SNOW_BOREAL:
                veg = VEG_BOREAL;
                break;
            case TUNDRA:
            case MOUNT_TUNDRA:
                veg = VEG_TUNDRA;
                break;
            case ICE_CAP:
            case GLACIER:
                veg = VEG_ICE;
                break;
            case WETLAND:
                veg = VEG_WETLAND;
                break;
            default:
                break;
            }
            layers.at(LAYER_VEGETATION,y,x) = static_cast<double>(veg);
        }
    }
}

// -------------------- Geology / resources --------------------
void WorldGenerator::generate_soil_fertility() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (layers.at(LAYER_ELEVATION,y,x) <= SEA_LEVEL) {
                layers.at(LAYER_SOIL_FERT,y,x) = 0.0;
                continue;
            }

            Biome b = static_cast<Biome>(static_cast<int>(layers.at(LAYER_BIOME,y,x)));
            double fert = BASE_FERTILITY;

            switch (b) {
            case DESERT:
                fert = 0.3;
                break;
            case SAVANNA:
                fert = 2.0;
                break;
            case GRASSLAND:
                fert = 3.5;
                break;
            case DECID_FOREST:
            case MIXED_FOREST:
                fert = 4.0;
                break;
            case RAINFOREST:
                fert = 4.5;
                break;
            case BOREAL_FOREST:
                fert = 2.0;
                break;
            case WETLAND:
                fert = 5.0;
                break;
            case TUNDRA:
                fert = 1.2;
                break;
            case MOUNT_GRASS:
                fert = 2.5;
                break;
            case MOUNT_TUNDRA:
                fert = 1.0;
                break;
            case BARE_ROCK:
            case GLACIER:
            case ICE_CAP:
                fert = 0.0;
                break;
            case SNOW_PLAINS:
            case SNOW_FOREST:
            case SNOW_BOREAL:
                fert = 1.5;
                break;
            default:
                fert = BASE_FERTILITY;
            }

            double precip = layers.at(LAYER_PRECIP,y,x);
            fert += (precip - 600.0) * 0.0008;
            double temp = layers.at(LAYER_TEMP,y,x);
            if (temp < -5.0) fert -= 1.0;
            else if (temp > 25.0) fert += 0.5;

            int rock = static_cast<int>(layers.at(LAYER_BEDROCK,y,x));
            if (rock == BEDROCK_LIMESTONE) fert *= 1.15;
            else if (rock == BEDROCK_GRANITE) fert *= 0.85;
            else if (rock == BEDROCK_VOLCANIC) fert *= 1.25;

            fert = std::max(0.0, std::min(MAX_FERTILITY, fert));
            layers.at(LAYER_SOIL_FERT,y,x) = fert;
        }
    }
}

void WorldGenerator::generate_bedrock() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double elev = layers.at(LAYER_ELEVATION,y,x);
            double crust = crust_thickness[y][x];
            double noise = noise_bedrock(x * 0.03, y * 0.03);

            Bedrock rock;
            if (elev <= SEA_LEVEL) {
                if (elev > SHALLOW_OCEAN_LIMIT && noise > 0.3)
                    rock = BEDROCK_SEDIMENTARY;
                else if (elev > MID_OCEAN_LIMIT && noise > 0.7)
                    rock = BEDROCK_LIMESTONE;
                else
                    rock = BEDROCK_BASALT;
            } else {
                if (crust > 40.0) {
                    rock = (noise > GRANITE_DENSITY_THRESHOLD) ? BEDROCK_GRANITE : BEDROCK_METAMORPHIC;
                } else if (crust > 25.0) {
                    if (noise > 0.7) rock = BEDROCK_GRANITE;
                    else if (noise > 0.4) rock = BEDROCK_METAMORPHIC;
                    else rock = BEDROCK_SEDIMENTARY;
                } else {
                    rock = (elev < COASTAL_PLAIN_MAX && noise > 0.5) ? BEDROCK_SEDIMENTARY : BEDROCK_BASALT;
                }
                if (elev > METAMORPHIC_ELEV_THRESHOLD) {
                    rock = (noise > 0.6) ? BEDROCK_GRANITE : BEDROCK_METAMORPHIC;
                }
                if (elev < LIMESTONE_FAVOR_ELEV && noise > 0.4 && noise < 0.7 && rock == BEDROCK_SEDIMENTARY)
                    rock = BEDROCK_LIMESTONE;
            }
            layers.at(LAYER_BEDROCK,y,x) = static_cast<double>(rock);
        }
    }
}

void WorldGenerator::generate_volcanic() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (layers.at(LAYER_ELEVATION,y,x) <= SEA_LEVEL) {
                layers.at(LAYER_VOLCANIC,y,x) = static_cast<double>(VOLCANIC_NONE);
                continue;
            }

            double noise_val = noise_volc(x * 0.1, y * 0.1);
            bool near_boundary = false;
            int pid = plate_id[y][x];
            for (int dy = -1; dy <= 1 && !near_boundary; ++dy)
                for (int dx = -1; dx <= 1; ++dx)
                    if (inside(x+dx, y+dy) && plate_id[y+dy][x+dx] != pid) {
                        near_boundary = true;
                        break;
                    }
            double hotspot_noise = noise_volc(x * 0.03 + 5.0, y * 0.03 + 5.0);
            bool is_hotspot = (hotspot_noise > 0.85);

            VolcanicActivity volc = VOLCANIC_NONE;
            if (near_boundary && noise_val > VOLCANO_NOISE_THRESHOLD_ACTIVE) {
                volc = VOLCANIC_ACTIVE;
                layers.at(LAYER_BEDROCK,y,x) = static_cast<double>(BEDROCK_VOLCANIC);
            } else if (near_boundary && noise_val > VOLCANO_NOISE_THRESHOLD_DORMANT) {
                volc = VOLCANIC_DORMANT;
            } else if (is_hotspot && noise_val > 0.6) {
                volc = VOLCANIC_ACTIVE;
                layers.at(LAYER_BEDROCK,y,x) = static_cast<double>(BEDROCK_VOLCANIC);
            } else if (is_hotspot && noise_val > 0.5) {
                volc = VOLCANIC_DORMANT;
            }
            layers.at(LAYER_VOLCANIC,y,x) = static_cast<double>(volc);
        }
    }
}

void WorldGenerator::generate_minerals() {
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (layers.at(LAYER_ELEVATION,y,x) <= SEA_LEVEL) {
                layers.at(LAYER_MINERAL,y,x) = 0.0;
                continue;
            }

            double chance = 0.0;
            int ore_type = 0;
            int ter  = static_cast<int>(layers.at(LAYER_TERRAIN,y,x));
            int rock = static_cast<int>(layers.at(LAYER_BEDROCK,y,x));
            double elev = layers.at(LAYER_ELEVATION,y,x);
            double noise_val = noise_mineral(x * 0.05, y * 0.05);

            if (rock == BEDROCK_SEDIMENTARY && elev < PLATEAU_MAX && ter <= TER_LOW_PLAIN) {
                chance = MINERAL_CHANCE_BASIN;
                ore_type = 1;
            }
            if ((ter == TER_MOUNTAIN || ter == TER_HIGH_MOUNTAIN) &&
                    (rock == BEDROCK_GRANITE || rock == BEDROCK_METAMORPHIC)) {
                double local_chance = MINERAL_CHANCE_OROGEN;
                if (noise_val > 0.5) local_chance += 0.1;
                chance = local_chance;
                ore_type = 2;
            }
            if (rock == BEDROCK_METAMORPHIC && elev < HILL_MAX) {
                chance = MINERAL_CHANCE_SHIELD;
                ore_type = 3;
            }
            if (layers.at(LAYER_VOLCANIC,y,x) > 0.5) {
                chance = 0.7;
                ore_type = 4;
            }

            if (chance > 0.0 && prob(rng) < chance)
                layers.at(LAYER_MINERAL,y,x) = static_cast<double>(ore_type);
            else
                layers.at(LAYER_MINERAL,y,x) = 0.0;
        }
    }
}

void WorldGenerator::generate_groundwater() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (layers.at(LAYER_ELEVATION,y,x) <= SEA_LEVEL) {
                layers.at(LAYER_GROUNDWATER,y,x) = 0.0;
                continue;
            }
            double precip = layers.at(LAYER_PRECIP,y,x);
            double pet_val = pet[y][x];
            double water_balance = precip - pet_val;
            double gw = 0.3 + GW_RECHARGE_RATE * (water_balance / 1000.0);
            int rock = static_cast<int>(layers.at(LAYER_BEDROCK,y,x));
            if (rock == BEDROCK_LIMESTONE) gw *= GW_LIMESTONE_BONUS;
            else if (rock == BEDROCK_GRANITE || rock == BEDROCK_METAMORPHIC) gw *= GW_GRANITE_PENALTY;
            else if (rock == BEDROCK_BASALT) gw *= 0.9;
            else if (rock == BEDROCK_VOLCANIC) gw *= 1.1;
            double slp = slope[y][x];
            gw *= (1.0 - std::min(1.0, slp / 45.0));
            if (layers.at(LAYER_TEMP,y,x) < 0.0) gw *= 0.7;
            gw = std::max(0.0, std::min(1.0, gw));
            layers.at(LAYER_GROUNDWATER,y,x) = gw;
        }
    }
}

// coastline type preserved
void WorldGenerator::generate_coastline_type() {
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            layers.at(LAYER_COASTTYPE,y,x) = static_cast<double>(COAST_NONE);

    const int dx[8] = {-1, -1, -1,  0,  1,  1,  1,  0};
    const int dy[8] = {-1,  0,  1,  1,  1,  0, -1, -1};

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (layers.at(LAYER_ELEVATION,y,x) <= SEA_LEVEL) continue;
            bool is_coast = false;
            for (int d = 0; d < 8; ++d) {
                int nx = x + dx[d], ny = y + dy[d];
                if (inside(nx, ny) && layers.at(LAYER_ELEVATION,ny,nx) <= SEA_LEVEL) {
                    is_coast = true;
                    break;
                }
            }
            if (!is_coast) continue;

            double elev = layers.at(LAYER_ELEVATION,y,x);
            double lat_factor = std::abs(2.0 * y / (height - 1.0) - 1.0);
            int rock = static_cast<int>(layers.at(LAYER_BEDROCK,y,x));
            double wave_energy = (lat_factor < 0.7 ? 1.0 : 0.4);
            double fetch = 1.0 / (1.0 + continental_index(x, y) * 5.0);
            wave_energy *= fetch;

            CoastType ctype;
            if (lat_factor > FJORD_LATITUDE_FACTOR && elev > CLIFF_ELEV_THRESHOLD &&
                    (rock == BEDROCK_GRANITE || rock == BEDROCK_METAMORPHIC)) {
                ctype = COAST_FJORD;
            } else if (elev > CLIFF_ELEV_THRESHOLD || rock == BEDROCK_GRANITE) {
                ctype = COAST_CLIFF;
            } else if (wave_energy < SAND_BEACH_WAVE_ENERGY) {
                ctype = COAST_MARSH;
            } else {
                ctype = COAST_SAND;
            }
            layers.at(LAYER_COASTTYPE,y,x) = static_cast<double>(ctype);
        }
    }
}

// wildlife preserved
void WorldGenerator::generate_wildlife() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (layers.at(LAYER_ELEVATION,y,x) <= SEA_LEVEL) {
                layers.at(LAYER_WILDLIFE,y,x) = static_cast<double>(WILDLIFE_NONE);
                continue;
            }

            Biome b = static_cast<Biome>(static_cast<int>(layers.at(LAYER_BIOME,y,x)));
            double temp = layers.at(LAYER_TEMP,y,x);
            double precip = layers.at(LAYER_PRECIP,y,x);

            auto temp_npp = [](double t) {
                if (t < -10.0) return 0.0;
                return 3000.0 / (1.0 + std::exp(1.315 - 0.119 * t));
            };
            auto precip_npp = [](double p) {
                return 3000.0 * (1.0 - std::exp(-0.000664 * p));
            };
            double npp = std::min(temp_npp(temp), precip_npp(precip));

            WildlifePotential wp = WILDLIFE_NONE;
            if (b == RAINFOREST || b == WETLAND) wp = WILDLIFE_HIGH;
            else if (b == DECID_FOREST || b == MIXED_FOREST) {
                wp = (npp > 1500) ? WILDLIFE_HIGH : WILDLIFE_MEDIUM;
            } else if (b == SAVANNA || b == GRASSLAND) {
                wp = (npp > 800) ? WILDLIFE_MEDIUM : WILDLIFE_LOW;
            } else if (b == BOREAL_FOREST) {
                wp = (npp > 600) ? WILDLIFE_LOW : WILDLIFE_NONE;
            } else if (b == TUNDRA) {
                wp = (npp > 300) ? WILDLIFE_LOW : WILDLIFE_NONE;
            }

            if (precip < WILDLIFE_PRECIP_MIN) wp = WILDLIFE_NONE;
            if (b == GLACIER || b == ICE_CAP || b == BARE_ROCK) wp = WILDLIFE_NONE;
            layers.at(LAYER_WILDLIFE,y,x) = static_cast<double>(wp);
        }
    }
}

// populate extra layers (PET, slope were computed earlier)
void WorldGenerator::populate_extra_layers() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            layers.at(LAYER_PET,y,x) = pet[y][x];
            layers.at(LAYER_SLOPE,y,x) = slope[y][x];

            double precip = layers.at(LAYER_PRECIP,y,x);
            double pet_val = pet[y][x];
            double aridity = (precip > 0.0 && pet_val > 0.0) ? precip / pet_val : 1.0;
            layers.at(LAYER_ARIDITY,y,x) = std::max(0.0, std::min(5.0, aridity));

            double temp = layers.at(LAYER_TEMP,y,x);
            double npp_temp = (temp < -10.0) ? 0.0 :
                              3000.0 / (1.0 + std::exp(1.315 - 0.119 * temp));
            double npp_precip = (precip > 0.0) ? 3000.0 * (1.0 - std::exp(-0.000664 * precip)) : 0.0;
            double npp = std::min(npp_temp, npp_precip);
            if (layers.at(LAYER_ELEVATION,y,x) <= SEA_LEVEL) npp = 0.0;
            layers.at(LAYER_NPP,y,x) = npp;

            layers.at(LAYER_CRUST_THICK,y,x) = crust_thickness[y][x];
            layers.at(LAYER_DIST_OCEAN,y,x) = static_cast<double>(dist_to_ocean[y][x]);
            layers.at(LAYER_CONTINENTALITY,y,x) = continental_index(x, y);
            layers.at(LAYER_MOISTURE,y,x) = air_moisture[y][x];
        }
    }
}

// -------------------- Save world --------------------
void save_world(const WorldGenerator& world, const std::string& filename, bool binary = true) {
    if (binary) {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            std::cerr << "Cannot open file for writing (binary): " << filename << "\n";
            return;
        }
        out << "WORLDMAPBIN"; // magic text header to help simple identification
        int32_t w = world.width, h = world.height, layers_count = TOTAL_LAYERS;
        out.write(reinterpret_cast<const char*>(&w), sizeof(w));
        out.write(reinterpret_cast<const char*>(&h), sizeof(h));
        out.write(reinterpret_cast<const char*>(&layers_count), sizeof(layers_count));
        for (int l = 0; l < TOTAL_LAYERS; ++l) {
            uint16_t len = static_cast<uint16_t>(std::strlen(all_layer_names[l]));
            out.write(reinterpret_cast<const char*>(&len), sizeof(len));
            out.write(all_layer_names[l], len);
        }
        out.write(reinterpret_cast<const char*>(world.layers.data.data()), world.layers.data.size() * sizeof(double));
        out.close();
        std::cout << "World saved (binary) to " << filename << " (" << TOTAL_LAYERS << " layers, size " << world.width << "x" << world.height << ")\n";
    } else {
        std::ofstream out(filename);
        if (!out) {
            std::cerr << "Cannot open file for writing: " << filename << "\n";
            return;
        }
        out << "WORLDMAP\n";
        out << world.width << " " << world.height << " " << TOTAL_LAYERS << "\n";
        for (int l = 0; l < TOTAL_LAYERS; ++l) {
            out << all_layer_names[l] << "\n";
            for (int y = 0; y < world.height; ++y) {
                for (int x = 0; x < world.width; ++x) {
                    out << world.layers.at(l,y,x);
                    if (x < world.width - 1) out << " ";
                }
                out << "\n";
            }
        }
        out.close();
        std::cout << "World saved (text) to " << filename << " (" << TOTAL_LAYERS << " layers, size " << world.width << "x" << world.height << ")\n";
    }
}

// ---------- Utility: robust seed generation ----------
unsigned int generate_random_seed() {
    try {
        std::random_device rd;
        if (rd.entropy() > 0) {
            return rd();
        }
    } catch (...) {
        // ignore
    }

    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    static volatile int dummy = 0;
    uintptr_t addr = reinterpret_cast<uintptr_t>(&dummy);
    return static_cast<unsigned int>(now) ^ static_cast<unsigned int>(addr);
}

// ---------- main ----------
int main(int argc, char** argv) {
    unsigned int seed = generate_random_seed();
    std::string outname = "worldmap.bin";
    bool text_out = false;

    for (int i = 1; i < argc; ++i) {
        std::string s = argv[i];
        if (s == "--seed" && i + 1 < argc) { seed = static_cast<unsigned int>(std::stoul(argv[++i])); }
        else if (s == "--out" && i + 1 < argc) { outname = argv[++i]; }
        else if (s == "--text") { text_out = true; }
        else {
            std::cerr << "Unknown arg: " << s << "\n";
            std::cerr << "Usage: " << argv[0] << " [--seed N] [--out filename] [--text]\n";
        }
    }

    // make RNG deterministic from seed (better mixing)
    std::seed_seq seq{ seed, 0x9e3779b9, static_cast<unsigned int>(seed ^ 0xdeadbeef) };
    std::vector<uint32_t> seeds(1);
    seq.generate(seeds.begin(), seeds.end());
    unsigned int final_seed = seeds[0];

    WorldGenerator world(GEN_WIDTH, GEN_HEIGHT, final_seed);
    world.generate_all();
    world.populate_extra_layers();

    save_world(world, outname, !text_out);
    return 0;
}
