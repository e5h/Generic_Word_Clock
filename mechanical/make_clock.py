import os
import subprocess

# Files to use
input_layout = "make_clock_body_characters.txt"
openscad_generator = "make_clock_body.scad"
output_stl = "Custom_Body.stl"

# (1) Parsing the layout file
with open(input_layout, "r") as f:
    lines = f.readlines()

start = lines.index("<BEGIN>\n") + 1
end = lines.index("<END>")
clock_lines = lines[start:end]

clock_array = [line.strip().split() for line in clock_lines]

print("Generating a clock with the following layout:\n")

clock_array_str = "[\n" + ",\n".join(
    ["  [" + ", ".join([f"\"{char}\"" for char in row]) + "]" for row in clock_array]
) + "\n]"

print(clock_array_str)

font = "Secrets Stencil"
font_size = 11

command = [
    "openscad",
    "-o", output_stl,
    openscad_generator,
    f"-DARG_clock_layout={clock_array_str}",
    f"-DARG_font=\"{font}\"",
    f"-DARG_font_size={font_size}",
]

print("Calling OpenSCAD process...\n")
try:
    subprocess.run(command, check=True)
    print(f"Exported custom clock body: {output_stl}")
except subprocess.CalledProcessError as e:
    print(f"Failed to generate clock body! Error: {e}")