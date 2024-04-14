#!/usr/bin/env python3
""" genpwd - generate a password. """
import sys


def from_hex(hexv):
    """ Convert XXXXXX to integer form.
        Args:
            hexv - hex value
    """
    if len(hexv) != 6:
        print(f"Invalid hex value: {hexv}")
        sys.exit(1)

    r_value = int(hexv[:2], 16)
    g_value = int(hexv[2:4], 16)
    b_value = int(hexv[4:], 16)
    return r_value, g_value, b_value


def from_int(r_cmp, g_cmp, b_cmp):
    """ Convert integer to fittet integer.
        Args:
            r_cmp - red component
            g_cmp - green component
            b_cmp - blue component
    """
    r_value = cmp_to_int(r_cmp, "Red")
    g_value = cmp_to_int(g_cmp, "Green")
    b_value = cmp_to_int(b_cmp, "Blue")
    return r_value, g_value, b_value


def cmp_to_int(cmp, name):
    """ Convert component to integer.
        Args:
            cmp - component
            name - component name
    """
    value = int(cmp)
    if value < 0 or value > 255:
        print(f"{name} component invalid")
        sys.exit(1)

    return value


def from_opengl(r_cmp, g_cmp, b_cmp):
    """ Convert opengl to fittet integer.
        Args:
            r_cmp - red component
            g_cmp - green component
            b_cmp - blue component
    """
    r_value = opengl_to_int(r_cmp, "Red")
    g_value = opengl_to_int(g_cmp, "Green")
    b_value = opengl_to_int(b_cmp, "Blue")
    return r_value, g_value, b_value


def opengl_to_int(cmp, name):
    """ Convert opengl to integer.
        Args:
            cmp - component
            name - component name
    """
    value = float(cmp)
    if value < 0.0 or value > 1.0:
        print(f"{name} component invalid")
        sys.exit(1)

    return int(value * 256)


def truncate_float(float_number, decimal_places):
    """ Truncate a float to "decimal_places".
        Args:
            float_number - float to truncate
            decimal_placers - ... to decimal places
    """
    multiplier = 10 ** decimal_places
    return int(float_number * multiplier) / multiplier


if __name__ == "__main__":
    ARG_CNT = len(sys.argv)

    if ARG_CNT not in (2, 4):
        print("Usage: colconv.py xxxxxx | r g b | r-gl g-gl b-gl")
        print("[OPTIONS]")
        print(" xxxxxx          Hex code")
        print(" r g b           Red, green and blue components as integers")
        print(" r-gl g-gl b-gl  Red, green and blue components in OpenGL form")
        sys.exit(0)

    if ARG_CNT == 2:
        v1, v2, v3 = from_hex(sys.argv[1])
    else:
        if "." in sys.argv[1]:
            v1, v2, v3 = from_opengl(sys.argv[1], sys.argv[2], sys.argv[3])
        else:
            v1, v2, v3 = from_int(sys.argv[1], sys.argv[2], sys.argv[3])

    rehex = hex(v1 << 16 | v2 << 8 | v3)
    print(f"Hexcolor:   {rehex}")
    print(f"RGB:        {v1}, {v2}, {v3}")
    gv1 = truncate_float(v1 / 255.0, 2)
    gv2 = truncate_float(v2 / 255.0, 2)
    gv3 = truncate_float(v3 / 255.0, 2)
    print(f"OpenGL ES:  {gv1:.2f}, {gv2:.2f}, {gv3:.2f}")
