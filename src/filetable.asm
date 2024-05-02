[bits 16]

;;
;; Filetable layout
;; 0-9 : Filename
;; 10-12 : Filetype
;; 13 : RESERVED
;; 14 : Starting sector
;; 15 : File Length (in sectors)
;;

filetable:
    db "boot",0,0,0,0,0,0, "bin",0,0,5
    db "filetable",0,      "tab",0,5,1
    db "kernel",0,0,0,0,   "bin",0,6,30
    db "term16n",0,0,0,    "fnt",0,36,4
    db "testfont",0,0,     "fnt",0,40,4
    db "test",0,0,0,0,0,0, "txt",0,44,1
    db "calc",0,0,0,0,0,0, "bin",0,45,2

times 512-($-$$) db 0

