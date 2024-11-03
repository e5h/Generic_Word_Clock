/* Variables */

// This is a default layout. Override it with a script.
ARG_clock_layout = [
  ["I", "T", "U", "I", "S", "J", "A", "X", "M", "Y"],
  ["T", "W", "E", "N", "T", "Y", "H", "A", "L", "F"],
  ["H", "A", "P", "P", "Y", "H", "F", "I", "V", "E"],
  ["T", "E", "N", "Q", "U", "A", "R", "T", "E", "R"],
  ["T", "O", "P", "A", "S", "T", "B", "D", "A", "Y"],
  ["O", "N", "E", "F", "O", "U", "R", "T", "W", "O"],
  ["F", "I", "V", "E", "S", "I", "X", "T", "E", "N"],
  ["T", "H", "R", "E", "E", "S", "E", "V", "E", "N"],
  ["T", "W", "E", "L", "V", "E", "I", "G", "H", "T"],
  ["N", "I", "N", "E", "L", "E", "V", "E", "N", "J"]
];

/*
Requires a stencil-type font (no closed off areas)

Some good options:
"CF StencilOrama" (personal use only)
"Crevice Stencil" (personal use only)
"Qawine Again" (personal use only)
"WC Wunderbach BTA"
"Secrets Stencil"
*/
ARG_font = "WC Wunderbach BTA";
ARG_font_size = 12;

// Total width of the body (square)
clock_width = 194.6;//mm

// Thickness of the body
clock_thickness = 15.0;//mm

// Total width of the divider (square)
divider_width = 168.0;//mm

// Thickness of the divider walls
divider_wall_thickness = 2.0;//mm

// Spacing between characters
char_spacing = 16.6;//mm

// Character bound box
char_bound_box = 14.6;//mm

// Movement constants
text_cut_slop = 5;//mm

move_clock_x = -clock_width/2;
move_clock_y = clock_thickness;
move_clock_z = -clock_width/2;

move_all_text_offset = divider_wall_thickness + char_bound_box/2;
move_all_text_x = move_all_text_offset - (divider_width/2);
move_all_text_y = clock_thickness - text_cut_slop;
move_all_text_z = (divider_width/2) - move_all_text_offset;

/* Parts */

// Blank clock body
module blank_clock_body()
{
  translate([move_clock_x, move_clock_y, move_clock_z])
  {
    mirror([0, 1, 0])
    {
      import("Generic_Body_NoText.STL");
    }
  }
}

// Individual extruded character
module extruded_char(char)
{
  linear_extrude(height = clock_thickness + text_cut_slop)
  {
    text(
      char,
      font = ARG_font,
      size = min(ARG_font_size, char_bound_box - 2),
      halign = "center",
      valign = "center"
    );
  }
}

// Grid of all characters
module extruded_clock_chars()
{
  for (row = [0 : len(ARG_clock_layout) - 1])
  {
    for (col = [0 : len(ARG_clock_layout[row]) - 1])
    {
      translate([col*char_spacing, -row*char_spacing, 0])
      {
        extruded_char(ARG_clock_layout[row][col]);
      }
    }
  }
}

// The clock characters object
module clock_chars()
{
  translate([move_all_text_x, move_all_text_y, move_all_text_z])
  {
    rotate([90, 0, 0])
    {
      extruded_clock_chars();
    }
  }
}


/* Operations */

// Subtracting a text grid from the blank clock body
difference()
{
  // Blank clock body
  blank_clock_body();
  // Text object
  clock_chars();
}