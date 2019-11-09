# Установка и запуск проекта (Ubuntu 16.x.x)
1. Установить https://www.virtualbox.org/wiki/Downloads
2. Скачать образ виртуальной машины (64-bit 16.x.x) https://www.osboxes.org/ubuntu/
3. В приложении VBox: "Создать" -> [Задать имя], [Использовать существующий жёсткий диск] -> [Выбрать *.vdi из предыдущего пункта]
4. После создания можно настроить машину: выделить пару ядер (если есть) и 1-2 GB оперативной памяти
5. Запустить машину
6. Войти. Пароль по умолчанию: **osboxes.org**
7. Открыть терминал
8. Выполнить следующие команды (https://askubuntu.com/questions/15433/unable-to-lock-the-administration-directory-var-lib-dpkg-is-another-process)
```
sudo rm /var/cache/apt/archives/lock
sudo rm /var/lib/dpkg/lock
```
9. Установить следующие пакеты:
```
sudo apt update
sudo apt install git autoconf zlib1g-dev libbz2-dev liblzma-dev libncurses5-dev
```
10. Склонировать проект (https://stackoverflow.com/questions/3796927/how-to-git-clone-including-submodules):
```
cd ~/Downloads
git clone --recurse-submodules https://gitlab.com/sort-dev/gray-team/gray-sort.git
cd gray-sort/
```
11. Установить библиотеку htslib (ignore warnings) (https://github.com/samtools/htslib):
```
cd htslib/
autoheader
autoconf
./configure
make
sudo make install
cd ..
```
12. Установить библиотеку samtools (ignore warnings) (https://github.com/samtools/samtools)
```
cd samtools/
autoheader
autoconf
./configure
make
sudo make install
cd ..
```
13. Подключить собранные библиотеки для динамического связывания
```
sudo ldconfig
```
14. Предварительно из папки проекта (1 раз):
```
autoconf
./configure
```
15. Можно собирать (сортировка чисел):
```
make meta
```

# Установка и запуск проекта (СКР-Торнадо)
in progress...

# FAQ
in progress...

Задавать ЛЮБЫЕ вопросы, связанные с проектом, МОЖНО и НУЖНО

Почта: volgarenok@gmail.com

VK: https://vk.com/datacrone

С помощью Issues -> New issue: https://gitlab.com/sort-dev/gray-team/gray-sort/issues

# TODO
TODO: CLI (N, M, input, output, silent...)

TODO: Bitonic merge sor

TODO: Makefile - fix include via -I

TODO: Project dependencies: samtools, etc.

TODO: clog/cout/cerr output policy

TODO: ...
