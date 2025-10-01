const std = @import("std");
const vaxis = @import("vaxis");
const syntax = @import("syntax");
const Highlighter = @import("highlighter.zig");
const Navigator = @import("navigation.zig").Navigator;

pub const Mode = enum {
    normal,
    insert,
    save_dialog,
};

pub const Editor = struct {
    lines: std.ArrayList(std.ArrayList(u8)),
    cursor_row: usize,
    cursor_col: usize,
    allocator: std.mem.Allocator,
    filename: ?std.ArrayList(u8),
    mode: Mode,
    save_input: std.ArrayList(u8),
    status_message: ?[]const u8,
    status_timestamp: i64,
    scroll_offset: usize,
    file_type: ?syntax.FileType,
    highlighter: Highlighter,
    navigator: Navigator,

    pub fn init(allocator: std.mem.Allocator) !Editor {
        var lines = std.ArrayList(std.ArrayList(u8)){};
        const first_line = std.ArrayList(u8){};
        try lines.append(allocator, first_line);

        const highlighter = try Highlighter.init(allocator);
        const navigator = Navigator.init(allocator);

        return Editor{
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
            .file_type = null,
            .highlighter = highlighter,
            .navigator = navigator,
        };
    }

    pub fn showStatus(self: *Editor, message: []const u8) void {
        self.status_message = message;
        self.status_timestamp = std.time.milliTimestamp();
    }

    pub fn shouldShowStatus(self: *Editor) bool {
        if (self.status_message == null) return false;
        const now = std.time.milliTimestamp();
        const elapsed = now - self.status_timestamp;
        return elapsed < 2000; // 2 seconds
    }

    pub fn loadFromFile(self: *Editor, path: []const u8) !void {
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

        // Detect file type from extension
        self.file_type = syntax.FileType.guess_static(path, content);

        // Initialize syntax highlighting if file type detected
        if (self.file_type) |ft| {
            self.highlighter.setFileType(ft, content) catch |err| {
                std.debug.print("Failed to initialize syntax highlighting: {}\n", .{err});
            };
        }

        // Detect blocks for navigation
        try self.navigator.detectBlocks(self.lines.items);

        // Reset cursor
        self.cursor_row = 0;
        self.cursor_col = 0;
    }

    pub fn deinit(self: *Editor) void {
        for (self.lines.items) |*line| {
            line.deinit(self.allocator);
        }
        self.lines.deinit(self.allocator);
        if (self.filename) |*fname| {
            fname.deinit(self.allocator);
        }
        self.save_input.deinit(self.allocator);
        self.highlighter.deinit();
        self.navigator.deinit();
    }

    pub fn saveToFile(self: *Editor) !void {
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

    pub fn insertChar(self: *Editor, char: u8) !void {
        if (self.cursor_row >= self.lines.items.len) return;
        try self.lines.items[self.cursor_row].insert(self.allocator, self.cursor_col, char);
        self.cursor_col += 1;
    }

    pub fn insertText(self: *Editor, text: []const u8) !void {
        if (self.cursor_row >= self.lines.items.len) return;
        for (text) |char| {
            try self.lines.items[self.cursor_row].insert(self.allocator, self.cursor_col, char);
            self.cursor_col += 1;
        }
    }

    pub fn insertNewline(self: *Editor) !void {
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

    pub fn adjustScroll(self: *Editor, viewport_height: usize) void {
        // Ensure cursor is visible in viewport
        if (self.cursor_row < self.scroll_offset) {
            self.scroll_offset = self.cursor_row;
        } else if (self.cursor_row >= self.scroll_offset + viewport_height) {
            self.scroll_offset = self.cursor_row - viewport_height + 1;
        }
    }

    /// Navigate down (blocks first, then lines)
    pub fn navigateDown(self: *Editor) void {
        if (self.navigator.navigateDown(self.cursor_row, self.lines.items.len)) |new_line| {
            self.cursor_row = new_line;
            self.cursor_col = 0;
        }
    }

    /// Navigate up (blocks first, then lines)
    pub fn navigateUp(self: *Editor) void {
        if (self.navigator.navigateUp(self.cursor_row)) |new_line| {
            self.cursor_row = new_line;
            self.cursor_col = 0;
        }
    }

    /// Navigate right (into child blocks)
    pub fn navigateRight(self: *Editor) void {
        _ = self.navigator.navigateRight(self.cursor_row, self.lines.items) catch {};
    }

    /// Navigate left (to parent level)
    pub fn navigateLeft(self: *Editor) void {
        if (self.navigator.navigateLeft(self.cursor_row)) |new_line| {
            self.cursor_row = new_line;
            self.cursor_col = 0;
        }
    }
};
