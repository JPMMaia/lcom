#include "Sprites.h"

char* mouseCursorSprite[33] =
{
		"30 30 2",
		". 0",
		"+ 7",

		"++..........................++", // 01
		"+++........................+++", // 02
		".++++++++++++++++++++++++++++.", // 03
		"..++++++++++++++++++++++++++..", // 04
		"..++......................++..", // 05
		"..++......................++..", // 06
		"..++......................++..", // 07
		"..++......................++..", // 08
		"..++......................++..", // 09
		"..++......................++..", // 10
		"..++......................++..", // 11
		"..++......................++..", // 12
		"..++......................++..", // 12
		"..++......................++..", // 13
		"..++......................++..", // 14
		"..++......................++..", // 15
		"..++......................++..", // 16
		"..++......................++..", // 17
		"..++......................++..", // 18
		"..++......................++..", // 19
		"..++......................++..", // 20
		"..++......................++..", // 21
		"..++......................++..", // 22
		"..++......................++..", // 23
		"..++......................++..", // 24
		"..++......................++..", // 25
		"..++......................++..", // 26
		"..++++++++++++++++++++++++++..", // 27
		".++++++++++++++++++++++++++++.", // 28
		"+++........................+++", // 29
		"++..........................++", // 30
};

char *pic1[] =
{
		"32 13 4",
		". 0",
		"x 2",
		"o 14",
		"+ 4",
		"................................",
		"..............xxx...............",
		"............xxxxxxx.............",
		".........xxxxxx+xxxxxx..........",
		"......xxxxxxx+++++xxxxxxx.......",
		"....xxxxxxx+++++++++xxxxxxx.....",
		"....xxxxxxx+++++++++xxxxxxx.....",
		"......xxxxxxx+++++xxxxxxx.......",
		".........xxxxxx+xxxxxx..........",
		"..........ooxxxxxxxoo...........",
		".......ooo...........ooo........",
		".....ooo...............ooo......",
		"...ooo...................ooo...."
};


char *pic2[] =
{
		"32 15 3",
		". 0",
		"x 4",
		"+ 1",
		"................................",
		"..............xxx...............",
		"............xxxxxxx.............",
		".........xxxxxx+xxxxxx..........",
		"......xxxxxxx+++++xxxxxxx.......",
		"......xxxxxxx+++++xxxxxxx.......",
		"++++xxxxxxx+++++++++xxxxxxx+++++",
		"++++xxxxxxx+++++++++xxxxxxx+++++",
		"++++xxxxxxx+++++++++xxxxxxx+++++",
		"......xxxxxxx+++++xxxxxxx.......",
		"......xxxxxxx+++++xxxxxxx.......",
		".........xxxxxx+xxxxxx..........",
		"............xxxxxxx.............",
		"..............xxx...............",
		"................................",
};

char *cross[35] =
{
		"32 32 2",
		". 0",
		"+ 13",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		".............++.++..............",
		"............+.....+.............",
		"...........+.......+............",
		"+++++++++++.........++++++++++++",
		"...........+.......+............",
		"............+.....+.............",
		".............++.++..............",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
		"...............+................",
};


char *pic3[36] =
{
		"64 32 3",
		". 0",
		"+ 1",
		"o 2",
		"..............................++++++............................",
		".........................++++++++++++++++.......................",
		"......................++++++++++++++++++++++....................",
		"....................++++++++++++++++++++++++++..................",
		".................++++++++++++++++++++++++++++++++...............",
		"...............++++++++++++++++++++++++++++++++++++.............",
		".............++++++++++++++++++++++++++++++++++++++++...........",
		"............++++++++++++++++++++++++++++++++++++++++++..........",
		"..........++++++++++++++++++++++++++++++++++++++++++++++........",
		".........++++++++++++++++++++++++++++++++++++++++++++++++.......",
		"........+++++++++++++++++++++++++++++++++++++++++++++++++.......",
		".......+++++++++++++++++++++++++++++++++++++++++++++++++++......",
		"......+++++++++++++++++++++++++++++++++++++++++++++++++++++.....",
		".....++++++++++++++++++++++++++++++++++++++++++++++++++++++.....",
		"....++++++++++++++++++++++++++++++++++++++++++++++++++++++++....",
		"...++++++++++++++++++++++++++++++++++++++++++++++++++++++++++...",
		"...++++++++++++++++++++++++++++++++++++++++++++++++++++++++++...",
		"..++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++..",
		"..++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++..",
		".++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.",
		".+++++ooo+++++++++ooo+++++++++ooo+++++++++ooo+++++++++ooo++++++.",
		".++++ooooo+++++++ooooo+++++++ooooo+++++++ooooo+++++++ooooo+++++.",
		".+++ooooooo+++++ooooooo+++++ooooooo+++++ooooooo+++++ooooooo++++.",
		".++++ooooo+++++++ooooo+++++++ooooo+++++++ooooo+++++++ooooo+++++.",
		".+++++ooo+++++++++ooo+++++++++ooo+++++++++ooo+++++++++ooo++++++.",
		".++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.",
		".++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.",
		".++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.",
		"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
		"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
		"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
		"++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++",
};

