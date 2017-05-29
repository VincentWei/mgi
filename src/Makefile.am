INCLUDES=-I../include -I./ -I./softkeyboard -I./softkeyboard/softkeyboard -I./softkeyboard/animate

lib_LTLIBRARIES=libmgi.la

libmgi_la_LDFLAGS=-release $(LT_RELEASE) -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

PY_SOURCES = hzinput.c hzinput.h pinyin.h misc.h

SOFTKEYBOARD_SOURCES = \
	softkeyboard/softkeywindow.c \
	softkeyboard/tooltip.h \
	softkeyboard/tooltip.c \
	softkeyboard/animate/animate.h \
	softkeyboard/animate/animate_sence.c \
	softkeyboard/animate/instruction.h \
	softkeyboard/animate/my_debug.h \
	softkeyboard/animate/p-code.c \
	softkeyboard/animate/p-code.h \
	softkeyboard/animate/timeline.c \
	softkeyboard/animate/common_animates/alpha_animate.c \
	softkeyboard/animate/common_animates/common_animates.c \
	softkeyboard/animate/common_animates/common_animates.h \
	softkeyboard/softkeyboard/common.c \
	softkeyboard/softkeyboard/common.h \
	softkeyboard/softkeyboard/en_kbd.c \
	softkeyboard/softkeyboard/ime.c \
	softkeyboard/softkeyboard/ime.h \
	softkeyboard/softkeyboard/num_kbd.c \
	softkeyboard/softkeyboard/pinyin_kbd.c \
	softkeyboard/softkeyboard/punct_kbd.c \
	softkeyboard/softkeyboard/resource.c \
	softkeyboard/softkeyboard/softkeyboard.h \
	$(NULL)

libmgi_la_SOURCES= \
	contbmp.c         jt_engine.c        mgpti_dict.c      \
	jt_input.h        mgpti_dict.h       mgpti_user_dict.c \
	hw_engine.c       keyboard.c         mgpti_dict_ops.c  mgpti_user_dict.h \
	hwinput.c         mgime.c            mgpti_dict_ops.h  \
	ime_pinyin.c      mgphone.c          wt_engine.c \
	mgpti_helper.c    wt_input.h \
	ime_tab_pinyin.h  mgpti.c            mgpti_helper.h \
	mgime_tab.h       hw.h				 mgi_helper.h \
	$(SOFTKEYBOARD_SOURCES) $(PY_SOURCES)

#del	240-320/bitmapfont.c \
RESOURCE_240_320 = \
	240-320/char_key_mask.c \
	240-320/en_kbd_bkg.c \
	240-320/func_key_mask.c \
	240-320/left_arrow_disable.c \
	240-320/left_arrow_enable.c \
	240-320/num_kbd_bkg.c \
	240-320/pinyin_kbd_bkg.c \
	240-320/punct_kbd_bkg.c \
	240-320/right_arrow_disable.c \
	240-320/right_arrow_enable.c \
	240-320/size_240x320.h \
	240-320/tooltip_bk.c \
	240-320/tooltip_mask.c \
	$(NULL)

#del	320-240/bitmapfont.c \
RESOURCE_320_240 = \
	320-240/char_key_mask.c \
	320-240/en_kbd_bkg.c \
	320-240/func_key_mask.c \
	320-240/left_arrow_disable.c \
	320-240/left_arrow_enable.c \
	320-240/num_kbd_bkg.c \
	320-240/pinyin_kbd_bkg.c \
	320-240/punct_kbd_bkg.c \
	320-240/right_arrow_disable.c \
	320-240/right_arrow_enable.c \
	320-240/size_240x320.h \
	320-240/tooltip_bk.c \
	320-240/tooltip_mask.c \
	$(NULL)

#del	480-272/bitmapfont.c \
RESOURCE_480_272 = \
	480-272/char_key_mask.c \
	480-272/en_kbd_bkg.c \
	480-272/func_key_mask.c \
	480-272/left_arrow_disable.c \
	480-272/left_arrow_enable.c \
	480-272/num_kbd_bkg.c \
	480-272/pinyin_kbd_bkg.c \
	480-272/punct_kbd_bkg.c \
	480-272/right_arrow_disable.c \
	480-272/right_arrow_enable.c \
	480-272/size_240x320.h \
	480-272/tooltip_bk.c \
	480-272/tooltip_mask.c \
	$(NULL)

SUBDIRS= softkeyboard

EXTRA_DIST = mgphone_nav_bmp.c default_kb_bmp.c skb_bmp.c \
	$(RESOURCE_240_320) \
	$(RESOURCE_320_240) \
	$(RESOURCE_480_272) \
	$(NULL)

