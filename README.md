# HyprDisplays

![image](https://github.com/user-attachments/assets/f10cc040-60df-423a-8192-6725915fdbd0)


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

### Required Dependencies

**Build Dependencies:**
- **Qt 6** (Core, Widgets, Gui)
- **CMake** (>= 3.16 recommended)
- **C++17 compiler** (GCC, Clang, or MSVC)
- **pkg-config** (for build configuration)
- **Wayland protocols** (for Wayland integration)
- **Vulkan headers** (for Qt6 ShaderTools support)

**Runtime Dependencies:**
- **Hyprland** (for monitor management)
- **hyprctl** (for Hyprland communication)

### Optional Dependencies

**For Enhanced Features:**
- **Vulkan utility libraries** (for advanced graphics features)
- **SPIR-V tools** (for shader compilation)

### Installation

**Arch Linux / Manjaro:**
```sh
# Required dependencies
sudo pacman -S qt6-base qt6-tools cmake gcc pkgconf wayland-protocols hyprland vulkan-headers

# Optional dependencies (for enhanced features)
sudo pacman -S vulkan-utility-libraries vulkan-extra-layers vulkan-extra-tools spirv-headers
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

## Troubleshooting

### Common Issues

**Segmentation Fault on Startup:**
- Ensure Hyprland is running and `hyprctl` is available
- Check that all Qt6 dependencies are properly installed
- Run with debug output: `QT_LOGGING_RULES="*=true" ./hyprdisplays`

**Build Errors:**
- **"wrapvulkanheaders not found"**: Install Vulkan development packages (see Optional Dependencies above)
- **"Qt6 not found"**: Ensure Qt6 development packages are installed for your distribution
- **"CMake version too old"**: Update CMake to version 3.16 or higher

**Runtime Issues:**
- **"Disconnected from Hyprland"**: Ensure Hyprland is running and `hyprctl version` works
- **"No monitors detected"**: Check that your monitors are properly connected and detected by Hyprland
- **"Permission denied"**: Ensure you have write access to `~/.config/hypr/`

### Debug Mode

Run the application with debug output to diagnose issues:
```bash
QT_LOGGING_RULES="*=true" ./hyprdisplays
```

### Testing Environment

Use the included debug script to check your environment:
```bash
chmod +x debug_test.sh
./debug_test.sh
```

## License

MIT

---

HyprDisplays is not affiliated with the Hyprland project, but is designed to be the best graphical monitor tool for Hyprland users. 
