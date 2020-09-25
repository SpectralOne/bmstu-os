sub_1		proc	near

;; Сохранение регистров DS, AX
020A:07B9  1E					push	ds
020A:07BA  50					push	ax

;; Установка сегмента данных = 0040
020A:07BB  B8 0040				mov	ax,40h
020A:07BE  8E D8				mov	ds,ax

;;Загрузка EFLAGS в AH
020A:07C0  9F					lahf				 ; Load ah from flags
020A:07C1  F7 06 0314 2400		test	word ptr ds:[314h],2400h		; (0040:0314=3200h)
020A:07C7  75 0C				jnz	loc_7			 ; Jump if not zero
020A:07C9  F0> 81 26 0314 FDFF	lock	and	word ptr ds:[314h],0FDFFh	; (0040:0314=3200h)
020A:07D0			loc_6:
020A:07D0  9E					sahf				; Store ah into flags
020A:07D1  58					pop	ax
020A:07D2  1F					pop	ds
020A:07D3  EB 03				jmp	short loc_8		; (07D8)
020A:07D5			loc_7:
020A:07D5  FA					cli					; Disable interrupts
020A:07D6  EB F8				jmp	short loc_6		; (07D0)
020A:07D8			loc_8:
020A:07D8  C3					retn
sub_1		endp
