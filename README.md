# Dot Files for Unix Systems

For a long time, I have maintained these [Dot Files](https://en.wikipedia.org/wiki/Hidden_file_and_hidden_directory)
so I can interoperate with different **Unix Systems** and
I can get a somewhat consistent environment. This also
contains a lot of **Scripts and Binaries**. So, if you
are looking for this repository for anything meaningful,
you may not want to.

## Basic Structure of local

The `local` directory is similar in nature to `/usr/local` but the localization
applies within the home directory.

The `local` structure has **Platform-Independant** and **Platform-Dependant**
directories in it. The **Platform-Dependant** has **Processor-Independant**
and **Processor-Dependant** directories in it.

Example:

```
local/bin              | Scripts that are known to work for all **Unix** Systems
local/darwin/bin       | Scripts that are only known to work in **Darwin/Mac**
local/darwin/x64/bin   | Binaries that only work on **Intel Macs**
local/darwin/arm64/bin | Binaries that only work on **Apple Silicon Macs**
```

## Basic Structure of .bashrc

The `bashrc` files are shared between **ZSH** and **Bash** shells.

They are written to set the appropriate `local` based `PATH` variables
when run. Not only that, they define **Platform-Dependant** and **Host-Specific**
files, and optionally **Interactive-Mode** files as well.

Example:

```
.bashrc-linux                 | Runs on **Linux** Systems
.bashrc-raspberry-pi          | Runs on a host named **raspberry-pi**
.bashrc-raspberry-pi-session  | Runs on a host named **raspberry-pi** on an Interactive Terminal
```

In addition to that, we use the following files for these purposes:

```
.bashrc-proper                | Top-Level file that invokes All
.bashrc-aliases               | Shortened Commands and Aliases
.bashrc-applications          | Define BROWSER, EDITOR, PAGER etc.
.bashrc-exports               | Overrides for PATHs
.bashrc-path                  | Detect **Platform** and **Processor**
.bashrc-toolchain             | Sets **Platform** specific Toolchain Properties
```

## History Files

All of the History files I care about are symlinked to `/dev/null`.

## ZSH Specific

I use [Oh My ZSH](https://ohmyz.sh/) with some customizations and it is
part of this repo.

## Vim Files

I need to get a better **VIM** customization. I use that sort of functionality
occasionally. I only have basic customization.

## Screen

I do not use **TMUX**. I have customized this decently.


## Instructions

1. Check for any customizations and commit them somewhere else.

2. `git clone` or extract this repo into a directory called `DotFiles` in your
home directory. Then run these commands:

```shell
echo I am ready to nuke my files     # Stop! This is going to nuke everything ...
rm -fr ~/local/ ~/.oh-my-zsh         # Clean State.
cp -a ~/DotFiles/local ~/local/      # Move the local folder over.
mv ~/DotFiles/.?*? ~/                # Move the Dot Files. Nukes History.
```

3. Log out and re-login.


## Thank You and Feedback

Reach out to me for any feedback.

Now Enjoy!

* Author: Karthik Kumar Viswanathan
* Web   : https://karthikkumar.org
* Email : karthikkumar@gmail.com
