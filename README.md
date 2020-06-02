# bomberman [[42](https://www.42.fr/) project] [![Build Status](https://travis-ci.com/tnicolas42/bomberman.svg?branch=master)](https://travis-ci.com/tnicolas42/bomberman)

![logo](https://github.com/tnicolas42/bomberman-assets/blob/master/imgs/bomberman-logo.png)

## 💣 The project

A 3d remake of the classic game Bomberman.

The project is fully written in C++ with the OpenGL framework.

### 📚 Rules

Bomberman is a series of video games from Hudson Soft where the player plays a bomber, the aim being to use bomb to protect yourself from opponents and exit the level.

> View the full game rules in the in-game **help menu**.

![demo gif](../media/bombermanDemo.gif?raw=true)

### ⌨ General controls

You can see and update all controls in the in-game **settings menu**.

![control menu](../media/ControlsMenu.png?raw=true)

Some hidden debug controls:

| Key  | Description |
| --- | --- |
| <kbd>F1</kbd> | Show the menu navigation shortcuts |
| <kbd>F3</kbd> | Show debugs infos (fps, nb of entities, ...) |
| <kbd>C</kbd> | Enable Free Flight Camera (only available with the [DEBUG flag](#-manual-installation)) |
| <kbd>W</kbd><kbd>A</kbd><kbd>S</kbd><kbd>D</kbd> | Move the camera (only available if Free Flight Camera is enabled) |

### 💾 save & load system

Save and load your progress:

![save menu](../media/saveMenu.png?raw=true)

### 👩‍💻 Cheatcodes

You can enter cheatcode by pressing the <kbd>/</kbd> key.

![cheatcode menu](../media/cheatcode.png?raw=true)

| Command  | Description |
| --- | --- |
| `/help` [&nbsp;command, ...&nbsp;]  | Get general help or help on a command.  |
| `/clear` [&nbsp;history \| all \| lines&nbsp;]  | Clear lines or/and history.  |
| `/log` \<type> \<message>  | Log a message in the system console, `/log list` to get the list of types.  |
| `/tp` \<x> \<y>  | Teleport player to a given position (if possible). Use relative position with `~` (~-1, ~3, ...)<br>`/tp ~3 ~`: Tp the player 3 blocks right to you.  |
| `/getbonus` <bonus ...>  | Get a bonus effect, `/getbonus list` to get the list of bonus.  |
| `/loop` \<nb> \<commands&nbsp;...>  | Execute commands `nb` times, example:<br>`/loop 3 "/clear" "/getbonus life"`  |
| `/exec` \<commands&nbsp;...>  | Execute multiples commands, example:<br>`/exec "/clear" "/log info \"cleared\""`  |
| `/summon` \<typename> \<x> \<y>  | Summon an entity at specified location, `/summon list` to get the list of entities   |
| `/unlock` \<levelId&nbsp;...>  | Unlock specified level  |
| `/rmbonus` \<bonus&nbsp;...>  | Remove a bonus effect `/rmbonus list` to get the list of bonus  |
| `/restart`   | Restart the current level  |
| `/debug` [&nbsp;show \| hide \| reset&nbsp;] \<elem ...>  | Show/Hide/Reset debug element, `/debug list` to get the list of elements, example:<br>`/debug show collider`  |
| `/volume` [&nbsp;master \| music \| sound&nbsp;] \<value>  | Change master/music/sound volume. `/volume list` to get the list of volumes's type |

## 💻 Installation

Bomberman is compatible with **OSX🍎** and **Linux🐧**.

If you are on **OSX🍎** you can directly download the dmg build [here](https://www.dropbox.com/s/uk0l9rlvxbrjpoo/bomberman.dmg?dl=0).

### 👷 Manual installation

Clone the repository and install the dependencies
```bash
git clone https://github.com/tnicolas42/bomberman
cd bomberman
make install
```

Compile the project

```bash
make -j8
```

```bash
make -j8 DEBUG=1  # to compile in debug mode
```

Execute the game
 ```usage
usage: ./bomberman [-u] [--reset] [--reset-settings] [--reset-history] [--reset-saves]
	--reset --reset-all: reset all users data (settings, history and saves) before starting
	--reset-settings: reset all users settings before starting
	--reset-history: reset commands history before starting
	--reset-saves: reset all users saves before starting
	-u, --usage: show usage
```

### 🗑 Uninstall projet

To uninstall the project, and remove app, settings and saves's games
```
make uninstall
```

### 🗜 Makefile and Build

If you need to generate a `.dmg` for osx
```
make create_dmg
```

ℹ️ To get more infos about the makefile commands
```bash
make help
```

### 📖 The code is fully documented with doxygen
Check the documentation [here](https://tnicolas42.github.io/bomberman)

### 🍩 3d models
All 3d models are made by us with blender, some are animated using Mixamo, [Inspect 3d models here](https://sketchfab.com/zer0nim/collections/bomberman-remake-assets).

## 🦄 Authors

* **Ernest Marin** - [github](https://github.com/zer0nim)
* **Emilien Baudet** - [github](https://github.com/ebaudet)
* **Tim Nicolas** - [github](https://github.com/tnicolas42)
* **Guilhem Smith** - [github](https://github.com/guilhemSmith)

See also the list of [contributors](https://github.com/tnicolas42/bomberman/graphs/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
