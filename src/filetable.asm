; ------------------
;  Simple Filetable
; ------------------
;
; 16 Byte entries
; 0-9   - Filename
; 10-12 - File extension (type)
; 13    - Number of filetable entries
; 14    - Starting Sector
; 15    - Filesize in sectors (n * 512)
;
; ------------------

filetable:
    ; db 'all',0,0,0,0,0,0,0,'txt',0,1,7 ; all           - start: 1, size: 7
    db 'boot',0,0,0,0,0,0, 'bin',0,1,1   ; boot.bin      - start: 1, size: 1
    db 'filetable',0,      'txt',0,2,1   ; filetable.txt - start: 2, size: 1
    db 'kernel',0,0,0,0,   'bin',0,3,4   ; kernel.bin    - start: 3, size: 4
    db 'calc',0,0,0,0,0,0, 'bin',0,7,1   ; calc.bin      - start: 7, size: 1
    db 'edit',0,0,0,0,0,0, 'bin',0,8,4   ; edit.bin      - start: 8, size: 2

times 512-($-$$) db 0
