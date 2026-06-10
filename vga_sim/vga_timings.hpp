#include <array>
#include <cstdint>

/*
 * Timings found at: http://www.tinyvga.com/vga-timing
 */

// Packed data struct for efficient storage
struct vga_timing {
	float    clock_mhz;
	uint64_t h_active_pixels : 11;
	uint64_t h_front_porch   :  8;
	uint64_t h_sync_pulse    :  8;
	uint64_t h_back_porch    :  9;
	uint64_t h_sync_pol      :  1;
	uint64_t v_active_lines  : 11;
	uint64_t v_front_porch   :  6;
	uint64_t v_sync_pulse    :  3;
	uint64_t v_back_porch    :  6;
	uint64_t v_sync_pol      :  1;
	uint64_t frame_cycles() const { return (h_active_pixels + h_front_porch + h_sync_pulse + h_back_porch) * (v_active_lines + v_front_porch + v_sync_pulse + v_back_porch); }
} __attribute__((packed));

enum vga_sync {
	VGA_SYNC_NEG,
	VGA_SYNC_POS
};

enum vga_format {
	VGA_640_350_70,
	VGA_640_350_85,
	VGA_640_400_70,
	VGA_640_400_85,
	VGA_640_480_60,
	VGA_640_480_72,
	VGA_640_480_75,
	VGA_640_480_85,
	VGA_720_400_85,
    VGA_768_576_60,
    VGA_768_576_72,
    VGA_768_576_75,
    VGA_768_576_85,
	VGA_800_600_56,
	VGA_800_600_60,
	VGA_800_600_72,
	VGA_800_600_75,
	VGA_800_600_85,
	VGA_1024_768_60,
	VGA_1024_768_70,
	VGA_1024_768_75,
	VGA_1024_768_85,
	VGA_1152_864_75,
	VGA_1280_960_60,
	VGA_1280_960_85,
	VGA_1280_1024_60,
	VGA_1280_1024_75,
	VGA_1280_1024_85,
	VGA_1600_1200_60,
	VGA_1600_1200_65,
	VGA_1600_1200_70,
	VGA_1600_1200_75,
	VGA_1600_1200_85,
	VGA_NUM_FORMAT
};

