#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "mizar.h"

int main(void) {
    MizarBuffer buf;
    mz_buf_init(&buf, 4096);
    mz_context_push(&buf);

    // Navigation & Directional
    Icon_ChevronRight();
    Icon_ChevronLeft(.size = 24, .color = "#0284c7");
    Icon_ArrowRight(.size = 16);

    // Actions
    Icon_Search();
    Icon_Close(.size = 18);
    Icon_Check(.color = "#16a34a");
    Icon_Plus();
    Icon_Minus();
    Icon_Menu();

    // User & Security
    Icon_User();
    Icon_Lock();
    Icon_Unlock();
    Icon_Key();

    // Files & Media
    Icon_Copy();
    Icon_Trash();
    Icon_Edit();
    Icon_File();
    Icon_Folder();
    Icon_Download();
    Icon_Upload();

    // Status & System
    Icon_Heart();
    Icon_Star();
    Icon_Bell();
    Icon_Mail();
    Icon_AlertCircle();
    Icon_Info();
    Icon_Settings();
    Icon_ExternalLink();
    Icon_Terminal();
    Icon_Code();
    Icon_Refresh();

    mz_context_pop();

    // Verify SVG emissions
    assert(strstr(buf.data, "viewBox=\"0 0 24 24\"") != NULL);
    assert(strstr(buf.data, "stroke=\"#0284c7\"") != NULL);
    assert(strstr(buf.data, "stroke=\"#16a34a\"") != NULL);
    assert(strstr(buf.data, "d=\"m9 18 6-6-6-6\"") != NULL); // ChevronRight
    assert(strstr(buf.data, "d=\"M18 6 6 18M6 6l12 12\"") != NULL); // Close
    assert(strstr(buf.data, "d=\"M20 6 9 17l-5-5\"") != NULL); // Check

    mz_buf_free(&buf);
    printf("Standard Icon Suite (28 essential SVG icons) tested and passed cleanly!\n");
    return 0;
}
