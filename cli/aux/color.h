#ifndef LIBTMAN_COLOR_H
#define LIBTMAN_COLOR_H

/*
 * This is free and unencumbered software released into the public domain.
 *
 * For more information, please refer to <https://unlicense.org>
 */

//Regular text
// pr_sgr_start("01;31");
#define BLK "00;30"
#define RED "00;31"
#define GRN "00;32"
#define YEL "00;33"
#define BLU "00;34"
#define MAG "00;35"
#define CYN "00;36"
#define WHT "00;37"

//Regular bold text
#define BBLK "01;30"
#define BRED "01;31"
#define BGRN "01;32"
#define BYEL "01;33"
#define BBLU "01;34"
#define BMAG "01;35"
#define BCYN "01;36"
#define BWHT "01;37"

//Regular underline text
#define UBLK "4;30"
#define URED "4;31"
#define UGRN "4;32"
#define UYEL "4;33"
#define UBLU "4;34"
#define UMAG "4;35"
#define UCYN "4;36"
#define UWHT "4;37"

//Regular background
#define BLKB "40"
#define REDB "41"
#define GRNB "42"
#define YELB "43"
#define BLUB "44"
#define MAGB "45"
#define CYNB "46"
#define WHTB "47"

//High intensty background
#define BLKHB "0;100"
#define REDHB "0;101"
#define GRNHB "0;102"
#define YELHB "0;103"
#define BLUHB "0;104"
#define MAGHB "0;105"
#define CYNHB "0;106"
#define WHTHB "0;107"

//High intensty text
#define HBLK "\33[0;90m"
#define HRED "\33[0;91m"
#define HGRN "\33[0;92m"
#define HYEL "\33[0;93m"
#define HBLU "\33[0;94m"
#define HMAG "\33[0;95m"
#define HCYN "\33[0;96m"
#define HWHT "\33[0;97m"

//Bold high intensity text
#define BHBLK "\33[1;90m"
#define BHRED "\33[1;91m"
#define BHGRN "\33[1;92m"
#define BHYEL "\33[1;93m"
#define BHBLU "\33[1;94m"
#define BHMAG "\33[1;95m"
#define BHCYN "\33[1;96m"
#define BHWHT "\33[1;97m"

//Reset
#define RST "\33[0m"
#define CRESET "\33[0m"
#define COLOR_RESET "\33[0m"

void color_print_str(const char *fmt, char *str, char *color);
void color_print_char(const char *fmt, char c, char *color);

#endif
