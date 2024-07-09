**[Polish]**

Ilość materiału do opisu jest zatrważająca, nie wiadomo czego się chwycić i jaką kolejnośc przyjąć, aby miało to ręce i nogi.

Dokument powstawał będzie kawałek po kawałku.

Pierwsza wersja utworzona będzie w języku ojczystym, 

### Menu
1. [Binary Memory Map](#binary-memory-map)

	1.1. [Kernel Binary Memory Map of Physical RAM Pages](#kernel-binary-memory-map-of-physical-ram-pages)

	1.2. [High Memory](#high-memory)

	1.3. [Lokalizacja i inicjalizacja](#)

# 1. Binary Memory Map
Przyjęta struktura binarnej mapy pamięci zakłada - **wbudowany** semafor odpowiedzialny za sygnalizację użycia w danym momencie przez jeden z procesów. Najlepszą lokalizacją tegoż semafora to ostatni bajt przestrzeni binarnej mapy pamięci.

	011111111110000000000001
	\---------------------/^
	|                      semafor
	^
	dostępne strony do wykorzystania

en. Atomic semantics - w wydaniu procesorów z rodziny x86 pozwala na semafory o rozmiarze nie mniejszej niż 1 Bajt zatem struktura powyższej binarnej mapy pamięci wyglądać będzie następująco:

	  bajt 0   bajt 1   [...] bajt 4095
	  11111110 00000111 [...] 00000001
	  ^      ^      ^         \------/
	  |      |      |          semafor
	  |    bit 0    |
	bit 7-my     bit 10-ty

Fizyczna pamięć RAM podzielona jest na strony o rozmiarze 4 KiB każda.

Na podstawie powyższego przykładu możemy wywnioskować:
- rozmiar dostępnej przestrzeni pamięci RAM to 11 Stron (44 KiB),
- pierwsza strona przeznaczona została na binarną mapę pamięci (bit 0),
- adres binarnej mapy pamięci to 0x0000 (bit **0** * **4096** bajtów = **0x0000** adres strony),
- semafor binarnej mapy pamięci znajduje sie na końcu przestrzeni binarnej mapy pamięci pod adresem **0x0FFF**,
- semafor jest **włączony**, co oznacza że któryś z procesów aktualnie z niej korzysta,
- adres pierwszej wolnej strony do wykorzystania to 0x1000.

## 1.1. Kernel Binary Memory Map of Physical RAM Pages

Jądro systemu jako jedyne zarządza binarną mapą pamięci opisującą dostępność stron w fizycznej pamięci RAM. Dostęp do niej odbywa się za pomocą funkcji:
- kernel_memory_alloc - *zwraca adres logiczny N kolejnych po sobie stron*
- kernel_memory_alloc_low - *dostęp do zbioru stron poniżej adresu fizycznego **0x100000** RAM (1 MiB)*
- kernel_memory_alloc_page - *zwraca adres fizyczny 1 strony w przestrzeni RAM*
- kernel_memory_release - *przyjmuje adres logiczny przestrzeni N kolejnych po sobie stron*
- kernel_memory_release_page - *przyjmuje adres fizyczny 1 strony z przestrzeni RAM*

**kernel_memory_alloc_low** wykorzystywany jest dla urządzeń które nie potrafią korzystać z adresacji logicznej, np. DMA

## 1.2. High Memory

Dostęp do stron pamięci fizycznej RAM odbywa się tylko i wyłącznie poprzez odzwierciedlenie przestrzeni tzw. High Memory

Przestrzeń pamięci logicznej (tzw. wirtualnej) podzielona jest na dwie strefy:

	           Przestrzeń Procesów                  Przestrzeń Jądra Systemu
	0x0000000000000000 - 0x00007FFFFFFFFFFF  0xFFFF800000000000 - 0xFFFFFFFFFFFFFFFF
	|-----------------...-----------------|  |-----------------...-----------------|



**Draft**
---
Główny failover odnośnie printk to RS 232 (**driver/serial.c**).



Dostęp do stron pamięci fizycznej RAM odbywa się tylko i wyłącznie poprzez odzwierciedlenie przestrzeni tzw. High Memory

	[przykład]

	dodając do adresu strony w przestrzeni fizycznej RAM wartości 0xFFFF800000000000 (oznaczonej później jako KERNEL_PAGE_mirror) udostępni nam jej zawartość w przestrzeni High Memory

	czyli dostęp do zawartości strony pod adresem fizycznym 0x2000 znajduje się w przestrzeni High Memory pod adresem 0xFFFF800000002000
	
	0x2000 | 0xFFFF800000000000 = 0xFFFF800000002000

Binarna mapa pamięci powinna znajdować się w największym niepodzielnym zbiorze stron pamięci fizycznej.

	[przykład]

	Pamięć RAM o rozmiarze 128 KiB:
	11100011111111111111000000110001
	|     \------------/
	|           |
	|           ^ największy niepodzielny zbiór
	|
	^ jeden bit odpowiada przestrzeni 4 KiB

1. znaleźć największy niepodzielny zbiór stron **kernel/init/memory.c** linie 15..31

Przed binarną mapą pamięci, znajduje się przestrzeń zmiennych globalnych o rozmiarze wyrównanym do pełnej strony.

	[przykład]

	jeśli rozmiar wszystkich zmiennych globalnych wynosi 264 Bajty to przestrzeń przeznaczona do ich przechowywania rozpościera się na całe 4 KiB (czyli tzw. stronę)

	i analogicznie do 5125 Bajtów będą to 2 strony tj. 8192 Bajty

2. początek największego niepodzielnego zbioru stron będzie zarazem lokalizacją zmiennych globalnych **kernel/init/memory.c** linia 27

3. binarna mapa pamięci stron fizycznych znajduje się od razu za przestrzenią zmiennych globalnych

Każda struktura binarnej mapy pamięci ma wbudowany w siebie semafor, określający jej przetwarzanie w danym momencie. Lokalizacja tegoż semafora znajduje się zawsze na samym końcu binarnej mapy pamięci
