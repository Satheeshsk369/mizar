const std = @import("std");
const vaxis = @import("vaxis");
const TextInput = vaxis.widgets.TextInput;

const Event = union(enum) {
    key_press: vaxis.Key,
    winsize: vaxis.Winsize,
};

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer std.debug.assert(gpa.deinit() == .ok);
    const allocator = gpa.allocator();

    var buffer: [1024]u8 = undefined;
    var tty = try vaxis.Tty.init(&buffer);
    defer tty.deinit();

    var vx = try vaxis.init(allocator, .{});
    defer vx.deinit(allocator, tty.writer());

    var loop: vaxis.Loop(Event) = .{ .tty = &tty, .vaxis = &vx };
    try loop.init();
    try loop.start();
    defer loop.stop();

    try vx.enterAltScreen(tty.writer());

    var text_input = TextInput.init(allocator, &vx.unicode);
    defer text_input.deinit();

    try vx.queryTerminal(tty.writer(), 1 * std.time.ns_per_s);
    while (true) {
        const event = loop.nextEvent();
        switch (event) {
            .key_press => |key| {
                if (key.matches('c', .{ .ctrl = true })) break;
                if (key.matches('l', .{ .ctrl = true })) vx.queueRefresh();
                if (key.matches(vaxis.Key.enter, .{ .shift = true })) {
                    try text_input.insertSliceAtCursor("\n");
                    vx.queueRefresh();
                } else {
                    try text_input.update(.{ .key_press = key });
                }
            },
            .winsize => |ws| try vx.resize(allocator, tty.writer(), ws),
        }

        const win = vx.window();
        win.clear();

        const child = win.child(.{
            .x_off = 2,
            .y_off = 2,
            .width = win.width - 4,
            .height = win.height - 4,
        });
        text_input.draw(child);

        try vx.render(tty.writer());
    }
}
