;*******************************
;       YAY Boot 3!            *
;*******************************

; db = define byte (1 byte)
; bpb = BIOS parameter Block
; dw = define word {2 bytes}
;lodsb = loads one byte from memory "LOad String Byte" 
;it reads from DS:SI in memory, puts that byte into AL and increments SI by 1
;dd = 4 bytes

bits 16                     ;16 bit real mode 

org  0x7C00                 ;Tells the Bios where to load the bootloader code (at 0x7C00)
							;org means "origin" 
							;the bios will have done that already but its telling the assembler
							;where it should start and put all of the other parts of the code

start:      jmp loader      ;goes straight to "loader" without conditions


;OEM FAT12 BIOS Parameter Block (BPB) "The OEM Parameter Block stores the Windows MBR and
;Boot Record information. Its primary purpose is to describe the filesystem on the disk"

;;	TIMES 0Bh-$+start DB 0		; The OEM Parameter Block is exactally 3 bytes
								; from where we are loaded at. This fills in those
								; 3 bytes, along with 8 more

bpbOEM			db "Bleba OS"; This member must be exactally 8 bytes. It is just
								; the name of your OS : Everything else remains the same.

bpbBytesPerSector:  	DW 512  ; 512 bytes per sector
bpbSectorsPerCluster: 	DB 1	; 
bpbReservedSectors: 	DW 1
bpbNumberOfFATs: 	    DB 2
bpbRootEntries: 	    DW 224
bpbTotalSectors: 	    DW 2880
bpbMedia: 	            DB 0xF0
bpbSectorsPerFAT: 	    DW 9
bpbSectorsPerTrack: 	DW 18
bpbHeadsPerCylinder: 	DW 2
bpbHiddenSectors: 	    DD 0
bpbTotalSectorsBig:     DD 0
bsDriveNumber: 	        DB 0
bsUnused: 	            DB 0
bsExtBootSignature: 	DB 0x29
bsSerialNumber:	        DD 0xa0a1a2a3
bsVolumeLabel: 	        DB "MOS FLOPPY "
bsFileSystem: 	        DB "FAT12   "

msg	db	"Welcome to My Operating System!", 0		; the string to print in db to store raw bytes
													; with 0 as null terminator so it stops at 0

;*************************************************************************

;***************************************
;	Prints a string
;	DS=>SI: 0 terminated string
;***************************************

Print:
			lodsb					; load next byte from string from SI to AL
			or			al, al		; Performs AL OR AL to update the Zero Flag based on whether AL is 0
			jz			PrintDone	; null terminator found so exit to printdone
			mov			ah,	0eh	; Nope-Print the character
			int			10h  ; Cpu runs this to jump into bios video driver code and tells it to 
							;check register ah where OEh (its character output function) is. 
							; it then takes the character from AL and prints it. 
			jmp			Print		; Repeat until null terminator found
PrintDone:
			ret					; we are done, so return

;*************************************************;
;	Bootloader Entry Point
;*************************************************;

loader:

	xor	ax, ax		; Setup segments to insure they are 0.
	mov	ds, ax		; We have ORG 0x7c00. This means all addresses are based
	mov	es, ax		; from 0x7c00:0. Because the data segments are within the same
				    ; code segment.

	mov	si, msg						; our message to print
	call	Print						; call our print function

	xor	ax, ax						; clear ax
	int	0x12						; get the amount of KB from the BIOS

	cli							; Disables Interrupts
	hlt							; halt the system
	
times 510 - ($-$$) db 0						; We have to be 512 bytes. Clear the rest of the bytes with 0

dw 0xAA55							; Boot Signiture
