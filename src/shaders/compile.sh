#!/usr/bin/env bash
set -euo pipefail

# Find glslc: prefer PATH, then VULKAN_SDK, then a common local fallback.
if command -v glslc >/dev/null 2>&1; then
GLSLC="glslc"
elif [ -n "${VULKAN_SDK:-}" ] && [ -x "$VULKAN_SDK/bin/glslc" ]; then
GLSLC="$VULKAN_SDK/bin/glslc"
elif [ -x "$HOME/VulkanSDK/1.4.328.1/macOS/bin/glslc" ]; then
GLSLC="$HOME/VulkanSDK/1.4.328.1/macOS/bin/glslc"
else
echo "Error: glslc not found. Ensure it is in PATH or set VULKAN_SDK." >&2
exit 1
fi

# Compile all .vert and .frag shaders in this directory.
for src in *.vert *.frag; do
    [ -e "$src" ] || continue
    "$GLSLC" "$src" -o "$src.spv"
done

echo "Shader compilation complete."