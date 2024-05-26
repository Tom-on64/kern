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
    db "boot",0,0,0,0,0,0, "bin",0,0,1
    db "stage2",0,0,0,0,   "bin",0,1,4
    db "stage3",0,0,0,0,   "bin",0,5,10
    db "filetable",0,      "tab",0,15,1
    db "kernel",0,0,0,0,   "bin",0,16,40
    db "term16n",0,0,0,    "fnt",0,56,4
    db "testfont",0,0,     "fnt",0,60,4
    db "test",0,0,0,0,0,0, "txt",0,64,1
    db "calc",0,0,0,0,0,0, "bin",0,65,2

times 512-($-$$) db 0

