#include "city_with_overlay.h"

#include "building/animation.h"
#include "building/industry.h"
#include "city/view.h"
#include "core/debug.h"
#include "game/resource.h"
#include "game/state.h"
#include "graphics/image.h"
#include "map/bridge.h"
#include "map/building.h"
#include "map/figure.h"
#include "map/image.h"
#include "map/property.h"
#include "map/random.h"
#include "map/terrain.h"
#include "widget/city_bridge.h"
#include "widget/city_building_ghost.h"
#include "widget/city_figure.h"
#include "widget/city_overlay.h"
#include "widget/city_overlay_education.h"
#include "widget/city_overlay_entertainment.h"
#include "widget/city_overlay_health.h"
#include "widget/city_overlay_other.h"
#include "widget/city_overlay_risks.h"
#include "widget/city_without_overlay.h"

#include "Data/State.h"

static const city_overlay *overlay = 0;

static const city_overlay *get_city_overlay()
{
    switch (game_state_overlay()) {
        case OVERLAY_FIRE:
            return city_overlay_for_fire();
        case OVERLAY_CRIME:
            return city_overlay_for_crime();
        case OVERLAY_DAMAGE:
            return city_overlay_for_damage();
        case OVERLAY_PROBLEMS:
            return city_overlay_for_problems();
        case OVERLAY_NATIVE:
            return city_overlay_for_native();
        case OVERLAY_ENTERTAINMENT:
            return city_overlay_for_entertainment();
        case OVERLAY_THEATER:
            return city_overlay_for_theater();
        case OVERLAY_AMPHITHEATER:
            return city_overlay_for_amphitheater();
        case OVERLAY_COLOSSEUM:
            return city_overlay_for_colosseum();
        case OVERLAY_HIPPODROME:
            return city_overlay_for_hippodrome();
        case OVERLAY_EDUCATION:
            return city_overlay_for_education();
        case OVERLAY_SCHOOL:
            return city_overlay_for_school();
        case OVERLAY_LIBRARY:
            return city_overlay_for_library();
        case OVERLAY_ACADEMY:
            return city_overlay_for_academy();
        case OVERLAY_BARBER:
            return city_overlay_for_barber();
        case OVERLAY_BATHHOUSE:
            return city_overlay_for_bathhouse();
        case OVERLAY_CLINIC:
            return city_overlay_for_clinic();
        case OVERLAY_HOSPITAL:
            return city_overlay_for_hospital();
        case OVERLAY_RELIGION:
            return city_overlay_for_religion();
        case OVERLAY_TAX_INCOME:
            return city_overlay_for_tax_income();
        case OVERLAY_FOOD_STOCKS:
            return city_overlay_for_food_stocks();
        case OVERLAY_WATER:
            return city_overlay_for_water();
        case OVERLAY_DESIRABILITY:
            return city_overlay_for_desirability();
        default:
            debug_log("Unknown city overlay!", 0, game_state_overlay());
            return 0;
    }
}

static int select_city_overlay()
{
    if (!overlay || overlay->type != game_state_overlay()) {
        overlay = get_city_overlay();
    }
    return overlay != 0;
}

static int is_drawable_farmhouse(int grid_offset, int map_orientation)
{
    if (!map_property_is_draw_tile(grid_offset)) {
        return 0;
    }
    int xy = map_property_multi_tile_xy(grid_offset);
    if (map_orientation == DIR_0_TOP && xy == Edge_X0Y1) {
        return 1;
    }
    if (map_orientation == DIR_2_RIGHT && xy == Edge_X0Y0) {
        return 1;
    }
    if (map_orientation == DIR_4_BOTTOM && xy == Edge_X1Y0) {
        return 1;
    }
    if (map_orientation == DIR_2_RIGHT && xy == Edge_X1Y1) {
        return 1;
    }
    return 0;
}

