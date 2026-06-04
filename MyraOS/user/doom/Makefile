TARGET  = doom.elf
CC      = i386-elf-gcc

CRT        = system/libc/src/libc_user/crt0.o
LIBC_USER  = system/libc/src/libc_user/libc_user.a
INC        = -I system/libc/include/libc_user -I system/include -I userspace/doom
LDS        = user/user.ld

SRC_DIR = user/doom/doomgeneric
OBJ_DIR = build/user/doom/doomgeneric
SRC     = dummy.c am_map.c doomdef.c doomstat.c dstrings.c d_event.c d_items.c d_iwad.c d_loop.c d_main.c d_mode.c d_net.c f_finale.c f_wipe.c g_game.c hu_lib.c hu_stuff.c info.c i_cdmus.c i_endoom.c i_joystick.c i_scale.c i_sound.c i_system.c i_timer.c memio.c m_argv.c m_bbox.c m_cheat.c m_config.c m_controls.c m_fixed.c m_menu.c m_misc.c m_random.c p_ceilng.c p_doors.c p_enemy.c p_floor.c p_inter.c p_lights.c p_map.c p_maputl.c p_mobj.c p_plats.c p_pspr.c p_saveg.c p_setup.c p_sight.c p_spec.c p_switch.c p_telept.c p_tick.c p_user.c r_bsp.c r_data.c r_draw.c r_main.c r_plane.c r_segs.c r_sky.c r_things.c sha1.c sounds.c statdump.c st_lib.c st_stuff.c s_sound.c tables.c v_video.c wi_stuff.c w_checksum.c w_file.c w_main.c w_wad.c z_zone.c w_file_stdc.c i_input.c i_video.c doomgeneric.c doomgeneric_myra.c main.c
SRCS    = $(addprefix $(SRC_DIR)/,$(SRC))
OBJS    = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

CFLAGS  = -ffreestanding -m32 -nostdlib -Wall -Wextra -fno-omit-frame-pointer -fno-stack-protector $(INC)
CFLAGS += -O0 -g -fno-omit-frame-pointer -mno-sse -mno-sse2 -fno-tree-vectorize -mstackrealign -fno-builtin -fno-strict-aliasing
LDFLAGS = -T $(LDS) -nostdlib -z max-page-size=0x1000

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) -ffreestanding -m32 -nostdlib $(INC) -T $(LDS) -o $@ \
		$(CRT) $(OBJS) \
		-Wl,--whole-archive $(LIBC_USER) -Wl,--no-whole-archive \
		-Wl,--start-group -lc -lm -lgcc -Wl,--end-group -z max-page-size=0x1000

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
