**[Polish]**

Główny failover odnośnie printk to RS 232 (driver\serial.c).

Fizyczna pamięć RAM podzielona jest na strony o rozmiarze 4 KiB każda.

Binarna mapa pamięci powinna znajdować się w największym niepodzielnym zbiorze stron pamięci fizycznej.

	[przykład]

	Pamięć RAM o rozmiarze 128 KiB:
	11100011111111111111000000110001
	|     \------------/
	|           |
	|           ^ największy niepodzielny zbiór
	|
	^ jeden bit odpowiada przestrzeni 4 KiB

1. znaleźć największy niepodzielny zbiór stron **kernel/init/memory.c** linie 15..33
