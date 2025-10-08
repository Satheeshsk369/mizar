const std = @import("std");
const vaxis = @import("vaxis");
const buffer = @import("buffer.zig");

const STATUS_MESSAGE_DURATION_MS = 2000;
const MAX_FILE_SIZE_BYTES = 10 * 1024 * 1024; // 10MB max

pub const Mode = enum {
    normal,
    insert,
};

pub const Editor = struct {
    buffer: @import("buffer.zig").Buffer,
    history: @import("history.zig").History,
    cursor_row: usize,
    cursor_col: usize,

    filename: ?std.ArrayList(u8),
    mode: Mode,
    save_input_active: bool,
    save_input: std.ArrayList(u8),
    status_message: ?[]const u8,
    status_timestamp: i64,
    scroll_offset: usize,
    modified: bool,

    pub fn init(allocator: std.mem.Allocator) !Editor {
        const new_buffer = try buffer.Buffer.init(allocator);
        const history = try @import("history.zig").History.init(allocator);

        return Editor{
            .buffer = new_buffer,
            .history = history,
            .cursor_row = 0,
            .cursor_col = 0,
            .filename = null,
            .mode = .normal,
            .save_input_active = false,
            .save_input = std.ArrayList(u8){},
            .status_message = null,
            .status_timestamp = 0,
            .scroll_offset = 0,
            .modified = false,
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
        return elapsed < STATUS_MESSAGE_DURATION_MS;
    }

    pub fn loadFromFile(self: *Editor, path: []const u8) !void {
        const file = std.fs.cwd().openFile(path, .{}) catch |err| {
            if (err == error.FileNotFound) {
                var fname = std.ArrayList(u8){};
                try fname.appendSlice(self.buffer.allocator, path);
                self.filename = fname;
                for (self.buffer.lines.items) |*line| line.deinit(self.buffer.allocator);
                self.buffer.lines.clearRetainingCapacity();
                const empty_line = std.ArrayList(u8){};
                try self.buffer.lines.append(self.buffer.allocator, empty_line);
                return;
            } else {
                return err;
            }
        };
        defer file.close();

        for (self.buffer.lines.items) |*line| line.deinit(self.buffer.allocator);
        self.buffer.lines.clearRetainingCapacity();

        const content = try file.readToEndAlloc(self.buffer.allocator, MAX_FILE_SIZE_BYTES);
        defer self.buffer.allocator.free(content);

        var iter = std.mem.splitScalar(u8, content, '\n');
        while (iter.next()) |line_data| {
            var line = std.ArrayList(u8){};
            try line.appendSlice(self.buffer.allocator, line_data);
            try self.buffer.lines.append(self.buffer.allocator, line);
        }

        if (self.buffer.lines.items.len == 0) {
            const empty_line = std.ArrayList(u8){};
            try self.buffer.lines.append(self.buffer.allocator, empty_line);
        }

        var fname = std.ArrayList(u8){};
        try fname.appendSlice(self.buffer.allocator, path);
        self.filename = fname;

        self.cursor_row = 0;
        self.cursor_col = 0;
    }

    pub fn saveState(self: *Editor) !void {
        const snapshot = try self.buffer.snapshot(self.buffer.allocator);
        const snapshot_ptr = try self.buffer.allocator.create(buffer.Buffer);
        snapshot_ptr.* = snapshot;
        try self.history.push(snapshot_ptr);
    }

    pub fn deinit(self: *Editor) void {
        self.buffer.deinit();
        self.history.deinit();
        if (self.filename) |*fname| fname.deinit(self.buffer.allocator);
        self.save_input.deinit(self.buffer.allocator);
    }

    pub fn saveToFile(self: *Editor) !void {
        const fname = if (self.filename) |f| f.items else return error.NoFilename;

        const file = try std.fs.cwd().createFile(fname, .{});
        defer file.close();

        for (self.buffer.lines.items, 0..) |line, i| {
            try file.writeAll(line.items);
            if (i < self.buffer.lines.items.len - 1) {
                try file.writeAll("\n");
            }
        }

        self.modified = false;
    }

    pub fn insertChar(self: *Editor, char: u8) !void {
        try self.saveState();
        if (self.cursor_row >= self.buffer.lines.items.len) return;
        try self.buffer.lines.items[self.cursor_row].insert(self.buffer.allocator, self.cursor_col, char);
        self.cursor_col += 1;
        self.modified = true;
    }

    pub fn insertText(self: *Editor, text: []const u8) !void {
        try self.saveState();
        if (self.cursor_row >= self.buffer.lines.items.len) return;
        for (text) |char| {
            try self.buffer.lines.items[self.cursor_row].insert(self.buffer.allocator, self.cursor_col, char);
            self.cursor_col += 1;
        }
        self.modified = true;
    }

    pub fn insertNewline(self: *Editor) !void {
        try self.saveState();
        if (self.cursor_row >= self.buffer.lines.items.len) return;

        const current_line = &self.buffer.lines.items[self.cursor_row];
        var new_line = std.ArrayList(u8){};

        if (self.cursor_col < current_line.items.len) {
            try new_line.appendSlice(self.buffer.allocator, current_line.items[self.cursor_col..]);
            current_line.shrinkRetainingCapacity(self.cursor_col);
        }

        try self.buffer.lines.insert(self.buffer.allocator, self.cursor_row + 1, new_line);
        self.cursor_row += 1;
        self.cursor_col = 0;
        self.modified = true;
    }

    pub fn adjustScroll(self: *Editor, viewport_height: usize) void {
        if (self.cursor_row < self.scroll_offset) {
            self.scroll_offset = self.cursor_row;
        } else if (self.cursor_row >= self.scroll_offset + viewport_height) {
            self.scroll_offset = self.cursor_row - viewport_height + 1;
        }
    }
};
