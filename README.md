This is a system about terrain and weather simulation.

# 🌍 Weather-Terrain Map Generator

> **A companion tool for the *Civilization Simulator* that generates high-resolution world maps.**  
> This program, based on an original climate-geology model (preserving all algorithmic logic), produces raster data containing **24 layers** (terrain, temperature, precipitation, biomes, geological structures, etc.) in a format fully compatible with the simulator.

---

## 📌 Relationship with the Civilization Simulator

- `Weather-Terrain` is a **map generation front-end** for `Civilization`.
- The simulator loads `worldmap.bin` (binary format) by default, and this program generates exactly that file.
- Both share the same layer definitions (`LAYER_*` enums) and geographic parameters (`WORLD_WIDTH`, `HEIGHT`, `BUFFER`, etc.), so **any map generated here can be used directly in the simulator** without any additional conversion.

> ⚠️ **Important**:  
> This program only generates maps; it **does not modify any code or logic of the simulator itself**. You can use maps produced by this tool without adjusting any parameters in `Civilization`.

---

## ✨ Features

- **Full reproduction of the original algorithm**: From plate tectonics, crustal thickness, and initial elevation, to climate (wind fields, temperature, precipitation, evapotranspiration), biomes, geology (bedrock, minerals, groundwater, volcanoes), coastal types, and wildlife potential—all computational steps are identical to the original code, with no simplifications.
- **High-resolution support**: Generates a `600×600` effective map (with a `150`‑cell buffer on the outer edges, total size `900×900`) containing 24 layers of geographic data.
- **Two output formats**:
  - **Binary** (default): compact and efficient, directly readable by the simulator (header `WORLDMAPBIN`).
  - **Text** (via `--text`): human‑readable, useful for debugging or post‑processing.
- **Fixed or random seed**: Uses system entropy for a random seed by default, or you can manually specify one with `--seed` to ensure reproducibility.
- **Performance optimised**: Uses contiguous `Grid3` memory for layer storage, speeding up read/write while maintaining semantic clarity.
- **Boundary gradient handling**: Edge regions are gradually lowered in elevation to avoid boundary effects in the simulator.

---

## 🛠️ Compilation & Execution

### Dependencies
- C++14‑compatible compiler (GCC 5+, Clang 3.4+, MSVC 2015+)
- Standard library only (no external dependencies)

### Compilation command
```bash
g++ -std=c++14 -O2 -o weather Weather-Terrain.cpp
```

### Basic usage
```bash
# Generate a binary map with a random seed (outputs worldmap.bin by default)
./weather

# Specify an output file name
./weather --out mymap.bin

# Manually set a random seed (for reproducibility)
./weather --seed 12345

# Output in text format (for easy inspection)
./weather --text --out worldmap.txt
```

### Command‑line options
| Option        | Description                                                           |
| ------------- | --------------------------------------------------------------------- |
| `--seed N`    | Specify a random seed (unsigned integer); auto‑generated if omitted   |
| `--out FILE`  | Output file name (default `worldmap.bin`)                             |
| `--text`      | Output in text format (`WORLDMAP` header, all layer values space‑separated) |
| `--help`      | Show usage (no such option, but a brief hint is printed on invalid arguments) |

---

## 📂 Output File Format

### Binary format (default)
- **Header**: `WORLDMAPBIN` (11‑byte ASCII for quick identification)
- **Followed by**: `int32_t width`, `int32_t height`, `int32_t layers` (fixed to `24`)
- **Layer name table**: for each layer, first write `uint16_t` length, then the name string (in the same order as `all_layer_names`)
- **Data section**: contiguous `double` array, arranged as `[layer][y][x]` (i.e., `layer` is the outermost dimension)

> The `load_world()` function in the simulator already supports this format and can read it directly.

### Text format (`--text`)
- First line: `WORLDMAP`
- Second line: `width height layers`
- Then, for each layer in order: one line with the layer name, followed by all `height` rows, each containing `width` floating‑point numbers separated by spaces.

---

## 🧩 Layer List (24 layers)

