const std = @import("std");
const vaxis = @import("vaxis");

const Event = union(enum) {
    key_press: vaxis.Key,
    winsize: vaxis.Winsize,
};

const MultilineEditor = struct {
    lines: std.ArrayList(std.ArrayList(u8)),
    cursor_row: usize,
    cursor_col: usize,
    allocator: std.mem.Allocator,

    fn init(allocator: std.mem.Allocator) !MultilineEditor {
        var lines = std.ArrayList(std.ArrayList(u8)){};
        const first_line = std.ArrayList(u8){};
        try lines.append(allocator, first_line);

        return MultilineEditor{
            .lines = lines,
            .cursor_row = 0,
            .cursor_col = 0,
            .allocator = allocator,
        };
    }

    fn deinit(self: *MultilineEditor) void {
        for (self.lines.items) |*line| {
            line.deinit(self.allocator);
        }
        self.lines.deinit(self.allocator);
    }

    fn insertChar(self: *MultilineEditor, char: u8) !void {
        if (self.cursor_row >= self.lines.items.len) return;
        try self.lines.items[self.cursor_row].insert(self.allocator, self.cursor_col, char);
        self.cursor_col += 1;
    }

    fn insertText(self: *MultilineEditor, text: []const u8) !void {
        if (self.cursor_row >= self.lines.items.len) return;
        for (text) |char| {
            try self.lines.items[self.cursor_row].insert(self.allocator, self.cursor_col, char);
            self.cursor_col += 1;
        }
    }

    fn insertNewline(self: *MultilineEditor) !void {
        if (self.cursor_row >= self.lines.items.len) return;

        // Split current line at cursor
        const current_line = &self.lines.items[self.cursor_row];
        var new_line = std.ArrayList(u8){};

        // Move text after cursor to new line
        if (self.cursor_col < current_line.items.len) {
            try new_line.appendSlice(self.allocator, current_line.items[self.cursor_col..]);
            current_line.shrinkRetainingCapacity(self.cursor_col);
        }

        // Insert new line
        try self.lines.insert(self.allocator, self.cursor_row + 1, new_line);
        self.cursor_row += 1;
        self.cursor_col = 0;
    }

    fn handleKey(self: *MultilineEditor, key: vaxis.Key) !void {
        // Arrow keys and navigation
        if (key.codepoint == vaxis.Key.left) {
            if (self.cursor_col > 0) {
                self.cursor_col -= 1;
            } else if (self.cursor_row > 0) {
                // Move to end of previous line
                self.cursor_row -= 1;
                self.cursor_col = self.lines.items[self.cursor_row].items.len;
            }
        } else if (key.codepoint == vaxis.Key.right) {
            if (self.cursor_col < self.lines.items[self.cursor_row].items.len) {
                self.cursor_col += 1;
            } else if (self.cursor_row < self.lines.items.len - 1) {
                // Move to start of next line
                self.cursor_row += 1;
                self.cursor_col = 0;
            }
        } else if (key.codepoint == vaxis.Key.up) {
            if (self.cursor_row > 0) {
                self.cursor_row -= 1;
                // Keep cursor_col, but clamp to line length
                self.cursor_col = @min(self.cursor_col, self.lines.items[self.cursor_row].items.len);
            }
        } else if (key.codepoint == vaxis.Key.down) {
            if (self.cursor_row < self.lines.items.len - 1) {
                self.cursor_row += 1;
                // Keep cursor_col, but clamp to line length
                self.cursor_col = @min(self.cursor_col, self.lines.items[self.cursor_row].items.len);
            }
        } else if (key.codepoint == vaxis.Key.home) {
            self.cursor_col = 0;
        } else if (key.codepoint == vaxis.Key.end) {
            self.cursor_col = self.lines.items[self.cursor_row].items.len;
        } else if (key.codepoint == '\r' or key.codepoint == '\n') {
            try self.insertNewline();
        } else if (key.codepoint == 127 or key.codepoint == 8) { // Backspace
            if (self.cursor_col > 0) {
                self.cursor_col -= 1;
                _ = self.lines.items[self.cursor_row].orderedRemove(self.cursor_col);
            } else if (self.cursor_row > 0) {
                // Join with previous line
                var current_line = self.lines.orderedRemove(self.cursor_row);
                self.cursor_row -= 1;
                self.cursor_col = self.lines.items[self.cursor_row].items.len;
                try self.lines.items[self.cursor_row].appendSlice(self.allocator, current_line.items);
                current_line.deinit(self.allocator);
            }
        } else if (key.text) |text| {
            // Use the text field which properly handles Shift and other modifiers
            if (text.len > 0) {
                try self.insertText(text);
            }
        } else if (key.codepoint >= 32 and key.codepoint <= 126) { // Fallback to codepoint
            try self.insertChar(@intCast(key.codepoint));
        }
    }

    fn draw(self: *MultilineEditor, win: vaxis.Window) void {
        for (self.lines.items, 0..) |line, row| {
            if (row >= win.height) break;

            const segments = [_]vaxis.Segment{
                .{ .text = line.items, .style = .{} },
            };
            _ = win.print(&segments, .{ .row_offset = @intCast(row) });
        }

        // Draw cursor
        if (self.cursor_row < win.height) {
            const cursor_segments = [_]vaxis.Segment{
                .{ .text = "█", .style = .{ .fg = .{ .index = 7 } } },
            };
            _ = win.print(&cursor_segments, .{
                .row_offset = @intCast(self.cursor_row),
                .col_offset = @intCast(self.cursor_col),
            });
        }
    }
};

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer std.debug.assert(gpa.deinit() == .ok);
    const allocator = gpa.allocator();

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

    var editor = try MultilineEditor.init(allocator);
    defer editor.deinit();

    try vx.queryTerminal(tty.writer(), 1 * std.time.ns_per_s);
    while (true) {
        const event = loop.nextEvent();
        switch (event) {
            .key_press => |key| {
                if (key.matches('c', .{ .ctrl = true })) {
                    break;
                } else if (key.matches('l', .{ .ctrl = true })) {
                    vx.queueRefresh();
                } else {
                    try editor.handleKey(key);
                }
            },
            .winsize => |ws| try vx.resize(allocator, tty.writer(), ws),
        }

        const win = vx.window();
        win.clear();

        const child = win.child(.{
            .x_off = 2,
            .y_off = 2,
            .width = win.width - 4,
            .height = win.height - 4,
        });

        editor.draw(child);

        try vx.render(tty.writer());
    }
}
