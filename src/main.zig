const std = @import("std");
const ui = @import("ui.zig");
const vaxis = @import("vaxis");
const keys = @import("keys.zig");
const editor = @import("editor.zig");

const Event = union(enum) {
    key_press: vaxis.Key,
    winsize: vaxis.Winsize,
};

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer std.debug.assert(gpa.deinit() == .ok);
    const allocator = gpa.allocator();

    // Parse command-line arguments
    var args = try std.process.argsWithAllocator(allocator);
    defer args.deinit();
    _ = args.next();

    const filename_arg = args.next();

    var buffer: [1024]u8 = undefined;
    var tty = try vaxis.Tty.init(&buffer);
    defer tty.deinit();

    var vx = try vaxis.init(allocator, .{});
    defer vx.deinit(allocator, tty.writer());

    var loop: vaxis.Loop(Event) = .{ .tty = &tty, .vaxis = &vx };
    try loop.init();
    try loop.start();
    defer loop.stop();

    try vx.enterAltScreen(tty.writer());

    var keymaps = try keys.Keymaps.init(allocator);
    defer keymaps.deinit();

    var ed = try editor.Editor.init(allocator);
    defer ed.deinit();

    if (filename_arg) |filename| {
        try ed.loadFromFile(filename);
    }

    try vx.queryTerminal(tty.writer(), 1 * std.time.ns_per_s);
    while (true) {
        const event = loop.nextEvent();

        const win = vx.window();
        const child_height = if (win.height > 4) win.height - 4 else 1;

        switch (event) {
            .key_press => |key| {
                if (key.matches('q', .{})) {
                    break;
                } else if (key.matches('l', .{ .ctrl = true })) {
                    vx.queueRefresh();
                } else if (key.matches('s', .{ .ctrl = true })) {
                    if (ed.filename == null) {
                        ed.save_input_active = true;
                        ed.save_input.clearRetainingCapacity();
                    } else {
                        ed.saveToFile() catch |err| {
                            var err_msg: [128]u8 = undefined;
                            const formatted_err = std.fmt.bufPrint(&err_msg, "Error saving file: {}", .{err}) catch "Error saving file";
                            ed.showStatus(formatted_err);
                        };
                    }
                } else if (ed.save_input_active) {
                    if (key.codepoint == '\r' or key.codepoint == '\n') {
                        if (ed.save_input.items.len > 0) {
                            var fname = std.ArrayList(u8){};
                            try fname.appendSlice(allocator, ed.save_input.items);
                            ed.filename = fname;
                            ed.saveToFile() catch |err| {
                                var err_msg: [128]u8 = undefined;
                                const formatted_err = std.fmt.bufPrint(&err_msg, "Error saving file: {}", .{err}) catch "Error saving file";
                                ed.showStatus(formatted_err);
                            };
                            ed.save_input_active = false;
                        }
                    } else if (key.codepoint == 27) {
                        ed.save_input_active = false;
                        ed.save_input.clearRetainingCapacity();
                    } else if (key.codepoint == 127 or key.codepoint == 8) {
                        if (ed.save_input.items.len > 0) {
                            _ = ed.save_input.pop();
                        }
                    } else if (key.text) |text| {
                        if (text.len > 0) {
                            try ed.save_input.appendSlice(allocator, text);
                        }
                    } else if (key.codepoint >= 32 and key.codepoint <= 126) {
                        try ed.save_input.append(allocator, @intCast(key.codepoint));
                    }
                } else {
                    try keys.handleKey(&ed, key, child_height, &keymaps, allocator);
                }
            },
            .winsize => |ws| try vx.resize(allocator, tty.writer(), ws),
        }

        win.clear();
        ui.draw(&ed, win);
        ui.drawFooter(&ed, win);

        if (ed.shouldShowStatus()) ui.drawStatusMessage(&ed, win);
        try vx.render(tty.writer());
    }
}
