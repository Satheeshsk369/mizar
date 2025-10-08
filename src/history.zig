const std = @import("std");
const buffer = @import("buffer.zig");

pub const History = struct {
    undo: std.ArrayList(*buffer.Buffer),
    redo: std.ArrayList(*buffer.Buffer),
    allocator: std.mem.Allocator,

    pub fn init(allocator: std.mem.Allocator) !History {
        return History{
            .undo = std.ArrayList(*buffer.Buffer){},
            .redo = std.ArrayList(*buffer.Buffer){},
            .allocator = allocator,
        };
    }

    pub fn deinit(self: *History) void {
        for (self.undo.items) |buf_ptr| {
            buf_ptr.deinit();
            self.allocator.destroy(buf_ptr);
        }
        self.undo.deinit(self.allocator);

        for (self.redo.items) |buf_ptr| {
            buf_ptr.deinit();
            self.allocator.destroy(buf_ptr);
        }
        self.redo.deinit(self.allocator);
    }

    pub fn push(self: *History, buf_ptr: *buffer.Buffer) !void {
        try self.undo.append(self.allocator, buf_ptr);
        for (self.redo.items) |redo_buf_ptr| {
            redo_buf_ptr.deinit();
            self.allocator.destroy(redo_buf_ptr);
        }
        self.redo.clearRetainingCapacity();
    }

    pub fn popUndo(self: *History) ?*buffer.Buffer {
        if (self.undo.items.len == 0) return null;
        return self.undo.pop();
    }

    pub fn pushRedo(self: *History, buf_ptr: *buffer.Buffer) !void {
        try self.redo.append(self.allocator, buf_ptr);
    }

    pub fn popRedo(self: *History) ?*buffer.Buffer {
        if (self.redo.items.len == 0) return null;
        return self.redo.pop();
    }
};
