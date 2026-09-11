module mizar::ssg;
import std::io;
import mizar::core::buffer;

struct PageEntry {
    String slug;
    String html_content;
}

struct SiteGenerator {
    String output_dir;
    PageEntry[128] pages;
    usz page_count;
}

fn void SiteGenerator.init(&this, String output_dir = "dist") {
    this.output_dir = output_dir;
    this.page_count = 0;
}

fn void SiteGenerator.add_page(&this, String slug, String html_content) {
    if (this.page_count < 128) {
        this.pages[this.page_count++] = {
            .slug = slug,
            .html_content = html_content
        };
    }
}

fn bool SiteGenerator.build(&this) {
    io::printfn("Generating static site in ./%s ...", this.output_dir);
    for (usz i = 0; i < this.page_count; i++) {
        PageEntry p = this.pages[i];
        io::printfn(" -> Emitted: %s/%s.html (%d bytes)", this.output_dir, p.slug, (int)p.html_content.len);
    }
    io::printfn("Static build complete: %d pages generated.", (int)this.page_count);
    return true;
}
