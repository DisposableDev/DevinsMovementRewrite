// Every sign read uses the same two-line box and screen position.
#define DEFINE_SIMPLEFLIPS_SIGN_DIALOG(id, text) DEFINE_DIALOG(id, 1, 2, 30, 200, _(text))

DEFINE_SIMPLEFLIPS_SIGN_DIALOG(DIALOG_SIMPLEFLIPS_SIGN, "Shout-outs to\nSimpleFlips")

// One line per page gives every word its own box and advance press.
DEFINE_DIALOG(DIALOG_SIMPLEFLIPS_TOAD, 1, 1, 95, 130, _("\
Now\n\
You\n\
Done\n\
Messed\n\
Up\n\
I\n\
Will\n\
Count\n\
To\n\
One\n\
Hundred\n\
And\n\
When\n\
I\n\
End\n\
I\n\
Will\n\
Count\n\
To\n\
One\n\
Hundred\n\
More\n\
Are\n\
You\n\
Ready\n\
One\n\
Two\n\
Oatmeal."))

DEFINE_DIALOG(DIALOG_SIMPLEFLIPS_MIPS, 1, 3, 30, 200, _("I have no paws, but\nI must game."))

// Three lines per page keep each sentence in its own box, including continuation lines.
DEFINE_DIALOG(DIALOG_SIMPLEFLIPS_MOTOS, 1, 3, 30, 200,
              _("Mario! You found me!\n\n\n"
                "You got rid of your Cap\nof Anti-visibility.\n\n"
                "Who am I?\n\n\n"
                "I'm the AI, Mario!\n\n\n"
                "I was personalizing you,\nall along!\n\n"
                "I don't have much time.\nI must be quick!\n\n"
                "There are too many SO\nRETRO discoveries -\n\n"
                "No! I can feel it!\n\n\n"
                "It's the phantom of\nBETA hacks!\n\n"
                "It's taking me back to\nthe LARP zone!\n\n"
                "Must... resist...\nLIMINAL...\n\n"
                "YEARRRGGGGGGGGGGGGHH\nHHHHHHHHHHHHHHHHHHHH"))

#define SIMPLEFLIPS_MESSAGE(read, text)                                                                \
    DEFINE_SIMPLEFLIPS_SIGN_DIALOG(DIALOG_SIMPLEFLIPS_READ_##read, text)
#include "simpleflips_messages.inc.h"
#undef SIMPLEFLIPS_MESSAGE

#undef DEFINE_SIMPLEFLIPS_SIGN_DIALOG

DEFINE_DIALOG(DIALOG_YOSHI_CREDIT, 1, 2, 30, 200,
              _("Sorry, Mario. I can't\n"
                "give credit.\n"
                "Come back when you're\n"
                "a little...\n"
                "Mmmmm...\n\n"
                "Richer!"))

DEFINE_DIALOG(DIALOG_CASTLE_SECRET_HOME, 1, 2, 30, 200,
              _("Please don't tell anyone\n"
                "how I live."))
DEFINE_DIALOG(DIALOG_CASTLE_SECRET_OW, 1, 2, 30, 200, _("Ow."))
DEFINE_DIALOG(DIALOG_CASTLE_SECRET_WHY, 1, 2, 30, 200, _("Why?"))

DEFINE_DIALOG(DIALOG_CASTLE_SECRET_AWAY, 1, 2, 30, 200,
              _("Stepped out for a bit!\n"
                "- Ms. Board"))

DEFINE_DIALOG(DIALOG_CASTLE_SECRET_COMPLETE, 1, 4, 30, 200,
              _("Wow, I've been waiting\n"
                "here for a long time.\n\n\n"
                "You must be really good\n"
                "at this game.\n\n\n"
                "You might even be...\n"
                "a SUPER PLAYER!\n\n\n"
                "I'd give you a reward...\n"
                "if I had one!\n\n\n"
                "There are still more\n"
                "easter eggs to be found.\n\n\n"
                "(Unless you've found\n"
                "them all, of course.)\n\n\n"
                "Hop in the painting to\n"
                "access the secret level.\n\n\n"
                "As for your friend,\n"
                "Lil Board...\n\n\n"
                "See if you can find me\n"
                "in the next game!"))

DEFINE_DIALOG(DIALOG_PHYSICS_LAB_WELCOME_COMPLETE, 1, 4, 30, 200,
              _("Welcome to the Physics\nLab!\n\n\n"
                "It's a secret to\neverybody.\n\n\n"
                "If you've made it here,\nyou've finished the\ngame.\n\n"
                "This level was made\nentirely by GPT-6.1\nSol.\n\n"
                "(I'm sure it's hard to\nbelieve.)\n\n\n"
                "Since you've made it\nthis far, I have one\nlast mission...\n\n"
                "Have fun!"))

DEFINE_DIALOG(DIALOG_PHYSICS_LAB_WELCOME_EARLY, 1, 4, 30, 200,
              _("Welcome to the Physics\nLab!\n\n\n"
                "Of course, you cheated\nto get here.\n\n\n"
                "(Or you're a developer.)\n\n\n\n"
                "This level was made\nby GPT-6.1 Sol.\n\n\n"
                "There's a legitimate\nway to get here, of\ncourse.\n\n"
                "If you find it, don't be\na sneaky snitch!\n\n\n"
                "I'd put in developer\ncomments, but it'd be\nway too long.\n\n"
                "Be sure to try and\nbreak the game.\n\n\n"
                "One day work will end.\n\n\n\n"
                "But play is forever!"))

DEFINE_DIALOG(DIALOG_PHYSICS_LAB_HIDDEN_SIGN, 1, 2, 30, 200,
              _("You really checked\ndown here?"))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_UNEARNED_ENDING, 1, 3, 30, 200,
_("Go back. We know you\ndidn't earn this ending."))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_DEBUG_STARS, 1, 3, 30, 200,
_("Go back. We know you\ndidn't earn those stars."))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_FLOOR_SIGN, 1, 1, 30, 200,
              _("How did we get here?"))

