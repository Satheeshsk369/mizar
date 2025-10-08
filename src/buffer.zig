const std = @import("std");

pub const Buffer = struct {
    lines: std.ArrayList(std.ArrayList(u8)),
    allocator: std.mem.Allocator,

    pub fn init(allocator: std.mem.Allocator) !Buffer {
        var lines = std.ArrayList(std.ArrayList(u8)){};
        const first_line = std.ArrayList(u8){};
        try lines.append(allocator, first_line);
        return Buffer{
            .lines = lines,
            .allocator = allocator,
        };
    }

    pub fn deinit(self: *Buffer) void {
        for (self.lines.items) |*line| {
            line.deinit(self.allocator);
        }
        self.lines.deinit(self.allocator);
    }
};
