from itertools import groupby

from PIL import Image

img = Image.open("car.png")

rgb_img = img.convert("RGB")

colors = rgb_img.getcolors()
print(colors)
assert colors is not None
assert len(colors) < 16
print(len(colors))
dat = rgb_img.get_flattened_data()
# print(dat)

rows = []
for y in range(rgb_img.height):
    start = y * rgb_img.width
    end = (y + 1) * rgb_img.width
    rows.append(dat[start:end])

palette = [x[1] for x in colors]
print(palette)
palettes = set()
rle_s = []
run_lengths = set()
for i in range(len(rows)):
    r = rows[i]
    colors = set(r)
    # assert len(colors) <= 4, "Cannot have more than 4 colors per line"
    palettes.add(tuple(sorted(colors)))

    i_row = [palette.index(c) for c in r]
    print(i, i_row)

    rle = [(x, len(list(y))) for x, y in groupby(i_row)]
    print(i, rle)
    rle_s.append(rle)


# RUN_MAX = 4

# extras = [24, 12, 8, 6, 4]

RUN_MAX = 12
extras = [32, 24, 12]


def rework(rle):
    runs = []
    for run in rle:
        run_length = run[1]

        while run_length > RUN_MAX:
            for c in extras:
                if run_length > c:
                    runs.append((run[0], c))
                    run_length -= c
                    break

        runs.append((run[0], run_length))
    return runs


rle_s = [rework(rle) for rle in rle_s]
run_lengths = set()
for rle in rle_s:
    for run in rle:
        run_lengths.add(run[1])
print(run_lengths)
print(len(run_lengths))

for i in range(len(rle_s)):
    r = rle_s[i]
    print(i, r)

n_rles = [len(x) for x in rle_s]
print(n_rles)
print(max(n_rles))
rle_max_runs = [max([x[1] for x in rle]) for rle in rle_s]
print(rle_max_runs)
max_run = max(rle_max_runs)
print(f"Max Run: {max_run}")
n_diff_runs = len(run_lengths)
lookup_run_length = (n_diff_runs - 1).bit_length()
print(f"Different runs: {run_lengths}")
print(f"Could be stored in {lookup_run_length}")

run_bit_length = max_run.bit_length()
min_run_bitlength = min(run_bit_length, lookup_run_length)
print(f"IE: {min_run_bitlength}bits needed for run length, with {max(n_rles)} per line")


lengths = [1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20]


print(len(palettes))
print(palettes)


def simplify(palettes):
    max_palette_length = max([len(p) for p in palettes])
    for p_c in palettes:
        for p in palettes:
            p_set = set(p)
            if p is p_c:
                continue
            if all([color in p_set for color in p_c]):
                palettes.remove(p_c)
                return palettes

    for p_c in palettes:
        if len(p_c) == max_palette_length:
            continue
        for p in palettes:
            if p is p_c:
                continue
            if len(p) == max_palette_length:
                continue
            common_colors = set(p) & set(p_c)
            not_included_colors = set(p) - set(p_c)
            print(f"Currently {len(palettes)} palettes")
            print(f"Found simplification candidate {p} {p_c}")
            print(f"Common colors: {common_colors}")
            print(f"Not included colors: {not_included_colors}")
            if len(not_included_colors) + len(p_c) <= max_palette_length:
                print("OK: simplifing")
                new_list = list(p_c) + list(not_included_colors)
                palettes.add(tuple(new_list))
                return palettes


while True:
    s = simplify(palettes)
    if s is None:
        break
    palettes = s


max_palette_length = max([len(p) for p in palettes])
palette_bit_length = (max_palette_length - 1).bit_length()
print(f"N palettes: {len(palettes)}")
for p in palettes:
    print(p)
print(f"Max palette length: {max_palette_length}")
print(f"IE: {palette_bit_length}")

n_colors = len(palette)
color_bit_length = (n_colors - 1).bit_length()
print(f"Num colors: {n_colors}")
min_color_bit = min(color_bit_length, palette_bit_length)
print(f"IE: {min_color_bit} needed for each color")
print()
run_bits = min_run_bitlength + min_color_bit
print(f"So: {run_bits} bits needed for each run")
total_runs = sum(n_rles)
print(f"Total: {total_runs} runs needed @ {run_bits}bits = {run_bits * total_runs}")

for i in range(len(palette)):
    if palette[i] == (255, 0, 255):
        palette[0], palette[i] = palette[i], palette[0]
        break

int_palette = [((r & 3) << 4) | ((g & 3) << 2) | (b & 3) for r, g, b in palette]
hex_palette = " ".join([hex(c)[2:] for c in int_palette])
print(hex_palette)
with open("car_palette.hex", "w") as f:
    f.write(hex_palette)

image = "\n".join([" ".join([hex(palette.index(c))[2:] for c in r]) for r in rows])
print(image)

with open("car_image.hex", "w") as f:
    f.write(image)
# with open("car_rle.h", "w") as f:
#     for p in palettes:
#         f.write("{\n\t")
#         colors = ["{" + ",".join([str(x) for x in c]) + "}" for c in p]
#         f.write(",\n\t".join(colors))
#         f.write("\n}\n")
