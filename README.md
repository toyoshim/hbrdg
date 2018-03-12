# High Bridge - 高橋くん

## Features

 - Button Rotate
 - Auto Fire

### Button Rotate

If DIP1-1 is ON, button 1,2,3,4 was reordered as 4,1,2,3.
This is useful on using 6-buttons controller as 4-buttons one.

```
Rotate OFF

       (3)  (4)
   (2)
      (5)  (6)
  (1)

Rotate ON

       (2)  (3)
   (1)
      (5)  (6)
  (4)


```
### Auto Fire

Button 1, 2, 3 has a DIP switch to enable Auto Fire feature.
Shot-speed can be changed by DIP1-[4:3].

 - OFF(H) - OFF(L): 10 shots/sec
 - OFF(H) - ON (L): 15 shots/sec
 - ON (H) - OFF(L): 20 shots/sec
 - ON-(H) - ON (L): 30 shots/sec

## Fuse

High: 0xDF
Low : 0x62
Ext : 0xF9

## Circuit

```
                                      +-----+
 B1 IN (w/ext-pull-up) o---o PD0 ( 2) |  A  |
 B2 IN (w/ext-pull-up) o---o PD1 ( 3) |  T  | (26) PC3 o---o B4 OUT
 B3 IN (w/ext-pull-up) o---o PD2 ( 4) |  M  | (25) PC2 o---o B3 OUT
 B4 IN (w/ext-pull-up) o---o PD3 ( 5) |  E  | (24) PC1 o---o B2 OUT
                                      |  G  | (23) PC0 o---o B1 OUT
                Rotate o---o PB6 ( 9) |  A  |
     Auto Fire Speed L o---o PD6 (12) |     |
     Auto Fiue Speed H o---o PD7 (13) |  8  | (16) PB2 o---o B3 Auto Fire
          B1 Auto Fire o---o PB0 (14) |  8  | (15) PB1 o---o B2 Auto Fire
                                      +-----+

 DIP1
         /
  1 +---o  o---o Rotate
         /
  2 +---o  o---o N/C
         /
  3 +---o  o---o Auto Fire Speed H
         /
  4 +---o  o---o Auto Fire Speed L
    |
   ---
   ///

 DIP2
         /
  1 +---o  o---o Button 1 Auto Fire
         /
  2 +---o  o---o Button 2 Auto Fire
         /
  3 +---o  o---o Button 3 Auto Fire
         /
  4 +---o  o---o N/C
    |
   ---
   ///

```