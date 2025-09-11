" Enable type file detection
filetype on

" Enable plugins and load plugin for the detected file type
filetype plugin on

" Do not backup files
set nobackup
set nowritebackup

" Default Theme with Highlighting
set background=dark
if exists("g:syntax_on")
    syntax on
endif
set number

" Set Mouse to Scroll
set mouse=a

" Detect VimFiles
if isdirectory(expand('~/VimFiles'))
    let $VIMDIR=expand('~/VimFiles')
    let $VIMPLUGINDIR=expand('~/VimFiles/Plugged')
else
    let $VIMDIR=expand('~/.vim')
    let $VIMPLUGINDIR=expand('~/.vim/plugged')
endif

" Plugins Start Here
if !empty(glob(expand($VIMDIR.'/autoload/plug.vim')))
    call plug#begin($VIMPLUGINDIR)

    " Support Editor Config
    Plug 'editorconfig/editorconfig-vim'

    " Surround with Braces
    Plug 'tpope/vim-surround'

    " Status Line
    Plug 'itchyny/lightline.vim'

    " FZF AutoComplete
    Plug 'junegunn/fzf'

    " NerdTree File Browser
    Plug 'preservim/nerdtree'
    Plug 'jistr/vim-nerdtree-tabs'
    if (!&diff) && isdirectory(expand($VIMPLUGINDIR.'/nerdtree'))
	" Start NERDTree and put the cursor back in the other window.
	autocmd VimEnter * NERDTree | wincmd p
    endif

    " Better looking NERDTree
    Plug 'ryanoasis/vim-devicons'

    " Vim-AI for AutoComplete / Inference
    if has('python3')
        Plug 'guilt/vim-ai'
    endif

    call plug#end()
endif
" Plugins End Here
