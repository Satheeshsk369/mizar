const std = @import("std");
const editor = @import("editor.zig");
const context = @import("context.zig");
const buffer = @import("buffer.zig");

pub const Action = *const fn (context.ActionContext) anyerror!void;

pub fn moveUp(ctx: context.ActionContext) !void {
    if (ctx.ed.cursor_row > 0) {
        ctx.ed.cursor_row -= 1;
        ctx.ed.cursor_col = @min(ctx.ed.cursor_col, ctx.ed.buffer.lines.items[ctx.ed.cursor_row].items.len);
        ctx.ed.adjustScroll(ctx.viewport_height);
    }
}

pub fn moveDown(ctx: context.ActionContext) !void {
    if (ctx.ed.cursor_row < ctx.ed.buffer.lines.items.len - 1) {
        ctx.ed.cursor_row += 1;
        ctx.ed.cursor_col = @min(ctx.ed.cursor_col, ctx.ed.buffer.lines.items[ctx.ed.cursor_row].items.len);
        ctx.ed.adjustScroll(ctx.viewport_height);
    }
}

pub fn moveLeft(ctx: context.ActionContext) !void {
    if (ctx.ed.cursor_col > 0) {
        ctx.ed.cursor_col -= 1;
    } else if (ctx.ed.cursor_row > 0) {
        ctx.ed.cursor_row -= 1;
        ctx.ed.cursor_col = ctx.ed.buffer.lines.items[ctx.ed.cursor_row].items.len;
    }
}

pub fn moveRight(ctx: context.ActionContext) !void {
    if (ctx.ed.cursor_col < ctx.ed.buffer.lines.items[ctx.ed.cursor_row].items.len) {
        ctx.ed.cursor_col += 1;
    } else if (ctx.ed.cursor_row < ctx.ed.buffer.lines.items.len - 1) {
        ctx.ed.cursor_row += 1;
        ctx.ed.cursor_col = 0;
    }
}

pub fn insertChar(ctx: context.ActionContext) !void {
    if (ctx.key.text) |text| {
        if (text.len > 0) {
            try ctx.ed.insertText(text);
        }
    } else if (ctx.key.codepoint >= 32 and ctx.key.codepoint <= 126) {
        try ctx.ed.insertChar(@intCast(ctx.key.codepoint));
    }
}

pub fn insertNewline(ctx: context.ActionContext) !void {
    try ctx.ed.insertNewline();
}

pub fn backspace(ctx: context.ActionContext) !void {
    if (ctx.ed.cursor_col > 0) {
        ctx.ed.cursor_col -= 1;
        _ = ctx.ed.buffer.lines.items[ctx.ed.cursor_row].orderedRemove(ctx.ed.cursor_col);
    } else if (ctx.ed.cursor_row > 0) {
        var current_line = ctx.ed.buffer.lines.orderedRemove(ctx.ed.cursor_row);
        ctx.ed.cursor_row -= 1;
        ctx.ed.cursor_col = ctx.ed.buffer.lines.items[ctx.ed.cursor_row].items.len;
        try ctx.ed.buffer.lines.items[ctx.ed.cursor_row].appendSlice(ctx.ed.buffer.allocator, current_line.items);
        current_line.deinit(ctx.ed.buffer.allocator);
    }
}

pub fn toNormalMode(ctx: context.ActionContext) !void {
    ctx.ed.mode = .normal;
}

pub fn toInsertMode(ctx: context.ActionContext) !void {
    ctx.ed.mode = .insert;
}

pub fn home(ctx: context.ActionContext) !void {
    ctx.ed.cursor_col = 0;
}

pub fn end(ctx: context.ActionContext) !void {
    ctx.ed.cursor_col = ctx.ed.buffer.lines.items[ctx.ed.cursor_row].items.len;
}

pub fn pageUp(ctx: context.ActionContext) !void {
    if (ctx.ed.cursor_row > ctx.viewport_height) {
        ctx.ed.cursor_row -= ctx.viewport_height;
    } else {
        ctx.ed.cursor_row = 0;
    }
    ctx.ed.cursor_col = @min(ctx.ed.cursor_col, ctx.ed.buffer.lines.items[ctx.ed.cursor_row].items.len);
    ctx.ed.adjustScroll(ctx.viewport_height);
}

pub fn pageDown(ctx: context.ActionContext) !void {
    if (ctx.ed.cursor_row + ctx.viewport_height < ctx.ed.buffer.lines.items.len) {
        ctx.ed.cursor_row += ctx.viewport_height;
    } else {
        ctx.ed.cursor_row = ctx.ed.buffer.lines.items.len - 1;
    }
    ctx.ed.cursor_col = @min(ctx.ed.cursor_col, ctx.ed.buffer.lines.items[ctx.ed.cursor_row].items.len);
    ctx.ed.adjustScroll(ctx.viewport_height);
}

pub fn undo(ctx: context.ActionContext) !void {
    if (ctx.ed.history.popUndo()) |prev_buffer_ptr| {
        try ctx.ed.history.pushRedo(try ctx.allocator.create(buffer.Buffer));
        ctx.ed.history.redo.items[ctx.ed.history.redo.items.len - 1].* = try ctx.ed.buffer.snapshot(ctx.allocator);

        ctx.ed.buffer.deinit();
        ctx.ed.buffer = prev_buffer_ptr.*;
        ctx.allocator.destroy(prev_buffer_ptr);
        ctx.ed.modified = true;
    }
}

pub fn redo(ctx: context.ActionContext) !void {
    if (ctx.ed.history.popRedo()) |next_buffer| {
        try ctx.ed.history.push(try ctx.allocator.create(buffer.Buffer));
        ctx.ed.history.undo.items[ctx.ed.history.undo.items.len - 1].* = try ctx.ed.buffer.snapshot(ctx.allocator);

        ctx.ed.buffer.deinit();
        ctx.ed.buffer = next_buffer.*;
        ctx.ed.modified = true;
    }
}
