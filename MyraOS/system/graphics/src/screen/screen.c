#include "screen/screen.h"

#include "desktop_icons/desktop_icons.h"
#include "font/fonts/myra_font.h"
#include "os_widgets/icon_widget.h"
#include "os_widgets/top_bar_widget.h"
#include "os_widgets/window_widget.h"
#include "ui/widgets/image_widget.h"
#include "ui/widgets/background_widget.h"
#include "ui/widgets/label_widget.h"
#include "ui/ui.h"

screen_t screen_desktop = {
    .init = screen_desktop_init,
    .handle_input = NULL,
};

void screen_desktop_init(void) {
    ui_add_widget(widget_os_top_bar_create(0, 0, 40, 0xFF111111));

    ui_add_widget(widget_background_create(0xFF222222));
    image_t* boot_image = image_parse("/myra/boot/boot_image.bmp");
    if (boot_image != NULL) {
        ui_add_widget(widget_image_create((fb_info.width / 2) - (boot_image->width / 2), (fb_info.height / 2) - (boot_image->height / 2), LAYER_BACKGROUND, boot_image));
    }

    int count = 0;
    desktop_icon_t* icons = icons_ini_load("/desktop/icons.ini", &count);
    if (icons != NULL) {
        for (int i = 0; i < count; i++) {
            icon_desc_t desc = {
                .title = icons[i].title,
                .icon_path = icons[i].icon_path,
                .exec_path = icons[i].exec_path
            };

            widget_t* w = widget_os_icon_create(icons[i].x, icons[i].y, &desc);
            ui_add_widget(w);
        }
    }
}