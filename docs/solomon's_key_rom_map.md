# Solomon's Key (NES) ROM map (v1.0-snapshot)

## Table of Contents
* [Introduction](#introduction)
* [Data Tables](#data-tables)
* Demon Mirror Drop Rates
* Demon Mirror Enemy Sets
* Enemy Data
* [Block Data](#block-data)
* Level Metadata and Item Data
* Game Metadata
* Map of Enemy Types
* Map of Item Types
* [Contact](#contact)
* [Changelog](#changelog)

<hr>

## Introduction

This document describes the ROM map layout for Solomon's Key (NES) to the best of my current knowledge. As more information is gathered, I will update this document. If you have anything to add, see my [contact information](#contact).

There are several data tables in the ROM: Demon Mirror Drop Rates, Demon Mirror Enemy Sets, Enemy Data and Item Data. The level data sections have the following layout, in order (5120 contiguous bytes):

| Name | Offset (US/JP) | Offset (EU) | Maximum Length (bytes) |
|------|---------------|-------------|------------------------|
| Mirror Drop Rate Table  | 0x5c10 | 0x5c30 | 32 (16x2) |
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

## Block Data

The block data section contains the definitions of the level block data, stored as bitmasks. Since each level is 16 by 12 tiles, one level's block definition containts (16x12)/8 = **24** bytes. Each level has a bitmask for brown (breakable) blocks, and white (unbreakable) blocks. The byte size is therefore 24x2 = **48** bytes per level, and with 53 levels the total block data section is 48x53 = **2544** bytes.

For each level, the first 12 bytes define the brown blocks, and the next 12 bytes define the white blocks.

The block data is read from top to bottom, left to right.

<span style="color:lightgreen">
Example: A given byte has value 0x63. In binary this is <b>01100011</b> This means that 4 of the 8 possible blocks are set: In this case at locations 2,3,7 and 8.
</span>

It is possible for a brown and white block to be set on the same location. In this case it will count as a white block.

<hr>

## Contact
This document is written and maintained by Kai E. Frøland aka [kaimitai](https://github.com/kaimitai/skchain). If you have comments, corrections or other feedback - contact me at kaimiai@pm.me.

<hr>

## Changelog
* 2023-15-01: v1.0 in progress