static int is_drawable_farm_corner(int grid_offset)
{
    if (!map_property_is_draw_tile(grid_offset)) {
        return 0;
    }

    int map_orientation = city_view_orientation();
    int xy = map_property_multi_tile_xy(grid_offset);
    if (map_orientation == DIR_0_TOP && xy == Edge_X0Y2) {
        return 1;
    }
    if (map_orientation == DIR_2_RIGHT && xy == Edge_X0Y0) {
        return 1;
    }
    if (map_orientation == DIR_4_BOTTOM && xy == Edge_X2Y0) {
        return 1;
    }
    if (map_orientation == DIR_2_RIGHT && xy == Edge_X2Y2) {
        return 1;
    }
    return 0;
}

static void draw_flattened_building_footprint(const building *b, int x, int y, int image_offset)
{
    int image_base = image_group(GROUP_TERRAIN_OVERLAY) + image_offset;
    if (b->houseSize) {
        image_base += 4;
    }
    if (b->size == 1) {
        image_draw_isometric_footprint_from_draw_tile(image_base, x, y, 0);
    } else if (b->size == 2) {
        int xTileOffset[] = {30, 0, 60, 30};
        int yTileOffset[] = {-15, 0, 0, 15};
        for (int i = 0; i < 4; i++) {
            image_draw_isometric_footprint_from_draw_tile(image_base + i, x + xTileOffset[i], y + yTileOffset[i], 0);
        }
    } else if (b->size == 3) {
        int graphicTileOffset[] = {0, 1, 2, 1, 3, 2, 3, 3, 3};
        int xTileOffset[] = {60, 30, 90, 0, 60, 120, 30, 90, 60};
        int yTileOffset[] = {-30, -15, -15, 0, 0, 0, 15, 15, 30};
        for (int i = 0; i < 9; i++) {
            image_draw_isometric_footprint_from_draw_tile(image_base + graphicTileOffset[i], x + xTileOffset[i], y + yTileOffset[i], 0);
        }
    } else if (b->size == 4) {
        int graphicTileOffset[] = {0, 1, 2, 1, 3, 2, 1, 3, 3, 2, 3, 3, 3, 3, 3, 3};
        int xTileOffset[] = {
            90,
            60, 120,
            30, 90, 150,
            0, 60, 120, 180,
            30, 90, 150,
            60, 120,
            90
        };
        int yTileOffset[] = {
            -45,
            -30, -30,
            -15, -15, -15,
            0, 0, 0, 0,
            15, 15, 15,
            30, 30,
            45
        };
        for (int i = 0; i < 16; i++) {
            image_draw_isometric_footprint_from_draw_tile(image_base + graphicTileOffset[i], x + xTileOffset[i], y + yTileOffset[i], 0);
        }
    } else if (b->size == 5) {
        int graphicTileOffset[] = {0, 1, 2, 1, 3, 2, 1, 3, 3, 2, 1, 3, 3, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
        int xTileOffset[] = {
            120,
            90, 150,
            60, 120, 180,
            30, 90, 150, 210,
            0, 60, 120, 180, 240,
            30, 90, 150, 210,
            60, 120, 180,
            90, 150,
            120
        };
        int yTileOffset[] = {
            -60,
            -45, -45,
            -30, -30, -30,
            -15, -15, -15, -15,
            0, 0, 0, 0, 0,
            15, 15, 15, 15,
            30, 30, 30,
            45, 45,
            60
        };
        for (int i = 0; i < 25; i++) {
            image_draw_isometric_footprint_from_draw_tile(image_base + graphicTileOffset[i], x + xTileOffset[i], y + yTileOffset[i], 0);
        }
    }
}

void city_with_overlay_draw_building_footprint(int x, int y, int grid_offset, int image_offset)
{
    int building_id = map_building_at(grid_offset);
    if (!building_id) {
        return;
    }
    building *b = building_get(building_id);
    if (overlay->show_building(b)) {
        if (building_is_farm(b->type)) {
            if (is_drawable_farmhouse(grid_offset, city_view_orientation())) {
                image_draw_isometric_footprint_from_draw_tile(map_image_at(grid_offset), x, y, 0);
            } else if (map_property_is_draw_tile(grid_offset)) {
                image_draw_isometric_footprint_from_draw_tile(map_image_at(grid_offset), x, y, 0);
            }
        } else {
            image_draw_isometric_footprint_from_draw_tile(map_image_at(grid_offset), x, y, 0);
        }
    } else {
        int draw = 1;
        if (b->size == 3 && building_is_farm(b->type)) {
            draw = is_drawable_farm_corner(grid_offset);
        }
        if (draw) {
            draw_flattened_building_footprint(b, x, y, image_offset);
        }
    }
}

static void draw_footprint(int x, int y, int grid_offset)
{
    if (grid_offset == Data_State.selectedBuilding.gridOffsetStart) {
        Data_State.selectedBuilding.reservoirOffsetX = x;
        Data_State.selectedBuilding.reservoirOffsetY = y;
    }
    if (grid_offset < 0) {
        // Outside map: draw black tile
        image_draw_isometric_footprint_from_draw_tile(image_group(GROUP_TERRAIN_BLACK), x, y, 0);
    } else if (overlay->draw_custom_footprint) {
        overlay->draw_custom_footprint(x, y, grid_offset);
    } else if (map_property_is_draw_tile(grid_offset)) {
        int terrain = map_terrain_get(grid_offset);
        if (terrain & (TERRAIN_AQUEDUCT | TERRAIN_WALL)) {
            // display grass
            int image_id = image_group(GROUP_TERRAIN_GRASS_1) + (map_random_get(grid_offset) & 7);
            image_draw_isometric_footprint_from_draw_tile(image_id, x, y, 0);
        } else if ((terrain & TERRAIN_ROAD) && !(terrain & TERRAIN_BUILDING)) {
            image_draw_isometric_footprint_from_draw_tile(map_image_at(grid_offset), x, y, 0);
        } else if (terrain & TERRAIN_BUILDING) {
            city_with_overlay_draw_building_footprint(x, y, grid_offset, 0);
        } else {
            image_draw_isometric_footprint_from_draw_tile(map_image_at(grid_offset), x, y, 0);
        }
    }
}

static void draw_overlay_column(int x, int y, int height, int is_red)
{
    int image_id = image_group(GROUP_OVERLAY_COLUMN);
    if (is_red) {
        image_id += 9;
    }
    if (height > 10) {
        height = 10;
    }
    int capital_height = image_get(image_id)->height;
    // base
    image_draw(image_id + 2, x + 9, y - 8);
    if (height) {
        // column
        for (int i = 1; i < height; i++) {
            image_draw(image_id + 1, x + 17, y - 8 - 10 * i + 13);
        }
        // capital
        image_draw(image_id, x + 5, y - 8 - capital_height - 10 * (height - 1) + 13);
    }
}

static void draw_building_top(int grid_offset, building *b, int x, int y)
{
    if (building_is_farm(b->type)) {
        if (is_drawable_farmhouse(grid_offset, city_view_orientation())) {
            image_draw_isometric_top_from_draw_tile(map_image_at(grid_offset), x, y, 0);
        } else if (map_property_is_draw_tile(grid_offset)) {
            image_draw_isometric_top_from_draw_tile(map_image_at(grid_offset), x, y, 0);
        }
        return;
    }
    if (b->type == BUILDING_GRANARY) {
        const image *img = image_get(map_image_at(grid_offset));
        image_draw(image_group(GROUP_BUILDING_GRANARY) + 1, x + img->sprite_offset_x, y + img->sprite_offset_y - 30 - (img->height - 90));
        if (b->data.granary.resource_stored[RESOURCE_NONE] < 2400) {
            image_draw(image_group(GROUP_BUILDING_GRANARY) + 2, x + 33, y - 60);
            if (b->data.granary.resource_stored[RESOURCE_NONE] < 1800) {
                image_draw(image_group(GROUP_BUILDING_GRANARY) + 3, x + 56, y - 50);
            }
            if (b->data.granary.resource_stored[RESOURCE_NONE] < 1200) {
                image_draw(image_group(GROUP_BUILDING_GRANARY) + 4, x + 91, y - 50);
            }
            if (b->data.granary.resource_stored[RESOURCE_NONE] < 600) {
                image_draw(image_group(GROUP_BUILDING_GRANARY) + 5, x + 117, y - 62);
            }
        }
    }
    if (b->type == BUILDING_WAREHOUSE) {
        image_draw(image_group(GROUP_BUILDING_WAREHOUSE) + 17, x - 4, y - 42);
    }

    image_draw_isometric_top_from_draw_tile(map_image_at(grid_offset), x, y, 0);
}

void city_with_overlay_draw_building_top(int x, int y, int grid_offset)
{
    building *b = building_get(map_building_at(grid_offset));
    if (overlay->type == OVERLAY_PROBLEMS) {
        overlay_problems_prepare_building(b);
    }
    if (overlay->show_building(b)) {
        draw_building_top(grid_offset, b, x, y);
    } else {
        int column_height = overlay->get_column_height(b);
        if (column_height != NO_COLUMN) {
            int draw = 1;
            if (building_is_farm(b->type)) {
                draw = is_drawable_farm_corner(grid_offset);
            }
            if (draw) {
                draw_overlay_column(x, y, column_height, overlay->column_type == COLUMN_TYPE_RISK);
            }
        }
    }
}

static void draw_top(int x, int y, int grid_offset)
{
    if (overlay->draw_custom_top) {
        overlay->draw_custom_top(x, y, grid_offset);
    } else if (map_property_is_draw_tile(grid_offset)) {
        if (!map_terrain_is(grid_offset, TERRAIN_WALL | TERRAIN_AQUEDUCT | TERRAIN_ROAD)) {
            if (map_terrain_is(grid_offset, TERRAIN_BUILDING) && map_building_at(grid_offset)) {
                city_with_overlay_draw_building_top(x, y, grid_offset);
            } else if (!map_terrain_is(grid_offset, TERRAIN_BUILDING)) {
                // terrain
                image_draw_isometric_top_from_draw_tile(map_image_at(grid_offset), x, y, 0);
            }
        }
    }
}

static void draw_animation(int x, int y, int grid_offset)
{
    int draw = 0;
    if (map_building_at(grid_offset)) {
        int btype = building_get(map_building_at(grid_offset))->type;
        switch (overlay->type) {
            case OVERLAY_FIRE:
            case OVERLAY_CRIME:
                if (btype == BUILDING_PREFECTURE || btype == BUILDING_BURNING_RUIN) {
                    draw = 1;
                }
                break;
            case OVERLAY_DAMAGE:
                if (btype == BUILDING_ENGINEERS_POST) {
                    draw = 1;
                }
                break;
            case OVERLAY_WATER:
                if (btype == BUILDING_RESERVOIR || btype == BUILDING_FOUNTAIN) {
                    draw = 1;
                }
                break;
            case OVERLAY_FOOD_STOCKS:
                if (btype == BUILDING_MARKET || btype == BUILDING_GRANARY) {
                    draw = 1;
                }
                break;
        }
    }

    int image_id = map_image_at(grid_offset);
    const image *img = image_get(image_id);
    if (img->num_animation_sprites && draw) {
        if (map_property_is_draw_tile(grid_offset)) {
            building *b = building_get(map_building_at(grid_offset));
            int color_mask = 0;
            if (b->type == BUILDING_GRANARY) {
                image_draw_masked(image_group(GROUP_BUILDING_GRANARY) + 1,
                                  x + img->sprite_offset_x,
                                  y + 60 + img->sprite_offset_y - img->height,
                                  color_mask);
                if (b->data.granary.resource_stored[RESOURCE_NONE] < 2400) {
                    image_draw_masked(image_group(GROUP_BUILDING_GRANARY) + 2, x + 33, y - 60, color_mask);
                }
                if (b->data.granary.resource_stored[RESOURCE_NONE] < 1800) {
                    image_draw_masked(image_group(GROUP_BUILDING_GRANARY) + 3, x + 56, y - 50, color_mask);
                }
                if (b->data.granary.resource_stored[RESOURCE_NONE] < 1200) {
                    image_draw_masked(image_group(GROUP_BUILDING_GRANARY) + 4, x + 91, y - 50, color_mask);
                }
                if (b->data.granary.resource_stored[RESOURCE_NONE] < 600) {
                    image_draw_masked(image_group(GROUP_BUILDING_GRANARY) + 5, x + 117, y - 62, color_mask);
                }
            } else {
                int animationOffset = building_animation_offset(b, image_id, grid_offset);
                if (animationOffset > 0) {
                    if (animationOffset > img->num_animation_sprites) {
                        animationOffset = img->num_animation_sprites;
                    }
                    int ydiff = 0;
                    switch (map_property_multi_tile_size(grid_offset)) {
                        case 1: ydiff = 30; break;
                        case 2: ydiff = 45; break;
                        case 3: ydiff = 60; break;
                        case 4: ydiff = 75; break;
                        case 5: ydiff = 90; break;
                    }
                    image_draw_masked(image_id + animationOffset,
                                      x + img->sprite_offset_x,
                                      y + ydiff + img->sprite_offset_y - img->height,
                                      color_mask);
                }
            }
        }
    } else if (map_is_bridge(grid_offset)) {
        city_draw_bridge(x, y, grid_offset);
    }
}

static void draw_figures(int x, int y, int grid_offset)
{
    int figure_id = map_figure_at(grid_offset);
    while (figure_id) {
        figure *f = figure_get(figure_id);
        if (!f->isGhost && overlay->show_figure(f)) {
            city_draw_figure(f, x, y);
        }
        figure_id = f->nextFigureIdOnSameTile;
    }
}

static void draw_elevated_figures(int x, int y, int grid_offset)
{
    int figure_id = map_figure_at(grid_offset);
    while (figure_id > 0) {
        figure *f = figure_get(figure_id);
        if (((f->useCrossCountry && !f->isGhost) || f->heightAdjustedTicks) && overlay->show_figure(f)) {
            city_draw_figure(f, x, y);
        }
        figure_id = f->nextFigureIdOnSameTile;
    }
}

void city_with_overlay_draw()
{
    if (!select_city_overlay()) {
        return;
    }

    city_view_foreach_map_tile(draw_footprint);
    city_view_foreach_valid_map_tile(
        draw_figures,
        draw_top,
        draw_animation
    );
    city_building_ghost_draw(&Data_State.map.current);
    city_view_foreach_valid_map_tile(draw_elevated_figures, 0, 0);
}

int city_with_overlay_get_tooltip_text(tooltip_context *c, int grid_offset)
{
    int overlay_type = overlay->type;
    int building_id = map_building_at(grid_offset);
    if (overlay->get_tooltip_for_building && !building_id) {
        return 0;
    }
    int overlay_requires_house =
        overlay_type != OVERLAY_WATER && overlay_type != OVERLAY_FIRE &&
        overlay_type != OVERLAY_DAMAGE && overlay_type != OVERLAY_NATIVE && overlay_type != OVERLAY_DESIRABILITY;
    building *b = building_get(building_id);
    if (overlay_requires_house && !b->houseSize) {
        return 0;
    }
    if (overlay->get_tooltip_for_building) {
        return overlay->get_tooltip_for_building(c, b);
    } else if (overlay->get_tooltip_for_grid_offset) {
        return overlay->get_tooltip_for_grid_offset(c, grid_offset);
    }
    return 0;
}
