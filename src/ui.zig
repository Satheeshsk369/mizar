const std = @import("std");
const vaxis = @import("vaxis");
const editor = @import("editor.zig");

pub fn draw(ed: *editor.Editor, win: vaxis.Window) void {
    // Draw editor content with scroll offset
    const start_line = ed.scroll_offset;
    const end_line = @min(start_line + win.height, ed.lines.items.len);

    for (start_line..end_line) |line_idx| {
        const screen_row = line_idx - start_line;
        if (screen_row >= win.height) break;

        const segments = [_]vaxis.Segment{
            .{ .text = ed.lines.items[line_idx].items, .style = .{} },
        };
        _ = win.print(&segments, .{ .row_offset = @intCast(screen_row) });
    }

    // Draw cursor (adjusted for scroll)
    if ((ed.mode == .normal or ed.mode == .insert) and ed.cursor_row >= ed.scroll_offset) {
        const screen_row = ed.cursor_row - ed.scroll_offset;
        if (screen_row < win.height) {
            // Different cursor style for different modes
            const cursor_text = if (ed.mode == .insert) "│" else "█";
            const cursor_segments = [_]vaxis.Segment{
                .{ .text = cursor_text, .style = .{ .fg = .{ .index = 7 } } },
            };
            _ = win.print(&cursor_segments, .{
                .row_offset = @intCast(screen_row),
                .col_offset = @intCast(ed.cursor_col),
            });
        }
    }
}

pub fn drawFooter(ed: *editor.Editor, win: vaxis.Window) void {
    // Don't show footer when save dialog is open
    if (ed.mode == .save_dialog) return;
    if (win.height == 0) return;

    const mode_text = switch (ed.mode) {
        .normal => " NOR ",
        .insert => " INS ",
        .save_dialog => return,
    };

    const mode_bg: u8 = switch (ed.mode) {
        .normal => 12, // Bright blue
        .insert => 10, // Bright green
        .save_dialog => 7,
    };

    // Create footer at the bottom of window
    const footer_row = if (win.height > 0) win.height - 1 else 0;

    // Draw mode indicator with background color
    const mode_segments = [_]vaxis.Segment{
        .{
            .text = mode_text,
            .style = .{
                .fg = .{ .index = 0 }, // Black text
                .bg = .{ .index = mode_bg }, // Colored background
                .bold = true,
            },
        },
    };
    _ = win.print(&mode_segments, .{ .row_offset = @intCast(footer_row), .col_offset = 0 });

    // Draw filename or [No Name] next to mode indicator
    const filename_text = if (ed.filename) |f| f.items else "[No Name]";

    // Create a buffer for the filename with padding
    var buf: [256]u8 = undefined;
    const formatted = std.fmt.bufPrint(&buf, " {s} ", .{filename_text}) catch " [Filename too long] ";

    // Draw filename section
    const filename_segments = [_]vaxis.Segment{
        .{
            .text = formatted,
            .style = .{
                .fg = .{ .index = 15 }, // White text
                .bg = .{ .index = 8 }, // Dark gray background
            },
        },
    };
    _ = win.print(&filename_segments, .{
        .row_offset = @intCast(footer_row),
        .col_offset = @intCast(mode_text.len),
    });

    // Fill the rest of the footer with gray background
    const filled_width = mode_text.len + formatted.len;
    if (filled_width < win.width) {
        var col = filled_width;
        while (col < win.width) : (col += 1) {
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
        const box_width = msg_len + 4; // padding
        const box_height: u16 = 3;

        // Position at top right
        const box_x = if (win.width > box_width) win.width - box_width - 2 else 0;
        const box_y: u16 = 1;

        // Create status box with border
        const status_box = win.child(.{
            .x_off = @intCast(box_x),
            .y_off = @intCast(box_y),
            .width = box_width,
            .height = box_height,
            .border = .{
                .where = .all,
                .style = .{ .fg = .{ .index = 10 } }, // Bright green border
                .glyphs = .single_rounded,
            },
        });

        // Draw message with bright green text
        const msg_segments = [_]vaxis.Segment{
            .{ .text = message, .style = .{ .fg = .{ .index = 10 }, .bold = true } },
        };
        _ = status_box.print(&msg_segments, .{ .row_offset = 0, .col_offset = 2 });
    }
}

pub fn drawSaveDialog(ed: *editor.Editor, win: vaxis.Window) void {
    const dialog_width: u16 = 50;
    const dialog_height: u16 = 5;
    const dialog_x = (win.width - dialog_width) / 2;
    const dialog_y = (win.height - dialog_height) / 2;

    // Create dialog window with rounded border
    const dialog = win.child(.{
        .x_off = @intCast(dialog_x),
        .y_off = @intCast(dialog_y),
        .width = dialog_width,
        .height = dialog_height,
        .border = .{
            .where = .all,
            .style = .{ .fg = .{ .index = 6 } },
            .glyphs = .single_rounded,
        },
    });

    // Draw title
    const title_segments = [_]vaxis.Segment{
        .{ .text = " Save File ", .style = .{ .fg = .{ .index = 6 }, .bold = true } },
    };
    _ = dialog.print(&title_segments, .{ .row_offset = 0, .col_offset = 2 });

    // Draw input prompt
    const prompt_segments = [_]vaxis.Segment{
        .{ .text = "Filename: ", .style = .{} },
    };
    _ = dialog.print(&prompt_segments, .{ .row_offset = 2, .col_offset = 2 });

    // Draw input text
    const input_segments = [_]vaxis.Segment{
        .{ .text = ed.save_input.items, .style = .{} },
    };
    _ = dialog.print(&input_segments, .{ .row_offset = 2, .col_offset = 12 });

    // Draw cursor in input
    const cursor_segments = [_]vaxis.Segment{
        .{ .text = "█", .style = .{ .fg = .{ .index = 7 } } },
    };
    _ = dialog.print(&cursor_segments, .{
        .row_offset = 2,
        .col_offset = @intCast(12 + ed.save_input.items.len),
    });
}
