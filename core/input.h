#pragma once

namespace oak {

	namespace action {
		constexpr int released = 0;
		constexpr int pressed = 1;
		constexpr int repeat = 2;
	}

	namespace button {
		constexpr int left = 0;
		constexpr int right = 1;
		constexpr int middle = 2;
		constexpr int max = 32;
	}

	namespace key {
		constexpr int space = 32;
		constexpr int apostrophe = 39;
		constexpr int comma = 44;
		constexpr int minus = 45;
		constexpr int period = 46;
		constexpr int slash = 47;
		constexpr int _0 = 48;
		constexpr int _1 = 49;
		constexpr int _2 = 50;
		constexpr int _3 = 51;
		constexpr int _4 = 52;
		constexpr int _5 = 53;
		constexpr int _6 = 54;
		constexpr int _7 = 55;
		constexpr int _8 = 56;
		constexpr int _9 = 57;
		constexpr int semi = 59;
		constexpr int equal = 61;
		constexpr int a = 65;
		constexpr int b = 66;
		constexpr int c = 67;
		constexpr int d = 68;
		constexpr int e = 69;
		constexpr int f = 70;
		constexpr int g = 71;
		constexpr int h = 72;
		constexpr int i = 73;
		constexpr int j = 74;
		constexpr int k = 75;
		constexpr int l = 76;
		constexpr int m = 77;
		constexpr int n = 78;
		constexpr int o = 79;
		constexpr int p = 80;
		constexpr int q = 81;
		constexpr int r = 82;
		constexpr int s = 83;
		constexpr int t = 84;
		constexpr int u = 85;
		constexpr int v = 86;
		constexpr int w = 87;
		constexpr int x = 88;
		constexpr int y = 89;
		constexpr int z = 90;
		constexpr int lbracket = 91;  // [
		constexpr int backslash = 92;  // "\"
		constexpr int rbracket = 93;  // ]
		constexpr int grave = 96;  // `
		constexpr int w1 = 161; // non-US #1
		constexpr int w2 = 162; // non-US #2

		/* Function keys */
		constexpr int esc = 256;
		constexpr int enter = 257;
		constexpr int tab = 258;
		constexpr int backspace = 259;
		constexpr int insert = 260;
		constexpr int del = 261;
		constexpr int right = 262;
		constexpr int left = 263;
		constexpr int down = 264;
		constexpr int up = 265;
		constexpr int page_up = 266;
		constexpr int page_down = 267;
		constexpr int home = 268;
		constexpr int end = 269;
		constexpr int caps = 280;
		constexpr int scroll = 281;
		constexpr int num = 282;
		constexpr int print_screen = 283;
		constexpr int pause = 284;
		constexpr int f1 = 290;
		constexpr int f2 = 291;
		constexpr int f3 = 292;
		constexpr int f4 = 293;
		constexpr int f5 = 294;
		constexpr int f6 = 295;
		constexpr int f7 = 296;
		constexpr int f8 = 297;
		constexpr int f9 = 298;
		constexpr int f10 = 299;
		constexpr int f11 = 300;
		constexpr int f12 = 301;
		constexpr int f13 = 302;
		constexpr int f14 = 303;
		constexpr int f15 = 304;
		constexpr int f16 = 305;
		constexpr int f17 = 306;
		constexpr int f18 = 307;
		constexpr int f19 = 308;
		constexpr int f20 = 309;
		constexpr int f21 = 310;
		constexpr int f22 = 311;
		constexpr int f23 = 312;
		constexpr int f24 = 313;
		constexpr int f25 = 314;
		constexpr int kpad_0 = 320;
		constexpr int kpad_1 = 321;
		constexpr int kpad_2 = 322;
		constexpr int kpad_3 = 323;
		constexpr int kpad_4 = 324;
		constexpr int kpad_5 = 325;
		constexpr int kpad_6 = 326;
		constexpr int kpad_7 = 327;
		constexpr int kpad_8 = 328;
		constexpr int kpad_9 = 329;
		constexpr int kpad_dec = 330;
		constexpr int kpad_div = 331;
		constexpr int kpad_mul = 332;
		constexpr int kpad_sub = 333;
		constexpr int kpad_add = 334;
		constexpr int kpad_enter = 335;
		constexpr int kpad_equal = 336;
		constexpr int lshift = 340;
		constexpr int lctrl = 341;
		constexpr int lalt = 342;
		constexpr int lsuper = 343;
		constexpr int rshift = 344;
		constexpr int rctrl = 345;
		constexpr int ralt = 346;
		constexpr int rsuper = 347;
		constexpr int menu = 348;
	}
}