" All system-wide defaults are set in $VIMRUNTIME/debian.vim and sourced by
" the call to :runtime you can find below.  If you wish to change any of those
" settings, you should do it in this file (/etc/vim/vimrc), since debian.vim
" will be overwritten everytime an upgrade of the vim packages is performed.
" It is recommended to make changes after sourcing debian.vim since it alters
" the value of the 'compatible' option.

" This line should not be removed as it ensures that various options are
" properly set to work with the Vim-related packages available in Debian.
runtime! debian.vim

" Vim will load $VIMRUNTIME/defaults.vim if the user does not have a vimrc.
" This happens after /etc/vim/vimrc(.local) are loaded, so it will override
" any settings in these files.
" If you don't want that to happen, uncomment the below line to prevent
" defaults.vim from being loaded.
" let g:skip_defaults_vim = 1

" Uncomment the next line to make Vim more Vi-compatible
" NOTE: debian.vim sets 'nocompatible'.  Setting 'compatible' changes numerous
" options, so any other options should be set AFTER setting 'compatible'.
"set compatible

" Vim5 and later versions support syntax highlighting. Uncommenting the next
" line enables syntax highlighting by default.
if has("syntax")
  syntax on
endif

" If using a dark background within the editing area and syntax highlighting
" turn on this option as well
"set background=dark

" Uncomment the following to have Vim jump to the last position when
" reopening a file
"if has("autocmd")
"  au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe "normal! g'\"" | endif
"endif

" Uncomment the following to have Vim load indentation rules and plugins
" according to the detected filetype.
"if has("autocmd")
"  filetype plugin indent on
"endif


inoremap {/<CR> { // <CR>}<ESC>ko<Space><Space><Up><End>
inoremap {<CR> {<CR>}<ESC>ko
" Tab o , tab x
inoremap {<Space> {   }<ESC>hhi
" inoremap { {}<ESC>i
inoremap ( ()<ESC>i
inoremap () ()<ESC>a
inoremap [ []<ESC>i
" inoremap < <><ESC>i
inoremap ' ''<ESC>i
inoremap " ""<ESC>i
imap if( if(){<CR><Up><End><Left><Left>
imap for( for(){<CR><Up><End><Left><Left>
imap while( while(){<CR><Up><End><Left><Left>
" 

nnoremap <A-Up> :m-2<CR>
nnoremap <A-Down> :m+<CR>
inoremap <A-Up> <Esc>:m-2<CR>
inoremap <A-Down> <Esc>:m+<CR>

nnoremap j <Up><Up><Up><Up>
nnoremap k <Down><Down><Down><Down>
nnoremap h <Left><Left>
nnoremap l <Right><Right>

" shift+arrow selection
nmap <S-Up> <Up><Up><Up><Up><Up><Up><Up><Up>
nmap <S-Down> <Down><Down><Down><Down><Down><Down><Down><Down>

"nmap <S-Left> v<Left>
"nmap <S-Right> v<Right>

imap <C-p> <Esc>pi

imap <S-Up> <Esc>v<Up>
imap <S-Down> <Esc>v<Down>
imap <S-Left> <Esc>v<Left>
imap <S-Right> <Esc>v<Right>

imap <C-r> <Esc><C-r>i
imap <C-u> <Esc>ui

"vmap <S-Up> <Up>
"vmap <S-Down> <Down>
"vmap <S-Left> <Left>
"vmap <S-Right> <Right>

map <C-Up> <Up><Up><Up><Up>
map <C-Down> <Down><Down><Down><Down>

vmap j <Up><Up>
vmap k <Down><Down>
vmap h <Left><Left>
vmap l <Right><Right>

" The following are commented out as they cause vim to behave a lot
" differently from regular Vi. They are highly recommended though.
"set showcmd		" Show (partial) command in status line.
"set showmatch		" Show matching brackets.
"set ignorecase		" Do case insensitive matching
"set smartcase		" Do smart case matching
"set incsearch		" Incremental search
"set autowrite		" Automatically save before commands like  :next and :make
"set hidden		" Hide buffers when they are abandoned
"set mouse=a		" Enable mouse usage (all modes)

"set relativenumber

set number
"set formatoptions+=r " comment? auto?
set formatoptions-=c " comment? auto?
set formatoptions+=o " comment? auto?
"set formatoptions " comment? auto?
"klla?
set tabstop=2
set shiftwidth=2
set expandtab
"set smartindent
"set autoindent

set encoding=utf-8
set fileencodings=utf-8,cp949


inoremap /*<CR> /** <CR> */<ESC><Up>o
inoremap /// /*  */<ESC>3hi 
imap <S-Return> <ESC>o
imap <C-Return> <ESC>o
imap <S-CR> <ESC>o
imap <C-CR> <ESC>o
imap <S-Enter> <ESC>o
imap <C-Enter> <ESC>o
imap o<CR> <Esc>o
imap c<CR> <CR>//<Space>
imap b<CR> <CR>*<Space>
imap ,, \n<Right><Right>;
imap ,. <><Left>
imap ,; <<"\n<Right><Right>;
imap /** /************************************************************<CR><CR>************************************************************/<Up><Space>
imap #d #define 
imap #i #include 
imap log<CR> console.log();<Left><Left>
imap table<CR> console.table();<Left><Left>
imap co<CR> cout<<"
imap sco<CR> std::cout<<"
"imap so<CR> std::cout<<"


" The following are commented out as they cause vim to behave a lot
" differently from regular Vi. They are highly recommended though.
"set showcmd		" Show (partial) command in status line.
"set showmatch		" Show matching brackets.
"set ignorecase		" Do case insensitive matching
"set smartcase		" Do smart case matching
"set incsearch		" Incremental search
"set autowrite		" Automatically save before commands like :next and :make
"set hidden		" Hide buffers when they are abandoned
set mouse=a		" Enable mouse usage (all modes)

" Source a global configuration file if available
if filereadable("/etc/vim/vimrc.local")
  source /etc/vim/vimrc.local
endif

