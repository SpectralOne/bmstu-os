#!/bin/bash
# Соберем модули ядра 
make
echo ">>>>> Loading md1.ko..."
echo
sleep 0.8

# Загрузим модули ядра 
insmod md1.ko
echo ">>>>> Loading md2.ko..."
echo
sleep 0.8
insmod md2.ko
echo ">>>>> Modules 1 and 2 was loaded. Let's look at them: "

# Выведем список загружен модулей ядра, чье название содержит строку «md» 
lsmod | grep md
echo
sleep 1

# Посмотрим последние 15 сообщений, выведенных модулями ядра
echo ">>>>> This is the last 15 lines in the system log: "
dmesg | tail -15
echo
read -n 1 -s -r -p "Press any key to continue..."
echo

# Выгрузим 2ой модуль ядра 
echo ">>>>> Remove md2..."
rmmod md2
echo
sleep 0.8

# Загрузим 3ий модуль ядра, возвращающий ошибку
echo ">>>>> Loading md3.ko..."
echo
insmod md3.ko
sleep 0.8

# Посмотрим последние 15 сообщений, выведенных модулями ядра
echo ">>>>> This is the last 15 lines in the system log: "
dmesg | tail -15
echo
read -n 1 -s -r -p "Press any key to continue..."
echo

# Выгрузим 3ий и 1ый модули ядра 
echo ">>>>> Remove md3..."
rmmod md3
echo
sleep 0.8
echo ">>>>> Remove md1..."
rmmod md1
echo
sleep 0.8

# Посмотрим последние 5 сообщений, выведенных модулями ядра
dmesg | tail -5
