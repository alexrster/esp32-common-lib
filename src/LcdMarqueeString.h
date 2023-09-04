#ifndef __LCD_MARQUEE_STRING_H
#define __LCD_MARQUEE_STRING_H

#include <Arduino.h>
#include <Print.h>

class LcdMarqueeString 
{
  public:
    LcdMarqueeString(uint8_t display_len, uint16_t speed_ms = 330) : display_len(display_len), speed_ms(speed_ms)
    {
      sprintf(str_format_right, "%%%us", display_len);
      sprintf(str_format_left, "%%-%us", display_len);

      textRenderBuffer = (char *)malloc(sizeof(char) * display_len + 2);
    }

    void setText(String text)
    {
      if (this->new_text.equals(text)) return;

      text_changed = true;
      if (text.length() > 0)
      {
        this->new_text = text.c_str();
        if (this->text.length() <= 0) {
          setInitialOffset();
        }
      }
      else
      {
        this->new_text = emptyString;
        current_offset = 0;
      }
    }

    void draw(Print* out)
    {
      draw(out, false);
    }

    void draw(Print* out, bool force)
    {
      if (!force && text.length() <= 0) return;

      now = millis();
      if (force || now - last_draw >= speed_ms) {
        last_draw = now;
        text_changed = false;

        if (text.length() == 0) {
          writeTextRenderBuffer(out, snprintf(textRenderBuffer, display_len, str_format_left, " "));
          return;
        }

        if (text.length() <= display_len + 1) {
          writeTextRenderBuffer(out, snprintf(textRenderBuffer, display_len + 1, str_format_left, text.c_str()));
          return;
        }

        if (--current_offset < 0)
        {
          setInitialOffset();
          return;
        }

        int left = current_offset >= text.length() ? 0 : text.length() - current_offset;
        int len = current_offset >= text.length() ? display_len - (current_offset - text.length()) : display_len;

        String buf = text.substring(left, left + len);
        if (buf.length() <= display_len + 1)
        {
          if (current_offset >= text.length()) {    // right-side
            writeTextRenderBuffer(out, sprintf(textRenderBuffer, str_format_right, buf.c_str()));
          }
          else {                                    // left-side
            writeTextRenderBuffer(out, snprintf(textRenderBuffer, display_len + 1, str_format_left, buf.c_str()));
          }
        }
        else
        {
          out->write(buf.c_str(), display_len);
        }
      }
    }
  
  private:
    uint8_t display_len = 0;
    uint16_t speed_ms;
    int current_offset = 0;
    long now, last_draw = 0;
    bool text_changed = false;
    String text = emptyString;
    String new_text = emptyString;
    char *textRenderBuffer;
    char str_format_left[6] = {0,0,0,0,0,0};
    char str_format_right[6] = {0,0,0,0,0,0};

    inline size_t writeTextRenderBuffer(Print* out, int n)
    {
      n = n < 0 ? 0 : n > display_len ? display_len : n;
      if (n > 0) {
#ifdef DEBUG
        auto writtenBytes = out->write(textRenderBuffer, n);
        if (writtenBytes != n) {
          log_w("Potential DATA LOSS during writing to Printable sink! sent=%u, written=%u", n, writtenBytes);
        }

        return writtenBytes;
#else
        return out->write(textRenderBuffer, n);
#endif
      }

      return 0;
    }

    void setInitialOffset()
    {
      if (!text.equals(new_text)) text = new_text.c_str();

      if (text.length() > display_len) {
        current_offset = text.length() + display_len;
      }
      else {
        current_offset = 0;
      }

      text_changed = true;
    }
};

#endif