| Index | Name               | Description (units / meaning)                     |
|-------|--------------------|---------------------------------------------------|
| 0     | `ELEVATION`        | Elevation (metres)                                |
| 1     | `TEMP`             | Mean annual temperature (°C)                      |
| 2     | `PRECIP`           | Annual precipitation (mm)                         |
| 3     | `BIOME`            | Biome (enum `Biome`)                              |
| 4     | `LAND_OCEAN`       | Land (1) / Ocean (0)                              |
| 5     | `TERRAIN`          | Terrain type (enum `TerrainClass`)                |
| 6     | `VEGETATION`       | Vegetation type (enum `VegetationClass`)          |
| 7     | `SOIL_FERT`        | Soil fertility (0–5)                              |
| 8     | `MINERAL`          | Mineral type (0 = none, 1–4 for different ores)   |
| 9     | `BEDROCK`          | Bedrock type (enum `Bedrock`)                     |
| 10    | `GROUNDWATER`      | Groundwater relative abundance (0–1)              |
| 11    | `COASTTYPE`        | Coast type (sand / cliff / fjord / marsh)         |
| 12    | `VOLCANIC`         | Volcanic activity (none / dormant / active)       |
| 13    | `WILDLIFE`         | Wildlife potential (none / low / medium / high)   |
| 14    | `WIND_U`           | Zonal wind speed (m/s)                            |
| 15    | `WIND_V`           | Meridional wind speed (m/s)                       |
| 16    | `PET`              | Potential evapotranspiration (mm)                 |
| 17    | `SLOPE`            | Slope (m/m)                                       |
| 18    | `ARIDITY`          | Aridity index (precipitation / PET)               |
| 19    | `NPP`              | Net primary productivity (g C/m²/yr)              |
| 20    | `CRUST_THICK`      | Crustal thickness (km)                            |
| 21    | `DIST_OCEAN`       | Distance to ocean (number of cells, BFS‑computed) |
| 22    | `CONTINENTALITY`   | Continentality (0–1)                              |
| 23    | `MOISTURE`         | Atmospheric moisture content (0–1)                |

---

## ⚙️ Tunable Parameters (located at the top of the code)

All key parameters are defined as `constexpr` at the beginning of the file. You may adjust them if needed (but **not recommended** unless you fully understand the consequences; otherwise, compatibility with the simulator may break):

- **Map dimensions**: `OUT_WIDTH`, `OUT_HEIGHT`, `BUFFER` (must match the simulator’s `WORLD_WIDTH` etc.; default is 600×600 with a 150‑cell buffer, giving a total size of 900×900).
- **Noise parameters**: `OCTAVES`, `PERSISTENCE`, `LACUNARITY`, `NOISE_SCALE_BASE`.
- **Climate parameters**: thresholds for temperature, precipitation, evapotranspiration, etc.
- **Geological parameters**: number of plates, speeds, crustal thickness, etc.

> If you change these, please update the corresponding constants in `Civilization7.1.cpp` as well; otherwise the simulator may not read the layers correctly.

---

## 🧪 Testing Suggestions

1. Run `./weather` to generate the default `worldmap.bin`.
2. Load the map in the simulator and observe whether civilisations evolve normally.
3. To inspect the map contents, use `--text` to output a text format and then check the value ranges of each layer with a script or editor (e.g., elevation should be between about –11000 and 8500, temperature between –50 and 50, precipitation non‑negative, etc.).

---

## 📝 Notes

- This program **does not perform erosion** (as per the original request), so the terrain is relatively “fresh” but retains all other geological processes.
- Random numbers are generated with `std::mt19937`; specifying a seed with `--seed` ensures complete reproducibility.
- The output file is large (binary about 24×900×900×8 ≈ 155 MB), so make sure you have enough disk space.

---

## 🤝 Contributing & Feedback

If you encounter any problems or have suggestions for improvement, feel free to open an Issue or Pull Request. However, **please note**: this tool aims to maintain strict compatibility with the Civilization simulator; any modification requests should include a rationale and must not alter the existing output format or algorithmic logic.
If you have a strong reason to change the format, feel free to do so, and I will consider adapting my `Civilization` code accordingly.

---

## ⚖️ License

This tool shares the **MIT License** with the Civilization simulator; see the [LICENSE](LICENSE) file for details.

---

## A Note from the Author

This `README.md` was generated with the help of AI (I’m not good at writing these).  
I’m a high school student with limited knowledge, so I used AI for assistance.  
For detailed author information, please see the “A Note from the Author” section in the main project README:  
[-The-Nameless-Legend-/README.md at Civilization · TheOne-Logos/-The-Nameless-Legend-](https://github.com/TheOne-Logos/-The-Nameless-Legend-/blob/Civilization/README.md#a-note-from-the-author)

---

**Generate your maps and begin your civilisation journey!** 🗺️
