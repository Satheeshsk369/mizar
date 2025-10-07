const std = @import("std");
const vaxis = @import("vaxis");
const editor = @import("editor.zig");

pub fn handleKey(ed: *editor.Editor, key: vaxis.Key, viewport_height: usize) !void {
    // Mode switching
    if (ed.mode == .normal) {
        // Normal mode: i enters insert mode, arrows for navigation
        if (key.matches('i', .{})) {
            ed.mode = .insert;
            return;
        }
        // Space as leader key (placeholder for future commands)
        if (key.matches(' ', .{})) {
            // TODO: Leader key commands
            return;
        }
    } else if (ed.mode == .insert) {
        // Insert mode: Esc returns to normal mode
        if (key.codepoint == 27) { // Escape
            ed.mode = .normal;
            return;
        }
    }

    // Navigation: Same behavior in both modes
    if (key.codepoint == vaxis.Key.left) {
        // Character left
        if (ed.cursor_col > 0) {
            ed.cursor_col -= 1;
        } else if (ed.cursor_row > 0) {
            ed.cursor_row -= 1;
            ed.cursor_col = ed.lines.items[ed.cursor_row].items.len;
        }
    } else if (key.codepoint == vaxis.Key.right) {
        // Character right
        if (ed.cursor_col < ed.lines.items[ed.cursor_row].items.len) {
            ed.cursor_col += 1;
        } else if (ed.cursor_row < ed.lines.items.len - 1) {
            ed.cursor_row += 1;
            ed.cursor_col = 0;
        }
    } else if (key.codepoint == vaxis.Key.up) {
        // Line up
        if (ed.cursor_row > 0) {
            ed.cursor_row -= 1;
            ed.cursor_col = @min(ed.cursor_col, ed.lines.items[ed.cursor_row].items.len);
            ed.adjustScroll(viewport_height);
        }
    } else if (key.codepoint == vaxis.Key.down) {
        // Line down
        if (ed.cursor_row < ed.lines.items.len - 1) {
            ed.cursor_row += 1;
            ed.cursor_col = @min(ed.cursor_col, ed.lines.items[ed.cursor_row].items.len);
            ed.adjustScroll(viewport_height);
        }
    } else if (key.codepoint == vaxis.Key.home) {
        ed.cursor_col = 0;
    } else if (key.codepoint == vaxis.Key.end) {
        ed.cursor_col = ed.lines.items[ed.cursor_row].items.len;
    } else if (key.codepoint == vaxis.Key.page_up) {
        // Page Up: Move up by viewport height
        if (ed.cursor_row > viewport_height) {
            ed.cursor_row -= viewport_height;
        } else {
            ed.cursor_row = 0;
        }
        ed.cursor_col = @min(ed.cursor_col, ed.lines.items[ed.cursor_row].items.len);
        ed.adjustScroll(viewport_height);
    } else if (key.codepoint == vaxis.Key.page_down) {
        // Page Down: Move down by viewport height
        if (ed.cursor_row + viewport_height < ed.lines.items.len) {
            ed.cursor_row += viewport_height;
        } else {
            ed.cursor_row = ed.lines.items.len - 1;
        }
        ed.cursor_col = @min(ed.cursor_col, ed.lines.items[ed.cursor_row].items.len);
        ed.adjustScroll(viewport_height);
    }
    // Text editing (only in insert mode)
    else if (ed.mode == .insert) {
        if (key.codepoint == '\r' or key.codepoint == '\n') {
            try ed.insertNewline();
        } else if (key.codepoint == 127 or key.codepoint == 8) { // Backspace
            if (ed.cursor_col > 0) {
                ed.cursor_col -= 1;
                _ = ed.lines.items[ed.cursor_row].orderedRemove(ed.cursor_col);
            } else if (ed.cursor_row > 0) {
                // Join with previous line
                var current_line = ed.lines.orderedRemove(ed.cursor_row);
                ed.cursor_row -= 1;
                ed.cursor_col = ed.lines.items[ed.cursor_row].items.len;
                try ed.lines.items[ed.cursor_row].appendSlice(ed.allocator, current_line.items);
                current_line.deinit(ed.allocator);
            }
        } else if (key.text) |text| {
            // Use the text field which properly handles Shift and other modifiers
            if (text.len > 0) {
                try ed.insertText(text);
            }
        } else if (key.codepoint >= 32 and key.codepoint <= 126) { // Fallback to codepoint
            try ed.insertChar(@intCast(key.codepoint));
        }
    }
}
