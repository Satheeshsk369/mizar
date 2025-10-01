const std = @import("std");
const vaxis = @import("vaxis");

const Event = union(enum) {
    key_press: vaxis.Key,
    winsize: vaxis.Winsize,
};

const EditorMode = enum {
    normal,
    save_dialog,
};

const MultilineEditor = struct {
    lines: std.ArrayList(std.ArrayList(u8)),
    cursor_row: usize,
    cursor_col: usize,
    allocator: std.mem.Allocator,
    filename: ?std.ArrayList(u8),
    mode: EditorMode,
    save_input: std.ArrayList(u8),
    status_message: ?[]const u8,
    status_timestamp: i64,
    scroll_offset: usize,

    fn init(allocator: std.mem.Allocator) !MultilineEditor {
        var lines = std.ArrayList(std.ArrayList(u8)){};
        const first_line = std.ArrayList(u8){};
        try lines.append(allocator, first_line);

        return MultilineEditor{
            .lines = lines,
            .cursor_row = 0,
            .cursor_col = 0,
            .allocator = allocator,
            .filename = null,
            .mode = .normal,
            .save_input = std.ArrayList(u8){},
            .status_message = null,
            .status_timestamp = 0,
            .scroll_offset = 0,
        };
    }

    fn showStatus(self: *MultilineEditor, message: []const u8) void {
        self.status_message = message;
        self.status_timestamp = std.time.milliTimestamp();
    }

    fn shouldShowStatus(self: *MultilineEditor) bool {
        if (self.status_message == null) return false;
        const now = std.time.milliTimestamp();
        const elapsed = now - self.status_timestamp;
        return elapsed < 2000; // 2 seconds
    }

    fn loadFromFile(self: *MultilineEditor, path: []const u8) !void {
        const file = try std.fs.cwd().openFile(path, .{});
        defer file.close();

        // Clear existing content
        for (self.lines.items) |*line| {
            line.deinit(self.allocator);
        }
        self.lines.clearRetainingCapacity();

        // Read entire file content
        const max_file_size = 10 * 1024 * 1024; // 10MB max
        const content = try file.readToEndAlloc(self.allocator, max_file_size);
        defer self.allocator.free(content);

        // Split content by newlines
        var iter = std.mem.splitScalar(u8, content, '\n');
        while (iter.next()) |line_data| {
            var line = std.ArrayList(u8){};
            try line.appendSlice(self.allocator, line_data);
            try self.lines.append(self.allocator, line);
        }

        // Ensure at least one line exists
        if (self.lines.items.len == 0) {
            const empty_line = std.ArrayList(u8){};
            try self.lines.append(self.allocator, empty_line);
        }

        // Store filename
        var fname = std.ArrayList(u8){};
        try fname.appendSlice(self.allocator, path);
        self.filename = fname;

        // Reset cursor
        self.cursor_row = 0;
        self.cursor_col = 0;
    }

    fn deinit(self: *MultilineEditor) void {
        for (self.lines.items) |*line| {
            line.deinit(self.allocator);
        }
        self.lines.deinit(self.allocator);
        if (self.filename) |*fname| {
            fname.deinit(self.allocator);
        }
        self.save_input.deinit(self.allocator);
    }

    fn saveToFile(self: *MultilineEditor) !void {
        const fname = if (self.filename) |f| f.items else return error.NoFilename;

        const file = try std.fs.cwd().createFile(fname, .{});
        defer file.close();

        for (self.lines.items, 0..) |line, i| {
            try file.writeAll(line.items);
            if (i < self.lines.items.len - 1) {
                try file.writeAll("\n");
            }
        }
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

    fn adjustScroll(self: *MultilineEditor, viewport_height: usize) void {
        // Ensure cursor is visible in viewport
        if (self.cursor_row < self.scroll_offset) {
            self.scroll_offset = self.cursor_row;
        } else if (self.cursor_row >= self.scroll_offset + viewport_height) {
            self.scroll_offset = self.cursor_row - viewport_height + 1;
        }
    }

    fn handleKey(self: *MultilineEditor, key: vaxis.Key, viewport_height: usize) !void {
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
                self.adjustScroll(viewport_height);
            }
        } else if (key.codepoint == vaxis.Key.down) {
            if (self.cursor_row < self.lines.items.len - 1) {
                self.cursor_row += 1;
                // Keep cursor_col, but clamp to line length
                self.cursor_col = @min(self.cursor_col, self.lines.items[self.cursor_row].items.len);
                self.adjustScroll(viewport_height);
            }
        } else if (key.codepoint == vaxis.Key.home) {
            self.cursor_col = 0;
        } else if (key.codepoint == vaxis.Key.end) {
            self.cursor_col = self.lines.items[self.cursor_row].items.len;
        } else if (key.codepoint == vaxis.Key.page_up) {
            // Page Up: Move up by viewport height
            if (self.cursor_row > viewport_height) {
                self.cursor_row -= viewport_height;
            } else {
                self.cursor_row = 0;
            }
            self.cursor_col = @min(self.cursor_col, self.lines.items[self.cursor_row].items.len);
            self.adjustScroll(viewport_height);
        } else if (key.codepoint == vaxis.Key.page_down) {
            // Page Down: Move down by viewport height
            if (self.cursor_row + viewport_height < self.lines.items.len) {
                self.cursor_row += viewport_height;
            } else {
                self.cursor_row = self.lines.items.len - 1;
            }
            self.cursor_col = @min(self.cursor_col, self.lines.items[self.cursor_row].items.len);
            self.adjustScroll(viewport_height);
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
        // Draw editor content with scroll offset
        const start_line = self.scroll_offset;
        const end_line = @min(start_line + win.height, self.lines.items.len);

        for (start_line..end_line) |line_idx| {
            const screen_row = line_idx - start_line;
            if (screen_row >= win.height) break;

            const segments = [_]vaxis.Segment{
                .{ .text = self.lines.items[line_idx].items, .style = .{} },
            };
            _ = win.print(&segments, .{ .row_offset = @intCast(screen_row) });
        }

        // Draw cursor in normal mode (adjusted for scroll)
        if (self.mode == .normal and self.cursor_row >= self.scroll_offset) {
            const screen_row = self.cursor_row - self.scroll_offset;
            if (screen_row < win.height) {
                const cursor_segments = [_]vaxis.Segment{
                    .{ .text = "█", .style = .{ .fg = .{ .index = 7 } } },
                };
                _ = win.print(&cursor_segments, .{
                    .row_offset = @intCast(screen_row),
                    .col_offset = @intCast(self.cursor_col),
                });
            }
        }
    }

    fn drawStatusMessage(self: *MultilineEditor, win: vaxis.Window) void {
        if (self.status_message) |message| {
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

    fn drawSaveDialog(self: *MultilineEditor, win: vaxis.Window) void {
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
            .{ .text = self.save_input.items, .style = .{} },
        };
        _ = dialog.print(&input_segments, .{ .row_offset = 2, .col_offset = 12 });

        // Draw cursor in input
        const cursor_segments = [_]vaxis.Segment{
            .{ .text = "█", .style = .{ .fg = .{ .index = 7 } } },
        };
        _ = dialog.print(&cursor_segments, .{
            .row_offset = 2,
            .col_offset = @intCast(12 + self.save_input.items.len),
        });
    }
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

    var editor = try MultilineEditor.init(allocator);
    defer editor.deinit();

    // Load file if provided as argument
    if (filename_arg) |filename| {
        editor.loadFromFile(filename) catch |err| {
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
                    if (editor.filename == null) {
                        // Open save dialog for first time
                        editor.mode = .save_dialog;
                        editor.save_input.clearRetainingCapacity();
                    } else {
                        // Save directly if filename exists
                        editor.saveToFile() catch |err| {
                            std.debug.print("Error saving file: {}\n", .{err});
                        };
                        editor.showStatus("File saved!");
                    }
                } else if (editor.mode == .save_dialog) {
                    // Handle save dialog input
                    if (key.codepoint == '\r' or key.codepoint == '\n') {
                        // Enter: Confirm save
                        if (editor.save_input.items.len > 0) {
                            var fname = std.ArrayList(u8){};
                            try fname.appendSlice(allocator, editor.save_input.items);
                            editor.filename = fname;
                            editor.saveToFile() catch |err| {
                                std.debug.print("Error saving file: {}\n", .{err});
                            };
                            editor.showStatus("File saved!");
                            editor.mode = .normal;
                        }
                    } else if (key.codepoint == 27) { // Escape
                        // Cancel save dialog
                        editor.mode = .normal;
                        editor.save_input.clearRetainingCapacity();
                    } else if (key.codepoint == 127 or key.codepoint == 8) { // Backspace
                        if (editor.save_input.items.len > 0) {
                            _ = editor.save_input.pop();
                        }
                    } else if (key.text) |text| {
                        if (text.len > 0) {
                            try editor.save_input.appendSlice(allocator, text);
                        }
                    } else if (key.codepoint >= 32 and key.codepoint <= 126) {
                        try editor.save_input.append(allocator, @intCast(key.codepoint));
                    }
                } else {
                    try editor.handleKey(key, child_height);
                }
            },
            .winsize => |ws| try vx.resize(allocator, tty.writer(), ws),
        }

        win.clear();

        const child = win.child(.{
            .x_off = 2,
            .y_off = 2,
            .width = if (win.width > 4) win.width - 4 else 0,
            .height = if (win.height > 4) win.height - 4 else 0,
        });

        editor.draw(child);

        // Draw status message on root window (not child)
        if (editor.shouldShowStatus()) {
            editor.drawStatusMessage(win);
        }

        // Draw save dialog on root window (not child)
        if (editor.mode == .save_dialog) {
            editor.drawSaveDialog(win);
        }

        try vx.render(tty.writer());
    }
}
