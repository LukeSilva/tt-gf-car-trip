/*
 * Copyright (c) 2024 Uri Shaked
 * SPDX-License-Identifier: Apache-2.0
 */

`default_nettype none


(* blackbox *) (* keep *)
module tt_gf_cartrip_lg_nameplate ();
endmodule


module tt_um_LukeSilva_cartrip(
  input  wire [7:0] ui_in,    // Dedicated inputs
  output wire [7:0] uo_out,   // Dedicated outputs
  input  wire [7:0] uio_in,   // IOs: Input path
  output wire [7:0] uio_out,  // IOs: Output path
  output wire [7:0] uio_oe,   // IOs: Enable path (active high: 0=input, 1=output)
  input  wire       ena,      // always 1 when the design is powered, so you can ignore it
  input  wire       clk,      // clock
  input  wire       rst_n     // reset_n - low to reset
);

  tt_gf_cartrip_lg_nameplate nameplate();
  // VGA signals
  wire hsync;
  wire vsync;
  wire [1:0] R;
  wire [1:0] G;
  wire [1:0] B;
  wire video_active;
  wire [9:0] pix_x;
  wire [9:0] pix_y;

  // TinyVGA PMOD
  assign uo_out = {hsync, B[0], G[0], R[0], vsync, B[1], G[1], R[1]};

  // Unused outputs assigned to 0.
  assign uio_out = 0;
  assign uio_oe  = 0;

  // Suppress unused signals warning
  wire _unused_ok = &{ena, ui_in, uio_in, font_color};

  reg [9:0] counter;

  hvsync_generator hvsync_gen(
    .clk(clk),
    .reset(~rst_n),
    .hsync(hsync),
    .vsync(vsync),
    .display_on(video_active),
    .hpos(pix_x),
    .vpos(pix_y)
  );

  wire [6:0] ascii_code;
  wire [6:0] code;

  reg [255:0] msgs [3:0];
  initial begin
    msgs [0] = "olleH";
    msgs [1] = "ouy era woH";
    msgs [2] = "si eman yM";
    msgs [3] = "avliS ekuL";
  end

  wire [255:0] msg = msgs[counter[9:8]];
  wire [4:0] max = counter[7] == 0 ? counter[6:2] : 5'h1f;

  assign ascii_code = (pix_x[9:8] > 0 || pix_y [9:7] >0 )  ? 0 : {pix_y[6:4], pix_x[7:4]};
  wire [6:0] msg_code = msg[pix_x[9:4]*8 +: 7];
  assign code =
                // (pix_y[9:7] == 3'h0) ? ascii_code :
                // (pix_x[8:4] < max) ?  msg_code:
                {~pix_y[6], pix_y[5:4], pix_x[7:4]};
                // 7'h0;


  reg [6:0] rom_code;
  reg [2:0] rom_y;
  reg [2:0] rom_x;
  reg [2:0] r_rom_x;
  always @(posedge clk)
  begin
    rom_code <= code;
    rom_y <= pix_y[3:1];
    rom_x <= pix_x[3:1];
    r_rom_x <= rom_x;
  end

  wire [5:0] font_row;
  rom font_rom
  (
    .code(rom_code),
    .y(rom_y),
    .x(0),
    .row(font_row)
  );

  reg [5:0] r_font;
  reg r_r_font;
  always @(posedge clk)
  begin
    r_font <= font_row;
    r_r_font <= r_font[r_rom_x];
  end


  wire [5:0] font_color;
  assign font_color = r_r_font ? 6'b111111 : 6'h00;
  //assign font_color = 6'h3f;

  wire [9:0] moving_x = pix_x - 2* counter;

  wire [5:0] road_color;
  assign road_color = (pix_y >= 10'd296 && pix_y <= 10'd304 && -moving_x[5]) ? 6'h3f : 6'b010101;

`define TEXT_LEFT_BOUND 9'd2
`define TEXT_RIGHT_BOUND 9'd317
`define TEXT_TOP_BOUND 9'd182
`define TEXT_BOTTOM_BOUND 9'd237
  wire x_out_of_border;
  assign x_out_of_border = (pix_x[9:1] < `TEXT_LEFT_BOUND) || (pix_x[9:1] > `TEXT_RIGHT_BOUND);
  wire x_on_border;
  assign x_on_border = ((pix_x[9:1] == `TEXT_LEFT_BOUND) || (pix_x[9:1] == `TEXT_RIGHT_BOUND));
  wire y_out_of_border;
  assign y_out_of_border = (pix_y[9:1] < `TEXT_TOP_BOUND) || (pix_y[9:1] > `TEXT_BOTTOM_BOUND);
  wire y_on_border;
  assign y_on_border = ((pix_y[9:1] == `TEXT_TOP_BOUND) || (pix_y[9:1] == `TEXT_BOTTOM_BOUND));

  wire in_msg_box;
  assign in_msg_box = !x_on_border && !x_out_of_border && !y_on_border && !y_out_of_border;

  wire [5:0] text_bg;

  assign text_bg = (y_on_border && !x_out_of_border) ? 6'h3f :
                    (x_on_border && !y_out_of_border) ? 6'h3f : 6'h0;


  reg [5:0] car_palette[0:7];
  wire [2:0] car_data[0:2047];
  initial begin
    $readmemh("../data/car_palette.hex", car_palette);
    $readmemh("../data/car_image.hex", car_data);
    // $readmemh("../data/rainbow_b.hex", rainbow_b);
  end


  wire [9:0] car_x;
  assign car_x = pix_x - 10'd384 - {4'd0,1'b0, {5{counter[5]}} ^ counter[4:0]};
  wire [9:0] car_y;
  assign car_y = pix_y - 10'd240;

  wire [2:0] car_idx;

  assign car_idx = car_data[{car_y[5:1], car_x[6:1]}];

  wire [5:0] car_color;
  assign car_color = car_palette[car_idx];
  wire car_valid;
  assign car_valid = car_color != 6'h33 && car_x < 10'd128 && car_y < 10'd64;

  wire [5:0] bg_color;
  assign bg_color = (pix_y < 10'd216) ? 6'b011111 :
                    (pix_y >= 10'd280 && pix_y <= 320) ? road_color :
                    (pix_y < 10'd360) ? 6'b101101 :
                    text_bg;

  wire [5:0] color;
  assign color = (ui_in[1] && car_valid) ? car_color :
                (pix_y[9:6] == 4'b0110) ? (counter[pix_x[7:4]] ? 6'h3f : 0) :
                 (in_msg_box || ui_in[0]) ? font_color : bg_color;


  assign R = video_active ? color[5:4] : 2'b00;
  assign G = video_active ? color[3:2] : 2'b00;
  assign B = video_active ? color[1:0] : 2'b00;

  always @(posedge vsync, negedge rst_n) begin
    if (~rst_n) begin
      counter <= 0;
    end else begin
      counter <= counter + 1;
    end
  end

  // Suppress unused signals warning
  wire _unused_ok_ = &{moving_x, pix_y};

endmodule
