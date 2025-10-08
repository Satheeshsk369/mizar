const std = @import("std");
const vaxis = @import("vaxis");
const editor = @import("editor.zig");
const actions = @import("actions.zig");

const Keymap = std.AutoHashMap(vaxis.Key, actions.Action);

pub const Keymaps = struct {
    normal: Keymap,
    insert: Keymap,

    pub fn init(allocator: std.mem.Allocator) !Keymaps {
        var normal_map = Keymap.init(allocator);
        try normal_map.put(.{ .codepoint = 'i', .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.toInsertMode);
        try normal_map.put(.{ .codepoint = vaxis.Key.up, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.moveUp);
        try normal_map.put(.{ .codepoint = vaxis.Key.down, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.moveDown);
        try normal_map.put(.{ .codepoint = vaxis.Key.left, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.moveLeft);
        try normal_map.put(.{ .codepoint = vaxis.Key.right, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.moveRight);
        try normal_map.put(.{ .codepoint = vaxis.Key.home, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.home);
        try normal_map.put(.{ .codepoint = vaxis.Key.end, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.end);
        try normal_map.put(.{ .codepoint = vaxis.Key.page_up, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.pageUp);
        try normal_map.put(.{ .codepoint = vaxis.Key.page_down, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.pageDown);

        var insert_map = Keymap.init(allocator);
        try insert_map.put(.{ .codepoint = 27, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.toNormalMode);
        try insert_map.put(.{ .codepoint = vaxis.Key.up, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.moveUp);
        try insert_map.put(.{ .codepoint = vaxis.Key.down, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.moveDown);
        try insert_map.put(.{ .codepoint = vaxis.Key.left, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.moveLeft);
        try insert_map.put(.{ .codepoint = vaxis.Key.right, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.moveRight);
        try insert_map.put(.{ .codepoint = vaxis.Key.home, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.home);
        try insert_map.put(.{ .codepoint = vaxis.Key.end, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.end);
        try insert_map.put(.{ .codepoint = vaxis.Key.page_up, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.pageUp);
        try insert_map.put(.{ .codepoint = vaxis.Key.page_down, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.pageDown);
        try insert_map.put(.{ .codepoint = '\r', .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.insertNewline);
        try insert_map.put(.{ .codepoint = '\n', .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.insertNewline);
        try insert_map.put(.{ .codepoint = 127, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.backspace);
        try insert_map.put(.{ .codepoint = 8, .mods = .{}, .text = null, .shifted_codepoint = null, .base_layout_codepoint = null }, actions.backspace);

        return Keymaps{
            .normal = normal_map,
            .insert = insert_map,
        };
    }

    pub fn deinit(self: *Keymaps) void {
        self.normal.deinit();
        self.insert.deinit();
    }
};

const context = @import("context.zig");

pub fn handleKey(ed: *editor.Editor, key: vaxis.Key, viewport_height: usize, keymaps: *const Keymaps, allocator: std.mem.Allocator) !void {
    const map = switch (ed.mode) {
        .normal => &keymaps.normal,
        .insert => &keymaps.insert,
    };

    const key_with_text = key;
    var key_without_text = key;
    key_without_text.text = null;

    const ctx = context.ActionContext{
        .ed = ed,
        .key = key,
        .viewport_height = viewport_height,
        .keymaps = keymaps,
        .allocator = allocator,
    };

    if (map.get(key_with_text)) |action| {
        try action(ctx);
    } else if (map.get(key_without_text)) |action| {
        try action(ctx);
    } else if (ed.mode == .insert) {
        try actions.insertChar(ctx);
    }
}
