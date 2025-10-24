#ifndef WALLPAPER_ROTATOR_H
#define WALLPAPER_ROTATOR_H 1

/* daemon.c */
void daemonize(void);

/* wallpaper-rotator.c */
void rotate(char *picture_path, int rotation_speed);

#endif
