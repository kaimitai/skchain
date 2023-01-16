# Solomon's Key (NES) ROM map (v1.0-snapshot)

## Table of Contents
* [Introduction](#introduction)
* [Data Tables](#data-tables)
* [Demon Mirror Drop Rates](#demon-mirror-drop-rates)
* [Demon Mirror Enemy Sets](#demon-mirror-enemy-sets)
* [Enemy Data](#enemy-data)
* [Block Data](#block-data)
* [Level Metadata and Item Data](#item-data)
* Game Metadata
* [Map of Enemy Types](#enemy-types)
* [Map of Item Types](#item-types)
* [Contact](#contact)
* [Changelog](#changelog)

<hr>

## Introduction

This document describes the ROM map layout for Solomon's Key (NES) to the best of my current knowledge. As more information is gathered, I will update this document. If you have anything to add, see my [contact information](#contact).

There are several data tables in the ROM: Demon Mirror Drop Rates, Demon Mirror Enemy Sets, Enemy Data and Item Data. The level data sections have the following layout, in order (5120 contiguous bytes):

| Name | Offset (US/JP) | Offset (EU) | Maximum Length (bytes) |
|------|---------------|-------------|------------------------|
| Mirror Drop Rate Table  | 0x5c10 | 0x5b90 | 32 (16x2) |
| Mirror Enemy Sets Table | 0x5c30 | 0x5bb0 | 34 (17x2) |
| Mirror Drop Rate Data | 0x5c52 | 0x5bd2 | 128 (16x8) |
| Mirror Enemy Sets Data | 0x5cd2 | 0x5c52 | 42 |
| Enemy Data Table | 0x5cfc | 0x5c7c | 106 (53x2) |
| Enemy Data | 0x5d66 | 0x5ce6 | 726 |
| Block Data | 0x603c | 0x5fbc | 2544 (2x2x12x53) |
| Item Data Table | 0x6a2c | 0x69ac | 106 (53x2) |
| Item Data | 0x6a96 | 0x6a16 | 1402 |

Information on how to decode the data tables, and the actual data they point to, follows. All numbers which start with **0x** are hexadecimal values. Otherwise, they are decimal.

<hr>

## Data Tables

As described in the introduction, there are four section tables in the ROM. Before decoding data, we need to know how to read these tables.

Each table points to certain locations in the ROM. Each location is given by two bytes; "low" and "high". The table starts at a certain offset in the ROM, and if it points to N different elements, the table consists of 2xN bytes.

The first half of the table describes the low bytes of the element offset, and the second half of the table describes the high bytes.

<span style="color:lightgreen">

Example: Find the offset of the enemy data for Room 1 in an unmodified version of the US version of the ROM:

</span>

<span style="color:yellow">

* The enemy data table is located at offset 0x5cfc. The first byte is the low byte of the first level's location: **0x56**.
* There are 53 levels in the game, so this table points at 53 different locations. Hence, the high byte of the location we seek is at offset 0x5cfc + 53 = 0x5d31: **0xdd**.
* The RAM location of the sought element is then given by concatenating these values: **0xdd56**.
* The ROM location is then found by subtracting **0x7ff0** - a constant which describes the difference between ROM and RAM addresses.
* 0xdd56 - 0x7ff0 = ***0x5d66***. This address corresponds to the start of the enemy data section given in the introduction.

</span>

All data tables are decoded in this manner. By knowing the table offset, and the number of elements it contains the locations of - we can now deduce the start of any element's data section in the ROM file.

Information on how decode the actual data for each section follows.

<hr>

## Demon Mirror Drop Rates

The demon mirror drop rate table points to 16 elements, which means that the drop rate table is 32 (2x16) bytes long. In the original game each element is 8 bytes long, and one might wonder why a data table was needed at all.

In any case, the demon mirrors in any level is associated with one of the sixteen drop rates. A drop rate is a bitmask, where a bit turned on means an enemy will be dropped at that time.

The first four bytes, 32 bits, define the intial schedule, which will be played through when the level starts.

The next four bytes, 32 bits, define the looping schedule, which will be played over and over during gameplay, after the initial schedule is done.

<hr>

## Demon Mirror Enemy Sets

The demon mirror enemy sets data table points to 17 elements, which means that the table is 34 (2x17) bytes long. The enemy set data section itself is 42 bytes long, and the enemy sets can be of varying length. In the original game the sets consist of 1-3 monsters.

The demon mirrors in any level is associated with one of the seventeen enemy sets. Combined with one of the sixteen drop rates, there are **16x17 = 272** different combinations; 272 ways in which a demon mirror can behave.

Each enemy set consists of a list of enemy codes, which can be looked up in the [Enemy Types](#enemy-types) table. The value 0x90 signifies end-of-stream for an enemy set list.

The enemy sets data section itself is up to 42 bytes long, and this leaves room for up to <b>1.47</b> enemies per set on average.

<hr>

## Enemy Data

Each of the 53 levels has an enemy data section, meaning that the Enemy Data Table consists of 106 (2x53) bytes.

Any given enemy data section is of variable length, as the different levels have varying amounts of enemies in them. The minimum length of an enemy data section is two bytes; one byte for the spawn lifetime, and one byte for the end-of-stream delimiter.

The enemy data section itself is up to 726 bytes long, and this leaves room for up to **5.85** enemies per level on average. Of course a level might have 0 enemies, or up to as many as 16 enemies - but the total data section cannot exceed 726 bytes.

The first byte of the enemy data is the spawn enemy lifetime. This value determines how long Demonheads and Saramandors will live, after they have been spawned in the level. The value can go from 0-255, but this byte is bit-rotated right by 3 bits in the ROM, for some reason. Rotate left and right by 3 bits to convert.

The next bytes will come in pairs; enemy number followed by enemy position.

Every two bytes will thus define an enemy, until the end-of-stream delimiter 0x00 is reached.

See the [Enemy Table](#enemy-types) for information on all possible enemy values.

Positions consist of one byte, where the high nibble is the y-position minus one, and the low nibble is the x-position. For example the value 0xa5 gives the position (5, 9). A high nibble value of 0 gives a y-value of -1, which is not visible on the screen.

<span style="color:lightgreen">
Example: Level 1's enemy data section consists of the following bytes: 0x20, 0x71, 0x67, 0x00.

The first byte is the spawn enemy lifetime, bit-shifted right by 3 bits. 0x20 in binary is <b>00100000</b>. Left-shift by 3 (or right-shift by 5) to get the binary value <b>00000001</b>, which means that the spawn lifetime is 1 (where 255 is the maximum).

The next two bytes define an enemy with enemy code 0x71 and position 0x67.

Enemy 0x71 is "Golem (left, speed 1)", and its position is (6, 6) when counting the top left corner of the level as (0, 0). Note that we subtracted 1 from the y-position.

The fourth byte is 0x00, which is the end-of-stream indicator. This means that level one only has one enemy.

</span>

<hr>

## Block Data

The block data section contains the definitions of the level block data, stored as bitmasks. Since each level is 16 by 12 tiles, one level's block definition containts (16x12)/8 = **24** bytes. Each level has a bitmask for brown (breakable) blocks, and white (unbreakable) blocks. The byte size is therefore 24x2 = **48** bytes per level, and with 53 levels the total block data section is 48x53 = **2544** bytes.

For each level, the first 12 bytes define the brown blocks, and the next 12 bytes define the white blocks.

The block data is read from top to bottom, left to right.

<span style="color:lightgreen">
Example: A given byte has value 0x63. In binary this is <b>01100011</b> This means that 4 of the 8 possible blocks are set: In this case at locations 2,3,7 and 8.
</span>

It is possible for a brown and white block to be set on the same location. In this case it will count as a white block.

The rightmost row always consists of white blocks in the original game. If one of these blocks is missing, it is possible to walk throught the wall and wrap around.

<hr>

## Item Data

Each of the 53 levels has an item data section, meaning that the Item Data Table consists of 106 (2x53) bytes.

Any given utem data section is of variable length, as the different levels have varying amounts of items in them. The item data section contains some level metadata too, and supports a simple compression routine when a level contains several items of the same type.

The first ten bytes of the data section for a given level make up a level metadata header:

| Byte | Description |
| ---- | ----------- |
| 0 | Demon mirror #2 spawn schedule (0-15) |
| 1 | Demon mirror #1 spawn schedule (0-15) |
| 2 | Demon mirror #2 enemy set (0-16) |
| 3 | Demon mirror #1 enemy set (0-16) |
| 4 | Key Status and time decrease rate* |
| 5 | Door Position |
| 6 | Key Position |
| 7 | Player Start Position |
| 8 | Demon mirror #1 position |
| 9 | Demon mirror #2 position |

Key Status and time decrease rate are stored in the same byte. If the second bit is set, the key is in a block. If the first bit is set, the key is hidden, meaning the player needs to create and destroy a block to reveal the key.

The time decrease rate is the last nibble of this byte, meaning it has a value of 0 (slow time decrease) to 15 (fast time decrease).

After the 10-byte metadata header, the item data follows. The items can come in pairs; item code and position. See the [item types](#item-types) section for details on each item code. Valid item codes are 0x01 - 0xbf.

The base item codes are 0x01-0x3f, but the first and second bits can be set to alter the item attributes; first bit set means the item is hidden in a block, second bit set means that the player must create and destroy a block to reveal the item. For this reason the maximum item code including modifiers is 0xbf.

Codes from 0xc0 to 0xdf indicates that the next item will be repeated, and provides a simple kind of compression. Take the code and subtract 0xc0, add one, and you have the number of items. The next byte will give the item code, and the next bytes give the position for each of these duplicate items.

<span style="color:lightgreen">
Example: In a data section we see the following bytes: 0xc2, 0x12, 0xa1, 0x22, 0x5a.

We see that the first byte is between 0xc0 and 0xdf, meaning we are compressing.  We subtract 0xc0 and add 1, which gives us 3. Three items of the same type will follow; first the item type; 0x12 - and then the positions of the three items; (1, 9), (2, 1) and (10, 4).

</span>

Codes 0x00 or 0xe0 - 0xef indicate end-of-stream, but also tells us which tileset the level uses. Take bits 5 and 6, and AND it with 3 to get the tileset number. (0-2).

<span style="color:yellow">

Example: If we have a code 0xe5. We calculate <b>(0xe5 >> 2) & 3</b> to get the tileset number.

</span>

Codes 0xf0 - 0xfb indicate constellation picture as well as tileset number. The difference here is that the constellation has a position too, which is the following byte. Once the position is consumed, the stream ends. In other words, constellations act as end-of-streams, and it follows that a level can only have one constellation background.

The tileset number is derived in the same way as for codes 0xe0 - 0xef, and the following table shows which constellations belong to which codes:

| Hex Code | Constellation Name | Tileset No |
| -------- | ------------------ | ---------- |
| 0xf0 | Aries | 0 |
| 0xf1 | Gemini | 0 |
| 0xf2 | Virgo | 0 |
| 0xf3 | Aquarius | 0 |
| 0xf4 | Cancer | 1 |
| 0xf5 | Scorpio | 1 |
| 0xf6 | Capricorn | 1 |
| 0xf7 | Pisces | 1 |
| 0xf8 | Taurus | 2 |
| 0xf9 | Leo | 2 |
| 0xfa | Libra | 2 |
| 0xfb | Sagittarius | 2 |

Codes 0xfc and up are invalid.

<hr>

## Enemy Types

The following enemies can safely be placed in the levels. The column "Original Use Count" shows how many times the enemy appears in the original ROM. Note that a lot of enemies have duplicate codes. I differentiate between them by adding **"#2"** to the name of the secondary version.

All enemies, apart from flames, have at least one of the properties "direction" and "speed".

Ghosts and Neul also have a property we call "noslow", which means that the ghost/Neul will not be slowed when turning around after hitting a wall.

Fireballs have a property "cw" or "ccw", which means they will go clockwise or counter-clockwise, respectively, after hitting a wall.

Demonheads and Saramandors only spawn from Demon Mirrors in the original game, but they can be placed in the levels from the start, like any other enemy.

| Hex Code | Description | Original Use Count |
| -------- | ----------- | ------------------ |
| 0x18 | Mighty Bomb Jack (right) | |
| 0x19 | Mighty Bomb Jack (left) | |
| 0x1a | Mighty Bomb Jack (up) | |
| 0x1b | Mighty Bomb Jack (down) | |
| 0x1c | Fairy| 3 |
| 0x1d | Fairy Princess| 1 |
| 0x1e | Fairy #2 | |
| 0x1f | Fairy Princess #2 | |
| 0x20 | Bullet (right)| |
| 0x21 | Bullet (left)| |
| 0x22 | Bullet (up)| |
| 0x23 | Bullet (down)| |
| 0x24 | Panel Monster (right)| 7 |
| 0x25 | Panel Monster (left)| 1 |
| 0x26 | Panel Monster (up)| 6 |
| 0x27 | Panel Monster (down)| 4 |
| 0x28 | Fireball (right, ccw, speed 1)| 3 |
| 0x29 | Fireball (left, cw, speed 1)| 6 |
| 0x2a | Fireball (up, cw, speed 1)| 13 |
| 0x2b | Fireball (down, ccw, speed 1)| 28 |
| 0x2c | Fireball (right, ccw, speed 2)| 1 |
| 0x2d | Fireball (left, cw, speed 2)| 2 |
| 0x2e | Fireball (up, cw, speed 2)| 11 |
| 0x2f | Fireball (down, ccw, speed 2)| 7 |
| 0x30 | Neul (up, speed 1)| 2 |
| 0x31 | Neul (up, speed 1) #2| |
| 0x32 | Neul (down, speed 1)| 2 |
| 0x33 | Neul (down, speed 1) #2| |
| 0x34 | Ghost (right, speed 1)| 6 |
| 0x35 | Ghost (right, speed 1) #2| |
| 0x36 | Ghost (left, speed 1)| 16 |
| 0x37 | Ghost (left, speed 1) #2| |
| 0x38 | Neul (up, speed 2)| |
| 0x39 | Neul (up, speed 2) #2| |
| 0x3a | Neul (down, speed 2)| |
| 0x3b | Neul (down, speed 2) #2| |
| 0x3c | Ghost (right, speed 2)| 2 |
| 0x3d | Ghost (right, speed 2) #2| |
| 0x3e | Ghost (left, speed 2)| 1 |
| 0x3f | Ghost (left, speed 2) #2| |
| 0x40 | Neul (up, speed 1, noslow)| 1 |
| 0x41 | Neul (up, speed 1, noslow) #2| |
| 0x42 | Neul (down, speed 1, noslow)| 2 |
| 0x43 | Neul (down, speed 1, noslow) #2| |
| 0x44 | Ghost (right, speed 1, noslow)| 7 |
| 0x45 | Ghost (right, speed 1, noslow) #2| |
| 0x46 | Ghost (left, speed 1, noslow)| 14 |
| 0x47 | Ghost (left, speed 1, noslow) #2| |
| 0x48 | Neul (up, speed 2, noslow)| 1 |
| 0x49 | Neul (up, speed 2, noslow) #2| |
| 0x4a | Neul (down, speed 2, noslow)| |
| 0x4b | Neul (down, speed 2, noslow) #2| |
| 0x4c | Ghost (right, speed 2, noslow)| |
| 0x4d | Ghost (right, speed 2, noslow) #2| 2 |
| 0x4e | Ghost (left, speed 2, noslow)| |
| 0x4f | Ghost (left, speed 2, noslow) #2| |
| 0x50 | Demonhead (right, speed 1)| |
| 0x51 | Demonhead (left, speed 1)| |
| 0x52 | Demonhead (right, speed 1) #2| |
| 0x53 | Demonhead (left, speed 1) #2| |
| 0x54 | Demonhead (right, speed 2)| |
| 0x55 | Demonhead (left, speed 2)| |
| 0x56 | Demonhead (right, speed 2) #2| |
| 0x57 | Demonhead (left, speed 2) #2| |
| 0x58 | Demonhead (right, speed 3)| |
| 0x59 | Demonhead (left, speed 3)| |
| 0x5a | Demonhead (right, speed 3) #2| |
| 0x5b | Demonhead (left, speed 3) #2| |
| 0x5c | Saramandor (right, speed 1)| |
| 0x5d | Saramandor (left, speed 1)| |
| 0x5e | Saramandor (right, speed 1) #2| |
| 0x5f | Saramandor (left, speed 1) #2| |
| 0x60 | Saramandor (right, speed 2)| |
| 0x61 | Saramandor (left, speed 2)| |
| 0x62 | Saramandor (right, speed 2) #2| |
| 0x63 | Saramandor (left, speed 2) #2| |
| 0x64 | Saramandor (right, speed 3)| |
| 0x65 | Saramandor (left, speed 3)| |
| 0x66 | Saramandor (right, speed 3) #2| |
| 0x67 | Saramandor (left, speed 3) #2| |
| 0x68 | Dragon (right, speed 1)| 5 |
| 0x69 | Dragon (left, speed 1)| 3 |
| 0x6a | Dragon (right, speed 1) #2| |
| 0x6b | Dragon (left, speed 1) #2| |
| 0x6c | Dragon (right, speed 2)| |
| 0x6d | Dragon (left, speed 2)| 1 |
| 0x6e | Dragon (right, speed 2) #2| |
| 0x6f | Dragon (left, speed 2) #2| |
| 0x70 | Golem (right, speed 1)| 11 |
| 0x71 | Golem (left, speed 1)| 3 |
| 0x72 | Golem (right, speed 1) #2| |
| 0x73 | Golem (left, speed 1) #2| |
| 0x74 | Golem (right, speed 2)| 9 |
| 0x75 | Golem (left, speed 2)| 5 |
| 0x76 | Golem (right, speed 2) #2| |
| 0x77 | Golem (left, speed 2) #2| |
| 0x78 | Gargoyle (right, speed 1)| 11 |
| 0x79 | Gargoyle (left, speed 1)| 12 |
| 0x7a | Gargoyle (right, speed 1) #2| |
| 0x7b | Gargoyle (left, speed 1) #2| |
| 0x7c | Gargoyle (right, speed 2)| |
| 0x7d | Gargoyle (left, speed 2)| 2 |
| 0x7e | Gargoyle (right, speed 2) #2| |
| 0x7f | Gargoyle (left, speed 2) #2| |
| 0x80 | Red Flame| 11 |
| 0x81 | White Flame| 88 |
| 0x82 | Red Flame #2| |
| 0x83 | White Flame #2| |

The following table describes glitchy and unusable enemy codes:

| Hex Code | Description |
| -------- | ----------- |
| 0x00 | Illegal - Acts as a data delimiter |
| 0x01 - 0x13 | Stationary fireballs |
| 0x14 - 0x17 | Glitchy "gravity fairies" |

Enemy codes 0x84 and up are not in use.

<hr>

## Item Types

The following items can safely be placed in the levels. The column "Original Use Count" shows how many times the item appears in the original ROM. Note also that a few items will look different based on the tileset used. (For example: the warp flag)

To make an item hidden, take its hex value and set the 2nd bit. To make an item appear inside a brown block, set the 1st bit.

| Hex Code | Description | Original Use Count |
| -------- | ----------- | ------------------ |
| 0x04 | Bat Symbol | 26 | 
| 0x05 | Demon Mirror | | 
| 0x08 | Diamond (blue, modifiable) | 11 | 
| 0x09 | Firejar (blue, modifiable) | | 
| 0x0a | Double Coin (gold, modifiable) | | 
| 0x0b | Jewels (orange, modifiable) | | 
| 0x0c | Diamond (orange, modifiable) | 4 | 
| 0x0d | Firejar (orange, modifiable) | | 
| 0x0e | Scroll (modifiable) | 1 | 
| 0x0f | Bell (modifiable) | | 
| 0x11 | Timebottle (half) | 5 | 
| 0x12 | Timebottle (full) | 11 | 
| 0x13 | Hourglass (blue) | 11 | 
| 0x14 | Hourglass (orange) | 11 | 
| 0x15 | Firejar (blue) | 12 | 
| 0x16 | Firejar (orange) | 3 | 
| 0x17 | Scroll | 3 | 
| 0x18 | Bell | 71 | 
| 0x19 | Explosion Jar | 10 | 
| 0x1a | Key (blue) | | 
| 0x1b | Jewels (blue) | 64 | 
| 0x1c | Shrine #1 | 3 | 
| 0x1d | Shrine #2 | 3 | 
| 0x1e | Shrine #3 | 3 | 
| 0x1f | Shrine #4 | 3 | 
| 0x20 | Solomon's Seal | | 
| 0x21 | Egyptian Head | | 
| 0x22 | Warp Item | 5 | 
| 0x25 | Coin (silver) | 11 | 
| 0x26 | Double Coin (silver) | 10 | 
| 0x27 | Opal (blue) | 21 | 
| 0x28 | Coin (gold) | 20 | 
| 0x29 | Double Coin (gold) | 17 | 
| 0x2a | Opal (orange) | 14 | 
| 0x2b | Star Coin | 17 | 
| 0x2c | Star Coin (double) | 6 | 
| 0x2d | Opal (dark orange) | 10 | 
| 0x2e | Origami Swan | 11 | 
| 0x2f | Demonhead Coin | 5 | 
| 0x30 | Sphinx | 2 | 
| 0x31 | Egyptian Head | 1 | 
| 0x32 | Magic Lamp | 1 | 
| 0x33 | E-bottle | 9 | 

The following table describes glitchy and unusable item codes:

| Hex Code | Description |
| -------- | ----------- |
| 0x00 | Illegal - Acts as a data delimiter |
| 0x01 | Glitch broken block |
| 0x02 | Glitch door |
| 0x03 | Glitch White Block |
| 0x06 | Glitch Key |
| 0x07 | Glitch Open Door |
| 0x10 | Nothing |
| 0x23 | Glitch open door |
| 0x24 | Glitch open door |
| 0x33 - 0x36 | Glitch 1-Up |
| 0x37 - 0x3f | Garbage gfx |

Item codes 0xc0 and up are illegal, as they signify other things in the item data stream. See the [Item Data section](#item-data) for details.

<hr>

## Contact
This document is written and maintained by Kai E. Frøland aka [kaimitai](https://github.com/kaimitai/skchain). If you have comments, corrections or other feedback - contact me at kaimiai@pm.me.

<hr>

## Changelog
* 2023-15-01: v1.0 in progress
