#ifndef WIDGET_IMAGE_H
#define WIDGET_IMAGE_H

#include "gfx/gfx.h"
#include "image/image_loader.h"
#include "ui/ui.h"

widget_t* widget_image_create(uint32_t x, uint32_t y, layer_id_t layer, const image_t* image);

#endif // WIDGET_IMAGE_H
