//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2001, 2002 Georges Menie. All rights reserved.
// Portions Copyright (c) 2009-2013 Daniel D Miller. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "printf.h"

// ASCII character set... 
#define NUL         0x00
#define STX         0x02
#define ETX         0x03
#define EOT         0x04
#define ACK         0x06
#define BS          0x08
#define LF          0x0A
#define CR          0x0D
#define DLE         0x10
#define ESC         0x1B
#define SPC         0x20
#define MAX_PRN     0x7E
#define DEL         0x7F

//extern int putchar (int c);

static unsigned int use_leading_plus = 0 ;

static int max_output_len = -1 ;
static int curr_output_len = 0 ;

unsigned int fixed_overrun = 0 ;
int last_bad_char = 0 ;
unsigned int invalid_char = 0 ;

static void printchar (char **str, int c)
{
   if (max_output_len >= 0  &&  curr_output_len >= max_output_len) {
      fixed_overrun++ ;
      return ;
   }

   //  check for overrun of our stuff_talkf[] buffer
//    if (curr_output_len >= TALK_LINE_LEN) {
//       fixed_overrun++ ;
//       return ;
//    }
   if (c > MAX_PRN) {
      invalid_char++ ;
      last_bad_char = c ;
      c = '?' ;
   } else
   if (c < SPC) {  //  ASCII 32 = Space
      switch (c) {
      case CR:
      case LF:
      case STX:
      case ETX:
         break;

      default:
         invalid_char++ ;
         last_bad_char = c ;
         c = '?' ;
         break;
      }
   }

	if (str) {
      **str = (char) c;
		++(*str);
      curr_output_len++ ;
	}
}

//****************************************************************************
//  This version returns the length of the output string.
//  It is more useful when implementing a walking-string function.
//****************************************************************************
static const double round_nums[8] = {  
   0.5,
   0.05,
   0.005,
   0.0005,
   0.00005,
   0.000005,
   0.0000005,
   0.00000005
};

static unsigned dbl2stri(char *outbfr, double dbl, unsigned dec_digits)
{
   static char local_bfr[128] ;
   char *output = (outbfr == 0) ? local_bfr : outbfr ;

   //*******************************************
   //  extract negative info
   //*******************************************
   if (dbl < 0.0) {
      *output++ = '-' ;
      dbl *= -1.0 ;
   } else {
      if (use_leading_plus) {
         *output++ = '+' ;
      }
      
   }

   //  handling rounding by adding .5LSB to the floating-point data
   if (dec_digits < 8) {
      dbl += round_nums[dec_digits] ;
   }

   //**************************************************************************
   //  construct fractional multiplier for specified number of digits.
   //**************************************************************************
   unsigned long long mult = 1 ;
   unsigned int idx ;
   for (idx=0; idx < dec_digits; idx++)
      mult *= 10 ;

   // printf("mult=%u\n", mult) ;
   unsigned int wholeNum = (unsigned int) dbl ;
   unsigned long long decimalNum = (unsigned long long) ((dbl - wholeNum) * mult);

   //*******************************************
   //  convert integer portion
   //*******************************************
   char tbfr[40] ;
   idx = 0 ;
   while (wholeNum != 0) {
      tbfr[idx++] = '0' + (wholeNum % 10) ;
      wholeNum /= 10 ;
   }
   // printf("%.3f: whole=%s, dec=%d\n", dbl, tbfr, decimalNum) ;
   if (idx == 0) {
      *output++ = '0' ;
   } else {
      while (idx > 0) {
         *output++ = tbfr[idx-1] ;  //lint !e771
         idx-- ;
      }
   }
   if (dec_digits > 0) {
      *output++ = '.' ;

      //*******************************************
      //  convert fractional portion
      //*******************************************
      idx = 0 ;
      while (decimalNum != 0) {
         tbfr[idx++] = '0' + (decimalNum % 10) ;
         decimalNum /= 10 ;
      }
      //  pad the decimal portion with 0s as necessary;
      //  We wouldn't want to report 3.093 as 3.93, would we??
      while (idx < dec_digits) {
         tbfr[idx++] = '0' ;
      }
      // printf("decimal=%s\n", tbfr) ;
      if (idx == 0) {
         *output++ = '0' ;
      } else {
         while (idx > 0) {
            *output++ = tbfr[idx-1] ;
            idx-- ;
         }
      }
   }
   *output = 0 ;

   //  prepare output
   output = (outbfr == 0) ? local_bfr : outbfr ;
   return strlen(output) ;
}

