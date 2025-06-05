def rgb_to_rgb332(r, g, b):
    # Reduce each channel
    r3 = (r >> 5) & 0x07  # 3 bits
    g3 = (g >> 5) & 0x07  # 3 bits
    b2 = (b >> 6) & 0x03  # 2 bits

    return (r3 << 5) | (g3 << 2) | b2


def rgb332_to_rgb565(rgb332):
    # Extract RGB332 components
    r3 = (rgb332 >> 5) & 0x07
    g3 = (rgb332 >> 2) & 0x07
    b2 = rgb332 & 0x03

    # Scale to RGB565 (5-6-5)
    r5 = (r3 * 255 // 7) >> 3  # 0–31
    g6 = (g3 * 255 // 7) >> 2  # 0–63
    b5 = (b2 * 255 // 3) >> 3  # 0–31

    return (r5 << 11) | (g6 << 5) | b5


def hex_to_rgb(hex_value):
    """hex_value: 0xRRGGBB"""
    r = (hex_value >> 16) & 0xFF
    g = (hex_value >> 8) & 0xFF
    b = hex_value & 0xFF
    return r, g, b

# --- Example usage ---


hex_color = 0xdedede  # Orange-ish
r, g, b = hex_to_rgb(hex_color)

rgb332 = rgb_to_rgb332(r, g, b)
rgb565 = rgb332_to_rgb565(rgb332)

print(f"RGB: ({r}, {g}, {b})")  # Print RGB values
print(f"RGB332: {rgb332:#04x}")  # Print as hex
print(f"RGB565: {rgb565:#06x}")  # Print as hex