DEFINE_DIALOG(DIALOG_CASTLE_BOARD_INTRUDER, 1, 4, 30, 200,
              _("Sorry, Mario. I can't -\n\n\n\n"
                "Did you just break into\nmy house?\n\n\n"
                "Do you have any idea\nhow much that cost?\n\n\n"
                "Oh man, my landlord is\ngoing to KILL me!\n\n\n"
                "And my beautiful wife,\ntoo!\n\n\n"
                "No, she's not just a\npainting, shut up.\n\n\n"
                "This is worse than\nwhen Bowser...\n\n\n"
                "Never mind! I gotta\nget that door fixed up!\n\n\n"
                "And you're supposed\nto be the hero?\n\n\n"
                "You're more like\na ZERO!"))

// Earned castle-lobby finale: each sentence occupies its own three-line page.
DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_PEACH, 1, 3, 30, 200,
_("Dear Mario...\n\n\nYou did it! You saved\nSuper Mario 64!\n\nYou really are a Super\nPlayer!\n\nCongratulations!"))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_BOBOMB, 1, 3, 30, 200,
_("Hi, Mario!\n\n\nRemember all those\ncannons I opened for\nyou?\nSure would have been\nhelpful if they were\nopen earlier.\nEnough about me,\nthough.\n\nCongratulations!"))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_KOOPA, 1, 3, 30, 200,
_("Hey there, Mario!\n\n\nEven for me, getting\nhere was a challenge.\n\nIt was worth it to see\nyou succeed, though.\n\nCongratulations!\n\n\nDo you know if a bus\nruns through here?"))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_SIGN, 1, 3, 30, 200,
_("Whoa, hey guys.\n\n\nLooks like the party's\nstarted.\n\nRemember all those\nsigns you read, Mario?\n\nFull of helpful tips and\ntricks?\n\nWe sure had good\ntimes, out there.\n\nYes, they were all me.\n\n\nI work fast, you know.\n\n\nCongratulations!"))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_BOWSER, 1, 3, 30, 200,
_("RARRRGHH!!!\n\n\nWhy do I get dragged\nto these stupid things?\n\nAlright, you beat me,\nfair and square.\n\nDon't get a big head\nover it!\n\nJust you wait until my\nnext game.\n\nSuper Bowser 64 - no,\n65!\n\nHar har har!\n\n\nCongratulations!"))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_PIRANHA, 1, 3, 30, 200,
_("I don't know why I'm\nhere.\n\nKing Bowser's\ninvitation had a\nplus-one.\nI guess that makes me\nyour most iconic enemy.\n\nCongratulations!\n\n\nDo you think they'll put\nme in Super Smash\nBros?"))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_MOTOS_NEW, 1, 3, 30, 200,
_("I'm in this game too,\nyou know!\n\nWhat? You've never\nseen me?\n\nNever even heard of\nme?\n\nAw man, this is the\nSECOND time this\nhappened!\nNow I'm in a bad mood.\n\n\nCongratulations!"))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_MOTOS_KNOWN, 1, 3, 30, 200,
_("Ahh! Mario! I made it\nout!\n\nI feel like I can finally\nbreathe!\n\nEspecially after that\nstinky spirit breath...\n\nNow I can finally tell\nyou what I was going\nto say.\nEspecially after I was\nso RUDELY interrupted.\n\nThe nerve of some\npeople, I tell you...\n\nHmm, what was it,\nagain?\n\nMan, I could never\nremember my lines.\n\nThat's why they took me\nout of the game.\n\nI'll be back for the\nnext one...\n\nbecause BETA always\ncomes back.\n\nCongratulations!"))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_TOAD, 1, 3, 30, 200,
_("Bup!"))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_PEACH_FINALE, 1, 3, 30, 200,
_("Oh, dear Mario, isn't\nthis wonderful?\n\nHaving your friends\nhere to congratulate\nyou?\nIt's so nice to have a\nproper finale, isn't it?\n\nCome along, now. I still\nhave that cake for you!\n\nHmm... I wonder,\nthough...\n\nAren't we forgetting\nsomebody?"))

DEFINE_DIALOG(DIALOG_CASTLE_LOBBY_MIPS, 1, 3, 30, 200,
_("I'm late! I'm late!\nI'm late for a - \n\n- oh! I'm here! I'm\nhere for an important\ndate!\nCongratulations, Mario!\nChasing me around was\na lot of fun.\nEspecially when you\ngrabbed me like that...\n\nPerhaps I can introduce\nyou to some friends of\nmine?\nWe'd have lots of fun\ntogether!\n\nOh, there's a Sub-Space\nhighway that runs\nthrough here.\nIt's a convenient\nshortcut through your\nmind.\nJust stare directly at\nthe sun!"))
