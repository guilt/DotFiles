# .zshrc
# shellcheck disable=SC2148,SC2034,SC1090,SC1091

#If not loaded.
if [ -z "$ZSH" ] ; then
export ZSH=true

if [ -f /etc/zshenv ]; then
	. /etc/zshenv
fi

if [ -f "$HOME/.bashrc" ]; then
	. "$HOME/.bashrc"
fi

#Add Oh My ZSH.
#Session Start
case "$-" in
	*i* ) 
	if [ -d "$HOME/.oh-my-zsh" ]; then
		DISABLE_UPDATE_PROMPT=true
		DISABLE_AUTO_UPDATE=true
		export ZSH="$HOME/.oh-my-zsh"
		ZSH_THEME=af-magic
		. "$ZSH/oh-my-zsh.sh"
	fi
	;;
esac
fi

ZSH=
export ZSH