//****************************************************************************
#define  PAD_RIGHT   1
#define  PAD_ZERO    2

static int prints (char **out, const char *string, int width, int pad)
{
   // [831326121984], width=0, pad=0
   // printf("[%s], width=%u, pad=%u\n", string, width, pad) ;
	register int pc = 0, padchar = ' ';
	if (width > 0) {
      int len = 0;
      const char *ptr;
		for (ptr = string; *ptr; ++ptr)
			++len;
		if (len >= width)
			width = 0;
		else
			width -= len;
		if (pad & PAD_ZERO)
			padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for (; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for (; *string; ++string) {
		printchar (out, *string);
		++pc;
	}
	for (; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}
	return pc;
}

//****************************************************************************
/* the following should be enough for 32 bit int */
//  -2,147,483,648
#define PRINT_BUF_LEN 12

static int printi (char **out, int i, unsigned int base, int sign, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
    char *s;
    int t, neg = 0, pc = 0;
    unsigned u = (unsigned) i;
	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}
   if (sign && base == 10 && i < 0) {
		neg = 1;
      u = (unsigned) -i;
	}
   //  make sure print_buf is NULL-term
	s = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	while (u) {
      t = u % base;  //lint !e573 !e737 !e713 Warning 573: Signed-unsigned mix with divide
		if (t >= 10)
			t += letbase - '0' - 10;
      *--s = (char) t + '0';
      u /= base;  //lint !e573 !e737  Signed-unsigned mix with divide
	}
	if (neg) {
		if (width && (pad & PAD_ZERO)) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
   } else {
      if (use_leading_plus) {
         *--s = '+';
      }
   }
	return pc + prints (out, s, width, pad);
}

//****************************************************************************
/* the following should be enough for 64 bit int, without commas */
// _UI64_MAX
// Maximum value for a variable of type unsigned __int64
//                         xx..xx..xx..xx..
// 18446744073709551615 (0xffffffffffffffff)
// -9223372036854775808
// 18,446,744,073,709,551,615
#define PRINT_LLBUF_LEN 22

static int printlli (char **out, long long i, unsigned int base, int sign, int width, int pad, int letbase)
{
   char print_buf[PRINT_LLBUF_LEN];
   char *s;
   int t, neg = 0, pc = 0;
   unsigned long long u = (unsigned long long) i;
   if (i == 0) {
      print_buf[0] = '0';
      print_buf[1] = '\0';
      return prints (out, print_buf, width, pad);
   }
   if (sign && base == 10 && i < 0) {
      neg = 1;
      u = (unsigned long long) -i;
   }
   //  make sure print_buf is NULL-term
   s = print_buf + PRINT_LLBUF_LEN - 1;
   *s = '\0';

   while (u) {
      t = u % base;  //lint !e573 !e737 !e713 Warning 573: Signed-unsigned mix with divide
      if (t >= 10)
         t += letbase - '0' - 10;
      *--s = (char) t + '0';
      u /= base;  //lint !e573 !e737  Signed-unsigned mix with divide
   }
   if (neg) {
      if (width && (pad & PAD_ZERO)) {
         printchar (out, '-');
         ++pc;
         --width;
      }
      else {
         *--s = '-';
      }
   } else {
      if (use_leading_plus) {
         *--s = '+';
      }
   }
   return pc + prints (out, s, width, pad);
}

//****************************************************************************
static int print (char **out, const char *format, va_list va)
{
   int post_decimal ;
   int width, pad ;
   unsigned dec_width = 6 ;
   int pc = 0;
   char scr[2];

   use_leading_plus = 0 ;  //  start out with this clear
	for (; *format != 0; ++format) {
		if (*format == '%') {
         dec_width = 6 ;
			++format;
			width = pad = 0;
			if (*format == '\0')
				break;
			if (*format == '%')
            goto out_lbl;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
         if (*format == '+') {
            ++format;
            use_leading_plus = 1 ;
         }
         while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
         post_decimal = 0 ;
         if (*format == '.'  ||
            (*format >= '0' &&  *format <= '9')) {

            while (1) {
               if (*format == '.') {
                  post_decimal = 1 ;
                  dec_width = 0 ;
                  format++ ;
               } else if ((*format >= '0' &&  *format <= '9')) {
                  if (post_decimal) {
                     dec_width *= 10;
                     dec_width += (unsigned int) (char) (*format - '0');
                  } else {
                     width *= 10;
                     width += *format - '0';
                  }
                  format++ ;
               } else {
                  break;
               }
            }
         }
         unsigned int use_longlong = 0 ;
         if (*format == 'l') {
            ++format;
            if (*format == 'l') {
               ++format;
               use_longlong = 1 ;
            }
         }
         switch (*format) {
         case 's':
            {
                char *s = va_arg(va, char *);
                pc += prints (out, s ? s : "(null)", width, pad);
                use_leading_plus = 0 ;  //  reset this flag after printing one value
            }
            break;
         case 'd':
            if (use_longlong) {
               pc += printlli(out, va_arg(va, long long), 10, 1, width, pad, 'a');
            }
            else {
               pc += printi  (out, va_arg(va, int), 10, 1, width, pad, 'a');
            }
            use_leading_plus = 0 ;  //  reset this flag after printing one value
            break;
         case 'x':
            if (use_longlong) {
               pc += printlli(out, va_arg(va, long long), 16, 0, width, pad, 'a');
            }
            else {
               pc += printi  (out, va_arg(va, int), 16, 0, width, pad, 'a');
            }
            use_leading_plus = 0 ;  //  reset this flag after printing one value
            break;
         case 'X':
            if (use_longlong) {
               pc += printlli(out, va_arg(va, long long), 16, 0, width, pad, 'A');
            }
            else {
               pc += printi  (out, va_arg(va, int), 16, 0, width, pad, 'A');
            }
            use_leading_plus = 0 ;  //  reset this flag after printing one value
            break;
         case 'p':
         case 'u':
            if (use_longlong) {
               pc += printlli(out, va_arg(va, unsigned long long), 10, 0, width, pad, 'a');
            }
            else {
               pc += printi  (out, va_arg(va, unsigned int), 10, 0, width, pad, 'a');
            }
            use_leading_plus = 0 ;  //  reset this flag after printing one value
            break;
         case 'c':
            /* char are converted to int then pushed on the stack */
            scr[0] = (char)va_arg(va, int);
            scr[1] = '\0';
            pc += prints (out, scr, width, pad);
            use_leading_plus = 0 ;  //  reset this flag after printing one value
            break;

         case 'f':
            {
                char bfr[81] ;
                dbl2stri(bfr, va_arg(va, double), dec_width) ;
                // stuff_talkf("[%s], width=%u, dec_width=%u\n", bfr, width, dec_width) ;
                pc += prints (out, bfr, width, pad);
                use_leading_plus = 0 ;  //  reset this flag after printing one value
            }
            break;

         default:
            printchar (out, '%');
            printchar (out, *format);
            use_leading_plus = 0 ;  //  reset this flag after printing one value
            break;
         }
      } else 
      // if (*format == '\\') {
      //    
      // } else 
      {
out_lbl:
			printchar (out, *format);
			++pc;
		}
   }  //  for each char in format string
   if (out) //lint !e850
		**out = '\0';
	return pc;
}

//****************************************************************************
int tiny_sprintf (char *out, const char *format, ...)
{
    max_output_len = -1 ;
    curr_output_len = 0 ;

    va_list va;
    va_start(va,format);
    int count =  print (&out, format, va);
    va_end(va);

    return count;
}

//****************************************************************************
int tiny_snprintf(char *out, unsigned int max_len, const char *format, ...)
{
    max_output_len = (int) max_len ;
    curr_output_len = 0 ;

    va_list va;
    va_start(va,format);
    int count =  print (&out, format, va);
    va_end(va);

    return count;
}

int tiny_vsnprintf(char *out, unsigned int max_len, const char *format, va_list va)
{
    max_output_len = (int) max_len;
    curr_output_len = 0;

    return print(&out, format, va);
}
