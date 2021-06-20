#ifndef _DEBUG_H
#define _DEBUG_H

enum { led_white = 1, led_blue = 2, led_yellow = 4, led_green = 8, led_red = 16 };

void chenillard();
void led_set(unsigned set_mask, unsigned clear_mask);

#endif // _DEBUG_H_
