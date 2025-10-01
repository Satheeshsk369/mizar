const std = @import("std");
const syntax = @import("syntax");
const vaxis = @import("vaxis");

const Self = @This();

pub const HighlightToken = struct {
    start_col: usize,
    end_col: usize,
    scope: []const u8,
    color: u8,
};

allocator: std.mem.Allocator,
query_cache: *syntax.QueryCache,
highlighter: ?*syntax = null,

pub fn init(allocator: std.mem.Allocator) !Self {
    const query_cache = try syntax.QueryCache.create(allocator, .{ .lock = false });
    return Self{
        .allocator = allocator,
        .query_cache = query_cache,
        .highlighter = null,
    };
}

pub fn deinit(self: *Self) void {
    if (self.highlighter) |h| {
        h.destroy(self.query_cache);
    }
    self.query_cache.deinit();
}

pub fn setFileType(self: *Self, file_type: syntax.FileType, content: []const u8) !void {
    // Destroy old highlighter if exists
    if (self.highlighter) |h| {
        h.destroy(self.query_cache);
        self.highlighter = null;
    }

    // Create new highlighter for file type
    self.highlighter = try syntax.create(file_type, self.allocator, self.query_cache);

    // Parse the content
    try self.highlighter.?.refresh_full(content);
}

pub fn refresh(self: *Self, content: []const u8) !void {
    if (self.highlighter) |h| {
        try h.refresh_full(content);
    }
}

/// Get color index for a syntax scope (simplified mapping)
fn scopeToColor(scope: []const u8) u8 {
    // Map tree-sitter scopes to vaxis color indices
    if (std.mem.indexOf(u8, scope, "keyword")) return 13; // Bright magenta
    if (std.mem.indexOf(u8, scope, "function")) return 14; // Bright cyan
    if (std.mem.indexOf(u8, scope, "string")) return 10; // Bright green
    if (std.mem.indexOf(u8, scope, "comment")) return 8; // Dark gray
    if (std.mem.indexOf(u8, scope, "number")) return 11; // Bright yellow
    if (std.mem.indexOf(u8, scope, "type")) return 12; // Bright blue
    if (std.mem.indexOf(u8, scope, "variable")) return 15; // White
    if (std.mem.indexOf(u8, scope, "operator")) return 9; // Bright red

    return 7; // Default white
}

/// Get highlight tokens for a specific line
/// TODO: Implement proper tree-sitter highlighting iteration
pub fn getLineHighlights(
    self: *Self,
    line_num: usize,
    allocator: std.mem.Allocator,
) ![]HighlightToken {
    _ = self;
    _ = line_num;
    _ = allocator;

    // For now, return empty - no highlighting yet
    // Will implement proper tree-sitter query iteration later
    return &[_]HighlightToken{};
}
