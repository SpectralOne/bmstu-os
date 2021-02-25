; far_jump на сегмент, смещение
far_jump macro label, segment  
    db	0EAh 
    dd	offset label
    dw	segment
endm

; печать строки на экран
print_str macro str 
    mov ah, 09h
    lea dx, str
    int 21h
	xor dx, dx
    mov ah, 2
    mov dl, 13
    int 21h
    mov dl, 10
    int 21h
endm

; ожидание ввода символа с клавиатуры
wait_key macro 
    push eax
    mov ah, 10h
    int 16h
    pop eax
endm

; очистить экран
clear_screen macro 
    mov	ax, 3
    int	10h
endm

; загружаем дескриптор в таблицу
load_gdt macro gdt_desc
    shl eax, 4                        ; eax - линейный базовый адрес (*2^4 = 16) (в eax был seg => он выравнен по параграфу => линейный адрес seg * 16)
    mov word ptr gdt_desc.base_l, ax  ; загрузка младшей часть базы
    shr eax, 16                       ; старшую половину eax в ax
    mov byte ptr gdt_desc.base_m, al  ; загрузка средней часть базы
    mov byte ptr gdt_desc.base_h, ah  ; загрузка старшей часть базы
endm

; загружает в регистр gdtr лин баз адрес gdt и ее размер
init_gdtr macro reg                 ; в reg полный линейный адрес GDT
    mov	dword ptr gdtr + 2, reg	    ; кладём полный линейный адрес в старшие 4 байта переменной gdtr
    mov word ptr  gdtr, gdt_size-1	; в младшие 2 байта заносим размер gdt, из-за определения gdt_size (через $) настоящий размер на 1 байт меньше
    lgdt fword ptr gdtr 		    ; загрузим GDT
endm

load_idt macro idt_desc
    mov	idt_desc.offs_l, ax ; загрузить младшую часть смещения
    shr	eax, 16             ; переместить старшую часть в младшую
    mov	idt_desc.offs_h, ax ; загрузить старшую часть смещения
endm

init_idtr macro reg                 ; в reg полный линейный адрес IDT
    mov	 dword ptr idtr + 2, reg	; загрузить полный линейный адрес в старшие 4 байта переменной idtr
    mov  word ptr  idtr, idt_size-1	; в младшие 2 байта заносим размер idt
endm

;перепрограммируем контроллер
set_interrupt_base macro base
	; чтобы начать инициализацию пик, нужно на порт команды 20h отправить команду 11h
	; она заставляет контроллер ждать слова инициализации
	mov	al, 11h						; команда - инициализировать ведущий контроллер
	out	20h, al						; отправить команду по шине данных ведущему контроллеру

	; отправляем новый базовый лин адрес
	mov	al, base					; базовый вектор (начальное смещение для обработчика) установить в 32
	out	21h, al						; отправить базовый вектор ведущему контроллеру
	
	mov	al, 4						; 4 = 0000 0100
	out	21h, al                     ; сообщить mask_master PIC, что mask_slave подключён к IRQ2

	; 
	mov	al, 1						; указываем, что нужно будет посылать
	out	21h, al                     ; команду завершения обработчика прерывания
endm

mem_str macro 
    mov dI, 0
    mov ah, 00000110b
    mov al, 'M'
    stosw
    mov al, 'e'
    stosw
    mov al, 'm'
    stosw
    mov al, 'o'
    stosw
    mov al, 'r'
    stosw
    mov al, 'y'
    stosw
    mov al, ':'
    stosw              
endm


