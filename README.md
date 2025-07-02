# HyprDisplays

HyprDisplays is a modern Qt6-based graphical utility for arranging and configuring monitors for the Hyprland Wayland compositor. It is a spiritual successor to nwg-displays, designed specifically for Hyprland, with advanced support for HDR, 10-bit color, wide gamut, SDR brightness/saturation, VRR, and more.

## Features

- **Drag-and-drop monitor arrangement** with real-time position feedback
- **Per-monitor settings panel** for resolution, refresh rate, scale, transform, and more
- **Advanced Hyprland options:**
  - **HDR** (enable/disable per monitor)
  - **10-bit color** (bitdepth,10)
  - **Wide gamut** (cm,wide)
  - **SDR brightness** (sdrbrightness)
  - **SDR saturation** (sdrsaturation)
  - **VRR** (variable refresh rate)
- **Real-time position editing:**
  - X/Y position spinboxes update as you drag or edit
  - Manual entry of monitor positions
- **Configuration is saved in Hyprland's native format** (`monitor=...` lines)
- **Workspace assignment UI**
- **Automatic normalization:** top-left monitor is always at 0x0
- **Beautiful, modern Qt6 interface**

## Dependencies

To build HyprDisplays, you need:

- **Qt 6** (Core, Widgets, Gui, etc.)
- **CMake** (>= 3.16 recommended)
- **A C++20 compiler** (GCC, Clang, or MSVC)
- **Hyprland** (for runtime usage)
- **pkg-config** (for build)
- **Wayland development headers** (for some features)

On Arch Linux:
```sh
sudo pacman -S qt6-base qt6-tools cmake gcc pkgconf wayland-protocols
```
## Building from Source

```sh
git clone https://github.com/ryzendew/HyprDisplays.git
cd HyprDisplays
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Usage

- Run `./hyprdisplays`
- Arrange your monitors visually
- Click a monitor to edit its settings (resolution, refresh, scale, HDR, 10-bit, wide gamut, etc.)
- Use the X/Y spinboxes for precise positioning
- Click **Apply** to save to `~/.config/hypr/monitors.conf`
- Reload Hyprland or use `hyprctl reload` to apply changes

## Advanced Monitor Options

- **HDR:** Enable HDR output for supported monitors
- **10-bit:** Set output to 10-bit color depth (`bitdepth,10`)
- **Wide Gamut:** Enable wide color gamut (`cm,wide`)
- **SDR Brightness/Saturation:** Fine-tune SDR content appearance in HDR mode
- **VRR:** Set variable refresh rate mode (off, global, fullscreen only)
- **All options are saved in Hyprland's native config format**

## License

MIT

---

HyprDisplays is not affiliated with the Hyprland project, but is designed to be the best graphical monitor tool for Hyprland users. 
