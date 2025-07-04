#!/bin/bash

echo "=== HyprDisplays Debug Test ==="
echo "Testing environment and dependencies..."

# Check if we're in a Wayland session
echo "1. Checking Wayland session..."
if [ -n "$WAYLAND_DISPLAY" ]; then
    echo "   ✓ Running in Wayland session: $WAYLAND_DISPLAY"
else
    echo "   ⚠ Not in Wayland session (this might be expected)"
fi

# Check if hyprctl is available
echo "2. Checking hyprctl availability..."
if command -v hyprctl >/dev/null 2>&1; then
    echo "   ✓ hyprctl found: $(which hyprctl)"
    echo "   Testing hyprctl version..."
    if hyprctl version >/dev/null 2>&1; then
        echo "   ✓ hyprctl version command works"
    else
        echo "   ✗ hyprctl version command failed"
    fi
else
    echo "   ✗ hyprctl not found"
fi

# Check if we're running Hyprland
echo "3. Checking if Hyprland is running..."
if pgrep -x "Hyprland" >/dev/null; then
    echo "   ✓ Hyprland is running"
else
    echo "   ⚠ Hyprland is not running (this might cause issues)"
fi

# Check Qt installation
echo "4. Checking Qt installation..."
if command -v qmake >/dev/null 2>&1; then
    echo "   ✓ qmake found: $(which qmake)"
    qmake_version=$(qmake -query QT_VERSION 2>/dev/null)
    if [ -n "$qmake_version" ]; then
        echo "   ✓ Qt version: $qmake_version"
    else
        echo "   ⚠ Could not determine Qt version"
    fi
else
    echo "   ✗ qmake not found"
fi

# Check if the executable exists
echo "5. Checking HyprDisplays executable..."
if [ -f "./hyprdisplays" ]; then
    echo "   ✓ hyprdisplays executable found"
    echo "   File size: $(ls -lh ./hyprdisplays | awk '{print $5}')"
    echo "   File type: $(file ./hyprdisplays)"
else
    echo "   ✗ hyprdisplays executable not found"
    echo "   Looking for build directory..."
    if [ -d "build" ]; then
        if [ -f "build/hyprdisplays" ]; then
            echo "   ✓ Found in build/hyprdisplays"
            echo "   File size: $(ls -lh build/hyprdisplays | awk '{print $5}')"
            echo "   File type: $(file build/hyprdisplays)"
        else
            echo "   ✗ Not found in build directory"
        fi
    else
        echo "   ✗ Build directory not found"
    fi
fi

echo ""
echo "=== Running with debug output ==="

# Try to run with debug output
if [ -f "./hyprdisplays" ]; then
    echo "Running: ./hyprdisplays"
    QT_LOGGING_RULES="*=true" ./hyprdisplays 2>&1 | head -20
elif [ -f "build/hyprdisplays" ]; then
    echo "Running: build/hyprdisplays"
    QT_LOGGING_RULES="*=true" build/hyprdisplays 2>&1 | head -20
else
    echo "No executable found to test"
fi

echo ""
echo "=== Environment variables ==="
echo "DISPLAY: $DISPLAY"
echo "WAYLAND_DISPLAY: $WAYLAND_DISPLAY"
echo "XDG_SESSION_TYPE: $XDG_SESSION_TYPE"
echo "QT_QPA_PLATFORM: $QT_QPA_PLATFORM"

echo ""
echo "=== Debug test completed ===" 