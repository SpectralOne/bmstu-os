## Part 1

Просят показать разделЫ TASKS и MAPS и сказать, что в MAPS мы видим адресса выделенные для программы, библиотек и прочего

## Part 2

### Fortune

Просят показать созданные файлы и симлинк с inode: `ls -ial /proc | grep fortune`

Потом просят записать 3 раза AA, BB, CC и прочитать 4 раза, потом показать dmesg

Сколько точек входа? 6 - exit, init, read, write, open, release

Какой файл создали? Виртуальный, т.к. /proc/ -- виртуальная ФС

Какие функции и зачем использовали? copy_from_user, copy_to_user. Почему? Процесс имеет виртуальное адресное пространство, ядро физическое. При попытке обратиться из ядра к буферу страница может быть выгружена.

### Seqfile

То же самое, только точек входа 9: exit, init, read, write, open, release, start, next, show

