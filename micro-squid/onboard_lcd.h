#ifndef LCD_H_
#define LCD_H_

void onboard_lcd_send_cmd(unsigned char data);

void onboard_lcd_init(void);

void onboard_lcd_set_cursor(unsigned int pos);

void onboard_lcd_set_str(char *str);

void onboard_lcd_clear(void);

#endif /* LCD_H_ */
