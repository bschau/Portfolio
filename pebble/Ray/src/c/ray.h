#ifndef RAY_H
#define RAY_H

#include <pebble.h>

/* beam.c */
void beam_init(Window *window);
void beam_deinit(void);
void beam_reset_animation(uint8_t *beam1, uint8_t *beam2, uint8_t *beam3);
void beam_clear(void);
void beam_shift(void);

/* heartbeat.c */
void heartbeat_init(Window *window);
void heartbeat_deinit(void);
void heartbeat_reset_animation(void);

#ifdef MAIN
	uint8_t beam_red[9] = { GColorBulgarianRoseARGB8, GColorDarkCandyAppleRedARGB8, GColorRedARGB8, GColorSunsetOrangeARGB8, GColorMelonARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8 };

	uint8_t beam_green[9] = { GColorDarkGreenARGB8, GColorIslamicGreenARGB8, GColorGreenARGB8, GColorBrightGreenARGB8, GColorInchwormARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8 };

	uint8_t beam_blue[9] = { GColorOxfordBlueARGB8, GColorDukeBlueARGB8, GColorBlueARGB8, GColorBlueMoonARGB8, GColorPictonBlueARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8 };

	uint8_t beam_yellow[9] = { GColorWindsorTanARGB8, GColorChromeYellowARGB8, GColorYellowARGB8, GColorIcterineARGB8, GColorPastelYellowARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8 };

	uint8_t beam_pink[9] = { GColorBulgarianRoseARGB8, GColorJazzberryJamARGB8, GColorMagentaARGB8, GColorShockingPinkARGB8, GColorRichBrilliantLavenderARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8 };

	uint8_t beam_turquoise[9] = { GColorMidnightGreenARGB8, GColorTiffanyBlueARGB8, GColorCyanARGB8, GColorElectricBlueARGB8, GColorCelesteARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8 };

	uint8_t beam_brown[9] = { GColorBulgarianRoseARGB8, GColorDarkCandyAppleRedARGB8, GColorWindsorTanARGB8, GColorRajahARGB8, GColorPastelYellowARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8 };

	uint8_t beam_purple[9] = { GColorImperialPurpleARGB8, GColorPurpleARGB8, GColorPurpureusARGB8, GColorLavenderIndigoARGB8, GColorRichBrilliantLavenderARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8 };

	uint8_t beam_gray[9] = { GColorDarkGrayARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorWhiteARGB8, GColorWhiteARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8 };


	uint8_t *beam_bands[9] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
#else
	extern uint8_t beam_red[9], beam_green[9], beam_blue[9], beam_yellow[9];
	extern uint8_t beam_pink[9], beam_turquoise[9], beam_brown[9];
	extern uint8_t beam_purple[9], beam_gray[9];
	extern uint8_t *beam_bands[9];
#endif

#endif
