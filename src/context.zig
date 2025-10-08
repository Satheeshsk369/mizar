const std = @import("std");
const editor = @import("editor.zig");
const vaxis = @import("vaxis");
const keys = @import("keys.zig");

pub const ActionContext = struct {
    ed: *editor.Editor,
    key: vaxis.Key,
    viewport_height: usize,
    keymaps: *const keys.Keymaps,
    allocator: std.mem.Allocator,
};
