const std = @import("std");
const vaxis = @import("vaxis");
const editor = @import("editor.zig");

pub fn draw(ed: *editor.Editor, win: vaxis.Window) void {
    const start_line = ed.scroll_offset;
    const end_line = @min(start_line + win.height, ed.buffer.lines.items.len);

    for (start_line..end_line) |line_idx| {
        const screen_row = line_idx - start_line;
        if (screen_row >= win.height) break;

        const line_text = ed.buffer.lines.items[line_idx].items;
        const text_to_show = if (line_text.len > win.width)
            line_text[0..win.width]
        else
            line_text;

        const text_segments = [_]vaxis.Segment{
            .{ .text = text_to_show, .style = .{} },
        };
        _ = win.print(&text_segments, .{ .row_offset = @intCast(screen_row) });
    }

    if ((ed.mode == .normal or ed.mode == .insert) and ed.cursor_row >= ed.scroll_offset) {
        const screen_row = ed.cursor_row - ed.scroll_offset;
        if (screen_row < win.height) {
            const cursor_shape: vaxis.Cell.CursorShape = if (ed.mode == .insert)
                .beam
            else
                .block;

            win.showCursor(@intCast(ed.cursor_col), @intCast(screen_row));
            win.setCursorShape(cursor_shape);
        }
    }
}

pub fn drawFooter(ed: *editor.Editor, win: vaxis.Window) void {
    if (win.height == 0) return;

    const mode_text = switch (ed.mode) {
        .normal => " NOR ",
        .insert => " INS ",
    };

    const mode_bg: u8 = switch (ed.mode) {
        .normal => 12, // Bright blue
        .insert => 10, // Bright green
    };

    const footer_row = if (win.height > 0) win.height - 1 else 0;

    const mode_segments = [_]vaxis.Segment{
        .{
            .text = mode_text,
            .style = .{
                .fg = .{ .index = 0 },
                .bg = .{ .index = mode_bg },
                .bold = true,
            },
        },
    };
    _ = win.print(&mode_segments, .{ .row_offset = @intCast(footer_row), .col_offset = 0 });

    var buf_filename: [256]u8 = undefined;
    const formatted_filename = if (ed.save_input_active) blk: {
        const input_text = if (ed.save_input.items.len > 0) ed.save_input.items else "[Enter filename]";
        break :blk std.fmt.bufPrint(&buf_filename, " {s} ", .{input_text}) catch " [Filename too long] ";
    } else blk: {
        const filename_text = if (ed.filename) |f| f.items else "[No Name]";
        const modified_marker = if (ed.modified) "*" else "";
        break :blk std.fmt.bufPrint(&buf_filename, " {s}{s} ", .{ modified_marker, filename_text }) catch " [Filename too long] ";
    };

    const filename_segments = [_]vaxis.Segment{
        .{
            .text = formatted_filename,
            .style = .{
                .fg = .{ .index = 15 },
                .bg = .{ .index = 8 },
            },
        },
    };
    _ = win.print(&filename_segments, .{
        .row_offset = @intCast(footer_row),
        .col_offset = @intCast(mode_text.len),
    });

    const current_line = ed.cursor_row + 1;
    const total_lines = ed.buffer.lines.items.len;

    var buf_right: [128]u8 = undefined;
    const right_info = std.fmt.bufPrint(&buf_right, " {d}:{d} ", .{ current_line, total_lines }) catch " ??? ";

    const right_col = if (win.width > right_info.len) win.width - right_info.len else 0;

    const right_segments = [_]vaxis.Segment{
        .{
            .text = right_info,
            .style = .{
                .fg = .{ .index = 15 },
                .bg = .{ .index = 8 },
            },
        },
    };
    _ = win.print(&right_segments, .{
        .row_offset = @intCast(footer_row),
        .col_offset = @intCast(right_col),
    });

    const left_end = mode_text.len + formatted_filename.len;
    if (left_end < right_col) {
        var col = left_end;
        while (col < right_col) : (col += 1) {
            const fill = [_]vaxis.Segment{
                .{
                    .text = " ",
                    .style = .{ .bg = .{ .index = 8 } },
                },
            };
            _ = win.print(&fill, .{
                .row_offset = @intCast(footer_row),
                .col_offset = @intCast(col),
            });
        }
    }
}

pub fn drawStatusMessage(ed: *editor.Editor, win: vaxis.Window) void {
    if (ed.status_message) |message| {
        const msg_len: u16 = @intCast(message.len);
        const box_width = msg_len + 4;
        const box_height: u16 = 3;

        const box_x = if (win.width > box_width) win.width - box_width - 2 else 0;
        const box_y: u16 = 1;

        const status_box = win.child(.{
            .x_off = @intCast(box_x),
            .y_off = @intCast(box_y),
            .width = box_width,
            .height = box_height,
            .border = .{
                .where = .all,
                .style = .{ .fg = .{ .index = 10 } },
                .glyphs = .single_rounded,
            },
        });

        const msg_segments = [_]vaxis.Segment{
            .{ .text = message, .style = .{ .fg = .{ .index = 10 }, .bold = true } },
        };
        _ = status_box.print(&msg_segments, .{ .row_offset = 0, .col_offset = 2 });
    }
}

