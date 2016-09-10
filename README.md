#About

_tmix_ is a terminal based program to set mixer values that uses ncurses
to visualize volume settings. It is heavily inspired by _alsamixer_.

#License

This project is licensed under BSD 3-Clause license.
See LICENSE file for detail

#Build

Simply type _make_.
The executable can be found in the _obj_ directory.

#Usage

| Key  | Function |
|------|----------|
| Left/Right arrow key | Select mixer |
| Up/Down arrow key | Adjust volume |
| l,r,b | Toogle channel: L)eft, R)ight, B)oth |
| m | (Un)mute mixer or toogled channel |
| a | Align volumnes of both channels to the higher one. Channels get unmuted. |
| q | Quit |