/* this one was converted from a 32 bits colors png! */

char * penguin[] =
{
		"64 64 9",
		"  0",
		". 15",
		"+ 8",
		"@ 14",
		"# 4",
		"$ 6",
		"% 7",
		"& 9",
		"* 1",
		"                                                                ",
		"                                                                ",
		"                           +#++#&+#+                            ",
		"                        +..%......&.+++                         ",
		"                     #...+..+.+.+.#.+.+++                       ",
		"                    .*#%..+.%.&.#*+.#%.++++                     ",
		"                  #.+.#*#%..+.#*#.%#*#+.++++                    ",
		"                 .&.%......#*#.%.+.#&.+%#%++++                  ",
		"                #*#..........+.+.+.+.+.++#+++++                 ",
		"               &#.+..........%.+.&#*#&#*.+&+++++                ",
		"              +.+.............#*#.+.......#++++++               ",
		"             #+.+..............+.+..........+++++               ",
		"            +&.+%..............%+............+++++              ",
		"            ++.+#.............................+++++             ",
		"           ++.+%.............#..#..#..........+++++             ",
		"          +++.++%..........#    .    #........++++++            ",
		"          ++++.+...........#   .     #.....%..+++++++           ",
		"          ++%+#+&..........#   .     #........+++++++           ",
		"         +++#*+#..%.........#  .    #........%+++++++           ",
		"         ++++%+++..%..%......#.#..#...%..%#..+++++++++          ",
		"        +++++#+&+#.......%..................++++++++++          ",
		"        ++++*++.+++.%.#...@@@@@@@@@@@@...%.+++++++++++          ",
		"        +++++#+++%#$#.&.+.%@@@@@@@@@@...#++++++++++++++         ",
		"        +++++&+.++++*.#.....@@@@@@@@......+++++++++&+++         ",
		"       ++++++++++%#%....%....@@@@@@........+++++++++++%         ",
		"       +++++++++++#...........@@@@@.........+++++++++++         ",
		"       ++++++++++..............@@@...........+++++++++++        ",
		"      +++++++++++...............@.............+++++%+++++       ",
		"    ++++++++++++.....................%.........+++++++++++      ",
		"   ++++++++++++%.........................%..%..%++++%++++++     ",
		"  +++++++++++++...............................#.++++++++++++    ",
		" +++++++++++++%..%.........................#.....++++++++++++   ",
		"++++++++++++++........................%..%..%.+.%++%++++++++++  ",
		"++++++++++++++...................................#++%++++++++++ ",
		"    +++++++++...%..%.....................%.+.%#.%.++++++        ",
		"       ++++++%....................%...........$.#.#+#%+         ",
		"       +++++#........................%..#..+.#.%$%+&+++         ",
		"        ++++&.#.%..%....................%...*#.#...+#++         ",
		"        ++++..%...........................%..%.+.+.+&++         ",
		"        +++++...%.....%.........%...%..+..#.#..#.%.+#+          ",
		"        ++++..#...%......%...............&.*.+.*#$#&++          ",
		"         +++..*#...%.........%.....#..%...#.%.#%.+..++          ",
		"         +++.#...#..............%...%..#..&.#*#.+.+++           ",
		"          ++.%.&...%..%..%..........#..*#..+..+.#%.++           ",
		"          ++..#..+..........%...%..$..%..+..&#*.+.+++           ",
		"           +*.+.....%............#..$.#...+.#.+.+%.+            ",
		"           +...%.+.#..%.%..#.%.#..%.#.*#%..&.+.+.+++            ",
		"            +.+.......#..%.....%...%.#..&#$#%.+.+.+             ",
		"            #..%#*#%....#..+..$..+..+.&#.+.+.+%+#&              ",
		"             +.....#*#$.+...%.#...+....+.%#*#.+.+               ",
		"              +.+.&..%..%.+.#.&.+..&#+.+.+.+.++++               ",
		"               %..+.#.+.+.+.*#..%#.$.+.&#*#&#%.+                ",
		"                +.%#*#%#$.%#.+.+.+.+%#$#+%#*+++                 ",
		"                 @#.#$#%++.+.%.+.&.#*#%+#$##$#                  ",
		"          .@.@@@@@@@@@@$#.++.+.#.+.+%#$......@@@@@@@            ",
		"        @@@@@.@.@@@@@@@@@$.+$#&+.+.+@@..@....@.@@@$@@$          ",
		"       @@@@@@.@@.@$@$@$$@$@+%#$#&#+@@.@..@@.@.@@$@@$$$$+        ",
		"      @@@@@@@@@@@@@$@$@$@$$$+%+.+%$$@@$@@.@.@$@$@$$@$$$$        ",
		"       @@@@$@$@$@$$@$$$$$$$#++++++#$@$@$$$$@$$@$$$$$$$$+        ",
		"        @$@@$@$@$@$$$$$$$$          $$$$@$$$$$$$$$$$$$+         ",
		"          $$@$$$$$$$$$$$               $$$$$$$$$$$$+            ",
		"                                                                ",
		"                                                                ",
		"                                                                "
};
