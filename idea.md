**[Polish]**

Główny failover odnośnie printk to RS 232 (driver\serial.c).

Fizyczna pamięć RAM podzielona jest na strony o rozmiarze 4 KiB każda.

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

