#include <iostream>
#include <vector>
#include <cstdint>
#include <SDL2/SDL.h>
#include "verilated.h"
#include "vga_timings.hpp"
#include "gif.h"

struct ABGR8888_t { uint8_t r, g, b, a; } __attribute__((packed));
union VGApinout_t {
	uint8_t pins;
	struct { // 6-bit color with sync
		uint8_t r1 :1; uint8_t g1 :1; uint8_t b1 :1; uint8_t vsync :1;
		uint8_t r0 :1; uint8_t g0 :1; uint8_t b0 :1; uint8_t hsync :1;
	} __attribute__((packed));
};

int main(int argc, char **argv)
{
	static Uint32 fullscreen = 0; // Defaul command line options
	bool polarity = false, slow = false, gif = false;
	int gif_frames = 0;
	std::vector<vga_format> modes{VGA_640_480_60, VGA_768_576_60, VGA_800_600_60, VGA_1024_768_60};
	vga_timing mode = vga_timings[modes[0]];

	for (int i = 1; i < argc; i++) { // Handle command line arguments
		char* p = argv[i];
		if (!strcmp("--", p)) break;
		else if (!strcmp("--fullscreen", p)) fullscreen = fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP;
		else if (!strcmp("--polarity", p)) polarity = !polarity;
		else if (!strcmp("--slow", p)) slow = !slow;
		else if (!strcmp("--mode", p)) {
			if (i + 1 < argc) {
				int m = atoi(argv[++i]);
				if (m >= 0 && m < modes.size()) mode = vga_timings[modes[m]];
			}
		} else if (!strcmp("--gif", p)) {
			gif = !gif;
			if (i + 1 < argc) gif_frames = atoi(argv[++i]);
		} else {
			printf("Command Line     | [Key]\n");
			printf("  --fullscreen   | [ F ]\tToggles SDL window size (default: %s)\n", fullscreen ? "maximized" : "minimized");
			printf("  --polarity     | [ P ]\tToggles the VGA polarity sync high/low (default: %s)\n", polarity ? "true" : "false");
			printf("  --slow         | [ S ]\tToggles the displayed frame rate (default: %s)\n", slow ? "true" : "false");
			printf("  --mode [#]            \tSets SDL VGA timing mode (value: [0:%ld])\n", modes.size()-1);
			printf("  --gif [#frames]       \tSaves animated GIF (default: %s [%d])\n", gif ? "true" : "false", gif_frames);
			printf("                 | [ Q ]\tQuits/Escapes (stops GIF if enabled).\n");
			return 1;
		}
	}

	vga_timing vga = mode; // Select the VGA timings from the list
	std::vector<ABGR8888_t> fb(vga.h_active_pixels * vga.v_active_lines);

	GifWriter g; // GIF output
	int delay = ceilf(vga.frame_cycles() / (vga.clock_mhz * 10000.f)); // 100ths of a second
	if (gif) GifBegin(&g, "output.gif", vga.h_active_pixels, vga.v_active_lines, delay);

	SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2
	SDL_Window* w = SDL_CreateWindow("Tiny Tapeout VGA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, vga.h_active_pixels, vga.v_active_lines, SDL_WINDOW_RESIZABLE | fullscreen);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
	SDL_Renderer* r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(r, vga.h_active_pixels, vga.v_active_lines);
	SDL_Texture* t = SDL_CreateTexture(r, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, vga.h_active_pixels, vga.v_active_lines);

	Verilated::commandArgs(argc, argv);
	TOP_MODULE *top = new TOP_MODULE;

	bool quit = false; // Main single frame loop
	while (!quit && !Verilated::gotFinish()) {
		int last_ticks = SDL_GetTicks();
		static int frame = 0;
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) quit = true;
			else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
					case SDLK_ESCAPE:
					case SDLK_q:
						quit = true;
						break;
					case SDLK_f: // toggle fullscreen window
						fullscreen = fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP;
						SDL_SetWindowFullscreen(w, fullscreen);
						break;
					case SDLK_p: // toggle VGA sync polarity
						polarity = !polarity;
						break;
					case SDLK_s: // toggle slow
						slow = !slow;
					default:
						break;
				}
			}
		}

		auto k = SDL_GetKeyboardState(NULL);
		auto rst_n = k[SDL_SCANCODE_R];
		static bool rst_init = false;
		if (!rst_init) { rst_n = rst_init = true; } // reset on first clock cycle
		uint8_t ui_in = 0;
		ui_in |= k[SDL_SCANCODE_0] << 0;
		ui_in |= k[SDL_SCANCODE_1] << 1;
		ui_in |= k[SDL_SCANCODE_2] << 2;
		ui_in |= k[SDL_SCANCODE_3] << 3;
		ui_in |= k[SDL_SCANCODE_4] << 4;
		ui_in |= k[SDL_SCANCODE_5] << 5;
		ui_in |= k[SDL_SCANCODE_6] << 6;
		ui_in |= k[SDL_SCANCODE_7] << 7;

		static int hnum = 0;
		static int vnum = 0;
		for (int cycle = 0; cycle < vga.frame_cycles(); cycle++) { // Intra-frame verilator cycles
			// set inputs and tick-tock
			top->clk = 0;
			top->eval();
			if (rst_n) top->rst_n = 0;
			top->ui_in = ui_in;
			top->clk = 1;
			top->eval();
			if (rst_n) top->rst_n = 1;
			top->ui_in = ui_in;

			VGApinout_t uo_out{top->uo_out};

			// h and v blank/sync logic
			if ((uo_out.hsync == vga.h_sync_pol) ^ polarity && (uo_out.vsync == vga.v_sync_pol) ^ polarity) {
				hnum = -vga.h_back_porch;
				vnum = -vga.v_back_porch;
			}

			// active frame, scaling for 6-bit color
			if ((hnum >= 0) && (hnum < vga.h_active_pixels) && (vnum >= 0) && (vnum < vga.v_active_lines)) {
				uint8_t rr = 85 * (uo_out.r1 << 1 | uo_out.r0);
				uint8_t gg = 85 * (uo_out.g1 << 1 | uo_out.g0);
				uint8_t bb = 85 * (uo_out.b1 << 1 | uo_out.b0);
				ABGR8888_t rgb = { .r = rr, .g = gg, .b = bb };
				fb[vnum * vga.h_active_pixels + hnum] = rgb;
			}

			// keep track of encountered fields
			hnum++;
			if (hnum >= vga.h_active_pixels + vga.h_front_porch + vga.h_sync_pulse) {
				hnum = -vga.h_back_porch;
				vnum++;
			}
		}

		SDL_RenderClear(r);
		SDL_UpdateTexture(t, NULL, fb.data(), vga.h_active_pixels * sizeof(ABGR8888_t));
		SDL_RenderCopy(r, t, NULL, NULL);
		SDL_RenderPresent(r);

		int ticks = SDL_GetTicks();
		static int last_update_ticks = 0;
		if (ticks - last_update_ticks > 500) {
			last_update_ticks = ticks;
			std::string fps = "Tiny Tapeout VGA (" + std::to_string((int)1000.0/(ticks - last_ticks)) + " FPS)";
			SDL_SetWindowTitle(w, fps.c_str());
		}
		if (gif) {
			GifWriteFrame(&g, (uint8_t*)fb.data(), vga.h_active_pixels, vga.v_active_lines, delay);
			if (++frame == gif_frames) quit = true;
		}
		if (slow) usleep(250000); // ~4 fps

	}

	if (gif) GifEnd(&g);

	top->final();
	delete top;

	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	SDL_Quit();
}
