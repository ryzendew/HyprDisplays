# HyprDisplays

A modern Qt6 C++ display management utility for Hyprland, inspired by [nwg-displays](https://github.com/nwg-piotr/nwg-displays).

## Features

- **Modern Qt6 Interface**: Clean, responsive GUI built with Qt6 and C++
- **Hyprland Integration**: Native support for Hyprland display management
- **Real-time Display Management**: Configure monitors, resolutions, refresh rates, and scaling
- **Workspace Assignment**: Assign workspaces to specific displays
- **Drag & Drop Interface**: Intuitive display positioning with visual feedback
- **Configuration Management**: Save and load display configurations
- **Auto-apply Support**: Option to automatically apply changes
- **Overlay Indicators**: Visual screen identification during configuration
- **Backup & Restore**: Create and restore configuration backups
- **Import/Export**: Support for importing from nwg-displays configurations

## Screenshots

*Screenshots will be added once the application is functional*

## Installation

### Prerequisites

- Qt6 (Core, Widgets, Gui)
- CMake 3.16 or higher
- C++17 compatible compiler
- Hyprland running on your system

### Building from Source

1. Clone the repository:
```bash
git clone https://github.com/yourusername/HyprDisplays.git
cd HyprDisplays
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Configure and build:
```bash
cmake ..
make -j$(nproc)
```

4. Install (optional):
```bash
sudo make install
```

### Package Dependencies

#### Arch Linux / Manjaro
```bash
sudo pacman -S qt6-base cmake gcc
```

#### Ubuntu / Debian
```bash
sudo apt install qt6-base-dev cmake build-essential
```

#### Fedora
```bash
sudo dnf install qt6-qtbase-devel cmake gcc-c++
```

## Usage

### Basic Usage

```bash
hyprdisplays
```

### Command Line Options

```bash
hyprdisplays -h
```

Available options:
- `-m, --monitors-path PATH`: Path to save monitors.conf (default: ~/.config/hypr/monitors.conf)
- `-n, --num-ws NUMBER`: Number of workspaces (default: 10)
- `-v, --version`: Display version information
- `-h, --help`: Show help message

### Hyprland Configuration

Add these lines to your Hyprland config (`~/.config/hypr/hyprland.conf`):

```conf
# Monitor configuration
source = ~/.config/hypr/monitors.conf

# Workspace assignments
source = ~/.config/hypr/workspaces.conf
```

**Note**: Do not set `disable_autoreload true` in Hyprland settings, or you'll need to manually reload Hyprland after applying changes.

## Configuration

### Application Settings

The application settings are stored in `~/.config/HyprDisplays/config.json`:

```json
{
  "autoApply": false,
  "showOverlay": true,
  "overlayTimeout": 500,
  "minimizeToTray": false,
  "startMinimized": false,
  "autoSave": true,
  "autoBackup": true,
  "maxBackups": 5
}
```

### Monitor Configuration

Monitor configurations are saved to `~/.config/hypr/monitors.conf` in Hyprland format:

```conf
monitor=DP-1,1920x1080@60,0x0,1
monitor=HDMI-A-1,2560x1440@144,1920x0,1.5
monitor=eDP-1,1920x1080@60,4480x0,1,transform,90
```

### Workspace Configuration

Workspace assignments are saved to `~/.config/hypr/workspaces.conf`:

```conf
workspace=1,monitor:DP-1
workspace=2,monitor:DP-1
workspace=3,monitor:HDMI-A-1
workspace=4,monitor:HDMI-A-1
workspace=5,monitor:eDP-1
```

## Features in Detail

### Display Management

- **Monitor Detection**: Automatically detects connected displays
- **Resolution Settings**: Configure native and custom resolutions
- **Refresh Rate**: Set optimal refresh rates for each display
- **Scaling**: Adjust display scaling (100%, 125%, 150%, 200%, custom)
- **Transformations**: Rotate and flip displays
- **Positioning**: Drag and drop displays to arrange them
- **Mirroring**: Set up display mirroring configurations

### Workspace Management

- **Workspace Assignment**: Assign workspaces to specific displays
- **Dynamic Workspaces**: Create and manage workspace configurations
- **Workspace Naming**: Custom workspace names and labels

### Advanced Features

- **Real-time Updates**: Live display information updates
- **Configuration Validation**: Validate settings before applying
- **Error Handling**: Comprehensive error reporting and recovery
- **Logging**: Detailed logging for troubleshooting
- **Backup System**: Automatic and manual configuration backups

## Development

### Project Structure

```
HyprDisplays/
├── src/
│   ├── main.cpp              # Application entry point
│   ├── mainwindow.h/cpp      # Main application window
│   ├── displaymanager.h/cpp  # Display management logic
│   ├── displaywidget.h/cpp   # Individual display widgets
│   ├── hyprlandinterface.h/cpp # Hyprland communication
│   ├── configmanager.h/cpp   # Configuration management
│   └── mainwindow.ui         # Qt Designer UI file
├── CMakeLists.txt            # Build configuration
├── hyprdisplays.desktop      # Desktop entry
├── hyprdisplays.svg          # Application icon
└── README.md                 # This file
```

### Building for Development

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

### Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## Troubleshooting

### Common Issues

**Hyprland not detected**
- Ensure Hyprland is running
- Check that `hyprctl` is available in PATH
- Verify Wayland session is active

**Displays not detected**
- Check display connections
- Verify graphics drivers are installed
- Run `hyprctl monitors` to test detection

**Configuration not applying**
- Ensure Hyprland autoreload is enabled
- Check file permissions for config directories
- Verify config file syntax

**Build errors**
- Ensure Qt6 is properly installed
- Check CMake version compatibility
- Verify C++17 compiler support

### Debug Mode

Run with debug output:
```bash
QT_LOGGING_RULES="*.debug=true" hyprdisplays
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by [nwg-displays](https://github.com/nwg-piotr/nwg-displays) by Piotr Miller
- Built with Qt6 framework
- Designed for Hyprland compositor

## Roadmap

- [ ] Multi-language support
- [ ] Plugin system for custom display features
- [ ] Advanced color calibration
- [ ] Display profile management
- [ ] Integration with other Wayland compositors
- [ ] Automated testing suite
- [ ] Performance optimizations

## Support

For support and questions:
- Open an issue on GitHub
- Check the troubleshooting section
- Review Hyprland documentation

---

**Note**: This is a work in progress. Some features may not be fully implemented yet. 