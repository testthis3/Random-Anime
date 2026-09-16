# RandomAnime

Minimal CLI anime list manager written in C.

## Dependencies

* gcc
* make
* glibc

# 1. Installation

Clone the project, and run

```
$ make
$ make install clean
```

This installs `ra` to:

```
~/.local/bin/ra
```

Make sure `~/.local/bin` is in your PATH.

```
echo $PATH | grep -q "$HOME/.local/bin" && echo "Yes, it is in PATH" || echo "No, it is NOT in PATH"
```

If it's not in PATH, go to `$HOME/.bashrc` and add

```
export PATH="$PATH:$HOME/.local/bin"
```

# 2. Usage

Run without arguments to get a random anime from your list:

```
$ ra
```

Add an anime:

```
$ ra add "anime name"
```

Query your anime list:

```
$ ra query "anime name"
```

Remove an anime:

```
$ ra remove "anime name"
```

Get information about an anime:

```
$ ra info "anime name"
```

## Examples

```
$ ra add "Samurai Champloo"
Samurai Champloo is added succesfully to the anime list

$ ra query "samurai"
Samurai Champloo

$ ra remove "Samurai Champloo"
Samurai Champloo successfully removed.

$ ra info "Samurai Champloo"

──────────────────────────────────────
  Samurai Champloo
──────────────────────────────────────

   Status   : FINISHED
   Episodes : 26
   Release  : 2004
   Genres   : Action Adventure Comedy
   Plot
   Let's break it down. Mugen's a reckless sword-slinger with a style
   that's more b-boy than Shaolin. He's got a nasty streak that makes
   people want to stick a knife in his throat. Then there's Jin, a
   deadbeat ronin who speaks softly but carries a big blade. He runs
   game old-school style, but he can make your blood spray with the
   quickness. When these roughnecks bring the ruckus, it ain't good
   for anybody, especially them. Enter Fuu, the ditzy waitress who
   springs her new friends from a deadly jam. All she wants in return
   is help solving a riddle from her past. She and the boys are
   tracking the scent, but there's 99 ways to die between them and the
   sunflower samurai.



(Source: Funimation)
```

# 3. Anime List

The anime list is stored automatically at:

```
~/Projects/C/RandomAnime/AnimeList.txt
```

The program creates and manages the file automatically.
