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
    db "boot",0,0,0,0,0,0, "bin",0,0,4
    db "filetable",0,      "tab",0,4,1
    db "kernel",0,0,0,0,   "bin",0,5,20
    db "test",0,0,0,0,0,0, "txt",0,25,1

times 512-($-$$) db 0