constexpr std::array<vga_timing, VGA_NUM_FORMAT> vga_timings = {{
	[  VGA_640_350_70] = { 25.175,  640,  16,  96,  48, VGA_SYNC_POS,  350, 37, 2, 60, VGA_SYNC_NEG},
	[  VGA_640_350_85] = { 31.500,  640,  32,  64,  96, VGA_SYNC_POS,  350, 32, 3, 60, VGA_SYNC_NEG},
	[  VGA_640_400_70] = { 25.175,  640,  16,  96,  48, VGA_SYNC_NEG,  400, 12, 2, 34, VGA_SYNC_POS},
	[  VGA_640_400_85] = { 31.500,  640,  32,  64,  96, VGA_SYNC_NEG,  400,  1, 3, 41, VGA_SYNC_POS},
	[  VGA_640_480_60] = { 25.175,  640,  16,  96,  48, VGA_SYNC_NEG,  480, 10, 2, 33, VGA_SYNC_NEG},
	[  VGA_640_480_72] = { 31.500,  640,  24,  40, 128, VGA_SYNC_NEG,  480,  9, 2, 29, VGA_SYNC_POS},
	[  VGA_640_480_75] = { 31.500,  640,  16,  64, 120, VGA_SYNC_NEG,  480,  1, 3, 16, VGA_SYNC_NEG},
	[  VGA_640_480_85] = { 36.000,  640,  56,  56,  80, VGA_SYNC_NEG,  480,  1, 3, 25, VGA_SYNC_NEG},
	[  VGA_720_400_85] = { 35.500,  720,  36,  72, 108, VGA_SYNC_NEG,  400,  1, 3, 42, VGA_SYNC_POS},
	[  VGA_768_576_60] = { 34.960,  768,  24,  80, 104, VGA_SYNC_NEG,  576,  1, 3, 17, VGA_SYNC_POS},
	[  VGA_768_576_72] = { 42.930,  768,  32,  80, 112, VGA_SYNC_NEG,  576,  1, 3, 21, VGA_SYNC_POS},
	[  VGA_768_576_75] = { 45.510,  768,  40,  80, 120, VGA_SYNC_NEG,  576,  1, 3, 22, VGA_SYNC_POS},
	[  VGA_768_576_85] = { 51.840,  768,  40,  80, 120, VGA_SYNC_NEG,  576,  1, 3, 25, VGA_SYNC_POS},
	[  VGA_800_600_56] = { 36.000,  800,  24,  72, 128, VGA_SYNC_POS,  600,  1, 2, 22, VGA_SYNC_POS},
	[  VGA_800_600_60] = { 40.000,  800,  40, 128,  88, VGA_SYNC_POS,  600,  1, 4, 23, VGA_SYNC_POS},
	[  VGA_800_600_72] = { 50.000,  800,  56, 120,  64, VGA_SYNC_POS,  600, 37, 6, 23, VGA_SYNC_POS},
	[  VGA_800_600_75] = { 49.500,  800,  16,  80, 160, VGA_SYNC_POS,  600,  1, 3, 21, VGA_SYNC_POS},
	[  VGA_800_600_85] = { 56.250,  800,  32,  64, 152, VGA_SYNC_POS,  600,  1, 3, 27, VGA_SYNC_POS},
	[ VGA_1024_768_60] = { 65.000, 1024,  24, 136, 160, VGA_SYNC_NEG,  768,  3, 6, 29, VGA_SYNC_NEG},
	[ VGA_1024_768_70] = { 75.000, 1024,  24, 136, 144, VGA_SYNC_NEG,  768,  3, 6, 29, VGA_SYNC_NEG},
	[ VGA_1024_768_75] = { 78.800, 1024,  16,  96, 176, VGA_SYNC_POS,  768,  1, 3, 28, VGA_SYNC_POS},
	[ VGA_1024_768_85] = { 94.500, 1024,  48,  96, 208, VGA_SYNC_POS,  768,  1, 3, 36, VGA_SYNC_POS},
	[ VGA_1152_864_75] = {108.000, 1152,  64, 128, 256, VGA_SYNC_POS,  864,  1, 3, 32, VGA_SYNC_POS},
	[ VGA_1280_960_60] = {108.000, 1280,  96, 112, 312, VGA_SYNC_POS,  960,  1, 3, 36, VGA_SYNC_POS},
	[ VGA_1280_960_85] = {148.500, 1280,  64, 160, 224, VGA_SYNC_POS,  960,  1, 3, 47, VGA_SYNC_POS},
	[VGA_1280_1024_60] = {108.000, 1280,  48, 112, 248, VGA_SYNC_POS, 1024,  1, 3, 38, VGA_SYNC_POS},
	[VGA_1280_1024_75] = {135.000, 1280,  16, 144, 248, VGA_SYNC_POS, 1024,  1, 3, 38, VGA_SYNC_POS},
	[VGA_1280_1024_85] = {157.500, 1280,  64, 160, 224, VGA_SYNC_POS, 1024,  1, 3, 44, VGA_SYNC_POS},
	[VGA_1600_1200_60] = {162.000, 1600,  64, 192, 304, VGA_SYNC_POS, 1200,  1, 3, 46, VGA_SYNC_POS},
	[VGA_1600_1200_65] = {175.500, 1600,  64, 192, 304, VGA_SYNC_POS, 1200,  1, 3, 46, VGA_SYNC_POS},
	[VGA_1600_1200_70] = {189.000, 1600,  64, 192, 304, VGA_SYNC_POS, 1200,  1, 3, 46, VGA_SYNC_POS},
	[VGA_1600_1200_75] = {202.500, 1600,  64, 192, 304, VGA_SYNC_POS, 1200,  1, 3, 46, VGA_SYNC_POS},
	[VGA_1600_1200_85] = {229.500, 1600,  64, 192, 304, VGA_SYNC_POS, 1200,  1, 3, 46, VGA_SYNC_POS}
}};
