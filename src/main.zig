const std = @import("std");
const vaxis = @import("vaxis");
const editor = @import("editor.zig");
const ui = @import("ui.zig");
const keybindings = @import("keybindings.zig");

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

    // Skip program name
    _ = args.next();

    // Get optional filename argument
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

    var ed = try editor.Editor.init(allocator);
    defer ed.deinit();

    // Load file if provided as argument
    if (filename_arg) |filename| {
        ed.loadFromFile(filename) catch |err| {
            std.debug.print("Error loading file '{s}': {}\n", .{ filename, err });
            // Continue with empty editor if file doesn't exist
        };
    }

    try vx.queryTerminal(tty.writer(), 1 * std.time.ns_per_s);
    while (true) {
        const event = loop.nextEvent();

        // Get window dimensions early for key handling
        const win = vx.window();
        const child_height = if (win.height > 4) win.height - 4 else 1;

        switch (event) {
            .key_press => |key| {
                if (key.matches('q', .{ .ctrl = true })) {
                    break;
                } else if (key.matches('l', .{ .ctrl = true })) {
                    vx.queueRefresh();
                } else if (key.matches('s', .{ .ctrl = true })) {
                    // Ctrl+S: Save file
                    if (ed.filename == null) {
                        // Enter filename edit mode
                        ed.mode = .filename_edit;
                        ed.save_input.clearRetainingCapacity();
                    } else {
                        // Save directly if filename exists
                        ed.saveToFile() catch |err| {
                            std.debug.print("Error saving file: {}\n", .{err});
                        };
                    }
                } else if (ed.mode == .filename_edit) {
                    // Handle filename editing in status bar
                    if (key.codepoint == '\r' or key.codepoint == '\n') {
                        // Enter: Confirm save
                        if (ed.save_input.items.len > 0) {
                            var fname = std.ArrayList(u8){};
                            try fname.appendSlice(allocator, ed.save_input.items);
                            ed.filename = fname;
                            ed.saveToFile() catch |err| {
                                std.debug.print("Error saving file: {}\n", .{err});
                            };
                            ed.mode = .normal;
                        }
                    } else if (key.codepoint == 27) { // Escape
                        // Cancel filename edit
                        ed.mode = .normal;
                        ed.save_input.clearRetainingCapacity();
                    } else if (key.codepoint == 127 or key.codepoint == 8) { // Backspace
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
                    try keybindings.handleKey(&ed, key, child_height);
                }
            },
            .winsize => |ws| try vx.resize(allocator, tty.writer(), ws),
        }

        win.clear();

        ui.draw(&ed, win);

        // Draw footer bar at bottom
        ui.drawFooter(&ed, win);

        // Draw status message on root window (not child)
        if (ed.shouldShowStatus()) {
            ui.drawStatusMessage(&ed, win);
        }


        try vx.render(tty.writer());
    }
}
