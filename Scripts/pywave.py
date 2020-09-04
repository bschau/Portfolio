#!/usr/bin/python3
import math

amplitude = 100
table_length = 256
max_entries = 16

def runner(table_name, func, amplitude, table_length, max_entries):
    step = (2 * math.pi) / table_length
    point = 0
    entry = 0
    print("static int {}[{}] = {}\n\t".format(table_name,
        table_length, '{'), end="")
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

print("#ifndef WAVE_TABLE_H\n#define WAVE_TABLE_H\n")
runner("sin_table", math.sin, amplitude, table_length, max_entries)
runner("cos_table", math.cos, amplitude, table_length, max_entries)

print("#endif\n")
