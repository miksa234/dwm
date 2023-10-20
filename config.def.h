/* See LICENSE file for copyright and license details. */

#define TERMINAL "st"
#define TERMCLASS "St"

/* appearance */
static unsigned int borderpx  = 4;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static int showbar            = 1;        /* 0 means no bar */
static int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]            = { "terminus:pixelsize=19:antialias=true:autohint=true:spacing=false", "Hack Nerd Font:pixelsize=16"};

static char normfgcolor[]       = "#6c6c93";
static char normbgcolor[]       = "#263238";
static char normbordercolor[]   = "#263238";
static char selfgcolor[]        = "#000000";
static char selbgcolor[]        = "#606F88";
static char selbordercolor[]    = "#6c6c93";
static const unsigned int baralpha = 125;
static const unsigned int borderalpha = 150;
static char *colors[][3]        = {
	/*               fg         bg         border   */
	[SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor},
	[SchemeSel]  = { selfgcolor, selbgcolor,  selbordercolor},
};
static const unsigned int alphas[][3]      = {
    /*               fg      bg        border*/
    [SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "Zathura", NULL,     NULL,           0,         0,          0,          -1,        -1 },
	{ "St",      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
		{ "normbgcolor",        STRING,  &normbgcolor },
		{ "normbordercolor",    STRING,  &normbordercolor },
		{ "normfgcolor",        STRING,  &normfgcolor },
		{ "selbgcolor",         STRING,  &selbgcolor },
		{ "selbordercolor",     STRING,  &selbordercolor },
		{ "selfgcolor",         STRING,  &selfgcolor },
		{ "borderpx",          	INTEGER, &borderpx },
		{ "snap",          		INTEGER, &snap },
		{ "showbar",          	INTEGER, &showbar },
		{ "topbar",          	INTEGER, &topbar },
		{ "nmaster",          	INTEGER, &nmaster },
		{ "resizehints",       	INTEGER, &resizehints },
		{ "mfact",      	 	FLOAT,   &mfact },
};


/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \
	{ MOD, XK_q,     ACTION##stack, {.i = 0 } }, \
	{ MOD, XK_a,     ACTION##stack, {.i = 1 } }, \
	{ MOD, XK_z,     ACTION##stack, {.i = 2 } }, \
	{ MOD, XK_x,     ACTION##stack, {.i = -1 } },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };

/* commands spawned when clicking statusbar, the mouse button pressed is exported as BUTTON */
static char *statuscmds[] = { "notify-send Mouse$BUTTON" };
static char *statuscmd[] = { "/bin/sh", "-c", NULL, NULL };


static Key keys[] = {
	/* modifier                     key        function        argument */
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)
	{ MODKEY,                       XK_Return,      spawn,          {.v = termcmd }},
	{ MODKEY,                       XK_grave,       togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_d,           spawn,          {.v = dmenucmd }},
	{ MODKEY,                       XK_p,           spawn,          SHCMD("passmenu-otp")},
	{ ControlMask,                  XK_space,       spawn,          SHCMD("dunstctl close")},
	{ ControlMask,                  XK_grave,       spawn,          SHCMD("dunstctl close-all")},
	{ MODKEY,                       XK_p,           spawn,          SHCMD("passmenu-otp")},
	{ MODKEY|ShiftMask,             XK_b,           spawn,          SHCMD("nautilus")},
	{ MODKEY,                       XK_b,           spawn,          SHCMD("dmenu-bluetooth")},
	{ MODKEY,                       XK_n,           spawn,          SHCMD(TERMINAL " -e newsboat")},
	{ MODKEY,                       XK_c,           spawn,          SHCMD("brave")},
	{ MODKEY|ShiftMask,             XK_c,           spawn,          SHCMD(TERMINAL " -e calcurse")},
	{ MODKEY,                       XK_m,           spawn,          SHCMD(TERMINAL " -e neomutt")},
	{ MODKEY|ShiftMask,             XK_w,           spawn,          SHCMD(TERMINAL " -e sudo nmtui")},
	{ MODKEY,                       XK_w,           spawn,          SHCMD("xdotool key Super_L+9 && spotify")},
    { ShiftMask,                    XK_Shift_R,     spawn,          SHCMD("pkill -RTMIN+11 dwmblocks")},
	{ MODKEY,                       XK_F1,          spawn,          SHCMD("pkill -RTMIN+8 dwmblocks ; toggle volume")},
	{ MODKEY,                       XK_F2,          spawn,          SHCMD("pkill -RTMIN+8 dwmblocks ; pactl set-sink-volume @DEFAULT_SINK@ -5%")},
	{ MODKEY,                       XK_F3,          spawn,          SHCMD("pkill -RTMIN+8 dwmblocks ; pactl set-sink-volume @DEFAULT_SINK@ +5%")},
	{ MODKEY,                       XK_F4,          spawn,          SHCMD("pkill -RTMIN+8 dwmblocks ; toggle sink")},
	{ MODKEY,                       XK_F5,          spawn,          SHCMD("sudo xbacklight -dec 10")},
	{ MODKEY,                       XK_F6,          spawn,          SHCMD("sudo xbacklight -inc 10")},
	{ MODKEY|Mod1Mask,              XK_l,           spawn,          SHCMD("slock")},
	{ MODKEY|ShiftMask,             XK_z,           togglebar,      {0}},
	{ MODKEY|Mod1Mask,              XK_i,           incnmaster,     {.i = +1 }},
	{ MODKEY|Mod1Mask,              XK_d,           incnmaster,     {.i = -1 }},
	{ MODKEY,                       XK_comma,       setmfact,       {.f = -0.05}},
    { MODKEY,                       XK_period,      setmfact,       {.f = +0.05}},
	{ MODKEY|ShiftMask,             XK_f,           zoom,           {0} },
	{ MODKEY|Mod1Mask,              XK_k,           incrgaps,       {.i = +1 }},
	{ MODKEY|Mod1Mask,              XK_j,           incrgaps,       {.i = -1 }},
	{ MODKEY|Mod1Mask,              XK_i,           incrigaps,      {.i = +1 }},
	{ MODKEY|Mod1Mask|ShiftMask,    XK_i,           incrigaps,      {.i = -1 }},
	{ MODKEY|Mod1Mask,              XK_o,           incrogaps,      {.i = +1 }},
	{ MODKEY|Mod1Mask|ShiftMask,    XK_o,           incrogaps,      {.i = -1 }},
	{ MODKEY|Mod1Mask,              XK_6,           incrihgaps,     {.i = +1 }},
	{ MODKEY|Mod1Mask|ShiftMask,    XK_6,           incrihgaps,     {.i = -1 }},
	{ MODKEY|Mod1Mask,              XK_7,           incrivgaps,     {.i = +1 }},
	{ MODKEY|Mod1Mask|ShiftMask,    XK_7,           incrivgaps,     {.i = -1 }},
	{ MODKEY|Mod1Mask,              XK_8,           incrohgaps,     {.i = +1 }},
	{ MODKEY|Mod1Mask|ShiftMask,    XK_8,           incrohgaps,     {.i = -1 }},
	{ MODKEY|Mod1Mask,              XK_9,           incrovgaps,     {.i = +1 }},
	{ MODKEY|Mod1Mask|ShiftMask,    XK_9,           incrovgaps,     {.i = -1 }},
	{ MODKEY|Mod1Mask,              XK_0,           togglegaps,     {0} },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_0,           defaultgaps,    {0} },
	{ MODKEY|ShiftMask,             XK_q,           killclient,     {0} },
	{ MODKEY,                       XK_F9,          setlayout,      {.v = &layouts[0]}},
	{ MODKEY,                       XK_F10,         setlayout,      {.v = &layouts[7]}},
	{ MODKEY,                       XK_F11,         setlayout,      {.v = &layouts[9]}},
	{ MODKEY,                       XK_F12,         setlayout,      {.v = &layouts[11]}},
	{ MODKEY,                       XK_Tab,         view,           {0} },
	{ MODKEY,                       XK_space,       setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,       togglefloating, {0} },
	{ MODKEY,                       XK_f,           togglefullscr,  {0} },
	{ MODKEY,                       XK_0,           view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,           tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_l,           focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_h,           focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_l,           tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_h,           tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,           0)
	TAGKEYS(                        XK_2,           1)
	TAGKEYS(                        XK_3,           2)
	TAGKEYS(                        XK_4,           3)
	TAGKEYS(                        XK_5,           4)
	TAGKEYS(                        XK_6,           5)
	TAGKEYS(                        XK_7,           6)
	TAGKEYS(                        XK_8,           7)
	TAGKEYS(                        XK_9,           8)
	{ MODKEY|ShiftMask,             XK_r,           quit,           {0}},
	{ MODKEY|ShiftMask,             XK_e,           spawn,          SHCMD("killall xinit")},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[13]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigdwmblocks,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigdwmblocks,   {.i = 5} },
	{ ClkStatusText,        ShiftMask,      Button1,        sigdwmblocks,   {.i = 6} },
	{ ClkStatusText,        ShiftMask,      Button3,        spawn,          SHCMD(TERMINAL " -e nvim ~/.local/src/dwmblocks/config.h") },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

