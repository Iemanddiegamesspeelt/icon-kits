# Icon Kits

A [Geode](https://geode-sdk.org/) mod for **Geometry Dash** that lets you save unlimited icon presets and instantly switch between them.

## Features

- **Unlimited Kits** — Save as many icon presets as you want
- **Full Cosmetic Capture** — Cube, Ship, Ball, UFO, Wave, Robot, Spider, Swing, Jetpack, Colors, Glow, Death Effect, Streak, Ship Fire
- **One-Click Load** — Apply any saved kit instantly
- **Favorites** — Star your go-to kits for quick access
- **Search** — Find kits by name
- **Sort** — By name, date, or favorites first
- **Random Kit** — Feeling lucky? Apply a random preset
- **Duplicate** — Clone any kit as a starting point
- **Rename** — Give your kits descriptive names
- **Import / Export** — Share kits via clipboard as JSON
- **Auto-Save** — All changes persist automatically

## Usage

1. Open the **Garage** (Icon Kit screen) in Geometry Dash
2. Click the **"Kits"** button (added to the garage UI)
3. Use the popup to create, load, and manage your icon kits

## Building

### Requirements

- [Geode SDK](https://geode-sdk.org/) v5.8.2+
- CMake 3.21+
- C++20 compiler

### Build Steps

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Build
cmake --build build --config RelWithDebInfo
```

Or use the Geode CLI:

```bash
geode build
```

## License

This project is provided as-is for the Geometry Dash modding community.
