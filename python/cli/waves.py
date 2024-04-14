#!/usr/bin/env python3
""" Waves - calculate sin + cos waves. """
import math
import sys


def runner(table_name, func, amplitude, table_length, max_entries):
    """ Do the calculations.
            table_name - 'cos' or 'sin' name.
            func - which function to calculate.
            amplitude - amplitude.
            table_length - table length.
            max_entries - max entries pr. line.
    """
    step = (2 * math.pi) / table_length
    point = 0
    entry = 0
    print(f"static int {table_name}[{table_length}] =")
    print("{\n\t", end="")
    for i in range(0, table_length):
        value = int((func(point) * amplitude) + amplitude)
        print(value, end="")
        entry = entry + 1
        if entry == max_entries:
            entry = 0
            if i == table_length - 1:
                print("\n", end="")
            else:
                print(",\n\t", end="")
        else:
            if i < table_length - 1:
                print(", ", end="")
            else:
                print()

        point = point + step

    print("}\n")

if __name__ == "__main__":
    AMPLITUDE = 100
    TABLE_LENGTH = 256
    MAX_ENTRIES = 16

    ARG_CNT = len(sys.argv)
    if ARG_CNT < 2 or ARG_CNT > 4:
        print("Usage: waves.py amplitude [table_length [max_entries]]")
        print("Defaults:")
        print(f"  amplitude:    {AMPLITUDE}")
        print(f"  table length: {TABLE_LENGTH}")
        print(f"  max entries:  {MAX_ENTRIES}")
        sys.exit(0)

    if ARG_CNT == 2:
        AMPLITUDE = int(sys.argv[1])
    elif ARG_CNT == 3:
        AMPLITUDE = int(sys.argv[1])
        TABLE_LENGTH = int(sys.argv[2])
    elif ARG_CNT == 4:
        AMPLITUDE = int(sys.argv[1])
        TABLE_LENGTH = int(sys.argv[2])
        MAX_ENTRIES = int(sys.argv[3])

    print("#ifndef WAVE_TABLE_H\n#define WAVE_TABLE_H\n")
    runner("sin_table", math.sin, AMPLITUDE, TABLE_LENGTH, MAX_ENTRIES)
    runner("cos_table", math.cos, AMPLITUDE, TABLE_LENGTH, MAX_ENTRIES)
    print("#endif\n")
