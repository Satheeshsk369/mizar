const std = @import("std");

pub const Block = struct {
    start_line: usize,
    end_line: usize,
    level: usize, // Depth in hierarchy

    pub fn contains(self: Block, line: usize) bool {
        return line >= self.start_line and line <= self.end_line;
    }
};

pub const Navigator = struct {
    blocks: std.ArrayList(Block),
    allocator: std.mem.Allocator,
    current_level: usize, // Which hierarchy level we're navigating at

    pub fn init(allocator: std.mem.Allocator) Navigator {
        return Navigator{
            .blocks = std.ArrayList(Block){},
            .allocator = allocator,
            .current_level = 0,
        };
    }

    pub fn deinit(self: *Navigator) void {
        self.blocks.deinit(self.allocator);
    }

    /// Detect blocks from lines using blank line detection
    pub fn detectBlocks(self: *Navigator, lines: []const std.ArrayList(u8)) !void {
        self.blocks.clearRetainingCapacity();

        if (lines.len == 0) return;

        var block_start: usize = 0;
        var in_block = false;

        for (lines, 0..) |line, i| {
            const is_blank = isBlankLine(line.items);

            if (!in_block and !is_blank) {
                block_start = i;
                in_block = true;
            } else if (in_block and is_blank) {
                if (i > block_start) {
                    try self.blocks.append(self.allocator, Block{
                        .start_line = block_start,
                        .end_line = i - 1,
                        .level = 0,
                    });
                }
                in_block = false;
            }
        }

        // Last block if file doesn't end with blank line
        if (in_block) {
            try self.blocks.append(self.allocator, Block{
                .start_line = block_start,
                .end_line = lines.len - 1,
                .level = 0,
            });
        }
    }

    /// Navigate up: try block navigation first, fall back to line
    pub fn navigateUp(self: *Navigator, current_line: usize) ?usize {
        // Try to find previous block at current level
        for (self.blocks.items, 0..) |block, i| {
            if (block.level != self.current_level) continue;

            if (block.end_line < current_line) {
                // This block is before current position
                // Check if there's no block after this but before current
                var is_prev = true;
                for (self.blocks.items) |other| {
                    if (other.level != self.current_level) continue;
                    if (other.end_line > block.end_line and other.start_line < current_line) {
                        is_prev = false;
                        break;
                    }
                }
                if (is_prev and i > 0) {
                    return block.start_line;
                }
            }
        }

        // No block found, fall back to line navigation
        if (current_line > 0) return current_line - 1;
        return null;
    }

    /// Navigate down: try block navigation first, fall back to line
    pub fn navigateDown(self: *Navigator, current_line: usize, total_lines: usize) ?usize {
        // Try to find next block at current level
        var closest: ?Block = null;

        for (self.blocks.items) |block| {
            if (block.level != self.current_level) continue;

            if (block.start_line > current_line) {
                if (closest == null or block.start_line < closest.?.start_line) {
                    closest = block;
                }
            }
        }

        if (closest) |block| {
            return block.start_line;
        }

        // No block found, fall back to line navigation
        if (current_line + 1 < total_lines) return current_line + 1;
        return null;
    }

    /// Navigate right: go into child level (increase depth)
    pub fn navigateRight(self: *Navigator, current_line: usize, lines: []const std.ArrayList(u8)) !?usize {
        // Find current block
        var current_block: ?Block = null;
        for (self.blocks.items) |block| {
            if (block.level == self.current_level and block.contains(current_line)) {
                current_block = block;
                break;
            }
        }

        if (current_block) |block| {
            // Try to detect sub-blocks within this block using indentation or brackets
            try self.detectSubBlocks(lines, block);
            self.current_level += 1;

            // Find first child block
            for (self.blocks.items) |child| {
                if (child.level == self.current_level and
                    child.start_line >= block.start_line and
                    child.end_line <= block.end_line) {
                    return child.start_line;
                }
            }

            // No sub-blocks, stay at current line
            self.current_level -= 1;
        }

        return null;
    }

    /// Navigate left: go to parent level (decrease depth)
    pub fn navigateLeft(self: *Navigator, current_line: usize) ?usize {
        if (self.current_level > 0) {
            self.current_level -= 1;

            // Find parent block at new level
            for (self.blocks.items) |block| {
                if (block.level == self.current_level and block.contains(current_line)) {
                    return block.start_line;
                }
            }
        }
        return null;
    }

    /// Detect sub-blocks within a parent block using indentation
    fn detectSubBlocks(self: *Navigator, lines: []const std.ArrayList(u8), parent: Block) !void {
        const new_level = parent.level + 1;

        // Remove old blocks at this level
        var i: usize = 0;
        while (i < self.blocks.items.len) {
            if (self.blocks.items[i].level >= new_level) {
                _ = self.blocks.orderedRemove(i);
            } else {
                i += 1;
            }
        }

        // Detect blocks by indentation changes
        var block_start: ?usize = null;
        var base_indent: ?usize = null;

        for (parent.start_line..parent.end_line + 1) |line_idx| {
            if (line_idx >= lines.len) break;

            const line = lines[line_idx].items;
            if (isBlankLine(line)) continue;

            const indent = getIndentation(line);

            if (base_indent == null) {
                base_indent = indent;
                continue;
            }

            if (indent > base_indent.?) {
                if (block_start == null) {
                    block_start = line_idx;
                }
            } else if (block_start != null) {
                // End of indented block
                try self.blocks.append(self.allocator, Block{
                    .start_line = block_start.?,
                    .end_line = line_idx - 1,
                    .level = new_level,
                });
                block_start = null;
            }
        }

        // Handle last block
        if (block_start) |start| {
            try self.blocks.append(self.allocator, Block{
                .start_line = start,
                .end_line = parent.end_line,
                .level = new_level,
            });
        }
    }

    fn isBlankLine(line: []const u8) bool {
        for (line) |char| {
            if (!std.ascii.isWhitespace(char)) return false;
        }
        return true;
    }

    fn getIndentation(line: []const u8) usize {
        var count: usize = 0;
        for (line) |char| {
            if (char == ' ') {
                count += 1;
            } else if (char == '\t') {
                count += 4; // Tab = 4 spaces
            } else {
                break;
            }
        }
        return count;
    }
};
